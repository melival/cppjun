#ifndef _TZINFO_HEAD_
#define _TZINFO_HEAD_

#include <iostream>
#include <fstream>
#include <map>

class TZinfo  // Class for timezones info loading
{
private:
    std::map <std::string,int> time_zones;
    int count = 0;
public:
	TZinfo(){}
	TZinfo(std::string fname){ load_base(fname); }

	int load_base(std::string);

	int get_count() {return count;}

	int get_offset(std::string name){ return time_zones[name]; }

	int find_zone(std::string name){ return (int) time_zones.count(name); }

	~TZinfo(){}
};

#endif
