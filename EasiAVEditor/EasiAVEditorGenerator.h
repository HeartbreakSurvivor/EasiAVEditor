#pragma once
#include <string>
#include <vector>
#include "EasiAVEditorConfig.h"
#include "MeltService.h"
#include <json\json.h>
#include <Logger\Logger.h>

class EasiAVEditorGenerator
{
public:
    EasiAVEditorGenerator();
    explicit EasiAVEditorGenerator(Json::Value jsonVideolist, Json::Value jsonAudiolist, Json::Value jsonZoomlist, Json::Value jsonGlobalinfo);
    ~EasiAVEditorGenerator();
    void setProgressReportcb(progresscbfun func);
    void setMsgReportcb(msgcbfun func);
    bool start();
    void stop();

private:
    //video and audio filters
    void attach_video_fadein_filter(const std::string &in, const std::string &out);
    void attach_video_fadeout_filter(const std::string &in, const std::string &out);
    void attach_volume_fadein_filter(const std::string &in, const std::string &out);
    void attach_volume_fadeout_filter(const std::string &in, const std::string &out);
    void attach_volume_gain_filter(float percentage);
    void attach_video_scale_filter(float x, float y, float width, float height);
    void attach_timewrarp_filter(uint16_t speedratio);

    //appl filters to corresponding video&audio clips.
    void attach_videoclips_filters();
    void attach_audioclips_filters();

    //generate video&audio single track and multitracks.
    void generate_video_tracks();
    void generate_audio_tracks();
    void generate_video_multitracks();
    void generate_audio_multitracks();

    //add transition
    void generate_transitions();
    void add_audio_mix_transition(uint16_t a_track, uint16_t b_track);
    void add_video_overlay_transition(uint16_t a_track, uint16_t b_track);

    //attach zoom animation
    void add_geometry();
    void add_zoom_animation_filter();

    //consumer settings
    void generate_consumer_settings();

    //generate the final melt parameters
    bool generate_parameters();

    //formatting string and print to log
    void formatting_parameters();

    //according to command line parameters to generate mlt file
    void generate_mlt_file();

private:
    int _videotracks, _audiotracks, _tracks;//all kinds of tracks
    Json::Value _jsonvideolist;
    Json::Value _jsonaudiolist;
    Json::Value _jsonzoomlist;
    Json::Value _jsonglobalinfo;

    std::vector<std::vector<std::string>> _videoMultitrack;//store multi video track's parameters.
    std::vector<std::vector<std::string>> _audioMultitrack;//store multi audio tracks's parameters.
    std::string _transitionPara;//store zoom in/out temp para.
    std::string _zoomlistPara;//store zoom in/out temp para.
    std::string _consumerPara;//store xml or avforamt consumer's configuration

    std::string _mltfilepath;//store the temp mlt file's name and path.

    std::string _MeltParameters;//final melt parameters
    std::unique_ptr<MeltService> _pMeltService;
};

