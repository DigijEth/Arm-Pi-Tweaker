/****************************************************************************
** Meta object code from reading C++ file 'systemmanager.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.2.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../systemmanager.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'systemmanager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.2.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_SystemManager_t {
    const uint offsetsAndSize[32];
    char stringdata0[210];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata_SystemManager_t, stringdata0) + ofs), len 
static const qt_meta_stringdata_SystemManager_t qt_meta_stringdata_SystemManager = {
    {
QT_MOC_LITERAL(0, 13), // "SystemManager"
QT_MOC_LITERAL(14, 15), // "progressUpdated"
QT_MOC_LITERAL(30, 0), // ""
QT_MOC_LITERAL(31, 10), // "percentage"
QT_MOC_LITERAL(42, 13), // "statusUpdated"
QT_MOC_LITERAL(56, 7), // "message"
QT_MOC_LITERAL(64, 18), // "operationCompleted"
QT_MOC_LITERAL(83, 7), // "success"
QT_MOC_LITERAL(91, 17), // "onProcessFinished"
QT_MOC_LITERAL(109, 8), // "exitCode"
QT_MOC_LITERAL(118, 20), // "QProcess::ExitStatus"
QT_MOC_LITERAL(139, 10), // "exitStatus"
QT_MOC_LITERAL(150, 14), // "onProcessError"
QT_MOC_LITERAL(165, 22), // "QProcess::ProcessError"
QT_MOC_LITERAL(188, 5), // "error"
QT_MOC_LITERAL(194, 15) // "onProcessOutput"

    },
    "SystemManager\0progressUpdated\0\0"
    "percentage\0statusUpdated\0message\0"
    "operationCompleted\0success\0onProcessFinished\0"
    "exitCode\0QProcess::ExitStatus\0exitStatus\0"
    "onProcessError\0QProcess::ProcessError\0"
    "error\0onProcessOutput"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_SystemManager[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   50,    2, 0x06,    1 /* Public */,
       4,    1,   53,    2, 0x06,    3 /* Public */,
       6,    2,   56,    2, 0x06,    5 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       8,    2,   61,    2, 0x08,    8 /* Private */,
      12,    1,   66,    2, 0x08,   11 /* Private */,
      15,    0,   69,    2, 0x08,   13 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::QString,    5,
    QMetaType::Void, QMetaType::Bool, QMetaType::QString,    7,    5,

 // slots: parameters
    QMetaType::Void, QMetaType::Int, 0x80000000 | 10,    9,   11,
    QMetaType::Void, 0x80000000 | 13,   14,
    QMetaType::Void,

       0        // eod
};

void SystemManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<SystemManager *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->progressUpdated((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 1: _t->statusUpdated((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 2: _t->operationCompleted((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 3: _t->onProcessFinished((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QProcess::ExitStatus>>(_a[2]))); break;
        case 4: _t->onProcessError((*reinterpret_cast< std::add_pointer_t<QProcess::ProcessError>>(_a[1]))); break;
        case 5: _t->onProcessOutput(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (SystemManager::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SystemManager::progressUpdated)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (SystemManager::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SystemManager::statusUpdated)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (SystemManager::*)(bool , const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SystemManager::operationCompleted)) {
                *result = 2;
                return;
            }
        }
    }
}

const QMetaObject SystemManager::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_SystemManager.offsetsAndSize,
    qt_meta_data_SystemManager,
    qt_static_metacall,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata_SystemManager_t
, QtPrivate::TypeAndForceComplete<SystemManager, std::true_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<bool, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>
, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<QProcess::ExitStatus, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<QProcess::ProcessError, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>


>,
    nullptr
} };


const QMetaObject *SystemManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SystemManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_SystemManager.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int SystemManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void SystemManager::progressUpdated(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void SystemManager::statusUpdated(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void SystemManager::operationCompleted(bool _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
