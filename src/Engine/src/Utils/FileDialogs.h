//
// Created by alexl on 05.06.2023.
//

#pragma once
#include "Core/Path.h"
#include "Core/String.h"

class TestFileDialogs;

namespace BeeEngine
{
    class FileDialogs
    {
    private:
        static String GetFilter(void* filter);

    public:
        struct Filter
        {
            friend String FileDialogs::GetFilter(void* filter);
            friend class ::TestFileDialogs;
            const char* name;
            const char* filter;
            Filter(const char* name, const char* filter) : name(name), filter(filter) {}
#if defined(WINDOWS)
        private:
            String WindowsFilter() { return String(name + String(" (") + filter + String(")") + '\0' + filter + '\0'); }
#endif
        };

    public:
        static Path OpenFile(Filter filter);
        static Path SaveFile(Filter filter);
        static Path OpenFolder();

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
} // namespace BeeEngine
