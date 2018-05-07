#include "EasiAVEditorGenerator.h"
#include <utils\CharsetUtils.h>
#include <time.h>
#include <iostream>

//static member variable initialize
EasiAVEditorGenerator* EasiAVEditorGenerator::_avgenerator;

EasiAVEditorGenerator::EasiAVEditorGenerator()
{
}    

EasiAVEditorGenerator::EasiAVEditorGenerator(Json::Value jsonVideolist, Json::Value jsonAudiolist, Json::Value jsonZoomlist, Json::Value jsonGlobalinfo)
    : _jsonvideolist(jsonVideolist)
    , _jsonaudiolist(jsonAudiolist)
    , _jsonzoomlist(jsonZoomlist)
    , _jsonglobalinfo(jsonGlobalinfo)
    , _MeltParameters(u8"")
    , _consumerPara(u8"")
    , _videotracks(0)
    , _audiotracks(0)
    , _tracks(0)
    , _framerate(25)
    , _logger()
{
    _avgenerator = this;
    _pMeltService = std::make_unique<MeltService>();
    resrptcbfun cb = (resrptcbfun)&EasiAVEditorGenerator::del_tmp_audiofile;
    _pMeltService->Resrpt_cbfun(cb);
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
    _clearflag = true;
    return _pMeltService->AsyncStartmelt(_MeltParameters);
    //return _pMeltService->AsyncStartmelt(" C:\\Users\\bigzh\\AppData\\Roaming\\EasiAVEditorTest\\Mlt\\20180417210249\\halo.mlt -consumer avformat target=\"./一个很皮的人123man.mp4\" -progress vcodec=libx264 acodec=aac frame_rate_num=25 ar=44100 ab=192k ac=2 width=1920 height=1080 real_time=-4 progressive=1 ");
    //return _pMeltService->AsyncStartmelt(u8" \"D:\\Code\\EasiAVEditor\\Debug\\videos\\你好\\big_buck_1_min.mp4\" out=00:00:5.000 -consumer avformat target=\"C:\\剪辑视频\\我的视频.mp4\"");
    //return _pMeltService->AsyncStartmelt(" D:\\Code\\EasiAVEditor\\Debug\\videos\\big_buck_1_min.mp4 in=\"00:00:10.000\" out=\"00:00:40.000\" -consumer avformat:D:\\Code\\EasiAVEditor\\Debug\\videos\\slow.mp4 -progress vcodec=libx264 acodec=aac frame_rate_num=25 width=1920 height=1080 real_time=-4");
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
    unsigned int hour = static_cast<unsigned int>(seconds / 3600000);
    unsigned int minute = millisecond / 60000 % 60000;
    //int second = (millisecond % 60000) / 1000;
    float second = (float)(millisecond % 60000) / 1000;

    //std::cout << "hour: " << hour << "  minute: " << minute << "  second: " << second << std::endl;
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

bool EasiAVEditorGenerator::isFltEqual(float a, float b)
{
    return (fabs(a - b) < DBL_EPSILON);
}

bool EasiAVEditorGenerator::create_temporary_directory()
{
    if (!_tmpfileDir.empty()) return true;

    //get current time and form the mlt file name.
    time_t timep;
    time(&timep);
    localtime_s(&_t, &timep);
    //create mlt temp file directory.
    char diretory[64];
    strftime(diretory, sizeof(diretory), "%Y%m%d%H%M%S", &_t);
    std::string Tempfiledir(diretory, sizeof(diretory));
    std::wstring WTempfiledir;
    CharsetUtils::ANSIStringToUnicodeString(Tempfiledir, WTempfiledir);
    _tmpfileDir = std::wstring(PathUtils::GetAppDataWPath()) + L"\\Mlt\\" + WTempfiledir + L"\\";
    boost::filesystem::wpath path(_tmpfileDir);
    if (!boost::filesystem::exists(path.parent_path()))
    {
        boost::filesystem::create_directories(path.parent_path());
    }
    return true;
}

int EasiAVEditorGenerator::getrealTime() const
{
    int realtime = 1;
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    int threadCount = si.dwNumberOfProcessors;
    threadCount = threadCount > 2 ? min(threadCount - 1, 4) : 1;
    realtime = -threadCount;
    return realtime;
}

bool EasiAVEditorGenerator::isImagefile(std::string str) const
{
    std::string substr(".jpg");
    std::string substr1(".png");
    std::string substr2(".jpeg");
    if (str.rfind(substr) == (str.length() - substr.length()) ||
        str.rfind(substr1) == (str.length() - substr1.length()) ||
        str.rfind(substr2) == (str.length() - substr2.length())) {
        return true;
    }
    return false;
}

std::string EasiAVEditorGenerator::attach_video_fadein_filter(const std::string & in, const std::string & out)
{
    std::string str(" -attach-clip brightness level=\"");
    str.append(in);
    str.append("=0; ");
    str.append(out);
    str.append("=1\" alpha=\"");
    str.append(in);
    str.append("=0; ");
    str.append(out);
    str.append("=1\"");
    return str;
}

std::string EasiAVEditorGenerator::attach_video_fadeout_filter(const std::string & in, const std::string & out)
{
    std::string str(" -attach-clip brightness level=\"");
    str.append(in);
    str.append("=1; ");
    str.append(out);
    str.append("=0\" alpha=\"");
    str.append(in);
    str.append("=1; ");
    str.append(out);
    str.append("=0\"");
    return str;
}

std::string EasiAVEditorGenerator::attach_volume_fadein_filter(const std::string & in, const std::string & out)
{
    std::string str(" -attach-clip volume gain=0 end=1 in=\"");
    str.append(in);
    str.append("\" out=\"");
    str.append(out);
    str.append("\"");
    return str;
}

std::string EasiAVEditorGenerator::attach_volume_fadeout_filter(const std::string & in, const std::string & out)
{
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
    std::string str(" -attach-clip volume gain=");
    str.append(std::to_string(gain));
    str.append(" ");
    return str;
}

std::string EasiAVEditorGenerator::attach_video_scale_filter(float x_ratio, float y_ratio, float width_ratio, float height_ratio)
{
    std::string str(" -attach-clip affine background=colour:0 transition.geometry=\"");
    std::string geometry = add_geometry(x_ratio, y_ratio, width_ratio, height_ratio);
    str.append(geometry);
    str.append("\"");
    return str;
}

std::string EasiAVEditorGenerator::attach_timewrarp_filter(float speedratio, std::string path, std::string cropstart, std::string cropend)
{
    std::string str("timewarp:");
    str.append(std::to_string(speedratio));
    str.append(":\"");
    str.append(path);
    str.append("\" in=");
    int inframe = static_cast<int>(timeStr2second(cropstart)*_framerate);
    str.append(std::to_string(inframe)); 
    str.append(" out=");
    int outframe = static_cast<int>(timeStr2second(cropend)*_framerate);
    str.append(std::to_string(outframe));
    str.append(" ");
    return str;
}

bool EasiAVEditorGenerator::generate_video_multitrack()
{
    if (_jsonvideolist.empty() || !_jsonvideolist.isArray()) {
        GLINFO << "video json list is empty ";
        return false;
    }

    GLINFO << "videotrack's size: " << _jsonvideolist.size();
    GLINFO << "videotrack is array: " << _jsonvideolist.isArray();

    //append black background track.
    _MeltParameters.append(" -producer color:black out=");
    _MeltParameters.append(_duration);
    _MeltParameters.append(" -track ");
    _videotracks++;
    std::string lastendpoint("00.00.00.000");
    for (int i = 0; i < _jsonvideolist.size(); i++) {
        for (int j = 0; j < _jsonvideolist[i].size(); j++) {
            //std::string path = CharsetUtils::ANSIStringToUTF8String(_jsonvideolist[i][j][TARGET_PATH].asString());
            std::string path = _jsonvideolist[i][j][RESOURCE_PATH].asString();
            std::string startposition = _jsonvideolist[i][j][ABSOLUTE_STARTPOSITION].asString();
            std::string endposition = _jsonvideolist[i][j][ABSOLUTE_EDNPOSITION].asString();
            std::string clipduration = _jsonvideolist[i][j][CLIPDURATION].asString();
            std::string cropStartPosition = _jsonvideolist[i][j][CROP_STARTPOSITION].asString();
            std::string cropEndPosition = _jsonvideolist[i][j][CROP_ENDPOSITION].asString();

            std::string videofadeInstartposition = _jsonvideolist[i][j][VIDEOFADEIN_STARTPOSITION].asString();
            std::string videofadeInDuration = _jsonvideolist[i][j][VIDEOFADEIN_DURATION].asString();
            std::string videofadeOutstartposition = _jsonvideolist[i][j][VIDEOFADEOUT_STARTPOSITION].asString();
            std::string videofadeOutDuration = _jsonvideolist[i][j][VIDEOFADEOUT_DURATION].asString();

            std::string audiofadeInstartposition = _jsonvideolist[i][j][AUDIOFADEIN_STARTPOSITION].asString();
            std::string audiofadeInDuration = _jsonvideolist[i][j][AUDIOFADEIN_DURATION].asString();
            std::string audiofadeOutstartposition= _jsonvideolist[i][j][AUDIOFADEOUT_STARTPOSITION].asString();
            std::string audiofadeOutDuration = _jsonvideolist[i][j][AUDIOFADEOUT_DURATION].asString();

            float xratio = _jsonvideolist[i][j][VIDEOGEOMETRY][GEOMETRY_X].asFloat();
            float yratio = _jsonvideolist[i][j][VIDEOGEOMETRY][GEOMETRY_Y].asFloat();
            float widthratio = _jsonvideolist[i][j][VIDEOGEOMETRY][GEOMETRY_WIDTH].asFloat();
            float heightratio = _jsonvideolist[i][j][VIDEOGEOMETRY][GEOMETRY_HEIGHT].asFloat();
            float speedRatio = _jsonvideolist[i][j][SPEEDRATIO].asFloat();
            float volume = _jsonvideolist[i][j][VOLUME].asFloat();

            if (!isFltEqual((timeStr2second(startposition) - timeStr2second(lastendpoint)), 0.0f)) {
                _MeltParameters.append(" -blank out=");
                std::string str = second2timeStr((timeStr2second(startposition) - timeStr2second(lastendpoint)));
                _MeltParameters.append(str);
                _MeltParameters.append(" ");
            }
            lastendpoint = endposition;//update last end point
            if (!isFltEqual(speedRatio, 1.0f)) {
                _MeltParameters.append(attach_timewrarp_filter(speedRatio, path, cropStartPosition, cropEndPosition));
            }
            else {
                _MeltParameters.append("\"");//append the resource file's path 
                _MeltParameters.append(path);
                _MeltParameters.append("\"");
                _MeltParameters.append(" in=");
                _MeltParameters.append(cropStartPosition);//append the audio clip's start point related to the starting point of resource file. 
                _MeltParameters.append(" out=");
                _MeltParameters.append(cropEndPosition);//append the audio file's 
                _MeltParameters.append(" ");
            }

            if (!isFltEqual(xratio, 0.0f) ||
                !isFltEqual(yratio, 0.0f) ||
                !isFltEqual(widthratio, 1.0f) ||
                !isFltEqual(heightratio, 1.0f)) {
                std::string str = attach_video_scale_filter(xratio, yratio, widthratio, heightratio);
                _MeltParameters.append(str);
            }

            if (videofadeInDuration != "") {
                std::string str = attach_video_fadein_filter("00:00:00.000", videofadeInDuration);
                _MeltParameters.append(str);
            }
            if (videofadeOutDuration != "") {
                std::string str = attach_video_fadeout_filter(videofadeOutstartposition, clipduration);
                _MeltParameters.append(str);
            }
            if (audiofadeInDuration != "") {
                std::string str = attach_volume_fadein_filter("00:00:00.000", audiofadeInDuration);
                _MeltParameters.append(str);
            }
            if (audiofadeOutDuration != "") {
                std::string str = attach_volume_fadeout_filter(audiofadeOutstartposition, clipduration);
                _MeltParameters.append(str);
            }
            if (!isFltEqual(volume, 1.0f)) {
                if (!isImagefile(path)) {//if the resource file is not image then apply the volume filter
                    _MeltParameters.append(attach_volume_gain_filter(volume));
                }
            }
            _MeltParameters.append(" ");
        }
        if (i != _jsonvideolist.size() - 1) {
            _MeltParameters.append(" -track ");
        }
        _videotracks++;
        lastendpoint = std::string("00:00:00.000");
    }
    
    GLINFO << "video track number: " << _videotracks;
    GLINFO << "video track list parameters: " << _MeltParameters;
    _MeltParameters.append(" ");
    return true;
}

std::string EasiAVEditorGenerator::generate_audio_tmpfile(float speedratio, std::string path)
{
    std::string str("timewarp:");
    str.append(std::to_string(speedratio));
    str.append(":\"");
    str.append(path);
    str.append("\" -consumer avformat target=\"");

    create_temporary_directory();

    std::string dir;
    CharsetUtils::UnicodeStringToANSIString(_tmpfileDir, dir);
    
    //generate audio temporary file's name.
    std::string audioname = "";
    audioname.append("jjs-audio-");
    audioname.append(std::to_string(_tempAudiofileNumber));
    audioname.append(".mp3");

    str.append(dir);
    str.append(audioname);
    str.append("\"");

    GLINFO << "\ntemporary audio file's name & path: \n" << "\t" << str;

    _pMeltService->Startmelt(str);

    GLINFO << "temporary audio file generated successfully!";

    _tempAudiofileNumber++;
    std::string tmpfilepath;
    tmpfilepath.append(dir);
    tmpfilepath.append(audioname);
    _tmpAudiofilelist.push_back(tmpfilepath);
    return tmpfilepath;
}

bool EasiAVEditorGenerator::generate_audio_multitrack()
{
    if (_jsonaudiolist.empty() || !_jsonaudiolist.isArray()) {
        GLINFO << "audio json list is empty ";
        return true;
    }
    GLINFO << "audiotrack's size: " << _jsonaudiolist.size();
    GLINFO << "audiotrack is array: " << _jsonaudiolist.isArray();

    std::string audioMultitrack(" ");//audiotrack temp parameters

    for (int i = 0; i< _jsonaudiolist.size(); i++) {
        //std::string path = CharsetUtils::ANSIStringToUTF8String(_jsonaudiolist[i][TARGET_PATH].asString());
        std::string path = _jsonaudiolist[i][RESOURCE_PATH].asString();
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

        audioMultitrack.append(" -hide-video ");
        if (timeStr2second(startposition) != 0.0f) {//the audio clip's startint point is not from zero, so add blank clip
            audioMultitrack.append(" -blank out=");
            audioMultitrack.append(startposition);
            audioMultitrack.append(" ");
        }
        
        if (speedRatio != 1.0f) {
            //generate temporary audio file and return file path.
            std::string tmpfilepath = generate_audio_tmpfile(speedRatio, path);
            audioMultitrack.append("\"");//append the tmp audio file's path 
            audioMultitrack.append(tmpfilepath); 
            audioMultitrack.append("\""); 
        }
        else {
            audioMultitrack.append("\"");//append the resource file's path 
            audioMultitrack.append(path);//append the resource file's path 
            audioMultitrack.append("\"");//append the resource file's path 
        }

        audioMultitrack.append(" in=");
        audioMultitrack.append(cropStartPosition);//append the audio clip's start point related to the starting point of resource file. 
        audioMultitrack.append(" out=");
        audioMultitrack.append(cropEndPosition);//append the audio file's 

        if (fadeInDuration != "") {
            audioMultitrack.append(attach_volume_fadein_filter("00:00:00.000", fadeInDuration));
        }
        if (fadeOutDuration != "") {
            std::string fadeoutstart = second2timeStr(timeStr2second(clipduration) - timeStr2second(fadeOutDuration));
            audioMultitrack.append(attach_volume_fadeout_filter(fadeoutstart, clipduration));
        }
        if (volume != 1.0f) {
            audioMultitrack.append(attach_volume_gain_filter(volume));
        }
        _audiotracks++;//increase the audio tracks number.
    }
    GLINFO << "audio track number: " << _audiotracks;
    GLINFO << "audio multitrack's parameters: " << audioMultitrack;
    _MeltParameters.append(audioMultitrack);
}

void EasiAVEditorGenerator::generate_transitions()
{
    _tracks = _audiotracks + _videotracks;
    GLINFO << "total tracks number: " << _tracks;
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
    std::string str(" -transition mix:-1 always_active=1 a_track=");
    str.append(std::to_string(a_track));
    str.append(" b_track=");
    str.append(std::to_string(b_track));
    str.append(" sum=1");
    _transitionPara.append(str);
}

void EasiAVEditorGenerator::add_video_overlay_transition(uint16_t a_track, uint16_t b_track)
{
    std::string str(" -transition frei0r.cairoblend a_track=");
    str.append(std::to_string(a_track));
    str.append(" b_track=");
    str.append(std::to_string(b_track));
    str.append(" disable=0");
    _transitionPara.append(str);
}

std::string EasiAVEditorGenerator::add_geometry(float x_ratio, float y_ratio, float width_ratio, float height_ratio)
{
    float x = x_ratio * 100;
    float y = y_ratio * 100;
    float width = width_ratio * 100;
    float height = height_ratio * 100;

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

    GLINFO << "zoomlist's size: " << _jsonzoomlist.size();
    GLINFO << "zoomlist is array: " << _jsonzoomlist.isArray();

    _MeltParameters.append(" -filter affine transition.cycle=0 transition.geometry=\"");

    auto f = [this](float timestamp, std::string geometry) {
        std::string str(u8"");
        int frame = static_cast<int>(timestamp * _framerate);
        str.append(std::to_string(frame));
        str.append("=");
        str.append(geometry);
        str.append("; ");
        return str;
    };

    //add the initial zoom geometry
    float timestamp = timeStr2second("00:00:00.000");
    std::string geometry = add_geometry(0.0f, 0.0f, 1.0f, 1.0f);
    _MeltParameters.append(f(timestamp, geometry));
    
    for (size_t i = 0; i< _jsonzoomlist.size(); i++) {
        std::string startposition = _jsonzoomlist[i][ABSOLUTE_STARTPOSITION].asString();
        std::string endposition = _jsonzoomlist[i][ABSOLUTE_EDNPOSITION].asString();
        float xratio = _jsonzoomlist[i][SCALEGEOMETRY][GEOMETRY_X].asFloat();
        float yratio = _jsonzoomlist[i][SCALEGEOMETRY][GEOMETRY_Y].asFloat();
        float widthratio = _jsonzoomlist[i][SCALEGEOMETRY][GEOMETRY_WIDTH].asFloat();
        float heightratio = _jsonzoomlist[i][SCALEGEOMETRY][GEOMETRY_HEIGHT].asFloat();

        timestamp = timeStr2second(startposition);
        std::string tmpstr = f(timestamp, geometry);
        _MeltParameters.append(tmpstr);

        timestamp = timeStr2second(endposition);
        geometry = add_geometry(xratio, yratio, widthratio, heightratio);
        tmpstr = f(timestamp, geometry);
        _MeltParameters.append(tmpstr);
    }
    
    _MeltParameters.append("\"");
    GLINFO << "zoom filter parameters: " << _MeltParameters;
}

void EasiAVEditorGenerator::generate_consumer_settings()
{
    std::wstring showpath = CharsetUtils::ANSIStringToUnicodeString(_jsonglobalinfo[TARGET_PATH].asString());
    std::string resolution = _jsonglobalinfo[RESOLUTION].asString();
    _width = resolution.substr(0, resolution.find('X'));
    _height = resolution.substr(resolution.find('X')+1,resolution.size());
    std::string framerate = _jsonglobalinfo[FRAME_RATE].asString();
    std::string samplerate = _jsonglobalinfo[SAMPLERATE].asString();

    _duration = _jsonglobalinfo[TOTAL_DURATION].asString();
    _framerate = std::stoi(framerate);

    GLINFO <<"\nresolution: " << resolution << "\n" << "Duration: " << _duration << "\n" << "Framerate: " << framerate << "\n" 
        << "Samplerate: " << samplerate << "\n";

    //fucking processing parameters order
    _consumerPara.append(" -consumer avformat");
    //Use YouTube preset 
    //Field Order https://www.provideocoalition.com/field_order/
    _consumerPara.append(" top_field_first=0");
    _consumerPara.append(" aspect=1.77778");
    //indicate the configuration of encoder. to balance the encoding speed and quality
    _consumerPara.append(" preset=fast");
    _consumerPara.append(" r=");
    _consumerPara.append(framerate);
    //Constant Rate Factor -- control the quality of output video rang=0~51 the higer value, the low quality 
    _consumerPara.append(" crf=21");
    //ffmpeg option. Set the pixel interpolation mode.
    _consumerPara.append(" rescale=bilinear");    
    _consumerPara.append(VIDEO_CODEC);
    _consumerPara.append(PROGRESSIVE);
    _consumerPara.append(" movflags=+faststart");
    //The numbers of B frame contorl
    _consumerPara.append(" bf=2");
    _consumerPara.append(" target=\"");
    _consumerPara.append(CharsetUtils::ANSIStringToUTF8String(_jsonglobalinfo[TARGET_PATH].asString()));
    _consumerPara.append("\"");
    _consumerPara.append(AUDIO_BITRATE);
    //ffmpeg --> deinterlace the input video(yet means "yet another deinterlacing filter") default value is send_frame.
    //website: https://ffmpeg.org/ffmpeg-filters.html#yadif
    _consumerPara.append(" deinterlace_method=yadif");
    _consumerPara.append(VIDEO_HEIGHT);
    _consumerPara.append(_height);
    _consumerPara.append(REALTIME);
    //get realtime cnt, apply parallelism.
    _consumerPara.append(std::to_string(getrealTime()));
    //ffmpeg choose the number of threads which will be used in the decoders
    _consumerPara.append(" thread=0");
    _consumerPara.append(AUDIO_CODEC);
    _consumerPara.append(" mlt_service=avformat");
    //ffmpeg options. f means format, it can be indicated the input and output format.
    _consumerPara.append(" f=mp4");
    _consumerPara.append(VIDEO_WIDTH);
    _consumerPara.append(_width);    
    //the number of Key Frame
    _consumerPara.append(" g=15");
    _consumerPara.append(AUDIO_SAMPLE_RATE);
    _consumerPara.append(samplerate);
    _consumerPara.append(AUDIO_CHANNEL);

    //global option
    _consumerPara.append(" -progress");//display processing progress 
    GLINFO << "consumer melt parameters: " << _consumerPara;
}

bool EasiAVEditorGenerator::generate_parameters()
{
    generate_consumer_settings();
    if(!generate_video_multitrack()) return false;
    if(!generate_audio_multitrack()) return false;
#if !GEN_XML
    add_zoom_animation_filter();
#endif
    //generate audio mix and video composite transitions.
    generate_transitions();
#if GEN_XML
    //generate the mlt file
    generate_mlt_file();
    
    //add zoom animation effect
    add_zoom_animation_filter(); 
#endif
    _MeltParameters.append(_consumerPara);
    GLINFO << "\nMeltParameters & Consumer: \n" << "\t" << _MeltParameters;
    return true;
}

void EasiAVEditorGenerator::formatting_parameters()
{
    GLINFO << "Total MeltParameters: \n" << _MeltParameters;
    //for (int i = 0; i < _MeltParameters.size();) {
    //    i = _MeltParameters.find("-track");
    //}
    //_MeltParameters;
}

void EasiAVEditorGenerator::generate_mlt_file()
{
    std::string str(u8"");
    if (_width == "1920" && _height == "1080") {
        str.append(" -profile atsc_1080p_25 -consumer xml:");
    }
    else {
        str.append(" -profile atsc_720p_25 -consumer xml:");
    }
    
    create_temporary_directory();

    std::string dir;
    CharsetUtils::UnicodeStringToANSIString(_tmpfileDir, dir);

    //create mlt file
    char mltfile[64];
    strftime(mltfile, sizeof(mltfile), "jjs-%Y-%m-%d-%H-%M-%S.mlt", &_t);
    
    _mltfilepath.append("\"");
    _mltfilepath.append(dir);
    _mltfilepath.append(mltfile);
    _mltfilepath.append("\"");
    str.append(_mltfilepath);
    //other consumer xml properties
    str.append(" time_format=\"clock\"");
    str.append(" no_meta=1");
    //str.append(" store=\"jjs\" ");
    GLINFO << "temporary mlt file's name & path: " << str;
    _MeltParameters.append(str);
    //output the melt command line to log.
    formatting_parameters();
    _pMeltService->Startmelt(_MeltParameters);
    _MeltParameters.clear();
    _MeltParameters.append(_mltfilepath);
    _tmpfileDir.clear();
}

void EasiAVEditorGenerator::del_tmp_audiofile()
{
    if (!_avgenerator->_clearflag) return;
    if (_avgenerator->_tmpAudiofilelist.empty()) return;
    GLINFO << "starting delete temporary audio file";
    
    while (!_avgenerator->_tmpAudiofilelist.empty())
    {
        std::string path = _avgenerator->_tmpAudiofilelist.back();
        _avgenerator->_tmpAudiofilelist.pop_back();
        boost::filesystem::path filePath(path);
        boost::filesystem::remove(filePath);
    }
    _avgenerator->_clearflag = false;
}
