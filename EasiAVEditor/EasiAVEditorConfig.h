#pragma once
#include <string>

//consumer related settings
const unsigned int FRAMET_RATE = 25;
const std::string VCODEC = " vcodec = mpeg4";
const std::string ACODEC = " acodec = aac";
const std::string VB = " vb = 4M";
const std::string AR = " ar = 48000";
const std::string AB = " ab = 192k";
const std::string AC = " ac = 2";

//json key
const static std::string JSON_WIDTH = "width";
const static std::string JSON_HEIGHT = "height";
const static std::string JSON_FRAME_RATE = "farame_rate";
const static std::string JSON_TARGET_PATH = "target";

