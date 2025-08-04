/****************************************************************************
** Meta object code from reading C++ file 'upgradewidget.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.2.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../widgets/upgradewidget.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'upgradewidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.2.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_UpgradeWidget_t {
    const uint offsetsAndSize[24];
    char stringdata0[166];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata_UpgradeWidget_t, stringdata0) + ofs), len 
static const qt_meta_stringdata_UpgradeWidget_t qt_meta_stringdata_UpgradeWidget = {
    {
QT_MOC_LITERAL(0, 13), // "UpgradeWidget"
QT_MOC_LITERAL(14, 23), // "extractDriversRequested"
QT_MOC_LITERAL(38, 0), // ""
QT_MOC_LITERAL(39, 19), // "runUpgradeRequested"
QT_MOC_LITERAL(59, 20), // "patchSystemRequested"
QT_MOC_LITERAL(80, 17), // "rollbackRequested"
QT_MOC_LITERAL(98, 14), // "updateProgress"
QT_MOC_LITERAL(113, 5), // "value"
QT_MOC_LITERAL(119, 12), // "updateStatus"
QT_MOC_LITERAL(132, 7), // "message"
QT_MOC_LITERAL(140, 17), // "setButtonsEnabled"
QT_MOC_LITERAL(158, 7) // "enabled"

    },
    "UpgradeWidget\0extractDriversRequested\0"
    "\0runUpgradeRequested\0patchSystemRequested\0"
    "rollbackRequested\0updateProgress\0value\0"
    "updateStatus\0message\0setButtonsEnabled\0"
    "enabled"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_UpgradeWidget[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   56,    2, 0x06,    1 /* Public */,
       3,    0,   57,    2, 0x06,    2 /* Public */,
       4,    0,   58,    2, 0x06,    3 /* Public */,
       5,    0,   59,    2, 0x06,    4 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       6,    1,   60,    2, 0x0a,    5 /* Public */,
       8,    1,   63,    2, 0x0a,    7 /* Public */,
      10,    1,   66,    2, 0x0a,    9 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    7,
    QMetaType::Void, QMetaType::QString,    9,
    QMetaType::Void, QMetaType::Bool,   11,

       0        // eod
};

void UpgradeWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<UpgradeWidget *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->extractDriversRequested(); break;
        case 1: _t->runUpgradeRequested(); break;
        case 2: _t->patchSystemRequested(); break;
        case 3: _t->rollbackRequested(); break;
        case 4: _t->updateProgress((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 5: _t->updateStatus((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 6: _t->setButtonsEnabled((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (UpgradeWidget::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&UpgradeWidget::extractDriversRequested)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (UpgradeWidget::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&UpgradeWidget::runUpgradeRequested)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (UpgradeWidget::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&UpgradeWidget::patchSystemRequested)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (UpgradeWidget::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&UpgradeWidget::rollbackRequested)) {
                *result = 3;
                return;
            }
        }
    }
}

const QMetaObject UpgradeWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_UpgradeWidget.offsetsAndSize,
    qt_meta_data_UpgradeWidget,
    qt_static_metacall,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata_UpgradeWidget_t
, QtPrivate::TypeAndForceComplete<UpgradeWidget, std::true_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>
, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<bool, std::false_type>


>,
    nullptr
} };


const QMetaObject *UpgradeWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *UpgradeWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_UpgradeWidget.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int UpgradeWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void UpgradeWidget::extractDriversRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void UpgradeWidget::runUpgradeRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void UpgradeWidget::patchSystemRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void UpgradeWidget::rollbackRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
