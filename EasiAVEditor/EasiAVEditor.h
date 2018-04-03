#pragma once

#ifdef EASIAVEDITOR_EXPORTS
#define EASIAVEDITOR_API extern "C" __declspec(dllexport)
#else
#define EASIAVEDITOR_API __declspec(dllimport)
#endif

#include <string>

EASIAVEDITOR_API bool av_editor_create(const std::string &videotracklist, const std::string &audiotracklist, const std::string &zoomlist, const std::string &global_info);
EASIAVEDITOR_API void av_editor_start();
EASIAVEDITOR_API void av_editor_stop();
EASIAVEDITOR_API void av_editor_release();
EASIAVEDITOR_API void av_editor_register_progresscb();

//EASIAVEDITOR_API int av_editor_report_result();
