using System;
using VideoOS.Vps.VpsCommon;

namespace VideoOS.Vps.VPService.WebSockets
{
    public class MessageReceivedEventArgs : EventArgs
    {
        public MessageReceivedEventArgs(MessageBase msg)
        {
            Message = msg;
        }
        public MessageBase Message { get; }
    }
}
