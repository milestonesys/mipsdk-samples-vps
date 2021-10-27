using System;
using System.Collections.Concurrent;
using System.Net.WebSockets;
using System.Runtime.InteropServices;
using System.Threading;
using System.Threading.Tasks;
using VideoOS.Vps.VpsCommon;
using VideoOS.Vps.VPService.WebSockets;

namespace VideoOS.Vps.VPService.GStreamer
{
    /// <summary>
    /// This class serves two purposes
    /// It receives media messages (video), which are fed into the actual 3rd party processing GStreamer pipeline via NativeMethods.
    /// It continuously pumps data, if available, from the GStreamer pipeline to the VpsDriver in the XProtect Recording Server.
    /// </summary>
    public class GStreamerRunner : IDisposable
    {
        private WebSocketHandler _webSocketHandler;
        private IntPtr _gStreamerWrapper;
        private ConcurrentQueue<MessageMedia> _dataQueue;
        private ManualResetEvent _dataReadyInQueueEvent = new ManualResetEvent(false);
        private Interfaces.ILogger _logger;
        private Guid _traceId;
        private const int _sleepOnEmptyQueueInMilliseconds = 100;

        public GStreamerRunner(IntPtr gStreamerWrapper, WebSocketHandler webSocketHandler, Interfaces.ILogger logger, Guid traceId)
        {
            if (webSocketHandler == null)
            {
                throw new ArgumentNullException(nameof(webSocketHandler));
            }
            if (gStreamerWrapper == IntPtr.Zero)
            {
                throw new ArgumentNullException(nameof(gStreamerWrapper));
            }
            _gStreamerWrapper = gStreamerWrapper;
            _webSocketHandler = webSocketHandler;
            _webSocketHandler.MessageReceived += WebSocketClient_MessageReceived;
            _dataQueue = new ConcurrentQueue<MessageMedia>();
            _logger = logger;
            _traceId = traceId;
        }

        protected virtual void Dispose(bool disposing)
        {
            if (disposing)
            {
                // dispose managed resources
                _dataReadyInQueueEvent.Dispose();
            }
            // free native resources
            if (_gStreamerWrapper != IntPtr.Zero)
            {
                NativeMethods.DeleteGStreamer(_gStreamerWrapper);
            }
        }

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        private void WebSocketClient_MessageReceived(object sender, MessageReceivedEventArgs e)
        {
            switch ((MessageBase.MessageType)e.Message.Header.MessageType)
            {
                case MessageBase.MessageType.Media:
                    {
                        MessageMedia nextMessage = e.Message as MessageMedia;
                        IntPtr unmanagedPointer = Marshal.AllocHGlobal(nextMessage.Payload.Length);
                        Marshal.Copy(nextMessage.Payload, 0, unmanagedPointer, nextMessage.Payload.Length);
                        NativeMethods.PutData(_gStreamerWrapper, unmanagedPointer, nextMessage.Payload.Length);
                        Marshal.FreeHGlobal(unmanagedPointer);
                    }
                    break;
                case MessageBase.MessageType.Configuration:
                    {
                        MessageConfiguration messageConfiguration = e.Message as MessageConfiguration;
                        _logger.LogTrace(_traceId, $"Configuration received: {messageConfiguration.Configuration}");
                        NativeMethods.SetGStreamerProperties(_gStreamerWrapper, messageConfiguration.Configuration);
                    }
                    break;
                case MessageBase.MessageType.Metadata:
                    // Metadata will not arrive from XProtect. They are to be created by this service and sent to XProtect.
                default:
                    _logger.LogTrace(_traceId, $"Invalid or unexpected message type received: {e.Message.Header.MessageType}");
                    break;
            }
        }

        /// <summary>
        /// This task pulls media and metadata messages off the GStreamer interface and sends them to the client
        /// </summary>
        /// <returns></returns>
        internal async Task GStreamerGetData()
        {
            await Task.Yield();
            while (_webSocketHandler != null && _webSocketHandler.IsConnectedOrComingUp)
            {
                IntPtr outData = NativeMethods.GetData(_gStreamerWrapper);
                if (outData == IntPtr.Zero)
                {
                    // Make sure we don't go 100% CPU when there is no data arriving. Can we make GetData() block until data is ready, max say 200 ms?
                    await Task.Delay(_sleepOnEmptyQueueInMilliseconds).ConfigureAwait(false);
                    continue;
                }

                switch (NativeMethods.GetDataType(outData))
                {
                    case DataType.MediaData:
                        {
                            MessageMedia media = ManagedToNativeConverter.ConvertMedia(outData);
                            await _webSocketHandler.Send(media).ConfigureAwait(false);
                        }
                        break;

                    case DataType.MetaData:
                        {
                            MessageMetadata metadata = ManagedToNativeConverter.ConvertMetadata(outData);
                            await _webSocketHandler.Send(metadata).ConfigureAwait(false);
                        }
                        break;
                    default:
                        break;
                }
            }
        }

        internal Task HandleWebSocket(WebSocket webSocket)
        {
            return _webSocketHandler.ReceiveLoop(webSocket);
        }
    }
}

