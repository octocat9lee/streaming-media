#include "util.h"

#include <stdio.h>

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

unsigned GetFileSize(const std::string &fullname)
{
    unsigned size = 0;
    FILE *fp = fopen(fullname.c_str(), "r");
    if(fp != nullptr)
    {
        fseek(fp, 0L, SEEK_END);
        size = ftell(fp);
        fclose(fp);
    }
    return size;
}

void ReadFile(const std::string &fullname,
    const unsigned char *buf, const unsigned &size)
{
    memset((void*)buf, 0, size);
    FILE *fp = fopen(fullname.c_str(), "rb");
    if(fp != nullptr)
    {
        fread((void*)buf, size, 1, fp);
        fclose(fp);
    }
}
