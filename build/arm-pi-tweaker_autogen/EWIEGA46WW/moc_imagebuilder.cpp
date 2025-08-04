/****************************************************************************
** Meta object code from reading C++ file 'imagebuilder.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.2.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../imagebuilder.h"
#include <QtNetwork/QSslError>
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'imagebuilder.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.2.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ImageBuilder_t {
    const uint offsetsAndSize[42];
    char stringdata0[263];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata_ImageBuilder_t, stringdata0) + ofs), len 
static const qt_meta_stringdata_ImageBuilder_t qt_meta_stringdata_ImageBuilder = {
    {
QT_MOC_LITERAL(0, 12), // "ImageBuilder"
QT_MOC_LITERAL(13, 12), // "buildStarted"
QT_MOC_LITERAL(26, 0), // ""
QT_MOC_LITERAL(27, 13), // "buildProgress"
QT_MOC_LITERAL(41, 10), // "percentage"
QT_MOC_LITERAL(52, 11), // "description"
QT_MOC_LITERAL(64, 16), // "buildStepChanged"
QT_MOC_LITERAL(81, 4), // "step"
QT_MOC_LITERAL(86, 15), // "buildLogMessage"
QT_MOC_LITERAL(102, 7), // "message"
QT_MOC_LITERAL(110, 14), // "buildCompleted"
QT_MOC_LITERAL(125, 7), // "success"
QT_MOC_LITERAL(133, 10), // "buildError"
QT_MOC_LITERAL(144, 5), // "error"
QT_MOC_LITERAL(150, 17), // "onProcessFinished"
QT_MOC_LITERAL(168, 8), // "exitCode"
QT_MOC_LITERAL(177, 20), // "QProcess::ExitStatus"
QT_MOC_LITERAL(198, 10), // "exitStatus"
QT_MOC_LITERAL(209, 14), // "onProcessError"
QT_MOC_LITERAL(224, 22), // "QProcess::ProcessError"
QT_MOC_LITERAL(247, 15) // "onProcessOutput"

    },
    "ImageBuilder\0buildStarted\0\0buildProgress\0"
    "percentage\0description\0buildStepChanged\0"
    "step\0buildLogMessage\0message\0"
    "buildCompleted\0success\0buildError\0"
    "error\0onProcessFinished\0exitCode\0"
    "QProcess::ExitStatus\0exitStatus\0"
    "onProcessError\0QProcess::ProcessError\0"
    "onProcessOutput"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ImageBuilder[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       6,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   68,    2, 0x06,    1 /* Public */,
       3,    2,   69,    2, 0x06,    2 /* Public */,
       6,    2,   74,    2, 0x06,    5 /* Public */,
       8,    1,   79,    2, 0x06,    8 /* Public */,
      10,    2,   82,    2, 0x06,   10 /* Public */,
      12,    1,   87,    2, 0x06,   13 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
      14,    2,   90,    2, 0x08,   15 /* Private */,
      18,    1,   95,    2, 0x08,   18 /* Private */,
      20,    0,   98,    2, 0x08,   20 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::QString,    4,    5,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,    7,    5,
    QMetaType::Void, QMetaType::QString,    9,
    QMetaType::Void, QMetaType::Bool, QMetaType::QString,   11,    9,
    QMetaType::Void, QMetaType::QString,   13,

 // slots: parameters
    QMetaType::Void, QMetaType::Int, 0x80000000 | 16,   15,   17,
    QMetaType::Void, 0x80000000 | 19,   13,
    QMetaType::Void,

       0        // eod
};

void ImageBuilder::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ImageBuilder *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->buildStarted(); break;
        case 1: _t->buildProgress((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 2: _t->buildStepChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 3: _t->buildLogMessage((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 4: _t->buildCompleted((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 5: _t->buildError((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 6: _t->onProcessFinished((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QProcess::ExitStatus>>(_a[2]))); break;
        case 7: _t->onProcessError((*reinterpret_cast< std::add_pointer_t<QProcess::ProcessError>>(_a[1]))); break;
        case 8: _t->onProcessOutput(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ImageBuilder::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ImageBuilder::buildStarted)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (ImageBuilder::*)(int , const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ImageBuilder::buildProgress)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (ImageBuilder::*)(const QString & , const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ImageBuilder::buildStepChanged)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (ImageBuilder::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ImageBuilder::buildLogMessage)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (ImageBuilder::*)(bool , const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ImageBuilder::buildCompleted)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (ImageBuilder::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ImageBuilder::buildError)) {
                *result = 5;
                return;
            }
        }
    }
}

const QMetaObject ImageBuilder::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_ImageBuilder.offsetsAndSize,
    qt_meta_data_ImageBuilder,
    qt_static_metacall,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata_ImageBuilder_t
, QtPrivate::TypeAndForceComplete<ImageBuilder, std::true_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<bool, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>
, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<QProcess::ExitStatus, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<QProcess::ProcessError, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>


>,
    nullptr
} };


const QMetaObject *ImageBuilder::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ImageBuilder::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ImageBuilder.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int ImageBuilder::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
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

// SIGNAL 0
void ImageBuilder::buildStarted()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void ImageBuilder::buildProgress(int _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void ImageBuilder::buildStepChanged(const QString & _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void ImageBuilder::buildLogMessage(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void ImageBuilder::buildCompleted(bool _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void ImageBuilder::buildError(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
