#ifndef _TIMEZONE_INFO_
#define _TIMEZONE_INFO_

#include <iostream>
#include <fstream>

//#include "TZinfo.h"
#include <map>

class TZinfo  // Class for timezones info loading
{
private:
    std::map <std::string,int> time_zones;
    int count = 0;

public:
	TZinfo(){}

	TZinfo(std::string fname){
		load_base(fname);
	}

	int load_base(std::string fname){ //Constructor
		std::string tzname;
		int tzhours = 0;
		int tzmins = 0;

		std::ifstream tz_file(fname);

		if (!tz_file.is_open()){
		    return -1;
		}else {
		    while (!tz_file.eof()){
		        tz_file >> tzname >> tzhours >> tzmins;
		        if ((tzname == "")||(tzname == "\n")) continue;
		        tzmins = tzhours * 60 + tzmins * ((tzhours < 0)?-1:1);
		        time_zones[tzname] = tzmins * 60; //in seconds offset
		        count++;
		    }
		    tz_file.close();
		}
		return count;
	}

	int get_count() {return count;}

	int get_offset(std::string name){
		return time_zones[name];
	}

	int find_zone(std::string name){
		return (int) time_zones.count(name);
	}

	~TZinfo(){}
};
#endif
