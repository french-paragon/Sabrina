#ifndef APP_INFO_H
#define APP_INFO_H

#include "utils/utils_global.h"

#define APP_NAME "Sabrina"
#define ORG_NAME "Paragon"
#define ORG_DOMAIN "famillejospin.ch"

namespace Sabrina {

extern "C" CATHIA_UTILS_EXPORT const char* appTag();
extern "C" CATHIA_UTILS_EXPORT const char* appHash();

extern "C" CATHIA_UTILS_EXPORT const char* compileDateFull();
extern "C" CATHIA_UTILS_EXPORT const char* compileDate();

} //namespace Sabrina
#endif // APP_INFO_H
