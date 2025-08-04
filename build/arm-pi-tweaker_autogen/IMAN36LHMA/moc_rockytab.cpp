/****************************************************************************
** Meta object code from reading C++ file 'rockytab.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.2.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../widgets/rockytab.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'rockytab.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.2.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_RockyTab_t {
    const uint offsetsAndSize[30];
    char stringdata0[163];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata_RockyTab_t, stringdata0) + ofs), len 
static const qt_meta_stringdata_RockyTab_t qt_meta_stringdata_RockyTab = {
    {
QT_MOC_LITERAL(0, 8), // "RockyTab"
QT_MOC_LITERAL(9, 13), // "onSendMessage"
QT_MOC_LITERAL(23, 0), // ""
QT_MOC_LITERAL(24, 11), // "onClearChat"
QT_MOC_LITERAL(36, 11), // "onLoadModel"
QT_MOC_LITERAL(48, 16), // "onStopGeneration"
QT_MOC_LITERAL(65, 13), // "onLlamaOutput"
QT_MOC_LITERAL(79, 5), // "token"
QT_MOC_LITERAL(85, 15), // "onLlamaFinished"
QT_MOC_LITERAL(101, 12), // "onLlamaError"
QT_MOC_LITERAL(114, 5), // "error"
QT_MOC_LITERAL(120, 13), // "onModelLoaded"
QT_MOC_LITERAL(134, 7), // "success"
QT_MOC_LITERAL(142, 7), // "message"
QT_MOC_LITERAL(150, 12) // "onClearDebug"

    },
    "RockyTab\0onSendMessage\0\0onClearChat\0"
    "onLoadModel\0onStopGeneration\0onLlamaOutput\0"
    "token\0onLlamaFinished\0onLlamaError\0"
    "error\0onModelLoaded\0success\0message\0"
    "onClearDebug"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_RockyTab[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   68,    2, 0x08,    1 /* Private */,
       3,    0,   69,    2, 0x08,    2 /* Private */,
       4,    0,   70,    2, 0x08,    3 /* Private */,
       5,    0,   71,    2, 0x08,    4 /* Private */,
       6,    1,   72,    2, 0x08,    5 /* Private */,
       8,    0,   75,    2, 0x08,    7 /* Private */,
       9,    1,   76,    2, 0x08,    8 /* Private */,
      11,    2,   79,    2, 0x08,   10 /* Private */,
      14,    0,   84,    2, 0x08,   13 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    7,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   10,
    QMetaType::Void, QMetaType::Bool, QMetaType::QString,   12,   13,
    QMetaType::Void,

       0        // eod
};

void RockyTab::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<RockyTab *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->onSendMessage(); break;
        case 1: _t->onClearChat(); break;
        case 2: _t->onLoadModel(); break;
        case 3: _t->onStopGeneration(); break;
        case 4: _t->onLlamaOutput((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 5: _t->onLlamaFinished(); break;
        case 6: _t->onLlamaError((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 7: _t->onModelLoaded((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 8: _t->onClearDebug(); break;
        default: ;
        }
    }
}

const QMetaObject RockyTab::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_RockyTab.offsetsAndSize,
    qt_meta_data_RockyTab,
    qt_static_metacall,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata_RockyTab_t
, QtPrivate::TypeAndForceComplete<RockyTab, std::true_type>
, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<bool, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>


>,
    nullptr
} };


const QMetaObject *RockyTab::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *RockyTab::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_RockyTab.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int RockyTab::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 9;
    }
    return _id;
}
struct qt_meta_stringdata_LlamaWorker_t {
    const uint offsetsAndSize[18];
    char stringdata0[71];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata_LlamaWorker_t, stringdata0) + ofs), len 
static const qt_meta_stringdata_LlamaWorker_t qt_meta_stringdata_LlamaWorker = {
    {
QT_MOC_LITERAL(0, 11), // "LlamaWorker"
QT_MOC_LITERAL(12, 8), // "newToken"
QT_MOC_LITERAL(21, 0), // ""
QT_MOC_LITERAL(22, 5), // "token"
QT_MOC_LITERAL(28, 8), // "finished"
QT_MOC_LITERAL(37, 5), // "error"
QT_MOC_LITERAL(43, 11), // "modelLoaded"
QT_MOC_LITERAL(55, 7), // "success"
QT_MOC_LITERAL(63, 7) // "message"

    },
    "LlamaWorker\0newToken\0\0token\0finished\0"
    "error\0modelLoaded\0success\0message"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_LlamaWorker[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   38,    2, 0x06,    1 /* Public */,
       4,    0,   41,    2, 0x06,    3 /* Public */,
       5,    1,   42,    2, 0x06,    4 /* Public */,
       6,    2,   45,    2, 0x06,    6 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    5,
    QMetaType::Void, QMetaType::Bool, QMetaType::QString,    7,    8,

       0        // eod
};

void LlamaWorker::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<LlamaWorker *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->newToken((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 1: _t->finished(); break;
        case 2: _t->error((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 3: _t->modelLoaded((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (LlamaWorker::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&LlamaWorker::newToken)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (LlamaWorker::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&LlamaWorker::finished)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (LlamaWorker::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&LlamaWorker::error)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (LlamaWorker::*)(bool , const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&LlamaWorker::modelLoaded)) {
                *result = 3;
                return;
            }
        }
    }
}

const QMetaObject LlamaWorker::staticMetaObject = { {
    QMetaObject::SuperData::link<QThread::staticMetaObject>(),
    qt_meta_stringdata_LlamaWorker.offsetsAndSize,
    qt_meta_data_LlamaWorker,
    qt_static_metacall,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata_LlamaWorker_t
, QtPrivate::TypeAndForceComplete<LlamaWorker, std::true_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<bool, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>



>,
    nullptr
} };


const QMetaObject *LlamaWorker::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *LlamaWorker::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_LlamaWorker.stringdata0))
        return static_cast<void*>(this);
    return QThread::qt_metacast(_clname);
}

int LlamaWorker::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void LlamaWorker::newToken(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void LlamaWorker::finished()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void LlamaWorker::error(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void LlamaWorker::modelLoaded(bool _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
