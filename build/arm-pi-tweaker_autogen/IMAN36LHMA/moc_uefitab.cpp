/****************************************************************************
** Meta object code from reading C++ file 'uefitab.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.2.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../widgets/uefitab.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'uefitab.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.2.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_UefiTab_t {
    const uint offsetsAndSize[32];
    char stringdata0[246];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata_UefiTab_t, stringdata0) + ofs), len 
static const qt_meta_stringdata_UefiTab_t qt_meta_stringdata_UefiTab = {
    {
QT_MOC_LITERAL(0, 7), // "UefiTab"
QT_MOC_LITERAL(8, 16), // "checkCurrentUefi"
QT_MOC_LITERAL(25, 0), // ""
QT_MOC_LITERAL(26, 15), // "checkForUpdates"
QT_MOC_LITERAL(42, 20), // "loadAvailablePatches"
QT_MOC_LITERAL(63, 20), // "applySelectedPatches"
QT_MOC_LITERAL(84, 9), // "flashUefi"
QT_MOC_LITERAL(94, 17), // "backupCurrentUefi"
QT_MOC_LITERAL(112, 17), // "restoreUefiBackup"
QT_MOC_LITERAL(130, 22), // "onFlashProcessFinished"
QT_MOC_LITERAL(153, 8), // "exitCode"
QT_MOC_LITERAL(162, 20), // "QProcess::ExitStatus"
QT_MOC_LITERAL(183, 10), // "exitStatus"
QT_MOC_LITERAL(194, 20), // "onFlashProcessOutput"
QT_MOC_LITERAL(215, 14), // "selectUefiFile"
QT_MOC_LITERAL(230, 15) // "verifyUefiImage"

    },
    "UefiTab\0checkCurrentUefi\0\0checkForUpdates\0"
    "loadAvailablePatches\0applySelectedPatches\0"
    "flashUefi\0backupCurrentUefi\0"
    "restoreUefiBackup\0onFlashProcessFinished\0"
    "exitCode\0QProcess::ExitStatus\0exitStatus\0"
    "onFlashProcessOutput\0selectUefiFile\0"
    "verifyUefiImage"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_UefiTab[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   80,    2, 0x08,    1 /* Private */,
       3,    0,   81,    2, 0x08,    2 /* Private */,
       4,    0,   82,    2, 0x08,    3 /* Private */,
       5,    0,   83,    2, 0x08,    4 /* Private */,
       6,    0,   84,    2, 0x08,    5 /* Private */,
       7,    0,   85,    2, 0x08,    6 /* Private */,
       8,    0,   86,    2, 0x08,    7 /* Private */,
       9,    2,   87,    2, 0x08,    8 /* Private */,
      13,    0,   92,    2, 0x08,   11 /* Private */,
      14,    0,   93,    2, 0x08,   12 /* Private */,
      15,    0,   94,    2, 0x08,   13 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 11,   10,   12,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void UefiTab::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<UefiTab *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->checkCurrentUefi(); break;
        case 1: _t->checkForUpdates(); break;
        case 2: _t->loadAvailablePatches(); break;
        case 3: _t->applySelectedPatches(); break;
        case 4: _t->flashUefi(); break;
        case 5: _t->backupCurrentUefi(); break;
        case 6: _t->restoreUefiBackup(); break;
        case 7: _t->onFlashProcessFinished((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QProcess::ExitStatus>>(_a[2]))); break;
        case 8: _t->onFlashProcessOutput(); break;
        case 9: _t->selectUefiFile(); break;
        case 10: _t->verifyUefiImage(); break;
        default: ;
        }
    }
}

const QMetaObject UefiTab::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_UefiTab.offsetsAndSize,
    qt_meta_data_UefiTab,
    qt_static_metacall,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata_UefiTab_t
, QtPrivate::TypeAndForceComplete<UefiTab, std::true_type>
, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<QProcess::ExitStatus, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>


>,
    nullptr
} };


const QMetaObject *UefiTab::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *UefiTab::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_UefiTab.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int UefiTab::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
QT_WARNING_POP
QT_END_MOC_NAMESPACE
