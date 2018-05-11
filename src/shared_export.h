#ifndef SHARED_EXPORT_H
#define SHARED_EXPORT_H

#include <QtCore/QtGlobal>

#ifdef SHARED_EXPORT
#define SHARED_EXPORT Q_DECL_EXPORT
#else
#define SHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // SHARED_EXPORT_H
