using System;
using System.Net.WebSockets;
using System.Threading.Tasks;

using Microsoft.AspNetCore.Http;

using VideoOS.Vps.VPService.GStreamer;
using VideoOS.Vps.VPService.Logging;
using VideoOS.Vps.VPService.WebSockets;

namespace VideoOS.Vps.VPService
{
    public class GStreamerMiddleware
    {
        private readonly RequestDelegate _next;

        public GStreamerMiddleware(RequestDelegate next)
        {
            _next = next;
        }

        public async Task Invoke(HttpContext context)
        {
            // This section accepts web sockets on one specific URL.
            // Except for the specific path, it is copied from Microsoft's documentation
            // This sample code intercepts any path beginning with "/gstreamer/pipelines".
            // It isolates what follows after a / and assumes that is the name of an element/pipeline
            // You can test that the element/pipeline is installed on the server computer by typing "gst-inspect-1.0 <pipelineName>"
            string prefixPath = "/gstreamer/pipelines";

            if (context.Request.Path.StartsWithSegments(new PathString(prefixPath), StringComparison.InvariantCulture))
            {
                // We want to return 404 if the pipeline is not installed. The status code can't be set after AcceptWebSocketAsync() was called
                string pipelineName = context.Request.Path.ToString().Substring(prefixPath.Length).TrimStart('/');
                IntPtr gStreamerWrapper = NativeMethods.CreateGStreamer(pipelineName);

                if (gStreamerWrapper == IntPtr.Zero)
                {
                    context.Response.StatusCode = 404;
                }
                else if (context.WebSockets.IsWebSocketRequest)
                {
                    using (WebSocket webSocket = await context.WebSockets.AcceptWebSocketAsync())
                    {
                        await ProcessWebSocket(context, webSocket, gStreamerWrapper);
                    }
                }
                else
                {
                    context.Response.StatusCode = 400;
                }
            }
            else
            {
                await _next(context).ConfigureAwait(false);
            }
        }

        private static async Task ProcessWebSocket(HttpContext context, WebSocket webSocket, IntPtr gStreamerWrapper)
        {
            Interfaces.ILogger logger = LoggerFactory.GetLogger();
            Guid traceId = Guid.NewGuid();

            logger.LogInfo(traceId, $"Connection made by {context.Connection.RemoteIpAddress}:{context.Connection.RemotePort}");
            await Task.Yield();

            WebSocketHandler webSocketHandler = new WebSocketHandler(logger, traceId);
            {
                using (GStreamerRunner gStreamerRunner = new GStreamerRunner(gStreamerWrapper, webSocketHandler, logger, traceId))
                {
                    var tasks = new Task[2] { gStreamerRunner.HandleWebSocket(webSocket), gStreamerRunner.GStreamerGetData() };

                    await Task.WhenAll(tasks).ConfigureAwait(false);

                    tasks[0].Dispose();
                    tasks[1].Dispose();
                }
            }

            logger.LogInfo(traceId, $"Connection finished for {context.Connection.RemoteIpAddress}:{context.Connection.RemotePort}");
        }
    }
}
