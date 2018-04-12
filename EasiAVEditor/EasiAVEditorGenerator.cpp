#include "EasiAVEditorGenerator.h"
#include <utils\CharsetUtils.h>
#include <time.h>
#include <iostream>

using namespace std;

EasiAVEditorGenerator::EasiAVEditorGenerator()
{
}

EasiAVEditorGenerator::EasiAVEditorGenerator(Json::Value jsonVideolist, Json::Value jsonAudiolist, Json::Value jsonZoomlist, Json::Value jsonGlobalinfo)
    : _jsonvideolist(jsonVideolist)
    , _jsonaudiolist(jsonAudiolist)
    , _jsonzoomlist(jsonZoomlist)
    , _jsonglobalinfo(jsonGlobalinfo)
    , _videotracks(0)
    , _audiotracks(0)
    , _tracks(0)
    , _framerate(25)
    , _logger()
{
    _pMeltService = std::make_unique<MeltService>();
    //GLINFO << _jsonvideolist;
}

EasiAVEditorGenerator::~EasiAVEditorGenerator()
{
    _pMeltService.release();
    _logger.~LoggerGuard();
}

void EasiAVEditorGenerator::setProgressReportcb(progresscbfun func)
{
    _pMeltService->ProgressReport_cbfun(func);
}

void EasiAVEditorGenerator::setMsgReportcb(msgcbfun func)
{
    _pMeltService->MsgReport_cbfun(func);
}

bool EasiAVEditorGenerator::start()
{
    if (!generate_parameters()) return false;
    //return _pMeltService->AsyncStartmelt(_MeltParameters);
    //return _pMeltService->AsyncStartmelt(" D:\\Code\\EasiAVEditor\\Debug\\videos\\big_buck_1_min.mp4 in=\"00:00:10.000\" out=\"00:00:40.000\" -progress -consumer avformat:D:\\Code\\EasiAVEditor\\Debug\\videos\\slow.mp4");
    return _pMeltService->Startmelt(" D:\\Code\\EasiAVEditor\\Debug\\videos\\big_buck_1_min.mp4 in=\"00:00:10.000\" out=\"00:00:40.000\" -progress -consumer avformat:D:\\Code\\EasiAVEditor\\Debug\\videos\\slow.mp4");
}

void EasiAVEditorGenerator::stop()
{
    _pMeltService->Stopmelt();
}

float EasiAVEditorGenerator::timeStr2second(const std::string & timestr)
{
    float total_seconds;
    int first_colon = timestr.find(':');
    int second_colon = timestr.find(':', first_colon + 1);
    int hour = std::stoi(timestr.substr(0, first_colon));
    int minute = std::stoi(timestr.substr(first_colon + 1, second_colon));
    float second = std::stof(timestr.substr(second_colon + 1, timestr.size()));

    total_seconds = hour * 3600 + minute * 60 + second;
    return total_seconds;
}

std::string EasiAVEditorGenerator::second2timeStr(float seconds)
{
    std::string str;
    char buf[10];
    unsigned int millisecond = (unsigned int)(seconds * 1000);
    unsigned int hour = seconds / 3600000;
    unsigned int minute = millisecond / 60000 % 60000;
    //int second = (millisecond % 60000) / 1000;
    float second = (float)(millisecond % 60000) / 1000;

    std::cout << "hour: " << hour << "  minute: " << minute << "  second: " << second << std::endl;
    sprintf_s(buf, sizeof(buf), "%02d", hour);
    str.append(buf);
    str.append(":");
    sprintf_s(buf, sizeof(buf), "%02d", minute);
    str.append(buf);
    str.append(":");
    sprintf_s(buf, sizeof(buf), "%.3f", second);
    str.append(buf);
    return str;
}

void EasiAVEditorGenerator::attach_video_fadein_filter(const std::string & in, const std::string & out)
{
    //std::string str(" -attach-clip brightness level=\"00:00:00.000=0; 00:00:00.000=1\" alpha=\"00:00:00.000=0; 00:00:00.000=1\"");
    std::string str(" -attach-clip brightness level=\"");
    str.append(in);
    str.append("=0; ");
    str.append(out);
    str.append("=1\" alpha=\"");
    str.append(in);
    str.append("=0; ");
    str.append(out);
    str.append("=1\"");
}

void EasiAVEditorGenerator::attach_video_fadeout_filter(const std::string & in, const std::string & out)
{
    //std::string str(" -attach-clip brightness level=\"00:00:00.000=1; 00:00:00.000=0\" alpha=\"00:00:00.000=1; 00:00:00.000=0\"");
    std::string str(" -attach-clip brightness level=\"");
    str.append(in);
    str.append("=1; ");
    str.append(out);
    str.append("=0\" alpha=\"");
    str.append(in);
    str.append("=1; ");
    str.append(out);
    str.append("=0\"");
}

std::string EasiAVEditorGenerator::attach_volume_fadein_filter(const std::string & in, const std::string & out)
{
    //std::string str(" -attach-clip volume gain=0 end=1 in=\"00:00:00.000\" out=\"00:00:00.000\"");
    std::string str(" -attach-clip volume gain=0 end=1 in=\"");
    str.append(in);
    str.append("\" out=\"");
    str.append(out);
    str.append("\"");
    return str;
}

std::string EasiAVEditorGenerator::attach_volume_fadeout_filter(const std::string & in, const std::string & out)
{
    //std::string str(" -attach-clip volume gain=1 end=0 in=\"00:00:00.000\" out=\"00:00:00.000\"");
    std::string str(" -attach-clip volume gain=1 end=0 in=\"");
    str.append(in);
    str.append("\" out=\"");
    str.append(out);
    str.append("\"");
    return str;
}

std::string EasiAVEditorGenerator::attach_volume_gain_filter(float percentage)
{
    double range1 = GAIN_MAX - 1;
    double range2 = 1 - GAIN_MIN;
    double gain = 1.0f;
    if (percentage < 1.0f) {
        gain -= range2*(1.0 - percentage);
    }
    else if (percentage > 1.0f) {
        gain += range1 *(percentage - 1.0);
    }
    std::string str(" -filter volume gain=1 ");
    str.replace(21, 1, std::to_string(gain));
    return str;
}

void EasiAVEditorGenerator::attach_video_scale_filter(float x, float y, float width, float height)
{
    std::string str(" -attach-clip affine background=colour:0 transition.geometry=\"00.00%/00.00%:00.00%x00.00%\"");


}

void EasiAVEditorGenerator::attach_timewrarp_filter(uint16_t speedratio)
{

}

void EasiAVEditorGenerator::generate_video_tracks()
{
}

void EasiAVEditorGenerator::generate_audio_tracks()
{
}

void EasiAVEditorGenerator::generate_video_multitrack()
{
    std::string videotrack;

    std::cout << "videotrack's size: " << _jsonvideolist.size();
    std::cout << "videotrack is array: " << _jsonvideolist.isArray();

    Json::Value::Members mem = _jsonvideolist.getMemberNames();
    for (auto iter = mem.begin(); iter != mem.end(); iter++) {
        std::cout << *iter << "\t: ";
    }
    std::cout << std::endl;
    _MeltParameters.append(videotrack);
}

void EasiAVEditorGenerator::generate_audio_multitrack()
{
    if (_jsonaudiolist.empty() || !_jsonaudiolist.isArray()) {
        GLINFO << "audio json list is empty ";
        return;
    }
    std::cout << "audiotrack's size: " << _jsonaudiolist.size();
    std::cout << "audiotrack is array: " << _jsonaudiolist.isArray();

    std::string audioMultitrack(" ");//audiotrack temp parameters

    for (int i = 0; i< _jsonaudiolist.size(); i++) {
        std::string path = CharsetUtils::ANSIStringToUTF8String(_jsonaudiolist[i][TARGET_PATH].asString());
        std::string startposition = _jsonaudiolist[i][ABSOLUTE_STARTPOSITION].asString();
        std::string endposition = _jsonaudiolist[i][ABSOLUTE_EDNPOSITION].asString();
        std::string clipduration = _jsonaudiolist[i][CLIPDURATION].asString();
        std::string cropStartPosition = _jsonaudiolist[i][CROP_STARTPOSITION].asString();
        std::string cropEndPosition = _jsonaudiolist[i][CROP_ENDPOSITION].asString();
        std::string fadeInDuration = _jsonaudiolist[i][AUDIOFADEIN_DURATION].asString();
        std::string fadeOutDuration = _jsonaudiolist[i][AUDIOFADEOUT_DURATION].asString();
        float speedRatio = _jsonaudiolist[i][SPEEDRATIO].asFloat();
        float volume = _jsonaudiolist[i][VOLUME].asFloat();
        bool isAudioResource = _jsonaudiolist[i][ISAUDIOSOURCE].asBool();

        if (speedRatio != 1.0f) {//generate temporary audio file to speed up or slow down.

        }
        if (!isAudioResource) { //audio separate from video.
            audioMultitrack.append(" -hide-video ");
        }
        if (timeStr2second(startposition) != 0.0f) {//the audio clip's startint point is not from zero, so add blank clip
            audioMultitrack.append(" -blank out=\"");
            audioMultitrack.append(startposition);
            audioMultitrack.append("\" ");
        }
        audioMultitrack.append("\"");//append the resource file's path 
        audioMultitrack.append(path);//append the resource file's path 
        audioMultitrack.append("\"");//append the resource file's path 
        audioMultitrack.append(" in=\"");
        audioMultitrack.append(cropStartPosition);//append the audio clip's start point related to the starting point of resource file. 
        audioMultitrack.append("\" out=\"");
        audioMultitrack.append(cropEndPosition);//append the audio file's 
        audioMultitrack.append("\"");

        if (timeStr2second(fadeInDuration) != 0.0f) {
            audioMultitrack.append(attach_volume_fadein_filter("00:00:00.000", fadeInDuration));
        }
        if (timeStr2second(fadeOutDuration) != 0.0f) {
            std::string fadeoutstart = second2timeStr(timeStr2second(clipduration) - timeStr2second(fadeOutDuration));
            audioMultitrack.append(attach_volume_fadeout_filter(fadeoutstart, clipduration));
        }
        if (volume != 1.0f) {
            audioMultitrack.append(attach_volume_gain_filter(volume));
        }
        if (i + 1 < _jsonaudiolist.size() && _jsonaudiolist[i + 1][ISAUDIOSOURCE].asBool()) {
            audioMultitrack.append(" -track ");
        }
        _audiotracks++;//increase the audio tracks number.
    }

    GLINFO << "audio multitrack's parameters: " << audioMultitrack;
    _MeltParameters.append(audioMultitrack);
}

void EasiAVEditorGenerator::generate_transitions()
{
    for (int i = 1; i < _tracks; ++i) {
        add_audio_mix_transition(0, i);
        if (i < _videotracks) {
            add_video_overlay_transition(0, i);
        }
    }
    _MeltParameters.append(_transitionPara);
}

void EasiAVEditorGenerator::add_audio_mix_transition(uint16_t a_track, uint16_t b_track)
{
    std::string str(" -transition mix:-1 always_active=1 a_track=0 b_track=1 sum=1");
    str.replace(44, 1, std::to_string(a_track)); 
    str.replace(54, 1, std::to_string(b_track));
    _transitionPara.append(str);
}

void EasiAVEditorGenerator::add_video_overlay_transition(uint16_t a_track, uint16_t b_track)
{
    std::string str(" -transition frei0r.cairoblend a_track=0 b_track=1 disable=0");
    str.replace(49, 1, std::to_string(a_track));
    str.replace(39, 1, std::to_string(b_track));
    _transitionPara.append(str);
}

std::string EasiAVEditorGenerator::add_geometry(std::string x_ratio, std::string y_ratio, std::string width_ratio, std::string height_ratio)
{
    float x = std::stof(x_ratio) * 100;
    float y = std::stof(y_ratio) * 100;
    float width = std::stof(width_ratio) * 100;
    float height = std::stof(height_ratio) * 100;

    //accurate to the second decimal place.
    auto f = [&](std::string str) {
        int decimalpoint = str.find('.');
        if (decimalpoint == std::string::npos)
            return str;
        return str.substr(0, (decimalpoint + 3) > str.length() ? str.length() : (decimalpoint + 3));
    };

    std::string geometry("");
    geometry.append(f(std::to_string(x)));
    geometry.append("%/");
    geometry.append(f(std::to_string(y)));
    geometry.append("%:");
    geometry.append(f(std::to_string(width)));
    geometry.append("%x");
    geometry.append(f(std::to_string(height)));
    geometry.append("%");
    return geometry;
}

void EasiAVEditorGenerator::add_zoom_animation_filter()
{
    if (_jsonzoomlist.empty()) {
        GLINFO << "zoom json list is empty ";
        return;
    }
    _MeltParameters.clear();
    _MeltParameters.append(_mltfilepath);

    std::cout << "zoomlist's size: " << _jsonzoomlist.size() << std::endl;
    std::cout << "zoomlist is array: " << _jsonzoomlist.isArray() << std::endl;

    _MeltParameters.append(" -filter affine transition.cycle=0 transition.geometry=\"");

    auto f = [this](float timestamp, std::string geometry) {
        std::string str;
        int frame = timestamp * _framerate;
        str.append(std::to_string(frame));
        str.append("=");
        str.append(geometry);
        str.append("; ");
        return str;
    };

    //add the initial zoom geometry
    float timestamp = timeStr2second("00:00:00.000");
    std::string geometry = add_geometry("0.0", "0.0", "1.0", "1.0");
    _MeltParameters.append(f(timestamp, geometry));
    
    for (int i = 0; i< _jsonzoomlist.size(); i++) {
        std::string startposition = _jsonzoomlist[i][ABSOLUTE_STARTPOSITION].asString();
        std::string endposition = _jsonzoomlist[i][ABSOLUTE_EDNPOSITION].asString();
        std::string xratio = _jsonzoomlist[i][SCALEGEOMETRY][GEOMETRY_X].asString();
        std::string yratio = _jsonzoomlist[i][SCALEGEOMETRY][GEOMETRY_Y].asString();
        std::string widthratio = _jsonzoomlist[i][SCALEGEOMETRY][GEOMETRY_WIDTH].asString();
        std::string heightratio = _jsonzoomlist[i][SCALEGEOMETRY][GEOMETRY_HEIGHT].asString();

        timestamp = timeStr2second(startposition);
        std::string tmpstr = f(timestamp, geometry);
        GLINFO << "zoom tmpstr: " << tmpstr;
        _MeltParameters.append(tmpstr);

        timestamp = timeStr2second(endposition);
        geometry = add_geometry(xratio, yratio, widthratio, heightratio);
        tmpstr = f(timestamp, geometry);
        GLINFO << "zoom tmpstr: " << tmpstr;
        _MeltParameters.append(tmpstr);
    }

    _MeltParameters.append("\"");
    std::cout << "zoom filter: " << _MeltParameters << std::endl;
    GLINFO << "zoom filter parameters: " << _MeltParameters;
}

void EasiAVEditorGenerator::generate_consumer_settings()
{
    std::wstring showpath = CharsetUtils::ANSIStringToUnicodeString(_jsonglobalinfo[TARGET_PATH].asString());
    std::string resolution = _jsonglobalinfo[RESOLUTION].asString();
    std::string width = resolution.substr(0, resolution.find('X'));
    std::string height = resolution.substr(resolution.find('X')+1,resolution.size());
    std::string framerate = _jsonglobalinfo[FRAME_RATE].asString();
    std::string samplerate = _jsonglobalinfo[SAMPLERATE].asString();

    _duration = _jsonglobalinfo[TOTAL_DURATION].asString();
    _framerate = std::stoi(framerate);
    std::cout << "gloabl info: " << resolution << " " << _duration << " " << framerate << " " << samplerate << " " << std::endl;

    _consumerPara.append(" -progress");
    _consumerPara.append(" -getc");
    _consumerPara.append(" -consumer avformat target=\"");
    _consumerPara.append(CharsetUtils::ANSIStringToUTF8String(_jsonglobalinfo[TARGET_PATH].asString()));
    _consumerPara.append("\"");
    _consumerPara.append(VIDEO_CODEC);
    _consumerPara.append(AUDIO_CODEC);
    _consumerPara.append(VIDEO_FRAMERATE);
    _consumerPara.append(framerate);
    _consumerPara.append(AUDIO_SAMPLE_RATE);
    _consumerPara.append(samplerate);
    _consumerPara.append(AUDIO_BITRATE);
    _consumerPara.append(AUDIO_CHANNEL);
    _consumerPara.append(VIDEO_WIDTH);
    _consumerPara.append(width);
    _consumerPara.append(VIDEO_WIDTH);
    _consumerPara.append(height);

    GLINFO << "consumer melt parameters: " << _consumerPara;
    _MeltParameters.append(_consumerPara);
}

bool EasiAVEditorGenerator::generate_parameters()
{
    generate_consumer_settings();

    //generate_video_multitracks();
    generate_audio_multitrack();
    //generate audio mix and video composite transitions.
    //generate_transitions();
    //generate the mlt file
    //generate_mlt_file();
    //output the melt command line to log.
    //formatting_parameters();
    add_zoom_animation_filter();
    return true;
}

void EasiAVEditorGenerator::formatting_parameters()
{

}

void EasiAVEditorGenerator::generate_mlt_file()
{
    std::string str(" -consumer xml:");
    //get current time and form the mlt file name.
    struct tm t;
    time_t timep;
    time(&timep);
    localtime_s(&t, &timep);
    char tmp[64];
    strftime(tmp, sizeof(tmp), " jjs-%Y-%m-%d-%H-%M-%S.mlt", &t);
    _mltfilepath.assign(tmp);
    str.append(_mltfilepath);
    GLINFO << "temporary mlt file's name & path: " << str;
    _MeltParameters.append(str);
    _pMeltService->Startmelt(str);
}


