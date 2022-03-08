using System;

using Microsoft.AspNetCore.Builder;

namespace VideoOS.Vps.VPService
{
    public static class GStreamerMiddlewareExtender
    {
        public static IApplicationBuilder UseGStreamer(this IApplicationBuilder appBuilder)
        {
            // This section enables web sockets. It is copied from Microsoft's documentation
            var webSocketOptions = new WebSocketOptions()
            {
                KeepAliveInterval = TimeSpan.Zero
            };

            appBuilder.UseWebSockets(webSocketOptions);

            return appBuilder.UseMiddleware<GStreamerMiddleware>();
        }
    }
}
