#include <map>
class TZinfo  // Class for timezones info loading
{
private:
    std::map <std::string,int> time_zones;
    int count = 0;

public:
    TZinfo();
    TZinfo(std::string fname);
    int load_base(std::string fname);
    int get_count();
    int get_offset(std::string name);
    int find_zone(std::string name);
    ~TZinfo();
};
