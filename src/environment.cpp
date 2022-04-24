#include"environment.hpp"

#include<iostream>
#include<string>
#include<vector>
#include<sstream>
#include<boost/filesystem.hpp>

#include"timestamp.hpp"
#include"files.hpp"

namespace csup
{
    environment::environment() : libraries()
    {
        this->project_name  = "main";
        this->source_dir    = "src";
        this->include_dir   = "include";
        this->library_dir   = "lib";
        this->object_dir    = "objs";
    }

    environment::environment(const std::string& proj_name, const std::string& src_dir, const std::string& inc_dir, const std::string& lib_dir, const std::string& obj_dir) : libraries()
    {
        this->project_name  = proj_name;
        this->source_dir    = src_dir;
        this->include_dir   = inc_dir;
        this->library_dir   = lib_dir;
        this->object_dir    = obj_dir;
    }

    bool environment::exists() const
    {
        return boost::filesystem::exists(this->source_dir) && boost::filesystem::exists(this->include_dir) && boost::filesystem::exists(this->library_dir) && boost::filesystem::exists(this->object_dir);
    }

    std::vector<std::string> environment::objects_paths(const std::vector<std::string>& sources) const
    {
        std::vector<std::string> result = {};

        for(auto it = sources.begin(); it != sources.end(); it++)
        {
            boost::filesystem::path p = *it;
            p.replace_extension(".o");

            result.emplace_back(this->object_dir + "/" + p.filename().string());
        }

        return result;
    }

    bool environment::compile() const
    {
        csup::timestamp ts = csup::timestamp(".csup");
        ts.read();

        std::vector<std::string> sources    = ts.filter_matched_path(csup::files::get_sources(this->source_dir));
        std::vector<std::string> objects    = this->objects_paths(sources);

        for(size_t i = 0; i < sources.size(); i++)
        {
            std::stringstream stream;
            stream << "g++ " << this->to_inc_opt() << "-c " << sources[i] << " -o " << objects[i];

            std::cout << stream.str() << std::endl;

            if(system(stream.str().c_str())) {
                return true;
            }
        }

        ts.write();

        return false;
    }

    bool environment::link() const
    {
        std::vector<std::string> objects = this->objects_paths(csup::files::get_sources(this->source_dir));
        std::stringstream stream;
        
        stream << "g++ ";

        for(auto it = objects.begin(); it != objects.end(); it++)
        {
            stream << *it << " ";
        }

        stream << "-o " << this->project_name << " " << this->to_lib_opt();

        std::cout << stream.str() << std::endl;

        system(stream.str().c_str());

        return false;
    }

    std::string environment::to_inc_opt() const
    {
        std::stringstream stream;
        stream << "-I" << this->include_dir << " ";

        return stream.str();
    }

    std::string environment::to_lib_opt() const
    {
        std::stringstream stream;
        stream << "-L" << this->library_dir << " ";

        for(auto it = this->libraries.begin(); it != this->libraries.end(); it++)
        {
            stream << "-l" << *it << " ";
        }

        return stream.str();
    }
}
