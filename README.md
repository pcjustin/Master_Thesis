# Master_Thesis
Adaptive Transcoding HTTP Live Streaming System

The project was my master's thesis. The project is `Open Source` for sharing if it can help you improve something.

## Introduction
The project based on `ffmpeg` and `HTTP Live Streaming` (aka. `HLS`). The `ffmpeg` is a transcoding tool that use for transcoding video and audio to specified codecs. `HLS` developed from Apple that is popular streaming technique.

This project is ruuning on Safari and Chrome and all iOS/Mac systems.

## Requirements
```
$ sudo apt-get install ffmpeg
```

## How to run it
```
$ ffmpeg -i $1 -f mpegts -vcodec libx264 -acodec libmp3lame -async 1 -threads 16 $ip.ts
$ segmenter -i $ip.ts -d 10 -o $ip -x $ip.m3u8 --strict-segment-duration
```
