#ifndef CORE_GLOBAL_H
#define CORE_GLOBAL_H

#if defined(CATHIA_CORE_LIBRARY)
#  define CATHIA_CORE_EXPORT Q_DECL_EXPORT
#else
#  define CATHIA_CORE_EXPORT Q_DECL_IMPORT
#endif

#include "utils/app_info.h"

#endif // CORE_GLOBAL_H
