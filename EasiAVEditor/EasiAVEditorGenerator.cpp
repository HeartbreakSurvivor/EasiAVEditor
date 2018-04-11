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

float EasiAVEditorGenerator::timeformat_convert(const std::string & timestr)
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

void EasiAVEditorGenerator::attach_video_fadein_filter(const std::string & in, const std::string & out)
{
    std::string str(" -attach-clip brightness level=\"00:00:00.000=0; 00:00:00.000=1\" alpha=\"00:00:00.000=0; 00:00:00.000=1\"");
    str.replace(32, 12, in);
    str.replace(48, 12, out);
    str.replace(73, 12, in);
    str.replace(89, 12, out);
}

void EasiAVEditorGenerator::attach_video_fadeout_filter(const std::string & in, const std::string & out)
{
    std::string str(" -attach-clip brightness level=\"00:00:00.000=1; 00:00:00.000=0\" alpha=\"00:00:00.000=1; 00:00:00.000=0\"");
    str.replace(32, 12, in);
    str.replace(48, 12, out);
    str.replace(73, 12, in);
    str.replace(89, 12, out);
}

void EasiAVEditorGenerator::attach_volume_fadein_filter(const std::string & in, const std::string & out)
{
    std::string str(" -attach-clip volume gain=0 end=1 in=\"00:00:00.000\" out=\"00:00:00.000\"");
    str.replace(38, 12, in);
    str.replace(57, 12, out);
}

void EasiAVEditorGenerator::attach_volume_fadeout_filter(const std::string & in, const std::string & out)
{
    std::string str(" -attach-clip volume gain=1 end=0 in=\"00:00:00.000\" out=\"00:00:00.000\"");
    str.replace(38, 12, in);
    str.replace(57, 12, out);
}

void EasiAVEditorGenerator::attach_volume_gain_filter(float percentage)
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
    std::string str(" -filter volume gain=1");
    str.replace(21, 1, std::to_string(gain));
}

void EasiAVEditorGenerator::attach_video_scale_filter(float x, float y, float width, float height)
{
    std::string str(" -attach-clip affine background=colour:0 transition.geometry=\"00.00%/00.00%:00.00%x00.00%\"");


}

void EasiAVEditorGenerator::attach_timewrarp_filter(uint16_t speedratio)
{

}

void EasiAVEditorGenerator::attach_videoclips_filters()
{

}

void EasiAVEditorGenerator::attach_audioclips_filters()
{
}

void EasiAVEditorGenerator::generate_video_tracks()
{
}

void EasiAVEditorGenerator::generate_audio_tracks()
{
}

void EasiAVEditorGenerator::generate_video_multitracks()
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

void EasiAVEditorGenerator::generate_audio_multitracks()
{
    std::string audiotrack;
    std::cout << "audiotrack's size: " << _jsonaudiolist.size();
    std::cout << "audiotrack is array: " << _jsonaudiolist.isArray();

    Json::Value::Members mem = _jsonaudiolist.getMemberNames();
    for (auto iter = mem.begin(); iter != mem.end(); iter++) {
        std::cout << *iter << "\t: ";
    }

    _MeltParameters.append(audiotrack);
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
    if (_jsonzoomlist.empty()) return;
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
    float timestamp = timeformat_convert("00:00:00.000");
    std::string geometry = add_geometry("0.0", "0.0", "1.0", "1.0");
    _MeltParameters.append(f(timestamp, geometry));
    
    for (int i = 0; i< _jsonzoomlist.size(); i++) {
        std::string startposition = _jsonzoomlist[i][ABSOLUTE_STARTPOSITION].asString();
        std::string endposition = _jsonzoomlist[i][ABSOLUTE_EDNPOSITION].asString();
        std::string xratio = _jsonzoomlist[i][SCALEGEOMETRY][GEOMETRY_X].asString();
        std::string yratio = _jsonzoomlist[i][SCALEGEOMETRY][GEOMETRY_Y].asString();
        std::string widthratio = _jsonzoomlist[i][SCALEGEOMETRY][GEOMETRY_WIDTH].asString();
        std::string heightratio = _jsonzoomlist[i][SCALEGEOMETRY][GEOMETRY_HEIGHT].asString();

        timestamp = timeformat_convert(startposition);
        std::string tmpstr = f(timestamp, geometry);
        std::cout << "tmpstr: " << tmpstr << std::endl;
        _MeltParameters.append(tmpstr);

        timestamp = timeformat_convert(endposition);
        geometry = add_geometry(xratio, yratio, widthratio, heightratio);
        tmpstr = f(timestamp, geometry);
        std::cout << "tmpstr: " << tmpstr << std::endl;
        _MeltParameters.append(tmpstr);
    }

    _MeltParameters.append("\"");
    std::cout << "zoom filter: " << _MeltParameters << std::endl;
    GLINFO << "zoom filter: " << _MeltParameters;
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
    //generate_audio_multitracks();
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


