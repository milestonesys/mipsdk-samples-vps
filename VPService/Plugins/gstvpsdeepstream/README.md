# GStreamer pipeline example vpsdeepstream

1. [Overview](#overview)
2. [VMS and VPS](#vms)
3. [DeepStream](#deepstream)
4. [Example GStreamer plugins vpsdeepstream and vpsnvdstoonvif from Milestone](#examples)


## Overview

This plugin demonstrates how to use Nvidia's DeepStream with the VPS. The plugin vpsdeepstream basically wraps a pipeline of DeepStream elements into a bin. In the end of this pipeline is the element vpsnvdstoonvif in which the bounding boxes produces by the DeepStream elements will be converted into Onvif format and passed along as a vpsonvifmeta. This makes it understandable by XProtect, so that the bounding boxes can be shown in XProtect SmartClient.
The sample plugin uses the configuration files and models provided when installing DeepStream 4.0.

The *vpsdeepstream* GStreamer pipeline is delivered as source code in two directories: gstvpsdeepstream and gstvpsnvdstoonvif.
Each of these directories contain one .cpp file, one .h file and a Makefile.
In order to get the pipeline up running, apart from the above source code, we in Milestone used the following 

* Ubuntu 18.04
* Nvidia Tesla P4 card with the driver version 440.33.01
* CUDA version 10.2
* DeepStream 4.0
* GStreamer 1.14.5
* The VpsSamples directory from Milestone MIP SDK 2020 R1 copied to Ubuntu
* XProtect VMS 2020 R1 installed on Windows.


## VMS and VPS

You will need to have XProtect VMS 2020 R1 or later installed, including its MIP SDK. Earlier versions will not work.
You will need to follow the instructions in the MIP SDK VpsSamples/Docs/README.md.
When that is done you should be able to run the simplistic vpsboundingboxes GStreamer pipeline in your VPService
and see some random bounding boxes in the XProtect Smart Client.


## DeepStream

We installed DeepStream for Tesla from: https://developer.nvidia.com/deepstream-download. This will install it to /opt/nvidia/deepstream/deepstream-4.0. 
DeepStream comes with a number of sample applications along with configuration files and models. Make sure you can run one of the sample applications before attempting to run the VPS sample.


## Example GStreamer plugins vpsdeepstream and vpsnvdstoonvif from Milestone

This example builds two GStreamer plugins and it exists as two directories with the names above.
vpsdeepstream is the pipeline which is exposed to VPS. It first uses the vpsfromxprotectconverter to remove XProtect headers from the video frames, so that they can be understood by the Nvidia decoder. Then comes a number of Nvidia plugins linked together, most importantly the nvv4l2decoder which will decode the image using the Nvidia GPU, the nvinfer element which will do the inference of objects based on a model provided by the DeepStream installation, and finally a nvtracker element that will track the objects detected by the nvinfer element. In the end of the pipeline is a vpsnvdstoonvif element, which will convert the bounding boxes delivered by DeepStream into ONVIF format, so that it can be understood by XProtect.

Each of the two plugins is built in exactly the same way as the official examples provided in the VpsSamples/Plugins directory from Milestone MIP SDK.
We therefore encourage you to copy the gstvpsdeepstream and gstvpsnvdstoonvif directories to VpsSamples/VPService/Plugins.

Please note that in the vpsdeepstream.cpp and in the makefiles for both of the plugins, you will have to adjust the file paths in case your DeepStream installation is in a different location.

Finally, to build the plugins, you must add gstvpsdeepstream and gstvpsnvdstoonvif to the *DIRS=* line in VpsSamples/VPService/Plugins/Makefile. Then, type *make* and *make run*

In XProtect Management Client, create a VPS camera with the URL set to http://vps_server_address:5000/gstreamer/pipelines/vpsdeepstream,
and it will process the video from the source camera you assign to it. VPS will return to XProtect bounding boxes for the objects it detects.
You will automatically be able to see these bounding boxes in the Smart Client, while they will never appear in the Management Client's video view.

