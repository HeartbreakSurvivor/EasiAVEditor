#include <iostream>
#include <string>
#include <time.h>

using namespace std;

void attach_video_fadein_filter(const std::string & in, const std::string & out)
{
    //std::string str(" -attach-clip brightness level=\"00:00:00.000=0; 00:00:00.000=1\" alpha=\"00:00:00.000=0; 00:00:00.000=1\"");
    //str.replace(32, 12, in);
    //str.replace(48, 12, out);
    //str.replace(73, 12, in);
    //str.replace(89, 12, out);

    std::string str(" -attach-clip volume gain=0 end=1 in=\"00:00:00.000\" out=\"00:00:00.000\"");
    str.replace(38, 12, in);
    str.replace(57, 12, out);

    std::cout << str << std::endl;
}

int main()
{
    int f = 23;
    double f2 = 1e-9;
    double f3 = 1e40;
    double f4 = 1e-40;
    double f5 = 123456789;
    std::string f_str = std::to_string(f);
    std::string f_str2 = std::to_string(f2); // Note: returns "0.000000"
    std::string f_str3 = std::to_string(f3); // Note: Does not return "1e+40".
    std::string f_str4 = std::to_string(f4); // Note: returns "0.000000"
    std::string f_str5 = std::to_string(f5);
    std::cout << "std::cout: " << f << '\n'
        << "to_string: " << f_str << "\n\n"
        << "std::cout: " << f2 << '\n'
        << "to_string: " << f_str2 << "\n\n"
        << "std::cout: " << f3 << '\n'
        << "to_string: " << f_str3 << "\n\n"
        << "std::cout: " << f4 << '\n'
        << "to_string: " << f_str4 << "\n\n"
        << "std::cout: " << f5 << '\n'
        << "to_string: " << f_str5 << '\n';

    std::string zoom;
    for (int i = 1; i < 5; ++i) {
        std::string str(" -transition mix:-1 always_active=1 a_track=0 b_track=1 sum=1");
        str.replace(54, 1, std::to_string(i));
        zoom.append(str);
        if (i < 4) {
            std::string str(" -transition frei0r.cairoblend a_track=0 b_track=1 disable=0");
            str.replace(49, 1, std::to_string(i));
            zoom.append(str);
        }
    }

    std::string str(" -consumer xml:");
    //get current time and form the mlt file name.
    struct tm t;   //tm结构指针
    time_t timep;
    time(&timep);
    localtime_s(&t, &timep);   //获取当地日期和时间
    char tmp[64];
    strftime(tmp, sizeof(tmp), "jjs-%Y-%m-%d-%H-%M-%S.mlt", &t);
    str.append(tmp);


    std::cout << str << std::endl;
    std::cout << zoom << std::endl;

    attach_video_fadein_filter("02:23:32.000","00:03:34.843");
    while (true)
    {

    }
}