#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
#include<vector>
#include<unordered_map>
#include<chrono>
#include<boost/filesystem.hpp>
#include<boost/chrono.hpp>

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

std::vector<std::string> get_sources(std::string path)
{
    std::vector<std::string> src = {};

    auto e = boost::filesystem::recursive_directory_iterator(path);

    for(auto f : e)
    {
        if(f.path().extension().string().compare(".cpp") != 0) {
            continue;
        }

        src.emplace_back(f.path().string());
    }

    return src;
}

bool compile_to_object(const std::vector<std::string>& src, const options& opt, std::vector<std::string>& obj)
{
    for(auto it = src.begin(); it != src.end(); it++)
    {
        const boost::filesystem::path p = *it;
        const std::string obj_path      = opt.obj_dir + "/" + p.filename().stem().string() + ".o";

        obj.emplace_back(obj_path);

        std::stringstream stream;
        stream << "g++ " << opt.to_gpp_options() << " -c " << p.string() << " -o " << obj_path; 

        if(opt.put_info) std::cout << "compiling>> " << stream.str() << std::endl;

        if(system(stream.str().c_str()) != 0) {
            return false;
        }
    }

    return true;
}

bool link_objects(const std::vector<std::string>& obj, const options& opt)
{
    std::stringstream targets;
    for(auto it = obj.begin(); it != obj.end(); it++)
    {
        targets << *it << " ";
    }

    const std::string command = "g++ " + targets.str() + "-o " + opt.project_name + " " + opt.to_gpp_options();

    system(command.c_str());

    return true;
}

bool filter_with_timestamp(std::unordered_map<std::string, time_t>& timestamps, std::vector<std::string>& sources)
{
    for(auto it = sources.begin(); it != sources.end(); )
    {
        boost::filesystem::path p(*it);

        const time_t time = boost::filesystem::last_write_time(p);

        //std::cout << ">> " << *it << "," << p.string() << std::endl;
        //std::cout << ">> " << timestamps[*it] << "," << time << std::endl;

        if(timestamps[*it] == time) {
            it = sources.erase(it);
            continue;
        }

        timestamps[*it] = time;

        it++;
    }

    return true;
}

bool read_timestamp(std::unordered_map<std::string, time_t>& times)
{
    std::ifstream stream(".csup");
    if(!stream) {
        stream.close();
        return false;
    }

    std::string line;
    for(; std::getline(stream, line);)
    {
        const size_t pos = line.find(' ');

        const std::string path              = line.substr(0, pos);
        const std::string last_write_time   = line.substr(pos + 1, line.size());

        const time_t time                   = std::stoull(last_write_time);

        times.emplace(path, time);
    }

    stream.close();

    return true;
}

bool write_timestamps(const std::unordered_map<std::string, time_t>& times)
{
    std::ofstream stream(".csup");

    for(auto it = times.begin(); it != times.end(); it++)
    {
        stream << it->first << " " << it->second << "\n";
    }
    
    stream.close();
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
    bool error = false;

    std::unordered_map<std::string, time_t> timestamps = {};
    read_timestamp(timestamps);

    //オプション解析
    options opt = {false,"main","./src","./include","./lib","./obj",{}};
    if(!perse_args(argc, argv, opt)) {
        return 0;
    }
    
    //ディレクトリが存在するか
    if(!boost::filesystem::exists(opt.src_dir) || !boost::filesystem::exists(opt.inc_dir) || !boost::filesystem::exists(opt.lib_dir)) {
        std::cout << "[" << opt.src_dir << "," << opt.inc_dir << "," << opt.lib_dir << "," <<  opt.obj_dir << "]" << std::endl;
        std::cout << ">> doesnt exist some directories to exist." << std::endl;
        return 0;
    }

    //ソースファイルパスの取得
    std::vector<std::string> sources = get_sources(opt.src_dir);
    if(sources.size() == 0) {
        std::cout << "'" << opt.src_dir << "' -> exists no sources error." << std::endl;
        return 0;
    }

    dump_targets(sources);
    dump_timestamp(timestamps);

    filter_with_timestamp(timestamps, sources);
    if(sources.size() == 0) {
        std::cout << ">> no written file." << std::endl;
        return 1;
    }

    std::vector<std::string> objects = {};
    if(!compile_to_object(sources, opt, objects)) {
        std::cout << ">> compiling error." << std::endl;
        return 0;
    }

    if(!link_objects(objects, opt)) {
        std::cout << ">> linking error." << std::endl;
        return 0;
    }

    write_timestamps(timestamps);

    return 1;
}

