#include "MeltService.h"
#include "EasiAVEditorConfig.h"
#include <winnt.h>
#include <utils\CharsetUtils.h>
#include <utils\ApplicationUtils.h>
#include <utils\PathUtils.h>
#include <Logger\Logger.h>

MeltService::MeltService()
    : _previousPercent(0)
    , _bIsRunning(false)
    , _bIsAsync(true)
    , _pipeOutputRead(nullptr)
    , _pipeInputWrite(nullptr)
{
}

MeltService::~MeltService()
{
}

bool MeltService::Startmelt(const std::string & para)
{
    if (!_progresscb || !_msgcb) return false;
    if (_bIsRunning) return false;
    _bIsAsync = false;
    const std::wstring lpara = CharsetUtils::UTF8StringToUnicodeString(para);
    _wkThread = std::make_unique<std::thread>(&MeltService::WorkingThread, this, lpara);
    _wkThread->join();
    _bIsAsync = true;
    return true;
}

bool MeltService::AsyncStartmelt(const std::string & para)
{
    if (!_progresscb || !_msgcb) return false;
    if (_bIsRunning) return false;
    _bIsAsync = true;
    GLINFO << "Async Start the melt process";
    const std::wstring lpara = CharsetUtils::UTF8StringToUnicodeString(para);
    _wkThread = std::make_unique<std::thread>(&MeltService::WorkingThread, this, lpara);
    _wkThread->detach();
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

    //char buf[] = "qqqqqqqqqq";
    //DWORD dwBytesWrite;
    //int si = sizeof(buf);
    //WriteFile(_pipeInputWrite1, buf, sizeof(buf)-1, &dwBytesWrite, NULL);
    //if (dwBytesWrite == sizeof(buf)-1) {
    //    GLINFO << "Send quit signal to Melt Service Process succeed.";
    //}
    _bIsRunning = false;
    GLINFO << "user kill the Melt Service Process";
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

void MeltService::Resrpt_cbfun(resrptcbfun func)
{
    _rescb = func;
}

std::wstring MeltService::Get_melt_Path()
{
    std::wstring appPath = L"C:\\Program Files\\Shotcut\\melt.exe";
    return appPath;
}

void MeltService::WorkingThread(const std::wstring &paras)
{
    int timeout = 20;
    _bIsRunning = true;
    std::wstring executePath = Get_melt_Path();
    std::wstring comlin = executePath + L" " + paras;

    //LPTSTR sConLin = const_cast<LPTSTR>(comlin.c_str());
    
    std::string sstr;
    bool f = CharsetUtils::UnicodeStringToUTF8String(comlin, sstr);
    char* sConLin = const_cast<char *>(sstr.c_str());

    GLINFO << "\nMelt parar:\n " << sstr;

    //create a anonymous pipe to get result from melt process
    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.lpSecurityDescriptor = NULL;
    sa.bInheritHandle = TRUE;
    if (!CreatePipe(&_pipeOutputRead, &_pipeInputWrite, &sa, 0)) {
        return;
    }

    //create a anonymous pipe to send command to melt process
    //SECURITY_ATTRIBUTES sa1;
    //sa1.nLength = sizeof(SECURITY_ATTRIBUTES);
    //sa1.lpSecurityDescriptor = NULL;
    //sa1.bInheritHandle = TRUE;
    //if (!CreatePipe(&_pipeOutputRead1, &_pipeInputWrite1, &sa1, 0)) {
    //    return;
    //}

    //create the read write thread
    _rwThread = std::make_unique<std::thread>(&MeltService::ReadWriteThread, this);
    if (_bIsAsync) {
        _rwThread->detach();
    }

    ////create the melt.exe process
    //STARTUPINFO si;
    //si.cb = sizeof(STARTUPINFO);
    //GetStartupInfo(&si);
    //si.hStdError = _pipeInputWrite; //connect the stderr endpoint of process to write endpoint of anonymous pipe. so we can read the output from the pipe.
    //si.hStdOutput = _pipeInputWrite;    
    ////si.hStdInput = _pipeOutputRead1;//connect the stdinput to the read endpoint of pipe, so we can write data to melt process with pipe.
    //si.wShowWindow = SW_HIDE;
    //si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;

    //if (!CreateProcess(NULL, sConLin, NULL, NULL, TRUE, NULL, NULL, NULL, &si, &_pi))
    //{
    //    GLERROR << "CreateProcess failed, error code:" << GetLastError();
    //    _msgcb(CREATE_MELT_FAILED);
    //    goto failed;
    //}
    
    STARTUPINFOA sia;
    sia.cb = sizeof(STARTUPINFOA);
    GetStartupInfoA(&sia);
    sia.hStdError = _pipeInputWrite; //connect the stderr endpoint of process to write endpoint of anonymous pipe. so we can read the output from the pipe.
    sia.hStdOutput = _pipeInputWrite;
    //si.hStdInput = _pipeOutputRead1;//connect the stdinput to the read endpoint of pipe, so we can write data to melt process with pipe.
    sia.wShowWindow = SW_HIDE;
    sia.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;

    if (!CreateProcessA(NULL, sConLin, NULL, NULL, TRUE, NULL, NULL, NULL, &sia, &_pi)) {
        GLERROR << "CreateProcess failed, error code:" << GetLastError();
        _msgcb(CREATE_MELT_FAILED);
        goto failed;
    }

    GLINFO << "create pipe and Melt Process success.";
    int ret = WaitForSingleObject(_pi.hProcess, INFINITE);
    if (ret == WAIT_OBJECT_0) {
        //the process has been finished and notify application and return.
        GLINFO << "Melt Service Process Exit Successfully.";
    }
    else if (ret == WAIT_FAILED) {
        //notify error occurs and return.
        GLINFO << "Waitfor Melt Service Process Failed, Error code: " << GetLastError();
    }
    
    GLINFO << "Melt Wroking thread exit";

failed:
    _bIsRunning = false;
    CloseHandle(_pi.hProcess);
    CloseHandle(_pipeInputWrite);
    //CloseHandle(_pipeOutputRead1);
    //CloseHandle(_pipeInputWrite1);
    if (!_bIsAsync) {
        if(_rwThread->joinable())
            _rwThread->join();
    }
    _rescb();
}

void MeltService::ReadWriteThread(void)
{
    GLINFO << "ReadWrite pipe thread start...";
    char buffer[512] = { 0 };
    DWORD bytesRead;
    int cnt = 0;
    while (true)
    {
        int res = ReadFile(_pipeOutputRead, buffer, sizeof(buffer), &bytesRead, NULL);
        if (!res && GetLastError() == ERROR_BROKEN_PIPE) {
            GLINFO << "the pipe's write handle has been closed, exit readwrite thread.";
            if (!_bIsRunning) {
                if (_bIsAsync) {//when generate the temp media file, don't need to report
                    _msgcb(SUCCESS);
                }
                break;
            }
            else {
                Stopmelt();
            }
        }
        if (!res || bytesRead == 0) {
            GLINFO << "there is no data come from pipe or other error occurs, error code: " << GetLastError();
            break;
        }
        if (cnt++ == 5) {//to control the percentage info parsing frequency.
            cnt = 0;
            //GLINFO << "bytesRead: " << bytesRead;
            std::string s(std::begin(buffer), std::begin(buffer) + bytesRead);
            //GLINFO << "extract string: " << s;
            unsigned int index = s.find("percentage:");
            if (index != std::string::npos) {
                if ((index + std::strlen("percentage:") + 11) <= s.length()) {//this means the distance between 'p' and the last character.
                    std::string tmpstr;
                    tmpstr = s.substr(index + std::strlen("percentage:"), 11);
                    int percent = atoi(tmpstr.c_str());
                    if (percent != _previousPercent && _bIsAsync)
                        _progresscb(percent);
                    _previousPercent = percent;
                    //GLINFO << "melt output current percentage: " << percent << "%";
                }
            }
        }
    }
    GLINFO << "Melt ReadWrite thread exit and IsWorking thread running: " << _bIsRunning;
    CloseHandle(_pipeOutputRead);
}
