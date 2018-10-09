#include <iostream>
#include <ctime>

#include "TZinfo.h"

std::string create_response_ctime(std::string z, int o){
    using namespace std;
    string result;
    time_t t = time(0) + o;
    result += ctime(&t);
    result.insert(result.size() - 5, z + " ");
    return result;
}


int main(int argc, char ** argv)
{
    using std::string;
    using std::cout;
    using std::cin;
    using std::endl;

    string zname = "";
    TZinfo ti;

    if(ti.load_base("tzones.txt") < 0){
        cout << "Loading timezones info file fault!" << endl
        << "Exiting the program." << endl;
        return 1;
    }else if (ti.get_count() > 0){
        cout << "Loading complete! Total " << ti.get_count() << " zones loaded." << endl;
    }else{
        cout << "Info-file found, but nothing loaded." << endl
        << "Exiting the program." << endl;
        return 0;
    }


    while (zname != "e"){
        cout << "Enter zone name: ";
        cin >> zname;
        if (ti.find_zone(zname) > 0){
            cout << create_response_ctime(zname, ti.get_offset(zname)) << endl;
        }
    }

    return 0;
}
