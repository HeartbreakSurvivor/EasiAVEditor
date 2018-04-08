#pragma once
#include <string>

//volume gain value 
const static double GAIN_MIN = 0.00316228f;
const static double GAIN_MAX = 15.8489f;

//consumer related settings
const unsigned int FRAMET_RATE = 25;
const std::string VIDEO_CODEC = " vcodec = mpeg4";
const std::string AUDIO_CODEC = " acodec = aac";
const std::string VIDEO_BITRATE = " vb = 4M";
const std::string AUDIO_SAMPLE_RATE= " ar = 48000";
const std::string AUDIO_BITRATE = " ab = 192k";
const std::string AUDIO_CHANNEL = " ac = 2";

//track json key
const static std::string RESOURCE_PATH = "path";
const static std::string ABSOLUTE_STARTPOSITION = "absoluteStartPosition";
const static std::string ABSOLUTE_EDNPOSITION = "absoluteEndPosition";
const static std::string CLIPDURATION = "clipDuration";
const static std::string CROP_STARTPOSITION = "cropStartDuration";
const static std::string CROP_ENDPOSITION = "cropEndDuration";
const static std::string VIDEOFADEIN_STARTPOSITION = "videoFadeInStartPosition";
const static std::string VIDEOFADEIN_DURATION= "videoFadeInDuration";
const static std::string VIDEOFADEOUT_STARTPOSITION = "videoFadeOutStartPosition";
const static std::string VIDEOFADEOUT_DURATION = "videoFadeOutDuration";
const static std::string AUDIOFADEIN_STARTPOSITION = "audioFadeInStartPosition";
const static std::string AUDIOFADEIN_DURATION = "audioFadeInDuration";
const static std::string AUDIOFADEOUT_STARTPOSITION = "audioFadeOutStartPosition";
const static std::string AUDIOFADEOUT_DURATION = "audioFadeOutDuration";
const static std::string VIDEOGEOMETRY = "videoGeometry";
const static std::string SPEEDRATIO = "speedRatio";
const static std::string VOLUME = "volume";

//audio list json key
const static std::string ISAUDIOSOURCE = "isAudioSource";

//zoom list json key
const static std::string SCALEGEOMETRY = "scaleGeometry";

//global key
const static std::string TARGET_PATH = "path";
const static std::string RESOLUTION = "resolution";
const static std::string TOTAL_DURATION = "totalDuration";
const static std::string FRAME_RATE = "framerate";
const static std::string SAMPLERATE = "sampleRate";
