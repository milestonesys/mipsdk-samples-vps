using System;
using System.Collections.Concurrent;
using System.Diagnostics;
using System.Diagnostics.CodeAnalysis;
using System.Globalization;
using System.IO;
using System.Net.WebSockets;
using System.Runtime.Serialization;
using System.Runtime.Serialization.Formatters.Binary;
using System.Threading;
using System.Threading.Tasks;

using Microsoft.AspNetCore.Connections;

using VideoOS.Vps.VpsCommon;

namespace VideoOS.Vps.VPService.WebSockets
{
    public enum WebSocketHandlerState
    {
        Created = 0,
        Opening = 1,
        Open = 2,
        Closing = 3,
        Closed = 4
    }
	
    public class WebSocketHandler : IDisposable
    {
        private CancellationToken _cancellationToken = new CancellationToken();
        private SemaphoreSlim _sendSemaphore = new SemaphoreSlim(1);
        private Interfaces.ILogger _logger;
        private Guid _traceId;
        private const int _initialBufferSize = 4096;
        private bool _closePending = false;
        private WebSocketHandlerState _state;
        private WebSocket _webSocket;

        public event EventHandler<MessageReceivedEventArgs> MessageReceived;

        public WebSocketHandler(Interfaces.ILogger logger, Guid traceId)
        {
            _logger = logger;
            _traceId = traceId;
            _state = WebSocketHandlerState.Created;
        }

        protected virtual void Dispose(bool disposing)
        {
            if (disposing)
            {
                // dispose managed resources
                _sendSemaphore.Dispose();
            }
            // free native resources
        }

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        public bool IsConnectedOrComingUp
        {
            get
            {
                return _state < WebSocketHandlerState.Closing;
            }
        }

        [SuppressMessage("Microsoft.Design", "CA1031:DoNotCatchGeneralExceptionTypes", Justification = "All exceptions deliberately caught for investigation.")]
        public async Task ReceiveLoop(WebSocket webSocket)
        {
            _logger.LogTrace(_traceId, "WebSocketHandler.ReceiveLoop - Tasks start");
            _webSocket = webSocket;
            _state = WebSocketHandlerState.Open;
            while (_webSocket.State == WebSocketState.Open || _webSocket.State == WebSocketState.CloseSent)
            {
                try
                {
                    var result = await ReceiveFullMessage().ConfigureAwait(false);
                    if (result.Result.MessageType == WebSocketMessageType.Close)
                    {
                        _state = WebSocketHandlerState.Closing;
                        _logger.LogTrace(_traceId, $"Received close connection with status {result.Result.CloseStatusDescription}");
                        if (_webSocket.State == WebSocketState.Open)
                        {
                            _logger.LogTrace(_traceId, $"Sending close connection with status {result.Result.CloseStatusDescription}");
                            await _webSocket.CloseAsync(WebSocketCloseStatus.NormalClosure, string.Empty, _cancellationToken).ConfigureAwait(false);
                        }
                        _state = WebSocketHandlerState.Closed;
                        break;
                    }
                    else
                    {
                        if (result.Message != null)
                        {
                            MessageReceived?.Invoke(this, new MessageReceivedEventArgs(result.Message));
                        }
                    }

                    Thread.Sleep(0);
                }
                catch (ConnectionAbortedException)
                {
                    _logger.LogInfo(_traceId, "ReceiveLoop canceled");
                    break;
                }
                catch (TaskCanceledException ex)
                {
                    _logger.LogInfo(_traceId, $"ReceiveLoop canceled: {ex.Message}");
                    break;
                }
                catch (WebSocketException ex)
                {
                    _logger.LogInfo(_traceId, $"ReceiveLoop websocket exception: {ex.Message}");
                }
                catch (Exception ex)
                {
                    _logger.LogInfo(_traceId, $"ReceiveLoop unplanned exception: {ex.Message}");
                }
            }

            _state = WebSocketHandlerState.Closed;
            _logger.LogTrace(_traceId, "ReceiveLoop - Task done");
        }

        private async Task<MessageReceivedResult> ReceiveFullMessage()
        {            
            byte[] firstFrame = new byte[_initialBufferSize];
            WebSocketReceiveResult response = await _webSocket.ReceiveAsync(new ArraySegment<byte>(firstFrame, 0, _initialBufferSize), _cancellationToken).ConfigureAwait(false);
            if (response.MessageType == WebSocketMessageType.Close)
            {
                _logger.LogTrace(_traceId, $"Receive close {response.Count.ToString(CultureInfo.InvariantCulture)}");
                return new MessageReceivedResult(response);
            }
            MessageBase message = MessageFactory.CreateMessage(firstFrame);
            if (message == null)
            {
                _logger.LogError(_traceId, $"Receive invalid message.");

                // CreateMessage will do checks if the header bytes are reasonable.
                // If they are not reasonable, skip the rest of the web socket message
                while (!response.EndOfMessage)
                {
                    response = await _webSocket.ReceiveAsync(new ArraySegment<byte>(firstFrame, 0, _initialBufferSize), _cancellationToken).ConfigureAwait(false);
                }
                return new MessageReceivedResult(response);
            }

            // Allocate room for reading as many bytes as stated in the header's length field
            int expectedPayloadBytes = (int)message.Header.PayloadLength;
            byte[] payload = new byte[expectedPayloadBytes];

            // We already read some of the payload. Skip the header bytes and copy the initial part of the payload
            int payloadBytesRead = response.Count - MessageHeader.HeaderSize;
            Array.Copy(firstFrame, MessageHeader.HeaderSize, payload, 0, payloadBytesRead);
            int payloadBytesRemaining = expectedPayloadBytes - payloadBytesRead;

            while (!response.EndOfMessage)
            {
                if (payloadBytesRemaining > 0)
                {
                    // As long as we have room in the allocated payload byte array, place data there.
                    response = await _webSocket.ReceiveAsync(new ArraySegment<byte>(payload, payloadBytesRead, payloadBytesRemaining), _cancellationToken).ConfigureAwait(false);
                    payloadBytesRead += response.Count;
                }
                else
                {
                    // If the length from the header info smaller than the actual bytes provided, skip the remaining bytes.
                    response = await _webSocket.ReceiveAsync(new ArraySegment<byte>(firstFrame, 0, _initialBufferSize), _cancellationToken).ConfigureAwait(false);
                }
            }

            message.SetPayload(payload);
            return new MessageReceivedResult(response, message);
        }

        [SuppressMessage("Microsoft.Design", "CA1031:DoNotCatchGeneralExceptionTypes", Justification = "Microsoft docs don't list exceptions")]

        public async Task Send(MessageBase msg)
        {
            if (msg != null && _webSocket.State == WebSocketState.Open)
            {
                await _sendSemaphore.WaitAsync().ConfigureAwait(false);
                try
                {
                    if (_closePending)
                    {
                        _logger.LogTrace(_traceId, $"SendLoop closing connection");
                        try
                        {
                            await _webSocket.CloseOutputAsync(WebSocketCloseStatus.NormalClosure, $"Server closing at {DateTime.Now.ToLongTimeString()}", _cancellationToken).ConfigureAwait(false);
                        }
                        catch (Exception e)
                        {
                            _logger.LogError(_traceId, $"Send exception : {e.Message}");
                        }
                        _state = WebSocketHandlerState.Closed;
                        _closePending = false;
                    }
                    else
                    {
                        try
                        {
                            await _webSocket.SendAsync(new ArraySegment<byte>(msg.MakeHeaderBytes(), 0, MessageHeader.HeaderSize), WebSocketMessageType.Binary, false, _cancellationToken).ConfigureAwait(false);
                            await _webSocket.SendAsync(new ArraySegment<byte>(msg.Payload, 0, (int)msg.Header.PayloadLength), WebSocketMessageType.Binary, true, _cancellationToken).ConfigureAwait(false);
                        }
                        catch (Exception e)
                        {
                            _logger.LogError(_traceId, $"Send exception : {e.Message}");
                        }
                    }
                }
                finally
                {
                    _sendSemaphore.Release();
                }
            }
        }

        public void Close()
        {
            _closePending = true;
        }
    }
}
