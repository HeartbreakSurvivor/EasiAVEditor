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
    if (num == 0 || num == 1) {
        exit(1);
    }
}

std::string add_geometry(std::string x_ratio, std::string y_ratio, std::string width_ratio, std::string height_ratio)
{
    float x = std::stof(x_ratio) * 100;
    float y = std::stof(y_ratio) * 100;
    float width = std::stof(width_ratio) * 100;
    float height = std::stof(height_ratio) * 100;
    
    //accurate to the second decimal place.
    auto f = [&](std::string str) {
        int decimalpoint = str.find('.');
        return str.substr(0, decimalpoint + 3);
    };

    std::string geometry("");
    geometry.append(f(std::to_string(x)));
    geometry.append("%/");
    geometry.append(f(std::to_string(y)));
    geometry.append("%:");
    geometry.append(f(std::to_string(width)));
    geometry.append("%x");
    geometry.append(f(std::to_string(height)));
    geometry.append("%");
    return geometry;
}

float timeformat_convert(const std::string & timestr)
{
    //"00:00:34.3820000"
    float total_seconds;
    int first_colon = timestr.find(':');
    int second_colon = timestr.find(':', first_colon + 1);
    int hour = std::stoi(timestr.substr(0, first_colon));
    int minute = std::stoi(timestr.substr(first_colon + 1, second_colon));
    float second = std::stof(timestr.substr(second_colon + 1, timestr.size()));

    total_seconds = hour * 3600 + minute * 60 + second;
    return total_seconds;
}

int main()
{
    //13.91% / 9.44%:83.83%x86.81%
    std::string halo =  add_geometry("1.202312321", "-1.731231230", "0.7812312", "0.3231123120");
    cout << halo << endl;

    //float seconds = timeformat_convert("00:20:34.3820000");
    //cout << seconds << endl;

    string cmd;
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
        std::cin >> cmd;
        if (cmd == "quit") {
            std::cout << "user quit melt service." << std::endl;
            av_editor_stop();
        }
        Sleep(5000);
    }
}