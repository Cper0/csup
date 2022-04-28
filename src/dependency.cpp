#include"dependency.hpp"

#include<iostream>
#include<unordered_map>
#include<vector>
#include<string>
#include<fstream>
#include<regex>
#include<boost/filesystem.hpp>

#include"timestamp.hpp"

namespace csup
{
    dependency::dependency() : map()
    {

    }

    std::vector<std::string> dependency::depended_files(const std::vector<std::string>& headers, csup::timestamp& ts)
    {
        std::vector<std::string> result = {};

        std::vector<std::string> filtered = ts.filter_matched_path(headers);
        for(auto it = filtered.begin(); it != filtered.end(); it++)
        {
            const std::string key = to_key(*it);

            if(this->map.find(key) != this->map.end()) {
                const std::vector<std::string> vec = this->map.at(key);
                merge(vec, result);
            }
        }

        return result;
    }

    void dependency::emplace(const std::vector<std::string>& sources)
    {
        for(auto it = sources.begin(); it != sources.end(); it++)
        {
            std::ifstream stream(*it);
            if(!stream) {
                std::cout << "couldn't open " << *it << std::endl;
                break;
            }

            const std::vector<std::string> including = extract_including(stream);
            for(auto inc = including.begin(); inc != including.end(); inc++)
            {
                this->map.emplace(*inc, std::vector<std::string>{});
                this->map[*inc].emplace_back(*it);
            }

            stream.close();
        }
    }

    std::vector<std::string> dependency::extract_including(std::ifstream& stream)
    {
        std::vector<std::string> result = {};
        
        std::regex r0(R"(#include<(.*)>)");
        std::regex r1(R"(#include\"(.*)\")");

        std::string line;
        for(; std::getline(stream, line);)
        {
            boost::filesystem::path path;

            std::smatch match;
            if(std::regex_match(line, match, r0)) {
                path = match[1].str();
            }
            else if(std::regex_match(line, match, r1)) {
                path = match[1].str();
            }
            else {
                continue;
            }

            std::string ext = path.extension().string();
            if(ext == ".h" || ext == ".hpp") {
                result.emplace_back(path.string());
            }
        }

        return result;
    }

    std::string dependency::to_string() const noexcept
    {
        std::stringstream stream;

        for(auto it = this->map.begin(); it != this->map.end(); it++)
        {
            stream << it->first << " - ";

            for(auto elm = it->second.begin(); elm != it->second.end(); elm++)
            {
                stream << *elm << ", ";
            }

            stream << "\n";
        }

        return stream.str();
    }

    std::string dependency::to_key(const std::string& path)
    {
        const size_t pos = path.find('\\');
        std::string s = path.substr(pos + 1, path.size());

        for(size_t i = 0; i < s.size(); i++)
        {
            if(s[i] == '\\') {
                s[i] = '/';
            }
        }

        return s;
    }

    void dependency::merge(const std::vector<std::string>& src, std::vector<std::string>& dst)
    {
        for(auto s = src.begin(); s != src.end(); s++)
        {
            bool matched = true;

            for(auto d = dst.begin(); d != dst.end(); d++)
            {
                if(s->compare(*d) == 0) {
                    matched = false;
                    break;
                }
            }

            if(matched) {
                dst.emplace_back(*s);
            }
        }
    }
}
