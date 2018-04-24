#pragma once
#include <string>

#define GEN_XML       1

#define sub = 0.0000001f;

//volume gain value 
const static double GAIN_MIN = 0.00316228;
const static double GAIN_MAX = 5.8489;

//consumer related settings
const std::string VIDEO_FRAMERATE = " r=";
const std::string VIDEO_CODEC = " vcodec=libx264";
const std::string AUDIO_CODEC = " acodec=aac";
const std::string VIDEO_BITRATE = " vb=4M";
const std::string AUDIO_SAMPLE_RATE= " ar=";
const std::string AUDIO_BITRATE = " ab=192k";
const std::string AUDIO_CHANNEL = " ac=2";
const std::string VIDEO_WIDTH = " width=";
const std::string VIDEO_HEIGHT = " height=";
const std::string REALTIME = " real_time=";
const std::string PROGRESSIVE = " progressive=1";

//track json key
const static std::string RESOURCE_PATH = "path";
const static std::string ABSOLUTE_STARTPOSITION = "absoluteStartPosition";
const static std::string ABSOLUTE_EDNPOSITION = "absoluteEndPosition";
const static std::string CLIPDURATION = "clipDuration";
const static std::string CROP_STARTPOSITION = "cropStartPosition";
const static std::string CROP_ENDPOSITION = "cropEndPosition";
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
const static std::string GEOMETRY_X = "xRatio";
const static std::string GEOMETRY_Y = "yRatio";
const static std::string GEOMETRY_WIDTH = "widthRatio";
const static std::string GEOMETRY_HEIGHT = "heightRatio";

//global key
const static std::string TARGET_PATH = "path";
const static std::string RESOLUTION = "resolution";
const static std::string TOTAL_DURATION = "totalDuration";
const static std::string FRAME_RATE = "frameRate";
const static std::string SAMPLERATE = "sampleRate";

//msg type 
enum MsgType
{
    SUCCESS,
    CREATE_MELT_FAILED,
    FAILED,
};

