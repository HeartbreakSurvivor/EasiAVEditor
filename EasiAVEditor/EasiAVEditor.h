#pragma once

#ifdef EASIAVEDITOR_EXPORTS
#define EASIAVEDITOR_API extern "C" __declspec(dllexport)
#else
#define EASIAVEDITOR_API __declspec(dllimport)
#endif

#include "MeltService.h"
#include <string>

/*
*Summary: pass four detail strings to create a AVEditor object.
*Parameters:
*   videotracklist��string contains video tracks info
*   audiotracklist��string contains audio tracks info
*   zoomlist: string contains zoom in/out related info
*   global_info: stirng contains global_info 
return :true measn success, else false.
*/
EASIAVEDITOR_API bool av_editor_create(const std::string &videotracklist, const std::string &audiotracklist, const std::string &zoomlist, const std::string &global_info);

/*
*Summary: register a callback function that report processing progress every-othersecond.
*Parameters:
*   cd: callback funtion which type is progresscbfun to register.
return :none
*/
EASIAVEDITOR_API void av_editor_register_progresscb(progresscbfun cb);

/*
*Summary: register a callback function that report message include error and result message.
*Parameters:
*   cd: callback funtion which type is msgcbfun to register.
return :none
*/
EASIAVEDITOR_API void av_editor_register_msgcb(msgcbfun cb);

/*
*Summary: start parse string and generate melt command line parameters.
*Parameters:
*   none
return :true measn success, else false.
*/
EASIAVEDITOR_API bool av_editor_start();

/*
*Summary: stop video&audio process service
*Parameters:
*   none
return :true measn success, else false.
*/
EASIAVEDITOR_API void av_editor_stop();
