using VideoOS.Vps.VPService.Interfaces;

namespace VideoOS.Vps.VPService.Logging
{
    // This factory makes sure the logger is only created once.
    public static class LoggerFactory
    {
        private static ILogger _instance = null;
        private static readonly object _lock = new object();

        public static ILogger GetLogger()
        {
            lock (_lock)
            {
                if (_instance == null)
                {
                    _instance = new VpsLogger();
                }
            }
            return _instance;
        }
    }
}
