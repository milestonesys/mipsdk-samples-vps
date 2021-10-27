using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using VideoOS.Vps.VPService.Interfaces;
using NLog;
using NLog.Config;
using NLog.Targets;
using System.Diagnostics.CodeAnalysis;

namespace VideoOS.Vps.VPService
{
    public class VpsLogger : Interfaces.ILogger
    {
        private NLog.Logger _logger;
        private const string _logFileTargetName = "logfile";
        private const string _consoleTargetName = "logconsole";
        private const string _nlogConfigFileName = "NLog.config";


        public VpsLogger()
        {
            // Write to the console if the log file is not writable for this process
            NLog.Common.InternalLogger.LogToConsole = true;
            NLog.Common.InternalLogger.LogLevel = LogLevel.Warn;

            if (File.Exists(_nlogConfigFileName))
            {
                _logger = NLog.Web.NLogBuilder.ConfigureNLog("NLog.config").GetCurrentClassLogger();
                _logger.Info("Using logging definitions from file NLog.config.");
            }
            else
            {
                _logger = CreateLogger();
                _logger.Info("Using built-in logging definitions. File NLog.config not found.");
            }

            // No further internal logging.
            NLog.Common.InternalLogger.LogToConsole = false;
        }

        // The default is to log to two places: file and console. Initially, both log events with severity Info and higher
        // The file is placed in the specialfolder CommonApplicationData (on Linux, .NET+NLog should figure out some parallel?)
        // Every day, the log file from the previous day is archived, and logs older than 20 days are deleted.

        [SuppressMessage("Microsoft.Reliability", "CA2000:Dispose objects before losing scope", Justification = "targetLogfile is added to loggingConfiguration")]
        protected static NLog.Logger CreateLogger()
        {

            try
            {
                string fileLayout = @"${date:format=yyyy-MM-dd HH\:mm\:ss.fffzzz}  ${uppercase:${level}} - ${message} ${exception:format=tostring}";
                string vendorName = "Milestone";
                string productName = "VPS";

                StringBuilder sbFileName = new StringBuilder();
                sbFileName.Append("${specialfolder:folder=CommonApplicationData}/");
                sbFileName.Append(vendorName);
                sbFileName.Append("/");
                sbFileName.Append(productName);
                sbFileName.Append("/");
                sbFileName.Append(productName);
                sbFileName.Append(".log");

                StringBuilder sbArchiveName = new StringBuilder();
                sbArchiveName.Append("${specialfolder:folder=CommonApplicationData}/");
                sbArchiveName.Append(vendorName);
                sbArchiveName.Append("/");
                sbArchiveName.Append(productName);
                sbArchiveName.Append("/archive/");
                sbArchiveName.Append(productName);
                sbArchiveName.Append("-{######}.log");

                LoggingConfiguration loggingConfiguration = new LoggingConfiguration();
                FileTarget targetLogfile = new FileTarget(_logFileTargetName)
                {
                    FileName = sbFileName.ToString(),
                    Layout = fileLayout,
                    ArchiveFileName = sbArchiveName.ToString(),
                    ArchiveNumbering = ArchiveNumberingMode.Rolling,
                    MaxArchiveFiles = 20,
                    ArchiveEvery = FileArchivePeriod.Day,
                    ArchiveOldFileOnStartup = false,
                    CreateDirs = true
                };
                loggingConfiguration.AddTarget(targetLogfile);
                loggingConfiguration.AddRule(NLog.LogLevel.Info, NLog.LogLevel.Fatal, targetLogfile);
                targetLogfile = null;
                SetupConsoleLogging(loggingConfiguration);

                Logger logger = NLog.Web.NLogBuilder.ConfigureNLog(loggingConfiguration).GetCurrentClassLogger();
                return logger;
            }
            catch (NLog.NLogConfigurationException)
            {
                return null;
            }
        }

        [SuppressMessage("Microsoft.Reliability", "CA2000:Dispose objects before losing scope", Justification = "targetLogConsole is added to loggingConfiguration")]
        private static void SetupConsoleLogging(LoggingConfiguration loggingConfiguration)
        {
            ColoredConsoleTarget targetLogConsole = new ColoredConsoleTarget(_consoleTargetName)
            {
                Layout = @"${date:format=HH\:mm\:ss} ${message} ${exception}"
            };
            loggingConfiguration.AddTarget(targetLogConsole);

            var highlightRule = new ConsoleRowHighlightingRule();
            highlightRule.Condition = NLog.Conditions.ConditionParser.ParseExpression("level == LogLevel.Info");
            highlightRule.ForegroundColor = ConsoleOutputColor.Cyan;
            targetLogConsole.RowHighlightingRules.Add(highlightRule);

            var highlightRule2 = new ConsoleRowHighlightingRule();
            highlightRule2.Condition = NLog.Conditions.ConditionParser.ParseExpression("level == LogLevel.Debug");
            highlightRule2.ForegroundColor = ConsoleOutputColor.Gray;
            targetLogConsole.RowHighlightingRules.Add(highlightRule2);

            var highlightRule3 = new ConsoleRowHighlightingRule();
            highlightRule3.Condition = NLog.Conditions.ConditionParser.ParseExpression("level == LogLevel.Trace");
            highlightRule3.ForegroundColor = ConsoleOutputColor.White;
            targetLogConsole.RowHighlightingRules.Add(highlightRule3);

            loggingConfiguration.AddRule(NLog.LogLevel.Info, NLog.LogLevel.Fatal, targetLogConsole);
        }


        public bool IsTraceEnabled
        {
            get
            {
                return _logger.IsTraceEnabled;
            }
        }
        public bool IsDebugEnabled
        {
            get
            {
                return _logger.IsDebugEnabled;
            }
        }
        public bool IsInfoEnabled
        {
            get
            {
                return _logger.IsInfoEnabled;
            }
        }
        public bool IsWarnEnabled
        {
            get
            {
                return _logger.IsWarnEnabled;
            }
        }
        public bool IsErrorEnabled
        {
            get
            {
                return _logger.IsErrorEnabled;
            }
        }
        public bool IsFatalEnabled
        {
            get
            {
                return _logger.IsFatalEnabled;
            }
        }

        public LoggerResult ChangeLogLevels(Dictionary<string, bool> levelsToChange)
        {
            LoggerResult result = LoggerResult.Ok;
            bool atLeastOneOk = false;
            bool atLeastOneError = false;
            if (levelsToChange == null)
            {
                return result;
            }

            foreach (var keyValue in levelsToChange)
            {
                LoggerResult partialResult = ChangeLogLevel(keyValue.Key, keyValue.Value);

                if (partialResult.Equals(LoggerResult.Ok))
                {
                    atLeastOneOk = true;
                }
                else
                {
                    atLeastOneError = true;
                }

                if ((int)partialResult > (int)result)
                {
                    result = partialResult;
                }
            }

            if (atLeastOneOk && atLeastOneError)
            {
                return LoggerResult.MultiStatus;
            }
            return result;
        }

        public LoggerResult ChangeLogLevel(string levelToChange, bool value)
        {
            bool changeAccepted = false;

            NLog.LogLevel logLevel = NLog.LogLevel.Off;
            try
            {
                logLevel = NLog.LogLevel.FromString(levelToChange);
            }
            catch (System.ArgumentException)
            {
                return LoggerResult.Error;
            }

            bool changeLogged = false;
            foreach (var loggingRule in NLog.LogManager.Configuration.LoggingRules)
            {
                if (value)
                {
                    loggingRule.EnableLoggingForLevel(logLevel);
                    if (!changeLogged)
                    {
                        _logger.Info("Accepted enabling of logging for level " + logLevel.Name);
                        changeLogged = true;
                        changeAccepted = true;
                    }
                }
                else
                {
                    if (logLevel < NLog.LogLevel.Info) // Do not allow disabling Info, Warn, Error and Fatal
                    {
                        loggingRule.DisableLoggingForLevel(logLevel);
                        if (!changeLogged)
                        {
                            _logger.Warn("Accepted disabling of logging for level " + logLevel.Name);
                            changeLogged = true;
                            changeAccepted = true;
                        }
                    }
                    else
                    {
                        if (!changeLogged)
                        {
                            _logger.Error("Rejected disabling of logging for level " + logLevel.Name);
                            changeLogged = true;
                        }
                    }
                }
            }

            if (changeAccepted)
            {
                NLog.LogManager.ReconfigExistingLoggers();
            }
            return changeAccepted ? LoggerResult.Ok : LoggerResult.Denied;
        }

        public byte[] RetrieveFromTime(DateTime logFileTime)
        {
            var fileTarget = (NLog.Targets.FileTarget)NLog.LogManager.Configuration.FindTargetByName(_logFileTargetName);
            var logEventInfo = new NLog.LogEventInfo { TimeStamp = logFileTime };
            string fileName = fileTarget.FileName.Render(logEventInfo);
            byte[] fileBytes = System.IO.File.ReadAllBytes(fileName);
            _logger.Info("Retrieved log file " + fileName);

            return fileBytes;
        }

        public void LogDebug(Guid traceId, string msg)
        {
            _logger.Debug("{0} {1}", traceId, msg);
        }

        public void LogTrace(Guid traceId, string msg)
        {
            _logger.Trace("{0} {1}", traceId, msg);
        }

        public void LogInfo(Guid traceId, string msg)
        {
            _logger.Info("{0} {1}", traceId, msg);
        }

        public void LogError(Guid traceId, string msg)
        {
            _logger.Error("{0} {1}", traceId, msg);
        }

        public void LogFatal(Guid traceId, string msg)
        {
            _logger.Fatal("{0} {1}", traceId, msg);
        }

        public void LogTrace(string msg)
        {
            LogTrace(Guid.Empty, msg);
        }

        public void LogInfo(string msg)
        {
            LogInfo(Guid.Empty, msg);
        }

        public void LogError(string msg)
        {
            LogError(Guid.Empty, msg);
        }

        public void LogDebug(string msg)
        {
            LogDebug(Guid.Empty, msg);
        }
    }
}