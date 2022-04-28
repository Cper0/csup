#pragma once

#include<unordered_map>
#include<string>
#include<chrono>
#include<vector>

namespace csup
{
    class timestamp
    {
        public:
            timestamp();
            timestamp(const std::string& p);

            void read();

            void write();

            std::vector<std::string> filter_matched_path(const std::vector<std::string>& paths);

            const std::unordered_map<std::string, time_t>& get() const noexcept { return this->map; }

        private:
            std::string path;
            std::unordered_map<std::string, time_t> map;

            template<class T> static void to_buffer(T value, size_t size, uint8_t* buffer);
            template<class T> static void to_integer(const uint8_t* buffer, size_t size, T& result);
    };
}
