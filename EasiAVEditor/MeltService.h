#pragma once
#include <string>
#include <thread>
#include <windows.h>
#include <shellapi.h>
#include <tchar.h>

typedef void(*progresscbfun)(int);
typedef void(*msgcbfun)(int);

class MeltService
{
public:
    MeltService();
    ~MeltService();

    bool Startmelt(const std::string &para);
    bool AsyncStartmelt(const std::string &para);
    bool Stopmelt();
    void ProgressReport_cbfun(progresscbfun func);
    void MsgReport_cbfun(msgcbfun func);

private:
    std::wstring Get_melt_Path();
    void WorkingThread(const std::wstring & paras);
    
private:
    std::unique_ptr<std::thread> _thread;
    progresscbfun _progresscb = nullptr;
    msgcbfun _msgcb = nullptr;
    HANDLE _pipeOutputRead, _pipeInputWrite;
    PROCESS_INFORMATION _pi;
    int _prevousPercent;
    bool _bIsRunning;
};

