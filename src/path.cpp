#include"path.hpp"

#include<string>

namespace csup
{
    namespace path
    {
        void pull(std::string& path)
        {
            size_t begin = 0;

            if(path[0] == '.' && path[1] == '/') {
                begin = 2;
            }

            const size_t pos = path.find_first_of('/', begin);
            path = path.substr(pos + 1, path.size());
        }

        void replace_ext(std::string& path, const std::string& ext)
        {
            const size_t size = path.find_last_of('.');

            //拡張子を除く
            const std::string no_ext = path.substr(0, size);

            path = no_ext + ext;
        }

        void globaling(std::string& path)
        {
            for(size_t i = 0; i < path.size(); i++)
            {
                if(path[i] == '\\') {
                    path[i] = '/';
                }
            }
        }
    }
}
