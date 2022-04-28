#include"timestamp.hpp"

#include<iostream>
#include<fstream>
#include<cstring>
#include<sstream>
#include<vector>
#include<string>
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
        std::ifstream stream(this->path, std::ios::binary);
        if(!stream) {
            std::cout << "couldnt open " << this->path << std::endl;
            return;
        }

        const size_t size   = boost::filesystem::file_size(this->path);
        char* buffer        = new char[size];

        stream.read(buffer, size);

        size_t i = 0;
        for(;;)
        {
            uint16_t length = 0;
            to_integer(reinterpret_cast<uint8_t*>(&buffer[i]), 2, length);
            i += 2;

            if(length == 0) {
                break;
            }

            std::string str(&buffer[i], length);
            i += length;

            time_t time = 0;
            to_integer(reinterpret_cast<uint8_t*>(&buffer[i]), 8, time);
            i += 8;

            this->map[str] = time;
        }

        delete[] buffer;

        stream.close();
    }

    void timestamp::write()
    {
        std::ofstream stream(this->path, std::ios::binary);
        if(!stream) {
            return;
        }

        for(auto it = this->map.begin(); it != this->map.end(); it++)
        {
            const size_t size = 2 + it->first.length() + sizeof(time_t);
            uint8_t* buffer = new uint8_t[size];
            std::memset(buffer, 0, size);

            const unsigned short length = it->first.length();

            to_buffer(length, 2, &buffer[0]);
            std::memcpy(&buffer[2], it->first.c_str(), length);
            to_buffer(it->second, 8, &buffer[2 + length]);

            stream.write(reinterpret_cast<char*>(buffer), size);

            delete[] buffer;
        }

        char end[2] = {0,0};
        stream.write(end, 2);

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

    template<class T> void timestamp::to_buffer(T value, size_t size, uint8_t* buffer)
    {
        for(size_t i = 0; i < size; i++)
        {
            const uint8_t shift = i << 3;
            buffer[i] = (value >> shift) & 0xFF;
        }
    }

    template<class T> void timestamp::to_integer(const uint8_t* buffer, size_t size, T& result)
    {
        for(size_t i = 0; i < size; i++)
        {
            if(i > 0) {
                result <<= 8;
            }
            
            result |= buffer[size - 1 - i];
        }
    }
}
