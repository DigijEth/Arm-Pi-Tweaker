/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.2.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../mainwindow.h"
#include <QtGui/qtextcursor.h>
#include <QtNetwork/QSslError>
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.2.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_MainWindow_t {
    const uint offsetsAndSize[34];
    char stringdata0[265];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata_MainWindow_t, stringdata0) + ofs), len 
static const qt_meta_stringdata_MainWindow_t qt_meta_stringdata_MainWindow = {
    {
QT_MOC_LITERAL(0, 10), // "MainWindow"
QT_MOC_LITERAL(11, 16), // "onExtractDrivers"
QT_MOC_LITERAL(28, 0), // ""
QT_MOC_LITERAL(29, 12), // "onRunUpgrade"
QT_MOC_LITERAL(42, 13), // "onPatchSystem"
QT_MOC_LITERAL(56, 17), // "onRollbackUpgrade"
QT_MOC_LITERAL(74, 9), // "showAbout"
QT_MOC_LITERAL(84, 12), // "showSettings"
QT_MOC_LITERAL(97, 24), // "onImageBuilderLogMessage"
QT_MOC_LITERAL(122, 7), // "message"
QT_MOC_LITERAL(130, 19), // "onSwitchToStatusTab"
QT_MOC_LITERAL(150, 20), // "toggleTabsVisibility"
QT_MOC_LITERAL(171, 7), // "visible"
QT_MOC_LITERAL(179, 18), // "showAllWidgetsMenu"
QT_MOC_LITERAL(198, 24), // "handleRequestDownloadDir"
QT_MOC_LITERAL(223, 30), // "handleRequestBuildConfirmation"
QT_MOC_LITERAL(254, 10) // "kernelPath"

    },
    "MainWindow\0onExtractDrivers\0\0onRunUpgrade\0"
    "onPatchSystem\0onRollbackUpgrade\0"
    "showAbout\0showSettings\0onImageBuilderLogMessage\0"
    "message\0onSwitchToStatusTab\0"
    "toggleTabsVisibility\0visible\0"
    "showAllWidgetsMenu\0handleRequestDownloadDir\0"
    "handleRequestBuildConfirmation\0"
    "kernelPath"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainWindow[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   86,    2, 0x08,    1 /* Private */,
       3,    0,   87,    2, 0x08,    2 /* Private */,
       4,    0,   88,    2, 0x08,    3 /* Private */,
       5,    0,   89,    2, 0x08,    4 /* Private */,
       6,    0,   90,    2, 0x08,    5 /* Private */,
       7,    0,   91,    2, 0x08,    6 /* Private */,
       8,    1,   92,    2, 0x08,    7 /* Private */,
      10,    0,   95,    2, 0x08,    9 /* Private */,
      11,    1,   96,    2, 0x08,   10 /* Private */,
      13,    0,   99,    2, 0x08,   12 /* Private */,
      14,    0,  100,    2, 0x08,   13 /* Private */,
      15,    1,  101,    2, 0x08,   14 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    9,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   12,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   16,

       0        // eod
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MainWindow *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->onExtractDrivers(); break;
        case 1: _t->onRunUpgrade(); break;
        case 2: _t->onPatchSystem(); break;
        case 3: _t->onRollbackUpgrade(); break;
        case 4: _t->showAbout(); break;
        case 5: _t->showSettings(); break;
        case 6: _t->onImageBuilderLogMessage((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 7: _t->onSwitchToStatusTab(); break;
        case 8: _t->toggleTabsVisibility((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 9: _t->showAllWidgetsMenu(); break;
        case 10: _t->handleRequestDownloadDir(); break;
        case 11: _t->handleRequestBuildConfirmation((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject MainWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_MainWindow.offsetsAndSize,
    qt_meta_data_MainWindow,
    qt_static_metacall,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata_MainWindow_t
, QtPrivate::TypeAndForceComplete<MainWindow, std::true_type>
, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<bool, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>


>,
    nullptr
} };


const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 12)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 12;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
