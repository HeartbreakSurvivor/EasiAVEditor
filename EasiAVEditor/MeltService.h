#pragma once
#include <string>
#include <thread>
#include <windows.h>
#include <shellapi.h>
#include <tchar.h>
#include <atomic>

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
    void ReadWriteThread(void);
    
private:
    std::unique_ptr<std::thread> _rwThread;
    std::unique_ptr<std::thread> _wkThread;
    progresscbfun _progresscb = nullptr;
    msgcbfun _msgcb = nullptr;
    HANDLE _pipeOutputRead, _pipeInputWrite;
    //HANDLE _pipeOutputRead1, _pipeInputWrite1;
    PROCESS_INFORMATION _pi;
    int _previousPercent;
    std::atomic_bool _bIsRunning;
    std::atomic_bool _bIsAsync;
};

