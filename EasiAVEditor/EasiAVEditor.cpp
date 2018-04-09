#include "EasiAVEditor.h"
#include <memory>
#include <json\json.h>
#include "EasiAVEditorGenerator.h"

std::unique_ptr<EasiAVEditorGenerator> EasiAVEditor;

bool av_editor_create(const std::string &videotracklist, const std::string &audiotracklist, const std::string &zoomlist, const std::string &global_info)
{
    Json::Reader jsonReader;
    Json::Value jsonVideolist,jsonAudiolist,jsonZoomlist,jsonGlobalinfo;
    //if (!jsonReader.parse(videotracklist, jsonVideolist) ||
    //    !jsonReader.parse(audiotracklist, jsonAudiolist) ||
    //    !jsonReader.parse(zoomlist, jsonZoomlist) ||
    //    !jsonReader.parse(global_info, jsonGlobalinfo)) {
    //    return false;
    //}
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
