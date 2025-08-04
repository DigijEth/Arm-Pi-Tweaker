/****************************************************************************
** Meta object code from reading C++ file 'storagemanager.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.2.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../storagemanager.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'storagemanager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.2.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_StorageManager_t {
    const uint offsetsAndSize[46];
    char stringdata0[329];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata_StorageManager_t, stringdata0) + ofs), len 
static const qt_meta_stringdata_StorageManager_t qt_meta_stringdata_StorageManager = {
    {
QT_MOC_LITERAL(0, 14), // "StorageManager"
QT_MOC_LITERAL(15, 16), // "operationStarted"
QT_MOC_LITERAL(32, 0), // ""
QT_MOC_LITERAL(33, 9), // "operation"
QT_MOC_LITERAL(43, 15), // "progressUpdated"
QT_MOC_LITERAL(59, 8), // "progress"
QT_MOC_LITERAL(68, 18), // "operationCompleted"
QT_MOC_LITERAL(87, 7), // "success"
QT_MOC_LITERAL(95, 7), // "message"
QT_MOC_LITERAL(103, 18), // "scanStorageDevices"
QT_MOC_LITERAL(122, 24), // "onDeviceSelectionChanged"
QT_MOC_LITERAL(147, 13), // "onMountDevice"
QT_MOC_LITERAL(161, 15), // "onUnmountDevice"
QT_MOC_LITERAL(177, 15), // "onCopyLiveImage"
QT_MOC_LITERAL(193, 14), // "onBurnToSDCard"
QT_MOC_LITERAL(208, 16), // "onCreateSnapshot"
QT_MOC_LITERAL(225, 11), // "onDriveCopy"
QT_MOC_LITERAL(237, 16), // "updateDeviceInfo"
QT_MOC_LITERAL(254, 17), // "onProcessFinished"
QT_MOC_LITERAL(272, 8), // "exitCode"
QT_MOC_LITERAL(281, 20), // "QProcess::ExitStatus"
QT_MOC_LITERAL(302, 10), // "exitStatus"
QT_MOC_LITERAL(313, 15) // "onProcessOutput"

    },
    "StorageManager\0operationStarted\0\0"
    "operation\0progressUpdated\0progress\0"
    "operationCompleted\0success\0message\0"
    "scanStorageDevices\0onDeviceSelectionChanged\0"
    "onMountDevice\0onUnmountDevice\0"
    "onCopyLiveImage\0onBurnToSDCard\0"
    "onCreateSnapshot\0onDriveCopy\0"
    "updateDeviceInfo\0onProcessFinished\0"
    "exitCode\0QProcess::ExitStatus\0exitStatus\0"
    "onProcessOutput"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_StorageManager[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
      14,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   98,    2, 0x06,    1 /* Public */,
       4,    1,  101,    2, 0x06,    3 /* Public */,
       6,    2,  104,    2, 0x06,    5 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       9,    0,  109,    2, 0x08,    8 /* Private */,
      10,    0,  110,    2, 0x08,    9 /* Private */,
      11,    0,  111,    2, 0x08,   10 /* Private */,
      12,    0,  112,    2, 0x08,   11 /* Private */,
      13,    0,  113,    2, 0x08,   12 /* Private */,
      14,    0,  114,    2, 0x08,   13 /* Private */,
      15,    0,  115,    2, 0x08,   14 /* Private */,
      16,    0,  116,    2, 0x08,   15 /* Private */,
      17,    0,  117,    2, 0x08,   16 /* Private */,
      18,    2,  118,    2, 0x08,   17 /* Private */,
      22,    0,  123,    2, 0x08,   20 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::Int,    5,
    QMetaType::Void, QMetaType::Bool, QMetaType::QString,    7,    8,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 20,   19,   21,
    QMetaType::Void,

       0        // eod
};

void StorageManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<StorageManager *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->operationStarted((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 1: _t->progressUpdated((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 2: _t->operationCompleted((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 3: _t->scanStorageDevices(); break;
        case 4: _t->onDeviceSelectionChanged(); break;
        case 5: _t->onMountDevice(); break;
        case 6: _t->onUnmountDevice(); break;
        case 7: _t->onCopyLiveImage(); break;
        case 8: _t->onBurnToSDCard(); break;
        case 9: _t->onCreateSnapshot(); break;
        case 10: _t->onDriveCopy(); break;
        case 11: _t->updateDeviceInfo(); break;
        case 12: _t->onProcessFinished((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QProcess::ExitStatus>>(_a[2]))); break;
        case 13: _t->onProcessOutput(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (StorageManager::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&StorageManager::operationStarted)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (StorageManager::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&StorageManager::progressUpdated)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (StorageManager::*)(bool , const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&StorageManager::operationCompleted)) {
                *result = 2;
                return;
            }
        }
    }
}

const QMetaObject StorageManager::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_StorageManager.offsetsAndSize,
    qt_meta_data_StorageManager,
    qt_static_metacall,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata_StorageManager_t
, QtPrivate::TypeAndForceComplete<StorageManager, std::true_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<bool, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>
, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<QProcess::ExitStatus, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>


>,
    nullptr
} };


const QMetaObject *StorageManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *StorageManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_StorageManager.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int StorageManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 14)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 14;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 14)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 14;
    }
    return _id;
}

// SIGNAL 0
void StorageManager::operationStarted(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void StorageManager::progressUpdated(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void StorageManager::operationCompleted(bool _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
