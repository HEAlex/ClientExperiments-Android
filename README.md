##ClientExperiments-Android
This repository includes experiments in recording, encoding and processing audio and video for streaming to the OpenWatch MediaCapture server.

###Capabilities

+ Audio and Video recording with `MediaRecorder` (HW Encoding) and `ffmpeg` (SW Encoding)
  + `net.openwatch.openwatch2.video.VideoHardwareRecorder`
  + `net.openwatch.openwatch2.video.VideoSoftwareRecorder`
+ Dual Video Recording (SW Encoding)
  + `net.openwatch.openwatch2.video.DualVideoRecorder`
      + Encodes a 640x480 and 320x240 video stream independently
