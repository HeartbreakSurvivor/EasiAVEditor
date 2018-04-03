#pragma once
#include <string>
#include <thread>
#include <windows.h>
#include <shellapi.h>
#include <tchar.h>

typedef void(*progresscbfun)(int);
typedef void(*resultcbfun)(int);

class MeltService
{
public:
    MeltService();
    ~MeltService();

    bool Startmelt(const std::string &para);
    bool Stopmelt();
    void ProgressReport_cbfun(progresscbfun func);
    void ErrorReport_cbfun(resultcbfun func);

private:
    std::wstring Get_melt_Path();
    void WorkingThread(const std::wstring & paras);

private:
    std::unique_ptr<std::thread> _thread;
    progresscbfun _progresscb;
    resultcbfun _resultcb;
};
