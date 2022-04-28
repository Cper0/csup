#pragma once

#include<unordered_map>
#include<vector>
#include<string>
#include<fstream>

#include"timestamp.hpp"

namespace csup
{
    class dependency
    {
        public:
            dependency();

            void emplace(const std::vector<std::string>& sources);

            std::vector<std::string> depended_files(const std::vector<std::string>& headers, csup::timestamp& ts);

            std::string to_string() const noexcept;

            static std::string to_key(const std::string& path);

            static void merge(const std::vector<std::string>& src, std::vector<std::string>& dst);

        private:
            std::unordered_map<std::string, std::vector<std::string>> map;

            static std::vector<std::string> extract_including(std::ifstream& stream);
    };
}
