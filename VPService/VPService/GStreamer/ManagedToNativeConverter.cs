using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Threading.Tasks;
using VideoOS.Vps.VpsCommon;

namespace VideoOS.Vps.VPService.GStreamer
{
    public static class ManagedToNativeConverter
    {
        public static MessageMedia ConvertMedia(IntPtr outData)
        {
            MessageMedia media = null;
            IntPtr rawData = NativeMethods.MediaData_GetDataPointer(outData);
            int dataSize = NativeMethods.MediaData_GetDataSize(outData);
            if (dataSize > 0 && rawData != IntPtr.Zero)
            {
                byte[] payload = new byte[dataSize];
                Marshal.Copy(rawData, payload, 0, dataSize);
                media = new MessageMedia(payload);
            }
            NativeMethods.DeleteMemory(outData);
            return media;
        }
        public static MessageMetadata ConvertMetadata(IntPtr outData)
        {
            MessageMetadata metadata = null;
            IntPtr rawData = NativeMethods.MetaData_GetDataPointer(outData);
            int dataSize = NativeMethods.MetaData_GetDataSize(outData);
            UInt64 timestamp = NativeMethods.MetaData_GetTimestamp(outData);
            if (dataSize > 0 && rawData != IntPtr.Zero)
            {
                byte[] payload = new byte[dataSize];
                Marshal.Copy(rawData, payload, 0, dataSize);
                metadata = new MessageMetadata(timestamp, payload);
            }
            NativeMethods.DeleteMemory(outData);
            return metadata;
        }
    }
}
