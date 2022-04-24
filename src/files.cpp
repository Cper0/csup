#include<vector>
#include<string>
#include<sstream>
#include<boost/filesystem.hpp>

namespace csup
{
    namespace files
    {
        std::vector<std::string> get_sources(const std::string& dir)
        {
            std::vector<std::string> result = {};
            auto it = boost::filesystem::recursive_directory_iterator(dir);

            for(auto e : it)
            {
                if(e.path().extension().string() != ".cpp") {
                    continue;
                }

                result.emplace_back(e.path().string());
            }

            return result;
        }

        std::vector<std::string> get_headers(const std::string& dir)
        {
            std::vector<std::string> result = {};
            auto it = boost::filesystem::recursive_directory_iterator(dir);

            for(auto e : it)
            {
                const std::string ext = e.path().extension().string();

                if(ext != ".hpp" && ext != ".h") {
                    continue;
                }

                result.emplace_back(e.path().string());
            }

            return result;
        }

        void convert_ext_o(std::vector<std::string>& paths)
        {
            for(auto it = paths.begin(); it != paths.end(); it++)
            {
                boost::filesystem::path p = *it;
                p.replace_extension(".o");

                *it = p.string();
            }
        }
    }
}