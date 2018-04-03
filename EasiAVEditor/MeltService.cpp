#include "MeltService.h"
#include <utils\CharsetUtils.h>
#include <utils\ApplicationUtils.h>
#include <utils\PathUtils.h>
#include <Logger\Logger.h>

MeltService::MeltService()
{
    
}

MeltService::~MeltService()
{
}

bool MeltService::Startmelt(const std::string & para)
{
    const std::wstring lpara = CharsetUtils::UTF8StringToUnicodeString(para);

    _thread = std::make_unique<std::thread>(&MeltService::WorkingThread, this, lpara);
    _thread->detach();
    return true;
}

bool MeltService::Stopmelt()
{
    _thread->join();
    return true;
}

void MeltService::ProgressReport_cbfun(progresscbfun func)
{
    _progresscb = func;
}

void MeltService::ErrorReport_cbfun(resultcbfun func)
{
    _resultcb = func;
}

std::wstring MeltService::Get_melt_Path()
{
    return std::wstring();
}

void MeltService::WorkingThread(const std::wstring &paras)
{
    int timeout = 1000;
    std::wstring executePath = Get_melt_Path();
    
    SHELLEXECUTEINFO shExecInfo = { 0 };
    shExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
    shExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
    shExecInfo.hwnd = NULL;
    shExecInfo.lpVerb = _T("open");
    shExecInfo.lpFile = _T("C:\\Program Files\\Shotcut\\melt.exe");
    shExecInfo.lpParameters = paras.c_str();
    shExecInfo.lpDirectory = _T("C:\\Program Files\\Shotcut\\");
    shExecInfo.nShow = SW_SHOW;
    shExecInfo.hInstApp = NULL;

    if (!ShellExecuteEx(&shExecInfo)) {
        GLERROR << "Start melt.exe service failed";
        return;
    }

    while (true)
    {
        int ret = WaitForSingleObject(shExecInfo.hProcess, timeout);
        if (ret == WAIT_TIMEOUT) {
            //read the melt output and notify application progress.
            _progresscb(20);
        }
        else if (ret == WAIT_OBJECT_0) {
            //the process has been finished and notify application and return.
            _resultcb(1);
        }
        else if (ret == WAIT_FAILED) {
            //notify error occurs and return.
           _resultcb(-1);
            return;
        }
    }
}
