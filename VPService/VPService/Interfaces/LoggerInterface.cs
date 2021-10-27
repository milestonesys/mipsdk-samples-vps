using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Microsoft.AspNetCore.Mvc;

namespace VideoOS.Vps.VPService.Interfaces
{
    public enum LoggerResult
    {
        Ok = 200,
        MultiStatus = 207,
        Error = 400,
        Denied = 403
    }

    public interface ILogger
    {
        void LogTrace(Guid traceId, string msg);
        void LogInfo(Guid traceId, string msg);
        void LogError(Guid traceId, string msg);
        void LogDebug(Guid traceId, string msg);
        void LogTrace(string msg);
        void LogInfo(string msg);
        void LogError(string msg);
        void LogDebug(string msg);
        LoggerResult ChangeLogLevel(string levelToChange, bool value);
        byte[] RetrieveFromTime(DateTime logFileTime);
        LoggerResult ChangeLogLevels(Dictionary<string, bool> levelsToChange);
    }
}
