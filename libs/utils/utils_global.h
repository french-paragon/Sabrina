#ifndef UTILS_GLOBAL_H
#define UTILS_GLOBAL_H

#include <QtCore/QtGlobal>

#if defined(CATHIA_UTILS_LIBRARY)
#  define CATHIA_UTILS_EXPORT Q_DECL_EXPORT
#else
#  define CATHIA_UTILS_EXPORT Q_DECL_IMPORT
#endif

#endif // UTILS_GLOBAL_H
