#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
#include<vector>
#include<unordered_map>
#include<boost/filesystem.hpp>
#include<boost/chrono.hpp>

#include"timestamp.hpp"
#include"files.hpp"
#include"environment.hpp"
#include"dependency.hpp"

struct options
{
    bool put_info;
    std::string project_name;
    std::string src_dir;
    std::string inc_dir;
    std::string lib_dir;
    std::string obj_dir;
    std::vector<std::string> libs;

    std::string to_gpp_options() const;
};

std::string options::to_gpp_options() const
{
    std::stringstream stream;
    stream << "-I" << inc_dir << " -L" << lib_dir;

    for(auto it = this->libs.begin(); it != this->libs.end(); it++)
    {
        stream << " -l" << *it;
    }

    return stream.str();
}

bool perse_args(int32_t size, char** args, options& opt)
{
    for(int32_t i = 1; i < size; i++)
    {
        const std::string a = args[i];

        if(a[0] == '-') {
            const size_t pos = a.find('=');
            if(pos == a.npos) {
                std::cout << ">> invalid property expression." << std::endl;
                return false;
            }

            const std::string property  = a.substr(1, pos - 1);
            const std::string value     = a.substr(pos + 1, a.size());

            if(property.compare("Sd") == 0) {
                opt.src_dir = value;
            }
            else if(property.compare("Id") == 0) {
                opt.inc_dir = value;
            }
            else if(property.compare("Ld") == 0) {
                opt.lib_dir = value;
            }
            else if(property.compare("l") == 0) {
                opt.libs.emplace_back(value);
            }
            else if(property.compare("D") == 0) {
                opt.put_info = true;
            }
            else if(property.compare("P") == 0 ) {
                opt.project_name = value;
            }
            else {
                std::cout << ">> invalid property name." << std::endl;
                return false;
            }

            continue;
        }

        std::cout << ">> there is no defined property." << std::endl;
        return false;
    }
    
    return true;
}

void dump_targets(const std::vector<std::string>& targets)
{
    std::cout << "--- targets ---" << std::endl;
    for(auto it = targets.begin(); it != targets.end(); it++)
    {
        std::cout << *it << std::endl;
    }
    std::cout << "---------------" << std::endl;
}

void dump_timestamp(const std::unordered_map<std::string, time_t>& timestamp)
{
    std::cout << "--- timestamps ---" << std::endl;
    for(auto it = timestamp.begin(); it != timestamp.end(); it++)
    {
        std::cout << it->first << ":" << it->second << std::endl;
    }
    std::cout << "------------------" << std::endl;
}

int main(int32_t argc, char** argv)
{
    //?????????????????????
    options opt = {false,"main","./src","./include","./lib","./obj",{}};
    if(!perse_args(argc, argv, opt)) {
        return 0;
    }

    csup::environment env = csup::environment(opt.project_name, opt.src_dir, opt.inc_dir, opt.lib_dir, opt.obj_dir);
    env.libs() = opt.libs;

    /*
    csup::dependency dep  = csup::dependency();
    dep.emplace(csup::files::get_sources(opt.src_dir));
    */

    //????????????????????????????????????
    if(!env.exists()) {
        std::cout << "[" << opt.src_dir << "," << opt.inc_dir << "," << opt.lib_dir << "," <<  opt.obj_dir << "]" << std::endl;
        std::cout << ">> doesnt exist some directories to exist." << std::endl;
        return 1;
    }

    std::cout << "start compiling" << std::endl;

    if(env.compile()) {
        return 1;
    }

    std::cout << "finished compiling" << std::endl;

    std::cout << "start linking" << std::endl;

    env.link();

    return 0;
}

