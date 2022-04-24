#include"timestamp.hpp"

#include<iostream>
#include<fstream>
#include<vector>
#include<boost/filesystem.hpp>

namespace csup
{
    timestamp::timestamp() : path(), map()
    {
        
    }

    timestamp::timestamp(const std::string& p) : path(p), map()
    {
        
    }

    void timestamp::read()
    {
        std::ifstream stream(this->path);
        if(!stream) {
            return;
        }

        std::string line;
        for(;std::getline(stream, line);)
        {
            const size_t pos = line.find(' ');

            const std::string target_path       = line.substr(0, pos);
            const std::string time_write_last   = line.substr(pos + 1, line.size());

            this->map.emplace(target_path, std::stoull(time_write_last));
        }

        stream.close();
    }

    void timestamp::write() const
    {
        std::ofstream stream(this->path);
        if(!stream) {
            return;
        }

        for(auto it = this->map.begin(); it != this->map.end(); it++)
        {
            stream << it->first << " " << it->second << "\n";
        }

        stream.close();
    }

    std::vector<std::string> timestamp::filter_matched_path(const std::vector<std::string>& paths)
    {
        std::vector<std::string> result = {};

        for(auto it = paths.begin(); it != paths.end(); it++)
        {
            const boost::filesystem::path p = *it;

            time_t time = boost::filesystem::last_write_time(p);

            if(this->map[*it] != time) {
                result.emplace_back(*it);
                this->map[*it] = time;
            }
        }

        return result;
    }
}
