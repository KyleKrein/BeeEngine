//
// Created by alexl on 05.06.2023.
//

#pragma once
#include <string>
#include <optional>

class TestFileDialogs;
namespace BeeEngine
{
    class FileDialogs
    {
    private:
        static std::string GetFilter(void* filter);

    public:
        struct Filter
        {
            friend std::string FileDialogs::GetFilter(void* filter);
            friend class ::TestFileDialogs;
            const char* name;
            const char* filter;
#if defined(WINDOWS)
            Filter(const char* name, const char* filter)
                    : name(name), filter(filter)
            {}

        private:
            std::string WindowsFilter()
            {
                return std::string(name + std::string(" (") + filter + std::string(")") + '\0' + filter + '\0');
            }
#endif

        };
    public:
        static std::string OpenFile(Filter filter);
        static std::string SaveFile(Filter filter);
    private:
        /*static void CheckFilter(Filter& filter)
        {
            BeeExpects(filter.name != nullptr);
            BeeExpects(filter.filter != nullptr);
            BeeExpects(filter.name[0] != '\0');
            BeeExpects(filter.filter[0] != '\0');
            BeeExpects(filter.filter[0] == '*' && filter.filter[1] == '.');
            const auto lenFilter = strlen(filter.filter);

            //TODO: finish writing this
        }*/
    };
}
