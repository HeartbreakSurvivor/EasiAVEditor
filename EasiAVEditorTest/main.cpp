#include "EasiAVEditor.h"
#include <iostream>
#include <string>
#include <time.h>

using namespace std;


void Progressreport(int percent)
{
    cout << "application current percentage: " << percent << endl;
}

void Msgreport(int num)
{
    cout << "melt service msg code: " << num << endl;
    if (num == -1 || num == -2) {
        //av_editor_stop();
        exit(1);
    }
}

int main()
{
    bool res = av_editor_create("A","B","C","D");
    if (!res) {
        std::cout << "easiaveditor create failed." << std::endl;
    }
    av_editor_register_progresscb(Progressreport);
    av_editor_register_msgcb(Msgreport);
    res = av_editor_start();
    if (!res) {
        std::cout << "easiaveditor start failed." << std::endl;
    }

    while (true)
    {
        Sleep(5000);
    }
}