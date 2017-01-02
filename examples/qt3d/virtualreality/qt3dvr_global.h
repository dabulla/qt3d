#ifndef QT3DVR_GLOBAL_H
#define QT3DVR_GLOBAL_H

#include <Qt3DCore/qt3dcore_global.h>

QT_BEGIN_NAMESPACE

#if defined(QT_SHARED) || !defined(QT_STATIC)
#  if defined(QT3DVR_LIBRARY)
#    define QT3DVR_EXPORT Q_DECL_EXPORT
#  else
#    define QT3DVR_EXPORT Q_DECL_IMPORT
#  endif
#else
#  define QT3DVR_EXPORT
#endif

QT_END_NAMESPACE

#endif // QT3DVR_GLOBAL_H
