#ifndef _CREATE_RESPONSE_TZTIME_
#define _CREATE_RESPONSE_TZTIME_

#include <boost/format.hpp>

namespace tztime_cr {
	const std::string week[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
	const std::string month[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul",
								"Aug", "Sep", "Oct", "Nov", "Dec"};

	std::string make(std::string tz_name, int tz_offset){
		//std::string result;
		time_t rawtime;
		time(&rawtime);
		rawtime += tz_offset;
		struct tm *t = gmtime(&rawtime);
		return str(
		//result = str(
			boost::format(
			"%1% %2% %3$02d %4$02d:%5$02d:%6$02d %7% %8%\n")
			% tztime_cr::week[t->tm_wday]
			% tztime_cr::month[t->tm_mon]
			% t->tm_mday
			% t->tm_hour % t->tm_min % t->tm_sec
			% tz_name % (t->tm_year + 1900)
		);
		//return result;
	}
}
#endif
