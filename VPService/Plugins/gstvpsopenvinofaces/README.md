# GStreamer pipeline example vpsopenvinofaces

1. [Overview](#overview)
2. [VMS and VPS](#vms)
3. [OpenVINO](#openvino)
4. [Sample GStreamer plugins from OpenVINO](#samples)
5. [Example GStreamer plugins vpsopenvinofaces and vpsmetafromroi from Milestone](#examples)


## Overview

The *vpsopenvinofaces* GStreamer pipeline allows you from the Milestone VPService to execute the same as the face recognition sample *face_detection_and_classification*
delivered by OpenVINO. The sample and the GStreamer API to the OpenVINO inference engine is delivered was downloaded from https://github.com/opencv/gst-video-analytics.

The *vpsopenvinofaces* GStreamer pipeline is delivered as source code in two directories: gstvpsopenvinofaces and gstvpsmetafromroi.
Each of these directories contain one .cpp file, one .h file and a Makefile.
In order to get the pipeline up running, apart from the above source code, we in Milestone used the following 

* Ubuntu 18.04 on a 64 bit Intel compatible computer for the VPS (16.04 will not work)
* Windows 10 on a 64 bit Intel compatible computer for the VMS.
* OpenVINO installed on Ubuntu
* The samples from https://github.com/opencv/gst-video-analytics cloned to Ubuntu and compiled
* The VpsSamples directory from Milestone MIP SDK 2020 R1 copied to Ubuntu
* GStreamer 1.14.5 and opencv-dev installed on Ubuntu
* XProtect VMS 2020 R1 installed on Windows.


## VMS and VPS

You will need to have XProtect VMS 2020 R1 or later installed, including its MIP SDK. Earlier versions will not work.
You will need to follow the instructions in the MIP SDK VpsSamples/Docs/README.md.
When that is done you should be able to run the simplistic vpsboundingboxes GStreamer pipeline in your VPService
and see some random bounding boxes in the XProtect Smart Client.


## OpenVINO

We installed OpenVINO for Linux from https://docs.openvinotoolkit.org/latest/_docs_install_guides_installing_openvino_linux.html.
The engine will install to /opt/intel/openvino.
Be aware that it is possible to install without downloading all the actual AI models.
There is a directory named downloads, from where you can download data for selected or all models.
Downloading all models takes quite a while.


## Sample GStreamer plugins from OpenVINO 

In the sample you cloned from GitHub, you must build its GStreamer plugins. Let's say you downloaded to a directory named a.
To build the sample, you must use cmake, so you may create an empty directory aside a, let's call it b.
Now, in a shell, go to b, then type *cmake ../a*. That will check that you have all dependencies installed, and if yes, create a Makefile.
When the Makefile is created, type make, and the plugins will be built.
Lastly, you copy the GStreamer plugins built above to VpsSamples/VPService/bin.


## Example GStreamer plugins vpsopenvinofaces and vpsmetafromroi from Milestone

This example builds two GStreamer plugins and it exists as two directories with the names above.
vpsopenvinofaces is the pipeline which is exposed to VPS. It invokes the video analytics plugins built from the gst-video-analytics GitHub download,
and passes its output to the vpsmetafromroi plugin, which converts the bounding boxes to ONVIF XML understood by XProtect.
It also strips the video. We don't want to return that to XProtect, where it came from.

Each of the two plugins is built in exactly the same way as the official examples provided in the VpsSamples/Plugins directory from Milestone MIP SDK.
We therefore encourage you to copy the gstvpsopenvinofaces and gstvpsmetafromroi directories to VpsSamples/VPService/Plugins.
Please note that in the vpsopenvinofaces.cpp, you will have to adjust two file paths, to reflect where you downloaded the OpenVINO GutHub sample to.
You must now add gstvpsopenvinofaces and gstvpsmetafromroi to the *DIRS=* line in VpsSamples/VPService/Plugins/Makefile. Then, type *make* and *make run*

In XProtect Management Client, create a VPS camera with the URL set to http://vps_server_address:5000/gstreamer/pipelines/vpsopenvinofaces,
and it will process the video from the source camera you assign to it. VPS will return to XProtect bounding boxes for the faces it detects.
You will automatically be able to see these bounding boxes in the Smart Client, while they will never appear in the Management Client's video view.

