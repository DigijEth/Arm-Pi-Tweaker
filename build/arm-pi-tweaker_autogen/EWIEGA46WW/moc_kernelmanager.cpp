/****************************************************************************
** Meta object code from reading C++ file 'kernelmanager.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.2.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../kernelmanager.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'kernelmanager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.2.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_KernelManager_t {
    const uint offsetsAndSize[112];
    char stringdata0[1005];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata_KernelManager_t, stringdata0) + ofs), len 
static const qt_meta_stringdata_KernelManager_t qt_meta_stringdata_KernelManager = {
    {
QT_MOC_LITERAL(0, 13), // "KernelManager"
QT_MOC_LITERAL(14, 25), // "setDefaultKernelRequested"
QT_MOC_LITERAL(40, 0), // ""
QT_MOC_LITERAL(41, 13), // "kernelVersion"
QT_MOC_LITERAL(55, 21), // "removeKernelRequested"
QT_MOC_LITERAL(77, 24), // "updateInitramfsRequested"
QT_MOC_LITERAL(102, 22), // "installKernelRequested"
QT_MOC_LITERAL(125, 13), // "kernelPackage"
QT_MOC_LITERAL(139, 30), // "installKernelToDeviceRequested"
QT_MOC_LITERAL(170, 10), // "devicePath"
QT_MOC_LITERAL(181, 19), // "applyPatchRequested"
QT_MOC_LITERAL(201, 9), // "patchFile"
QT_MOC_LITERAL(211, 20), // "revertPatchRequested"
QT_MOC_LITERAL(232, 9), // "patchName"
QT_MOC_LITERAL(242, 20), // "createPatchRequested"
QT_MOC_LITERAL(263, 12), // "originalFile"
QT_MOC_LITERAL(276, 12), // "modifiedFile"
QT_MOC_LITERAL(289, 29), // "applyKernelParameterRequested"
QT_MOC_LITERAL(319, 9), // "parameter"
QT_MOC_LITERAL(329, 5), // "value"
QT_MOC_LITERAL(335, 29), // "updateBootParametersRequested"
QT_MOC_LITERAL(365, 10), // "parameters"
QT_MOC_LITERAL(376, 27), // "updateKernelConfigRequested"
QT_MOC_LITERAL(404, 12), // "configOption"
QT_MOC_LITERAL(417, 19), // "loadModuleRequested"
QT_MOC_LITERAL(437, 10), // "moduleName"
QT_MOC_LITERAL(448, 21), // "unloadModuleRequested"
QT_MOC_LITERAL(470, 24), // "blacklistModuleRequested"
QT_MOC_LITERAL(495, 16), // "onRefreshKernels"
QT_MOC_LITERAL(512, 18), // "onSetDefaultKernel"
QT_MOC_LITERAL(531, 14), // "onRemoveKernel"
QT_MOC_LITERAL(546, 17), // "onUpdateInitramfs"
QT_MOC_LITERAL(564, 15), // "onInstallKernel"
QT_MOC_LITERAL(580, 24), // "onKernelSelectionChanged"
QT_MOC_LITERAL(605, 12), // "onUpdateGrub"
QT_MOC_LITERAL(618, 18), // "onViewKernelConfig"
QT_MOC_LITERAL(637, 23), // "onInstallKernelToDevice"
QT_MOC_LITERAL(661, 20), // "onUpdateGrubOnDevice"
QT_MOC_LITERAL(682, 23), // "onBrowseKernelDirectory"
QT_MOC_LITERAL(706, 19), // "onCopyCurrentKernel"
QT_MOC_LITERAL(726, 14), // "onBackupKernel"
QT_MOC_LITERAL(741, 17), // "onShowJoshuaFixes"
QT_MOC_LITERAL(759, 12), // "onApplyPatch"
QT_MOC_LITERAL(772, 13), // "onRevertPatch"
QT_MOC_LITERAL(786, 13), // "onCreatePatch"
QT_MOC_LITERAL(800, 15), // "onLoadPatchFile"
QT_MOC_LITERAL(816, 16), // "onRefreshPatches"
QT_MOC_LITERAL(833, 22), // "onApplyKernelParameter"
QT_MOC_LITERAL(856, 22), // "onUpdateBootParameters"
QT_MOC_LITERAL(879, 18), // "onEditKernelConfig"
QT_MOC_LITERAL(898, 18), // "onSaveKernelConfig"
QT_MOC_LITERAL(917, 12), // "onLoadModule"
QT_MOC_LITERAL(930, 14), // "onUnloadModule"
QT_MOC_LITERAL(945, 17), // "onBlacklistModule"
QT_MOC_LITERAL(963, 16), // "onRefreshModules"
QT_MOC_LITERAL(980, 24) // "onModuleSelectionChanged"

    },
    "KernelManager\0setDefaultKernelRequested\0"
    "\0kernelVersion\0removeKernelRequested\0"
    "updateInitramfsRequested\0"
    "installKernelRequested\0kernelPackage\0"
    "installKernelToDeviceRequested\0"
    "devicePath\0applyPatchRequested\0patchFile\0"
    "revertPatchRequested\0patchName\0"
    "createPatchRequested\0originalFile\0"
    "modifiedFile\0applyKernelParameterRequested\0"
    "parameter\0value\0updateBootParametersRequested\0"
    "parameters\0updateKernelConfigRequested\0"
    "configOption\0loadModuleRequested\0"
    "moduleName\0unloadModuleRequested\0"
    "blacklistModuleRequested\0onRefreshKernels\0"
    "onSetDefaultKernel\0onRemoveKernel\0"
    "onUpdateInitramfs\0onInstallKernel\0"
    "onKernelSelectionChanged\0onUpdateGrub\0"
    "onViewKernelConfig\0onInstallKernelToDevice\0"
    "onUpdateGrubOnDevice\0onBrowseKernelDirectory\0"
    "onCopyCurrentKernel\0onBackupKernel\0"
    "onShowJoshuaFixes\0onApplyPatch\0"
    "onRevertPatch\0onCreatePatch\0onLoadPatchFile\0"
    "onRefreshPatches\0onApplyKernelParameter\0"
    "onUpdateBootParameters\0onEditKernelConfig\0"
    "onSaveKernelConfig\0onLoadModule\0"
    "onUnloadModule\0onBlacklistModule\0"
    "onRefreshModules\0onModuleSelectionChanged"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KernelManager[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
      42,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      14,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,  266,    2, 0x06,    1 /* Public */,
       4,    1,  269,    2, 0x06,    3 /* Public */,
       5,    1,  272,    2, 0x06,    5 /* Public */,
       6,    1,  275,    2, 0x06,    7 /* Public */,
       8,    2,  278,    2, 0x06,    9 /* Public */,
      10,    1,  283,    2, 0x06,   12 /* Public */,
      12,    1,  286,    2, 0x06,   14 /* Public */,
      14,    2,  289,    2, 0x06,   16 /* Public */,
      17,    2,  294,    2, 0x06,   19 /* Public */,
      20,    1,  299,    2, 0x06,   22 /* Public */,
      22,    2,  302,    2, 0x06,   24 /* Public */,
      24,    1,  307,    2, 0x06,   27 /* Public */,
      26,    1,  310,    2, 0x06,   29 /* Public */,
      27,    1,  313,    2, 0x06,   31 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
      28,    0,  316,    2, 0x08,   33 /* Private */,
      29,    0,  317,    2, 0x08,   34 /* Private */,
      30,    0,  318,    2, 0x08,   35 /* Private */,
      31,    0,  319,    2, 0x08,   36 /* Private */,
      32,    0,  320,    2, 0x08,   37 /* Private */,
      33,    0,  321,    2, 0x08,   38 /* Private */,
      34,    0,  322,    2, 0x08,   39 /* Private */,
      35,    0,  323,    2, 0x08,   40 /* Private */,
      36,    0,  324,    2, 0x08,   41 /* Private */,
      37,    0,  325,    2, 0x08,   42 /* Private */,
      38,    0,  326,    2, 0x08,   43 /* Private */,
      39,    0,  327,    2, 0x08,   44 /* Private */,
      40,    0,  328,    2, 0x08,   45 /* Private */,
      41,    0,  329,    2, 0x08,   46 /* Private */,
      42,    0,  330,    2, 0x08,   47 /* Private */,
      43,    0,  331,    2, 0x08,   48 /* Private */,
      44,    0,  332,    2, 0x08,   49 /* Private */,
      45,    0,  333,    2, 0x08,   50 /* Private */,
      46,    0,  334,    2, 0x08,   51 /* Private */,
      47,    0,  335,    2, 0x08,   52 /* Private */,
      48,    0,  336,    2, 0x08,   53 /* Private */,
      49,    0,  337,    2, 0x08,   54 /* Private */,
      50,    0,  338,    2, 0x08,   55 /* Private */,
      51,    0,  339,    2, 0x08,   56 /* Private */,
      52,    0,  340,    2, 0x08,   57 /* Private */,
      53,    0,  341,    2, 0x08,   58 /* Private */,
      54,    0,  342,    2, 0x08,   59 /* Private */,
      55,    0,  343,    2, 0x08,   60 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QString,    7,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,    3,    9,
    QMetaType::Void, QMetaType::QString,   11,
    QMetaType::Void, QMetaType::QString,   13,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,   15,   16,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,   18,   19,
    QMetaType::Void, QMetaType::QStringList,   21,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,   23,   19,
    QMetaType::Void, QMetaType::QString,   25,
    QMetaType::Void, QMetaType::QString,   25,
    QMetaType::Void, QMetaType::QString,   25,

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

       0        // eod
};

void KernelManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<KernelManager *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->setDefaultKernelRequested((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 1: _t->removeKernelRequested((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 2: _t->updateInitramfsRequested((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 3: _t->installKernelRequested((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 4: _t->installKernelToDeviceRequested((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 5: _t->applyPatchRequested((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 6: _t->revertPatchRequested((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 7: _t->createPatchRequested((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 8: _t->applyKernelParameterRequested((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 9: _t->updateBootParametersRequested((*reinterpret_cast< std::add_pointer_t<QStringList>>(_a[1]))); break;
        case 10: _t->updateKernelConfigRequested((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 11: _t->loadModuleRequested((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 12: _t->unloadModuleRequested((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 13: _t->blacklistModuleRequested((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 14: _t->onRefreshKernels(); break;
        case 15: _t->onSetDefaultKernel(); break;
        case 16: _t->onRemoveKernel(); break;
        case 17: _t->onUpdateInitramfs(); break;
        case 18: _t->onInstallKernel(); break;
        case 19: _t->onKernelSelectionChanged(); break;
        case 20: _t->onUpdateGrub(); break;
        case 21: _t->onViewKernelConfig(); break;
        case 22: _t->onInstallKernelToDevice(); break;
        case 23: _t->onUpdateGrubOnDevice(); break;
        case 24: _t->onBrowseKernelDirectory(); break;
        case 25: _t->onCopyCurrentKernel(); break;
        case 26: _t->onBackupKernel(); break;
        case 27: _t->onShowJoshuaFixes(); break;
        case 28: _t->onApplyPatch(); break;
        case 29: _t->onRevertPatch(); break;
        case 30: _t->onCreatePatch(); break;
        case 31: _t->onLoadPatchFile(); break;
        case 32: _t->onRefreshPatches(); break;
        case 33: _t->onApplyKernelParameter(); break;
        case 34: _t->onUpdateBootParameters(); break;
        case 35: _t->onEditKernelConfig(); break;
        case 36: _t->onSaveKernelConfig(); break;
        case 37: _t->onLoadModule(); break;
        case 38: _t->onUnloadModule(); break;
        case 39: _t->onBlacklistModule(); break;
        case 40: _t->onRefreshModules(); break;
        case 41: _t->onModuleSelectionChanged(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (KernelManager::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KernelManager::setDefaultKernelRequested)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (KernelManager::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KernelManager::removeKernelRequested)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (KernelManager::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KernelManager::updateInitramfsRequested)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (KernelManager::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KernelManager::installKernelRequested)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (KernelManager::*)(const QString & , const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KernelManager::installKernelToDeviceRequested)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (KernelManager::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KernelManager::applyPatchRequested)) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (KernelManager::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KernelManager::revertPatchRequested)) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (KernelManager::*)(const QString & , const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KernelManager::createPatchRequested)) {
                *result = 7;
                return;
            }
        }
        {
            using _t = void (KernelManager::*)(const QString & , const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KernelManager::applyKernelParameterRequested)) {
                *result = 8;
                return;
            }
        }
        {
            using _t = void (KernelManager::*)(const QStringList & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KernelManager::updateBootParametersRequested)) {
                *result = 9;
                return;
            }
        }
        {
            using _t = void (KernelManager::*)(const QString & , const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KernelManager::updateKernelConfigRequested)) {
                *result = 10;
                return;
            }
        }
        {
            using _t = void (KernelManager::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KernelManager::loadModuleRequested)) {
                *result = 11;
                return;
            }
        }
        {
            using _t = void (KernelManager::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KernelManager::unloadModuleRequested)) {
                *result = 12;
                return;
            }
        }
        {
            using _t = void (KernelManager::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&KernelManager::blacklistModuleRequested)) {
                *result = 13;
                return;
            }
        }
    }
}

const QMetaObject KernelManager::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_KernelManager.offsetsAndSize,
    qt_meta_data_KernelManager,
    qt_static_metacall,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata_KernelManager_t
, QtPrivate::TypeAndForceComplete<KernelManager, std::true_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const QStringList &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<const QString &, std::false_type>
, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>


>,
    nullptr
} };


const QMetaObject *KernelManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KernelManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_KernelManager.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int KernelManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 42)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 42;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 42)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 42;
    }
    return _id;
}

// SIGNAL 0
void KernelManager::setDefaultKernelRequested(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void KernelManager::removeKernelRequested(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void KernelManager::updateInitramfsRequested(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void KernelManager::installKernelRequested(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void KernelManager::installKernelToDeviceRequested(const QString & _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void KernelManager::applyPatchRequested(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void KernelManager::revertPatchRequested(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void KernelManager::createPatchRequested(const QString & _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void KernelManager::applyKernelParameterRequested(const QString & _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void KernelManager::updateBootParametersRequested(const QStringList & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 9, _a);
}

// SIGNAL 10
void KernelManager::updateKernelConfigRequested(const QString & _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 10, _a);
}

// SIGNAL 11
void KernelManager::loadModuleRequested(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 11, _a);
}

// SIGNAL 12
void KernelManager::unloadModuleRequested(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 12, _a);
}

// SIGNAL 13
void KernelManager::blacklistModuleRequested(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 13, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
