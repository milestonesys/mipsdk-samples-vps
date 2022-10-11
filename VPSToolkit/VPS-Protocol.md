# The VPS Protocol

This document describes the protocol which pushes video data into a VPS service and returns video and/or metadata.
The protocol needs to support full duplex transmission over HTTP(S), so websockets is the obvious choice. 
Websockets is a standard described in many places, for example <https://en.wikipedia.org/wiki/WebSocket>.
The typical client is the VPS driver module inside an XProtect Recording Server. The typical server is the sample VPService delivered here.

The client initiates the web socket session by sending an HTTP(S) request like "GET ws://<host>:<port>/path". Host and port decide
which VPService instance is hit. wss: can be used instead of just ws:. That will enable secure websockets.
The path must be accepted by the VPService implementation. The sample VPService accepts paths beginning with /gstreamer/pipelines/,
and assumes that what comes next is the name of an installed GStreamer pipeline.
On success, the service responds with an UPGRADE command, which is echoed by the client. If the path does not match any installed GStreamer pipeline
HTTP 404 must be returned.

Next, the client sends a web socket message of type 'configuration' containing a string with the service parameters for the processing. 
When that is done, the client sequentially pushes web socket messages carrying media video frames. 
The service side may choose to return websocket metadata messages and/or websocket video frames messages.
The client must be able to receive and handle such messages.
The session is closed by shutting down the web socket from either side.

## Web socket message format

Each web socket message consists of a header and a payload. The headers are binary and contain in the following order:

- 2 bytes: Message type as 16 bit unsigned integer, network (big) endianness. 1 = configuration, 2 = media, 3 = metadata, 4 = event (introduced in version 2)
- 2 bytes: Version as 16 bit unsigned integer, network (big) endianness, it is value from 1 to 2.
- 4 bytes: Length of payload exclusive this header as 32 bit unsigned integer, network (big) endianness

The payload varies with the message type:

- Configuration: A UTF-8 encoded string, not necessarily null terminated. Such service parameters are specific to a given GStreamer pipeline.
- Media: Binary data, usually video frames. Data from XProtect has an initial “milestone generic byte data header” prepended to describe data.
- Metadata: A timestamp in milliseconds after UNIX Epoch as 64 bit unsigned integer, network (big) endianness,
  immediately followed by metadata in Onvif XML encoded as UTF-8.
- Event: Has the following structure:
    *  8 bytes TimeStamp - in milliseconds after UNIX Epoch as 64 bit unsigned integer, network (big) endianness
    *  16 bytes SourceId - GUID of the source sending the event mixed-endian format: https://en.wikipedia.org/wiki/Universally_unique_identifier#Encoding
    *  16 bytes EventId - GUID of the event being trigered in mixed-endian format: https://en.wikipedia.org/wiki/Universally_unique_identifier#Encoding
    *  2 bytes SourceNameLength - integer value containing the number of bytes for the next data (Trigger Source Name), network (big) endianness
    *  SourceNameLength number of bytes Trigger Source Name - the name of the source trigerring the event in ASCII
    *  Byte array of data - the rest of the payload is cutom data send direcly.

## The XProtect Recording Server’s Client Implementation

The VPS client, which sits in the XProtect Recording Server’s VPS driver, is delivered with XProtect in binary format only.
Its implementation is based on the default behavior of the ClientWebSocket class from .Net, currently version 4.7. When the
end user configures a “VPS camera” in XProtect, a URL is specified, which is fed into the ClientWebSocket with 1
modification only: We allow end users to type `https://` or `http://` and convert that to the `wss://` or `ws://` scheme
respectively. If the resulting scheme is `wss://`, your server will need to return a certificate which is signed by an
authority which is trusted by the computer which runs the XProtect Recording Server. There is no way you can disable
certificate handling to use only the encryption with https. If you want to experiment with VPS for the first time, go
for http/ws, at least in the beginning.

In the XProtect Management Client, you may for each VPS camera enter a string value for "Service parameters". That string
is sent as to the VPService as a websocket message of type 'configuration'. Here you can specify how the GStreamer pipeline instance
is to act. You can specify key value pairs like OBJECT=Bird,Direction=south.

The URL which you typed in when creating the camera, must be one which enables the client to connect to the VPService. 
The "Driver parameters" field located next contains this URL. It is not meant to be manually edited, though it is allowed if you misspelled the URL.

## The Behavior of the Sample Service

The VPS server-side implementation is up to you. Milestone delivers a functional sample VPService in C# source code, which you can use as-is or modify as you want.
Alternatively, you may want to re-implement it using another language or other web components than .Net.

The sample service as delivered from Milestone listens on all available networks.
It listens for http requests on port 5000 and for https requests on port 5001. 
These ports were chosen because that is the behavior of the default Microsoft server template code.
It means you must in the XProtect Management Client enter URLs like
<http://myserver:5000/gstreamer/pipelines/vpspasstru> or <https://myserver:5001/gstreamer/pipelines/myownpipeline>.
If you want to release your end user from the effort of typing a port number, you can add a “Kestrel” object in the `appsettings.json` file.
For changing https, see the section “Certificates”. For a sample to copy and paste from, look at `appsettings.Development.json`,
which is also the settings used when you press F5 in Visual Studio, where the value of `ASPNETCORE_ENVIRONMENT` is set to `Development`.

~~~~json
{
  "Logging": {
    "LogLevel": {
      "Default": "Warning"
    }
  },
  "AllowedHosts": "*",
  "Kestrel": {
    "Endpoints": {
      "HttpNoCertificate": {
        "Url": "http://0.0.0.0:80"
      },
    }
  }
}
~~~~

In the current sample VPService code, each websocket message of type 'configuration' is transferred to the GStreamer pipeline,
where it is captured and logged in `GStreamerRunner.cs`. It is split by commas into what is thought to be key-value pairs,
where key and value are separated by an equal sign. Each key value pair then causes a "set property(key, value)" on the
GStreamer pipeline.

The sample service passes received video frames to your GStreamer pipeline immediately when they are accepted.

The sample service accepts metadata and processed video returned back from your GStreamer pipeline.
If you return neither or just one of them, that is no problem. If you don’t return video, your VPS camera will not show any video, that’s all.
If you return no metadata, XProtect will collect no metadata, that is no problem either. In your GStreamer pipeline, if
you want to send metadata or video elsewhere than back to XProtect, that is all fully up to you.

If you choose to pass back video or metadata from your pipeline, the service will try return that data to the XProtect Recording Server
using the same websocket connection where it received the video. That is all fine, as long as the network and the recording server
is able to consume the amount of data you return. If not, data will be accumulated in the sample service’s AppSink module in file `vps2gstreamer.cpp`,
which owns a queue named `m_sinkQueue`. You can tune your implementation to keep such data
or clean up this queue acording to what you feel is best practise in your situation.

## Certificates

When turning to https/wss, you will need a certificate. The end user solution is to have or buy a certificate for the
computer or domain you want to run the service on, from a vendor which is trusted on the computer where you run the
recording server. Unless you are making a cloud solution, that will hardly be your preferred development situation,
because it is not for free.

If you search the Microsoft documentation, you will learn about typing `dotnet dev-certs https --trust`. On Linux as
well as on Windows, that will install and trust a certificate for running client and server on the same “localhost”.
Again, that may not be your preferred development situation, because then you need to run the service on the same
computer where you run the XProtect Recording Server. In the service project, we make available a PowerShell script
named `CreateVPServiceCertificate.ps1` which you can run on the Windows computer which runs the service. It 

- creates a CA certificate (#1) of your own, intended only for signing other certificates
- creates another certificate (#2) with `-DnsName myserver`, and signs that other certificate with the CA certificate (#1).
- instructs you how to export, copy and import the CA certificate (#1) to the certificate store on the recording server
  computer and trust it there

The same can most probably be done on Linux, but we have no script available for that. You must now be able to pick a
specific certificate (#2) from the service source code. You do that by changing the file named `appsettings.json`. The
following should work for changing https to port 443 and picking a certificate from a local store.

~~~~json
  "Kestrel": {
    "Endpoints": {
      "HttpNoCertificate": {
        "Url": "http://0.0.0.0:80"
      },
      "HttpsCertificateFromStore": {
        "Url": "https://0.0.0.0:443",
        "Certificate": {
          "Subject": "myserver",             // Change 'myserver' to fit the certificate you created or bought.
          "Store": "My",                     // This store is called 'Personal' in MMC
          "Location": "LocalMachine",        // This location is called 'Local Computer' in MMC
          "AllowInvalid": true               // True allows self-signed certificates
        }
      }
    }
  }
~~~~

If you don’t have access to a certificate store, you can use the exported pfx file instead. The password is the string
which was used when exporting the #2 certificate into the .pfx file.

~~~~json
  "Kestrel": {
    "Endpoints": {
      "HttpNoCertificate": {
        "Url": "http://0.0.0.0:80"
      },
      "HttpsCertificateFromFile": {
        "Url": "https://0.0.0.0:443",
        "Certificate": {
          "Path": "./hostname.pfx",        // Change 'hostname' to name of the pfx file you have, and which must include the private key.
          "Password": "5678"               // Consider storing this more secretly. The web is full of discussions on this subject, it is beyond our scope.
        }
      }
    }
  }
~~~~

You may encounter the following warning printed out in the VPS log: `Uncaught exception from the OnConnectionAsync
method of an IConnectionAdapter. The credentials supplied to the package were not recognized.` That indicates that the
private key is missing or not accessible. In the latter scenario, make sure that the pfx file was exported including its
private key, and that the password used to retrieve it is the same as was used when writing the file. In the first
scenario, using certificate store, try the following if you are working on Windows:

- Click the Windows start icon and type cert. Choose ‘Manage computer certificates’.
- Click ‘Personal’, ‘Certificates’ and double click the certificate (#2) you created with the `myserver` name.
- In the ‘General’ tab, make sure it says that the certificate has a private key with it.
- Right-click the certificate (#2) and select ‘All Tasks’ – ‘Manage Private Keys…’
- Add an entry to give the user account under which the VPService runs at least read access.


## Security

In the current sample code, no authorization is implemented.
