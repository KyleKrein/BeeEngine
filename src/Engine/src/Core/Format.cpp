//
// Created by alexl on 08.10.2023.
//

#include "Format.h"
#include <algorithm>
#include <sstream>

namespace BeeEngine::Internal
{
    UTF8String CustomFormat_Impl(const UTF8String& format_str, const std::vector<UTF8String>& args)
    {
        std::ostringstream result;
        std::size_t pos = 0;
        std::size_t next_arg = 0;

        while (pos < format_str.size())
        {
            if (format_str[pos] == '{')
            {
                std::size_t end_pos = format_str.find('}', pos);
                if (end_pos != String::npos)
                {
                    String index_str = format_str.substr(pos + 1, end_pos - pos - 1);
                    if (index_str.empty())
                    {
                        // Handle unindexed placeholders like {}
                        if (next_arg < args.size())
                        {
                            result << args[next_arg++];
                        }
                        else
                        {
                            result << "{}"; // Not enough arguments, leave it as is
                        }
                    }
                    else if (std::all_of(index_str.begin(), index_str.end(), ::isdigit))
                    {
                        // Handle indexed placeholders like {0}, {1}, etc.
                        int index = std::stoi(index_str.c_str());
                        if (index < static_cast<int>(args.size()))
                        {
                            result << args[index];
                        }
                        else
                        {
                            result << "{" << index_str << "}"; // Not enough arguments, leave it as is
                        }
                    }
                    else
                    {
                        result << "{" << index_str << "}"; // Invalid format, leave it as is
                    }
                    pos = end_pos + 1;
                }
                else
                {
                    result << "{"; // No closing '}', leave it as is
                    ++pos;
                }
            }
            else
            {
                result << format_str[pos];
                ++pos;
            }
        }
        return String{result.str()};
    }
} // namespace BeeEngine::Internal