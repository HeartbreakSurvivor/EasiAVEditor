#include "MeltService.h"
#include <winnt.h>
#include <utils\CharsetUtils.h>
#include <utils\ApplicationUtils.h>
#include <utils\PathUtils.h>
#include <Logger\Logger.h>

MeltService::MeltService()
    : _prevousPercent(0)
    , _bIsRunning(false)
{
}

MeltService::~MeltService()
{
}

bool MeltService::Startmelt(const std::string & para)
{
    if (!_progresscb || !_msgcb) return false;
    if (_bIsRunning) return false;
    _bIsRunning = true;
    const std::wstring lpara = CharsetUtils::UTF8StringToUnicodeString(para);
    _thread = std::make_unique<std::thread>(&MeltService::WorkingThread, this, lpara);
    _thread->join();
    return true;
}

bool MeltService::AsyncStartmelt(const std::string & para)
{
    if (!_progresscb || !_msgcb) return false;
    if (_bIsRunning) return false;
    _bIsRunning = true;
    GLINFO << "Async Start the melt process";
    const std::wstring lpara = CharsetUtils::UTF8StringToUnicodeString(para);
    _thread = std::make_unique<std::thread>(&MeltService::WorkingThread, this, lpara);
    _thread->detach();
    return true;
}

bool MeltService::Stopmelt()
{
    if (!_bIsRunning) return true;
    //terminate the melt process
    DWORD ret = TerminateProcess(_pi.hProcess, 0);
    if (ret == 0) {
        GLERROR << "kill melt process failed, error code :" << GetLastError();
    }
    _thread->join();
    _bIsRunning = false;
    GLINFO << "user kill the melt process";
    return true;
}

void MeltService::ProgressReport_cbfun(progresscbfun func)
{
    _progresscb = func;
}

void MeltService::MsgReport_cbfun(msgcbfun func)
{
    _msgcb = func;
}

std::wstring MeltService::Get_melt_Path()
{
    //std::wstring appPath = std::wstring(PathUtils::GetAppExecuteWPath()) + L"\\Extensions\\MeltModules\\melt.exe";
    std::wstring appPath = std::wstring(PathUtils::GetAppExecuteWPath()) + L"\\melt.exe";
    return appPath;
}

void MeltService::WorkingThread(const std::wstring &paras)
{
    int timeout = 20;
    std::wstring executePath = Get_melt_Path();
    std::wstring comlin = executePath + paras;

    LPTSTR sConLin = const_cast<LPTSTR>(comlin.c_str());
    
    //GLINFO << "Melt command line: " << comlin.c_str();
    //create a anonymous pipe 
    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.lpSecurityDescriptor = NULL;
    sa.bInheritHandle = TRUE;
    if (!CreatePipe(&_pipeOutputRead, &_pipeInputWrite, &sa, 0)) {
        return;
    }

    //create the melt.exe process
    STARTUPINFO si;
    si.cb = sizeof(STARTUPINFO);
    GetStartupInfo(&si);
    si.hStdError = _pipeInputWrite; //connect the stderr endpoint of process to write endpoint of anonymous pipe. so we can read the output from the pipe.
    si.hStdOutput = _pipeInputWrite;    
    si.wShowWindow = SW_HIDE;
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
    
    if (!CreateProcess(NULL, sConLin, NULL, NULL, TRUE, NULL, NULL, NULL, &si, &_pi))
    {
        GLERROR << "CreateProcess failed, error code:" << GetLastError();
        goto failed;
    }
    GLINFO << "create pipe and Melt Process success.";
    char buffer[64] = { 0 };
    DWORD bytesRead;

    while (true)
    {
        //int ret = WaitForSingleObject(shExecInfo.hProcess, timeout);
        int ret = WaitForSingleObject(_pi.hProcess, timeout);
        if (ret == WAIT_TIMEOUT) {
            //time out 
        }
        else if (ret == WAIT_OBJECT_0) {
            //the process has been finished and notify application and return.
            _msgcb(-1);
            _bIsRunning = false;
            break;
        }
        else if (ret == WAIT_FAILED) {
            //notify error occurs and return.
            _msgcb(-1);
            _bIsRunning = false;
            break;
        }

        int res = ReadFile(_pipeOutputRead, buffer, 64, &bytesRead, NULL);
        if (!res || bytesRead == 0) {
            _msgcb(-2);
            break;
        }
        GLINFO << "bytesRead: " << bytesRead;
        std::string s(std::begin(buffer), std::begin(buffer) + bytesRead);
        GLINFO << "extract string: " << s;
        unsigned int index = s.find("percentage:");
        if (index != std::string::npos) {
            if ((index + std::strlen("percentage:") + 11) <= s.length()) {//this means the distance between 'p' and the last character.
                std::string tmpstr;
                tmpstr = s.substr(index + std::strlen("percentage:"), 11);
                int percent = atoi(tmpstr.c_str());
                if(percent!= _prevousPercent)
                    _progresscb(percent);
                _prevousPercent = percent;
                GLINFO << "melt output current percentage: " << percent << "%";
            }
        }
    }
    GLINFO << "Melt Service Process exit";

failed:
    CloseHandle(_pipeInputWrite);
    CloseHandle(_pipeOutputRead);
}
