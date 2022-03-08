using System;

using Microsoft.AspNetCore.Builder;
using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Hosting;

namespace VideoOS.Vps.VPService
{
    public class Startup
    {
        public Startup(IConfiguration configuration)
        {

        }

        // This method gets called by the runtime. Use this method to add services to the container.
        public void ConfigureServices(IServiceCollection services)
        {

        }

        // This method gets called by the runtime. Use this method to configure the HTTP request pipeline.
        public void Configure(IApplicationBuilder app)
        {
            // If you choose to re-activate the line below, your server will force clients to switch to HTTPS/WSS, also when HTTP/WS was specified by the user.
            // app.UseHttpsRedirection();

            // Add gstreamer to the pipeline 
            app.UseGStreamer();
        }
    }
}
