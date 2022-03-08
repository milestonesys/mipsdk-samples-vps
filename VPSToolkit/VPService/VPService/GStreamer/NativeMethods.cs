using System;
using System.Diagnostics.CodeAnalysis;
using System.Runtime.InteropServices;

namespace VideoOS.Vps.VPService.GStreamer
{
    public enum DataType
    {
        Unknown = 0,
        MediaData = 1,
        MetaData = 2
    }
    /// <summary>
    /// This class wraps the interface between VPS and a GStreamer plugin delivered by Milestone.
    /// That plugin in turn activates the actual processing GStreamer plugin.
    /// </summary>
    public static class NativeMethods
    {
        // The extension .dll is chosen because that can work on both Windows and Linux, which .so can't.
        private const string vps2gstreamer = "vps2gstreamer.dll";

        [SuppressMessage("Microsoft.Globalization", "CA2101:SpecifyMarshalingForPInvokeStringArguments", Justification = "It does not work without MarshalAs(UnmanagedType.LPUTF8Str), which FxCop does not approve of.")]
        [DllImport(vps2gstreamer, CharSet = CharSet.Unicode)]
        internal extern static IntPtr CreateGStreamer([MarshalAs(UnmanagedType.LPUTF8Str)] string pipelineName);

        [DllImport(vps2gstreamer)]
        internal extern static void DeleteGStreamer(IntPtr instance);

        [SuppressMessage("Microsoft.Globalization", "CA2101:SpecifyMarshalingForPInvokeStringArguments", Justification = "It does not work without MarshalAs(UnmanagedType.LPUTF8Str), which FxCop does not approve of.")]
        [DllImport(vps2gstreamer, CharSet = CharSet.Unicode)]
        internal extern static void SetGStreamerProperties(IntPtr instance, [MarshalAs(UnmanagedType.LPUTF8Str)] string serviceParameters);

        [DllImport(vps2gstreamer)]
        internal extern static void PutData(IntPtr instance, IntPtr dataPtr, int dataSize);

        [DllImport(vps2gstreamer)]
        internal extern static IntPtr GetData(IntPtr instance);

        [DllImport(vps2gstreamer)]
        internal extern static DataType GetDataType(IntPtr data);

        [DllImport(vps2gstreamer)]
        internal extern static void DeleteMemory(IntPtr ptr);

        [DllImport(vps2gstreamer)]
        internal extern static IntPtr MediaData_GetDataPointer(IntPtr data);

        [DllImport(vps2gstreamer)]
        internal extern static int MediaData_GetDataSize(IntPtr data);

        [DllImport(vps2gstreamer)]
        internal extern static IntPtr MetaData_GetDataPointer(IntPtr data);

        [DllImport(vps2gstreamer)]
        internal extern static int MetaData_GetDataSize(IntPtr data);

        [DllImport(vps2gstreamer)]
        internal extern static UInt64 MetaData_GetTimestamp(IntPtr data);
    }
}