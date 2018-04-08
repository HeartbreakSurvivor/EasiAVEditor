#include "EasiAVEditorGenerator.h"
#include <time.h>


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
{
    _pMeltService = std::make_unique<MeltService>();
}

EasiAVEditorGenerator::~EasiAVEditorGenerator()
{
    _pMeltService.release();
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
    _pMeltService->Startmelt(_MeltParameters);
    return true;
}

void EasiAVEditorGenerator::stop()
{
    _pMeltService->Stopmelt();
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


    _MeltParameters.append(videotrack);
}

void EasiAVEditorGenerator::generate_audio_multitracks()
{
    std::string audiotrack;


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

void EasiAVEditorGenerator::add_geometry()
{
    std::string geometry("0%/0%:0%x0%");

}

void EasiAVEditorGenerator::add_zoom_animation_filter()
{
    if (_jsonzoomlist.empty()) return;
    _MeltParameters.clear();
    _MeltParameters.append(_mltfilepath);
    _MeltParameters.append(" -filter affine transition.cycle=0 transition.geometry=\"");
    //generate zoomlist 
}

void EasiAVEditorGenerator::generate_consumer_settings()
{
    _consumerPara.append(" -progress");
    _consumerPara.append(" -getc");
    _consumerPara.append(" -consumer avformat target=");
    _consumerPara.append(_jsonglobalinfo[TARGET_PATH].asString());
    _consumerPara.append(VIDEO_CODEC);
    _consumerPara.append(AUDIO_CODEC);
    _consumerPara.append(AUDIO_SAMPLE_RATE);
    _consumerPara.append(AUDIO_BITRATE);
    _consumerPara.append(AUDIO_CHANNEL);

    GLINFO << "consumer melt parameters: " << _consumerPara;
    _MeltParameters.append(_consumerPara);
}

bool EasiAVEditorGenerator::generate_parameters()
{
    generate_video_multitracks();
    generate_audio_multitracks();
    //generate audio mix and video composite transitions.
    generate_transitions();
    //generate the mlt file
    generate_mlt_file();
    //output the melt command line to log.
    formatting_parameters();
    //add zoom animation filter
    add_zoom_animation_filter();

    generate_consumer_settings();
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


