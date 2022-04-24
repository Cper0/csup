#pragma once

#include<string>
#include<vector>

namespace csup
{
    class environment
    {
        public:
            environment();
            environment(const std::string& proj_name, const std::string& src_dir, const std::string& inc_dir, const std::string& lib_dir, const std::string& obj_dir);

            bool exists() const;

            std::vector<std::string> objects_paths(const std::vector<std::string>& sources) const;

            bool compile() const;

            bool link() const;

            std::string to_inc_opt() const;
            std::string to_lib_opt() const;

            std::vector<std::string>& libs() noexcept { return this->libraries; }

        private:
            std::string project_name;
            std::string source_dir;
            std::string include_dir;
            std::string library_dir;
            std::string object_dir;
            std::vector<std::string> libraries;
    };
}
