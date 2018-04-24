#include "EasiAVEditor.h"
#include <memory>
#include <json\json.h>
#include "EasiAVEditorGenerator.h"

std::unique_ptr<EasiAVEditorGenerator> EasiAVEditor;

Json::Value GetJson(const std::wstring& path)
{
    static std::string s_File;

    Json::Value root;
    std::ifstream iStream(path.c_str(), std::ios::binary);
    if (iStream.is_open()) {
        Json::Reader reader;
        if (!reader.parse(iStream, root)) {
            root.clear();
        }
        iStream.close();
    }
    return root;
}


bool av_editor_create(const char* videotracklist, const char* audiotracklist, const char* zoomlist, const char* global_info)
{
    Json::Reader jsonReader;
    Json::Value jsonVideolist,jsonAudiolist,jsonZoomlist,jsonGlobalinfo;
    /*jsonVideolist = GetJson(L"D:\\Code\\EasiAVEditor\\video.txt");
    jsonAudiolist = GetJson(L"D:\\Code\\EasiAVEditor\\audio.txt");
    jsonZoomlist = GetJson(L"D:\\Code\\EasiAVEditor\\zoom.txt");
    jsonGlobalinfo = GetJson(L"D:\\Code\\EasiAVEditor\\global.txt");*/
    //bool f = jsonReader.parse(std::string(zoomlist), jsonZoomlist);
    //f = jsonReader.parse(std::string(videotracklist), jsonVideolist);
    //f = jsonReader.parse(std::string(audiotracklist), jsonAudiolist);
    //f = jsonReader.parse(std::string(global_info), jsonGlobalinfo);

    if (!jsonReader.parse(std::string(videotracklist), jsonVideolist) ||
        !jsonReader.parse(std::string(audiotracklist), jsonAudiolist) ||
        !jsonReader.parse(std::string(zoomlist), jsonZoomlist) ||
        !jsonReader.parse(std::string(global_info), jsonGlobalinfo)) {
        return false;
    }
    EasiAVEditor = std::make_unique<EasiAVEditorGenerator>(jsonVideolist,jsonAudiolist,jsonZoomlist,jsonGlobalinfo);
    return true;
}

void av_editor_register_progresscb(progresscbfun cb)
{
    EasiAVEditor->setProgressReportcb(cb);
}

void av_editor_register_msgcb(msgcbfun cb)
{
    EasiAVEditor->setMsgReportcb(cb);
}

bool av_editor_start()
{
    return EasiAVEditor->start();
}

void av_editor_stop()
{
    EasiAVEditor->stop();
}
