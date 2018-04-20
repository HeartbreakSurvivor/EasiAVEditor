#pragma once
#include <string>
#include <vector>
#include "EasiAVEditorConfig.h"
#include "MeltService.h"
#include <json\json.h>
#include <Logger\Logger.h>
#include <utils\PathUtils.h>

class LoggerGuard
{
public:
    LoggerGuard() {
        InitLogger("Meltservice", PathUtils::GetSystemAppDataPath() + "\\jjs\\Log", 10, 10);
    }
    ~LoggerGuard() {
        ReleaseLogger();
    }
};

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
    float timeStr2second(const std::string &timestr);
    std::string second2timeStr(float seconds);
    bool isFltEqual(float a, float b);
    bool create_temporary_directory();
    int getrealTime() const;
    bool isImagefile(std::string str) const;

    //video and audio filters
    std::string attach_video_fadein_filter(const std::string &in, const std::string &out);
    std::string attach_video_fadeout_filter(const std::string &in, const std::string &out);
    std::string attach_volume_fadein_filter(const std::string &in, const std::string &out);
    std::string attach_volume_fadeout_filter(const std::string &in, const std::string &out);
    std::string attach_volume_gain_filter(float percentage);
    std::string attach_video_scale_filter(float x_ratio, float y_ratio, float width_ratio, float height_ratio);
    std::string attach_timewrarp_filter(float speedratio, std::string path, std::string cropstart, std::string cropend);

    //generate video&audio single track and multitracks.
    bool generate_video_multitrack();
    //some audio file may be speed up or slow down, generate temp timewarp file
    std::string generate_audio_tmpfile(float speedratio, std::string path);
    bool generate_audio_multitrack();

    //add transition
    void generate_transitions();
    void add_audio_mix_transition(uint16_t a_track, uint16_t b_track);
    void add_video_overlay_transition(uint16_t a_track, uint16_t b_track);

    //attach zoom animation
    std::string add_geometry(float x_ratio, float y_ratio, float width_ratio, float height_ratio);
    void add_zoom_animation_filter();

    //consumer settings
    void generate_consumer_settings();

    //generate the final melt parameters
    bool generate_parameters();

    //formatting string and print to log
    void formatting_parameters();

    //according to command line parameters to generate mlt file
    void generate_mlt_file();

    //result report function
    static void del_tmp_audiofile();

private:
    bool _clearflag = false;
    int _videotracks, _audiotracks, _tracks;//all kinds of tracks
    int _framerate;
    int _tempAudiofileNumber = 0;//how many audio file has been generated.
    struct tm _t;
    std::string _width, _height;

    Json::Value _jsonvideolist;
    Json::Value _jsonaudiolist;
    Json::Value _jsonzoomlist;
    Json::Value _jsonglobalinfo;

    std::vector<std::vector<std::string>> _videoMultitrack;//store multi video track's parameters.
    std::vector<std::vector<std::string>> _audioMultitrack;//store multi audio tracks's parameters.
    std::vector<std::string> _tmpAudiofilelist;//store the tmp audio file name.

    std::string _transitionPara;//store zoom in/out temp para.
    std::string _zoomlistPara;//store zoom in/out temp para.
    std::string _consumerPara;//store xml or avforamt consumer's configuration
    std::string _duration;//the total length of video
    std::string _mltfilepath;//store the temp mlt file's name and path.

    std::wstring _tmpfileDir;//directory to store temporary files.

    std::string _MeltParameters;//final melt parameters
    std::unique_ptr<MeltService> _pMeltService;

    LoggerGuard _logger;
    static EasiAVEditorGenerator* _avgenerator;
};

