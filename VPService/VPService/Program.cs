using System;
using System.Diagnostics.CodeAnalysis;
using System.Linq;
using Microsoft.AspNetCore;
using Microsoft.AspNetCore.Hosting;
using Microsoft.Extensions.Logging;
using NLog;
using NLog.Web;

namespace VideoOS.Vps.VPService
{
    public static class Program
    {
        [SuppressMessage("Microsoft.Design", "CA1031:DoNotCatchGeneralExceptionTypes", Justification = "Deliberately catching all exceptions in Main")]
        [SuppressMessage("Microsoft.Design", "CA1303:DoNotPassLiteralsAsLocalizedParameters", Justification = "This is a sample application")]

        public static void Main(string[] args)
        {
            Interfaces.ILogger logger = Logging.LoggerFactory.GetLogger();

            if (logger == null)
            {
                // Creating the logger failed, so bail out. LoggingNotInitialized
                Console.WriteLine("Error instantiating logging. Terminating.");

                return;
            }

            if (args.Contains("-log_trace"))
            {
                logger.ChangeLogLevel("trace", true);
            }
            if (args.Contains("-log_debug"))
            {
                logger.ChangeLogLevel("debug", true);
            }

            try
            {
                logger.LogInfo("Main - VPService starting...");
                CreateWebHostBuilder(args).Build().Run();
            }
            catch (System.Exception e)
            {
                logger.LogError(e.Message);
            }
            finally
            {
                LogManager.Shutdown();
            }
        }

        public static IWebHostBuilder CreateWebHostBuilder(string[] args) =>
            WebHost.CreateDefaultBuilder(args)
                .UseKestrel() // Please refer to appsettings.$(ASPNETCORE_ENVIRONMENT).json for webserver settings
                .UseStartup<Startup>()
                .ConfigureLogging(logging =>
                {
                    logging.ClearProviders();
                    logging.SetMinimumLevel(Microsoft.Extensions.Logging.LogLevel.Trace);
                })
                .UseNLog();
    }
}
