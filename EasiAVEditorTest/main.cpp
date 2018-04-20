#include "EasiAVEditor.h"
#include <iostream>
#include <string>
#include <time.h>
#include <stdio.h>

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

std::string attach_video_scale_filter(std::string x_ratio, std::string y_ratio, std::string width_ratio, std::string height_ratio)
{
    //-attach-clip affine background=colour:0 transition.geometry="0.00%/0.00%:86.02%x85.97%"
    //std::string str(" -attach-clip affine background=colour:0 transition.geometry=\"00.00%/00.00%:00.00%x00.00%\"");
    std::string str(" -attach-clip affine background=colour:0 transition.geometry=\"");
    std::string geometry = add_geometry(x_ratio, y_ratio, width_ratio, height_ratio);
    str.append(geometry);
    str.append("\"");
    return str;
}

float timeStr2second(const std::string & timestr)
{
    float total_seconds;
    int first_colon = timestr.find(':');
    int second_colon = timestr.find(':', first_colon + 1);
    int hour = std::stoi(timestr.substr(0, first_colon));
    int minute = std::stoi(timestr.substr(first_colon + 1, second_colon));
    float second = std::stof(timestr.substr(second_colon + 1, timestr.size()));

    total_seconds = hour * 3600 + minute * 60 + second;
    return total_seconds;
}

std::string second2timeStr(float seconds)
{
    std::string str;
    char buf[10];
    unsigned int millisecond = (unsigned int)(seconds * 1000);
    int hour = seconds / 3600000;
    int minute = millisecond / 60000 % 60000;
    //int second = (millisecond % 60000) / 1000;
    float second = (float)(millisecond % 60000)/1000;

    std::cout << "hour: " << hour << "  minute: " << minute << "  second: " << second << std::endl;
    sprintf_s(buf, sizeof(buf), "%02d", hour);
    str.append(buf);
    str.append(":");
    sprintf_s(buf, sizeof(buf), "%02d", minute);
    str.append(buf);
    str.append(":");
    sprintf_s(buf, sizeof(buf), "%.3f", second);
    str.append(buf);
    return str;
}

bool isImagefile(std::string str)
{
    std::string substr(".jpg");
    std::string substr1(".png");
    std::string substr2(".jpeg");
    if (str.rfind(substr) == (str.length() - substr.length()) ||
        str.rfind(substr1) == (str.length() - substr1.length()) ||
        str.rfind(substr2) == (str.length() - substr2.length())) {
        return true;
    }
    return false;
}


int main()
{
    //13.91% / 9.44%:83.83%x86.81%
    //std::string halo =  add_geometry("1.202312321", "-1.731231230", "0.7812312", "0.3231123120");
    //cout << halo << endl;
    //52.42%/50.14%:47.73%x53.06%
    //halo = attach_video_scale_filter("0.5242312312321", "0.501431231230", "0.477327812312", "0.530623231123120");
    //cout << halo << endl;
    //float seconds = timeStr2second("00:01:34.3820000");
    //cout << seconds << endl;
    //std::string str = second2timeStr(seconds);

    //std::string fadeoutstart = second2timeStr(timeStr2second("00:01:34.3820000") - timeStr2second("00:00:1.5000000"));

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