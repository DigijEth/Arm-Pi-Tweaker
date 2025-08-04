/****************************************************************************
** Meta object code from reading C++ file 'gpumanager.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.2.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../gpumanager.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'gpumanager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.2.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_GpuManager_t {
    const uint offsetsAndSize[32];
    char stringdata0[252];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata_GpuManager_t, stringdata0) + ofs), len 
static const qt_meta_stringdata_GpuManager_t qt_meta_stringdata_GpuManager = {
    {
QT_MOC_LITERAL(0, 10), // "GpuManager"
QT_MOC_LITERAL(11, 22), // "installDriverRequested"
QT_MOC_LITERAL(34, 0), // ""
QT_MOC_LITERAL(35, 10), // "driverPath"
QT_MOC_LITERAL(46, 21), // "removeDriverRequested"
QT_MOC_LITERAL(68, 10), // "driverName"
QT_MOC_LITERAL(79, 21), // "switchDriverRequested"
QT_MOC_LITERAL(101, 10), // "driverType"
QT_MOC_LITERAL(112, 13), // "onScanDrivers"
QT_MOC_LITERAL(126, 15), // "onInstallDriver"
QT_MOC_LITERAL(142, 14), // "onRemoveDriver"
QT_MOC_LITERAL(157, 14), // "onSwitchDriver"
QT_MOC_LITERAL(172, 24), // "onDriverSelectionChanged"
QT_MOC_LITERAL(197, 18), // "updateDriverStatus"
QT_MOC_LITERAL(216, 14), // "updateGpuGraph"
QT_MOC_LITERAL(231, 20) // "onOpenDriverLocation"

    },
    "GpuManager\0installDriverRequested\0\0"
    "driverPath\0removeDriverRequested\0"
    "driverName\0switchDriverRequested\0"
    "driverType\0onScanDrivers\0onInstallDriver\0"
    "onRemoveDriver\0onSwitchDriver\0"
    "onDriverSelectionChanged\0updateDriverStatus\0"
    "updateGpuGraph\0onOpenDriverLocation"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_GpuManager[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   80,    2, 0x06,    1 /* Public */,
       4,    1,   83,    2, 0x06,    3 /* Public */,
       6,    1,   86,    2, 0x06,    5 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       8,    0,   89,    2, 0x08,    7 /* Private */,
       9,    0,   90,    2, 0x08,    8 /* Private */,
      10,    0,   91,    2, 0x08,    9 /* Private */,
      11,    0,   92,    2, 0x08,   10 /* Private */,
      12,    0,   93,    2, 0x08,   11 /* Private */,
      13,    0,   94,    2, 0x08,   12 /* Private */,
      14,    0,   95,    2, 0x08,   13 /* Private */,
      15,    0,   96,    2, 0x08,   14 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QString,    5,
    QMetaType::Void, QMetaType::QString,    7,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void GpuManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<GpuManager *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->installDriverRequested((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 1: _t->removeDriverRequested((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 2: _t->switchDriverRequested((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 3: _t->onScanDrivers(); break;
        case 4: _t->onInstallDriver(); break;
        case 5: _t->onRemoveDriver(); break;
        case 6: _t->onSwitchDriver(); break;
        case 7: _t->onDriverSelectionChanged(); break;
        case 8: _t->updateDriverStatus(); break;
        case 9: _t->updateGpuGraph(); break;
        case 10: _t->onOpenDriverLocation(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (GpuManager::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&GpuManager::installDriverRequested)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (GpuManager::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&GpuManager::removeDriverRequested)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (GpuManager::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&GpuManager::switchDriverRequested)) {
                *result = 2;
                return;
            }
        }
    }
}

const QMetaObject GpuManager::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_GpuManager.offsetsAndSize,
    qt_meta_data_GpuManager,
    qt_static_metacall,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata_GpuManager_t
, QtPrivate::TypeAndForceComplete<GpuManager, std::true_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>
, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>


>,
    nullptr
} };


const QMetaObject *GpuManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *GpuManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_GpuManager.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int GpuManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 11)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 11;
    }
    return _id;
}

// SIGNAL 0
void GpuManager::installDriverRequested(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void GpuManager::removeDriverRequested(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void GpuManager::switchDriverRequested(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
