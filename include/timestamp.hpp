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

            void write() const;

            std::vector<std::string> filter_matched_path(const std::vector<std::string>& paths);

            std::unordered_map<std::string, time_t>& get() noexcept { return this->map; }

        private:
            std::string path;
            std::unordered_map<std::string, time_t> map;
    };
}
