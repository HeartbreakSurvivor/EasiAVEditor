#include "MeltService.h"
#include <winnt.h>
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

void MeltService::Startmelt(const std::string & para)
{
    const std::wstring lpara = CharsetUtils::UTF8StringToUnicodeString(para);
    _thread = std::make_unique<std::thread>(&MeltService::WorkingThread, this, lpara);
    _thread->join();
}

void MeltService::AsyncStartmelt(const std::string & para)
{
    const std::wstring lpara = CharsetUtils::UTF8StringToUnicodeString(para);

    _thread = std::make_unique<std::thread>(&MeltService::WorkingThread, this, lpara);
    _thread->detach();
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

void MeltService::MsgReport_cbfun(msgcbfun func)
{
    _msgcb = func;
}

std::wstring MeltService::Get_melt_Path()
{
    LPTSTR cWinDir = new TCHAR[MAX_PATH];
    GetCurrentDirectory(MAX_PATH, cWinDir);
    //LPTSTR sConLin = wcscat_s(cWinDir, MAX_PATH, L"\\..\\Debug\\another.exe a b c d");
    return std::wstring();
}

void MeltService::WorkingThread(const std::wstring &paras)
{
    int timeout = 1000;
    std::wstring executePath = Get_melt_Path();
    std::wstring parameters = paras;
    std::wstring comlin = executePath + std::wstring(L" ") + paras;

    LPTSTR sConLin = const_cast<LPTSTR>(comlin.c_str());
    
    LPTSTR cWinDir = new TCHAR[MAX_PATH];
    GetCurrentDirectory(MAX_PATH, cWinDir);
    //LPTSTR sCon1Lin = wcscat(cWinDir, L"\\..\\Debug\\another.exe a b c d");

    //create a anontmous pipe 
    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.lpSecurityDescriptor = NULL;
    sa.bInheritHandle = TRUE;
    if (!CreatePipe(&_pipeOutputRead, &_pipeInputWrite, &sa, 0)) {
        return;
    }

    //create the melt.exe process
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    si.cb = sizeof(STARTUPINFO);
    GetStartupInfo(&si);
    si.hStdError = _pipeInputWrite; //connect the stderr endpoint of process to write endpoint of anonymous pipe. so we can read the output from the pipe.
    si.hStdOutput = _pipeInputWrite;    
    si.wShowWindow = SW_HIDE;
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
    
    if (!CreateProcess(NULL, sConLin, NULL, NULL, TRUE, NULL, NULL, NULL, &si, &pi))
    {
        CloseHandle(_pipeInputWrite);
        CloseHandle(_pipeOutputRead);
        return ;
    }
    GLINFO << "create pipe and Melt Process success.";
    char buffer[4096] = { 0 };
    DWORD bytesRead;

    while (true)
    {
        //int ret = WaitForSingleObject(shExecInfo.hProcess, timeout);
        int ret = WaitForSingleObject(pi.hProcess, timeout);
        if (ret == WAIT_TIMEOUT) {
            //read the melt output and notify application progress.
            if (_progresscb) {
                _progresscb(20);
            }
        }
        else if (ret == WAIT_OBJECT_0) {
            //the process has been finished and notify application and return.
            if (_msgcb) {
                _msgcb(1);
            }
        }
        else if (ret == WAIT_FAILED) {
            //notify error occurs and return.
            if (_msgcb) {
                _msgcb(-1);
            }
            return;
        }

        int res = ReadFile(_pipeOutputRead, buffer, 4096, &bytesRead, NULL);
        if (!res || bytesRead == 0) {
            if (_msgcb) {
                _msgcb(-2);
            }
            break;
        }

        std::string s(std::begin(buffer) + 8, std::end(buffer) - 1);
        GLINFO << s;
        unsigned int index = s.find("percentage:");
        if (index != std::string::npos) {
            if ((index + 11) < s.length()) {
                std::string tmpstr;
                tmpstr.assign(s[index+11], s[index + 11]);    
                int percent = atoi(s.c_str());
                if(percent!= _prevousPercent)
                if (_progresscb) {
                    _progresscb(percent);
                    _prevousPercent = percent;
                }
            }
        }
    }
}
