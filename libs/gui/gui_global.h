#ifndef GUI_GLOBAL_H
#define GUI_GLOBAL_H

#if defined(CATHIA_GUI_LIBRARY)
#  define CATHIA_GUI_EXPORT Q_DECL_EXPORT
#else
#  define CATHIA_GUI_EXPORT Q_DECL_IMPORT
#endif

#endif // GUI_GLOBAL_H
