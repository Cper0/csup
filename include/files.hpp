#pragma once

#include<vector>
#include<string>

namespace csup
{
    namespace files
    {
        std::vector<std::string> get_sources(const std::string& dir);

        std::vector<std::string> get_headers(const std::string& dir);

        void convert_ext_o(std::vector<std::string>& paths);
    }
}