using System.Net.WebSockets;
using VideoOS.Vps.VpsCommon;

namespace VideoOS.Vps.VPService.WebSockets
{
    public class MessageReceivedResult
    {
        public WebSocketReceiveResult Result { get; set; }
        public MessageBase Message { get; set; }

        public MessageReceivedResult(WebSocketReceiveResult result)
        {
            Result = result;
            Message = null;
        }

        public MessageReceivedResult(WebSocketReceiveResult result, MessageBase message)
        {
            Result = result;
            Message = message;
        }
    }
}