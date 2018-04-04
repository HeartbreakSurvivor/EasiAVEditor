#include "EasiAVEditorGenerator.h"


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
}

void EasiAVEditorGenerator::attach_video_fadeout_filter(const std::string & in, const std::string & out)
{
}

void EasiAVEditorGenerator::attach_volume_fadein_filter(const std::string & in, const std::string & out)
{
}

void EasiAVEditorGenerator::attach_volume_fadeout_filter(const std::string & in, const std::string & out)
{
}

void EasiAVEditorGenerator::attach_volume_gain_filter(float percentage)
{
}

void EasiAVEditorGenerator::attach_video_scale_filter(float x, float y, float width, float height)
{
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
}

void EasiAVEditorGenerator::generate_audio_multitracks()
{
}

void EasiAVEditorGenerator::generate_audio_mix()
{
}

void EasiAVEditorGenerator::generate_video_composite()
{
}

void EasiAVEditorGenerator::add_audio_mix_transition(uint16_t a_track, uint16_t b_track)
{
}

void EasiAVEditorGenerator::add_video_overlay_transition(uint16_t a_track, uint16_t b_track)
{
}

void EasiAVEditorGenerator::add_zoom_animation_filter()
{
}

void EasiAVEditorGenerator::generate_consumer_settings()
{
    if (!_zoomlistPara.empty()) {
        add_xml_consumer_settings();
    }
    else {

    }
}

void EasiAVEditorGenerator::add_xml_consumer_settings()
{
}

void EasiAVEditorGenerator::add_avformat_consumer_settings()
{
    //-progress2 -getc
}

bool EasiAVEditorGenerator::generate_parameters()
{
    generate_video_multitracks();
    generate_audio_multitracks();
    generate_audio_mix();
    generate_video_composite();
    generate_consumer_settings();
    return true;
}

void EasiAVEditorGenerator::formatting_parameters()
{
}


