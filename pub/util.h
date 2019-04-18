#ifndef PUB_UTIL_H
#define PUB_UTIL_H

#include <string>

#ifdef PUB_EXPORTS
#define PUB_API __declspec(dllexport)
#else
#define PUB_API __declspec(dllimport)
#endif

PUB_API std::string GetFilename(const std::string &fullname);

#endif
