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
}

EasiAVEditorGenerator::~EasiAVEditorGenerator()
{
}

void EasiAVEditorGenerator::start()
{
}

void EasiAVEditorGenerator::stop()
{
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

void EasiAVEditorGenerator::add_audio_mix_transition(uint16_t a_track, uint16_t b_track)
{
}

void EasiAVEditorGenerator::add_video_overlay_transition(uint16_t a_track, uint16_t b_track)
{
}

void EasiAVEditorGenerator::add_zoom_animation_filter()
{
}

void EasiAVEditorGenerator::add_xml_consumer_settings()
{
}

void EasiAVEditorGenerator::add_avformat_consumer_settings()
{
}

bool EasiAVEditorGenerator::generate_para()
{
    return false;
}

void EasiAVEditorGenerator::formatting_parameters()
{
}


