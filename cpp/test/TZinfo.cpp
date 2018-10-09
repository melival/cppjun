#include <iostream>
#include <fstream>

#include "TZinfo.h"

TZinfo::TZinfo(){}
TZinfo::TZinfo(std::string fname){
	TZinfo::load_base(fname);
}

int TZinfo::load_base(std::string fname){ //Constructor
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
int TZinfo::get_count() {return count;}
int TZinfo::get_offset(std::string name){
	return time_zones[name];
}

int TZinfo::find_zone(std::string name){
	return (int) time_zones.count(name);
}

TZinfo::~TZinfo(){}

