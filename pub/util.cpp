#include "util.h"

std::string GetFilename(const std::string &fullname)
{
    std::string filename(fullname);
    auto pos = fullname.find_last_of(".");
    if(pos != std::string::npos)
    {
        filename = fullname.substr(0, pos);
    }
    return filename;
}
