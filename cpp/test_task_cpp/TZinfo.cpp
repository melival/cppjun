#include "TZinfo.hpp"

int TZinfo::load_base(std::string fname){
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

