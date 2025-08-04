/****************************************************************************
** Meta object code from reading C++ file 'backupwizard.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.2.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../widgets/backupwizard.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'backupwizard.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.2.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_BackupWizard_t {
    const uint offsetsAndSize[60];
    char stringdata0[480];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata_BackupWizard_t, stringdata0) + ofs), len 
static const qt_meta_stringdata_BackupWizard_t qt_meta_stringdata_BackupWizard = {
    {
QT_MOC_LITERAL(0, 12), // "BackupWizard"
QT_MOC_LITERAL(13, 13), // "backupStarted"
QT_MOC_LITERAL(27, 0), // ""
QT_MOC_LITERAL(28, 14), // "backupProgress"
QT_MOC_LITERAL(43, 10), // "percentage"
QT_MOC_LITERAL(54, 15), // "backupCompleted"
QT_MOC_LITERAL(70, 7), // "success"
QT_MOC_LITERAL(78, 7), // "message"
QT_MOC_LITERAL(86, 15), // "wizardCancelled"
QT_MOC_LITERAL(102, 13), // "onNextClicked"
QT_MOC_LITERAL(116, 13), // "onBackClicked"
QT_MOC_LITERAL(130, 15), // "onCancelClicked"
QT_MOC_LITERAL(146, 18), // "onSkipRisksClicked"
QT_MOC_LITERAL(165, 17), // "onContinueClicked"
QT_MOC_LITERAL(183, 24), // "onDeviceSelectionChanged"
QT_MOC_LITERAL(208, 19), // "onBackupTypeChanged"
QT_MOC_LITERAL(228, 20), // "onWholeSystemClicked"
QT_MOC_LITERAL(249, 19), // "onAddFoldersClicked"
QT_MOC_LITERAL(269, 17), // "onAddFilesClicked"
QT_MOC_LITERAL(287, 22), // "onRemoveFoldersClicked"
QT_MOC_LITERAL(310, 20), // "onRemoveFilesClicked"
QT_MOC_LITERAL(331, 22), // "onFileSelectionChanged"
QT_MOC_LITERAL(354, 21), // "onFormatDeviceClicked"
QT_MOC_LITERAL(376, 21), // "onMakeBootableChanged"
QT_MOC_LITERAL(398, 7), // "enabled"
QT_MOC_LITERAL(406, 17), // "onProcessFinished"
QT_MOC_LITERAL(424, 8), // "exitCode"
QT_MOC_LITERAL(433, 20), // "QProcess::ExitStatus"
QT_MOC_LITERAL(454, 10), // "exitStatus"
QT_MOC_LITERAL(465, 14) // "updateProgress"

    },
    "BackupWizard\0backupStarted\0\0backupProgress\0"
    "percentage\0backupCompleted\0success\0"
    "message\0wizardCancelled\0onNextClicked\0"
    "onBackClicked\0onCancelClicked\0"
    "onSkipRisksClicked\0onContinueClicked\0"
    "onDeviceSelectionChanged\0onBackupTypeChanged\0"
    "onWholeSystemClicked\0onAddFoldersClicked\0"
    "onAddFilesClicked\0onRemoveFoldersClicked\0"
    "onRemoveFilesClicked\0onFileSelectionChanged\0"
    "onFormatDeviceClicked\0onMakeBootableChanged\0"
    "enabled\0onProcessFinished\0exitCode\0"
    "QProcess::ExitStatus\0exitStatus\0"
    "updateProgress"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_BackupWizard[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
      21,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,  140,    2, 0x06,    1 /* Public */,
       3,    1,  141,    2, 0x06,    2 /* Public */,
       5,    2,  144,    2, 0x06,    4 /* Public */,
       8,    0,  149,    2, 0x06,    7 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       9,    0,  150,    2, 0x08,    8 /* Private */,
      10,    0,  151,    2, 0x08,    9 /* Private */,
      11,    0,  152,    2, 0x08,   10 /* Private */,
      12,    0,  153,    2, 0x08,   11 /* Private */,
      13,    0,  154,    2, 0x08,   12 /* Private */,
      14,    0,  155,    2, 0x08,   13 /* Private */,
      15,    0,  156,    2, 0x08,   14 /* Private */,
      16,    0,  157,    2, 0x08,   15 /* Private */,
      17,    0,  158,    2, 0x08,   16 /* Private */,
      18,    0,  159,    2, 0x08,   17 /* Private */,
      19,    0,  160,    2, 0x08,   18 /* Private */,
      20,    0,  161,    2, 0x08,   19 /* Private */,
      21,    0,  162,    2, 0x08,   20 /* Private */,
      22,    0,  163,    2, 0x08,   21 /* Private */,
      23,    1,  164,    2, 0x08,   22 /* Private */,
      25,    2,  167,    2, 0x08,   24 /* Private */,
      29,    0,  172,    2, 0x08,   27 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    4,
    QMetaType::Void, QMetaType::Bool, QMetaType::QString,    6,    7,
    QMetaType::Void,

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
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   24,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 27,   26,   28,
    QMetaType::Void,

       0        // eod
};

void BackupWizard::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<BackupWizard *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->backupStarted(); break;
        case 1: _t->backupProgress((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 2: _t->backupCompleted((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 3: _t->wizardCancelled(); break;
        case 4: _t->onNextClicked(); break;
        case 5: _t->onBackClicked(); break;
        case 6: _t->onCancelClicked(); break;
        case 7: _t->onSkipRisksClicked(); break;
        case 8: _t->onContinueClicked(); break;
        case 9: _t->onDeviceSelectionChanged(); break;
        case 10: _t->onBackupTypeChanged(); break;
        case 11: _t->onWholeSystemClicked(); break;
        case 12: _t->onAddFoldersClicked(); break;
        case 13: _t->onAddFilesClicked(); break;
        case 14: _t->onRemoveFoldersClicked(); break;
        case 15: _t->onRemoveFilesClicked(); break;
        case 16: _t->onFileSelectionChanged(); break;
        case 17: _t->onFormatDeviceClicked(); break;
        case 18: _t->onMakeBootableChanged((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 19: _t->onProcessFinished((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QProcess::ExitStatus>>(_a[2]))); break;
        case 20: _t->updateProgress(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (BackupWizard::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&BackupWizard::backupStarted)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (BackupWizard::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&BackupWizard::backupProgress)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (BackupWizard::*)(bool , const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&BackupWizard::backupCompleted)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (BackupWizard::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&BackupWizard::wizardCancelled)) {
                *result = 3;
                return;
            }
        }
    }
}

const QMetaObject BackupWizard::staticMetaObject = { {
    QMetaObject::SuperData::link<QDialog::staticMetaObject>(),
    qt_meta_stringdata_BackupWizard.offsetsAndSize,
    qt_meta_data_BackupWizard,
    qt_static_metacall,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata_BackupWizard_t
, QtPrivate::TypeAndForceComplete<BackupWizard, std::true_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<bool, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>
, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<bool, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<QProcess::ExitStatus, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>


>,
    nullptr
} };


const QMetaObject *BackupWizard::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *BackupWizard::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_BackupWizard.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int BackupWizard::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 21)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 21;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 21)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 21;
    }
    return _id;
}

// SIGNAL 0
void BackupWizard::backupStarted()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void BackupWizard::backupProgress(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void BackupWizard::backupCompleted(bool _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void BackupWizard::wizardCancelled()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
