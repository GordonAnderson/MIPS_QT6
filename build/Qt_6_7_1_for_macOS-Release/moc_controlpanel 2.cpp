/****************************************************************************
** Meta object code from reading C++ file 'controlpanel.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.7.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../controlpanel.h"
#include <QtGui/qtextcursor.h>
#include <QtGui/qscreen.h>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'controlpanel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.7.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSTextLabelENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSTextLabelENDCLASS = QtMocHelpers::stringData(
    "TextLabel"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSTextLabelENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

Q_CONSTINIT const QMetaObject TextLabel::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_CLASSTextLabelENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSTextLabelENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSTextLabelENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<TextLabel, std::true_type>
    >,
    nullptr
} };

void TextLabel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject *TextLabel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TextLabel::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSTextLabelENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int TextLabel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSShutdownENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSShutdownENDCLASS = QtMocHelpers::stringData(
    "Shutdown",
    "ShutdownSystem",
    "",
    "EnableSystem",
    "pbPressed"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSShutdownENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   32,    2, 0x06,    1 /* Public */,
       3,    0,   33,    2, 0x06,    2 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       4,    0,   34,    2, 0x08,    3 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject Shutdown::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_CLASSShutdownENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSShutdownENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSShutdownENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<Shutdown, std::true_type>,
        // method 'ShutdownSystem'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'EnableSystem'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'pbPressed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void Shutdown::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Shutdown *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->ShutdownSystem(); break;
        case 1: _t->EnableSystem(); break;
        case 2: _t->pbPressed(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (Shutdown::*)();
            if (_t _q_method = &Shutdown::ShutdownSystem; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (Shutdown::*)();
            if (_t _q_method = &Shutdown::EnableSystem; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
    }
    (void)_a;
}

const QMetaObject *Shutdown::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Shutdown::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSShutdownENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int Shutdown::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void Shutdown::ShutdownSystem()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void Shutdown::EnableSystem()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSSaveLoadENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSSaveLoadENDCLASS = QtMocHelpers::stringData(
    "SaveLoad",
    "Save",
    "",
    "Load",
    "pbSavePressed",
    "pbLoadPressed"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSSaveLoadENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   38,    2, 0x06,    1 /* Public */,
       3,    0,   39,    2, 0x06,    2 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       4,    0,   40,    2, 0x08,    3 /* Private */,
       5,    0,   41,    2, 0x08,    4 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject SaveLoad::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_CLASSSaveLoadENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSSaveLoadENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSSaveLoadENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<SaveLoad, std::true_type>,
        // method 'Save'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'Load'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'pbSavePressed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'pbLoadPressed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void SaveLoad::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<SaveLoad *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->Save(); break;
        case 1: _t->Load(); break;
        case 2: _t->pbSavePressed(); break;
        case 3: _t->pbLoadPressed(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (SaveLoad::*)();
            if (_t _q_method = &SaveLoad::Save; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (SaveLoad::*)();
            if (_t _q_method = &SaveLoad::Load; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
    }
    (void)_a;
}

const QMetaObject *SaveLoad::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SaveLoad::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSSaveLoadENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int SaveLoad::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
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
void SaveLoad::Save()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void SaveLoad::Load()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSDACchannelENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSDACchannelENDCLASS = QtMocHelpers::stringData(
    "DACchannel",
    "VdacChange",
    ""
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSDACchannelENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   20,    2, 0x08,    1 /* Private */,

 // slots: parameters
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject DACchannel::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_CLASSDACchannelENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSDACchannelENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSDACchannelENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<DACchannel, std::true_type>,
        // method 'VdacChange'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void DACchannel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<DACchannel *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->VdacChange(); break;
        default: ;
        }
    }
    (void)_a;
}

const QMetaObject *DACchannel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DACchannel::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSDACchannelENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int DACchannel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 1;
    }
    return _id;
}
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSDCBiasGroupsENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSDCBiasGroupsENDCLASS = QtMocHelpers::stringData(
    "DCBiasGroups",
    "disable",
    "",
    "enable",
    "slotEnableChange"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSDCBiasGroupsENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   32,    2, 0x06,    1 /* Public */,
       3,    0,   33,    2, 0x06,    2 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       4,    0,   34,    2, 0x08,    3 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject DCBiasGroups::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_CLASSDCBiasGroupsENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSDCBiasGroupsENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSDCBiasGroupsENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<DCBiasGroups, std::true_type>,
        // method 'disable'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'enable'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'slotEnableChange'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void DCBiasGroups::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<DCBiasGroups *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->disable(); break;
        case 1: _t->enable(); break;
        case 2: _t->slotEnableChange(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (DCBiasGroups::*)();
            if (_t _q_method = &DCBiasGroups::disable; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (DCBiasGroups::*)();
            if (_t _q_method = &DCBiasGroups::enable; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
    }
    (void)_a;
}

const QMetaObject *DCBiasGroups::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DCBiasGroups::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSDCBiasGroupsENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int DCBiasGroups::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void DCBiasGroups::disable()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void DCBiasGroups::enable()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSESIENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSESIENDCLASS = QtMocHelpers::stringData(
    "ESI",
    "ESIChange",
    "",
    "ESIenaChange"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSESIENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   26,    2, 0x08,    1 /* Private */,
       3,    0,   27,    2, 0x08,    2 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject ESI::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_CLASSESIENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSESIENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSESIENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<ESI, std::true_type>,
        // method 'ESIChange'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'ESIenaChange'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void ESI::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ESI *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->ESIChange(); break;
        case 1: _t->ESIenaChange(); break;
        default: ;
        }
    }
    (void)_a;
}

const QMetaObject *ESI::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ESI::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSESIENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int ESI::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 2;
    }
    return _id;
}
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSCcontrolENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSCcontrolENDCLASS = QtMocHelpers::stringData(
    "Ccontrol",
    "change",
    "",
    "scriptName",
    "VspChange",
    "pbButtonPressed",
    "chkBoxStateChanged",
    "Qt::CheckState",
    "comboBoxChanged"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSCcontrolENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   44,    2, 0x06,    1 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       4,    0,   47,    2, 0x08,    3 /* Private */,
       5,    0,   48,    2, 0x08,    4 /* Private */,
       6,    1,   49,    2, 0x08,    5 /* Private */,
       8,    1,   52,    2, 0x08,    7 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 7,    2,
    QMetaType::Void, QMetaType::Int,    2,

       0        // eod
};

Q_CONSTINIT const QMetaObject Ccontrol::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_CLASSCcontrolENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSCcontrolENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSCcontrolENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<Ccontrol, std::true_type>,
        // method 'change'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'VspChange'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'pbButtonPressed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'chkBoxStateChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<Qt::CheckState, std::false_type>,
        // method 'comboBoxChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>
    >,
    nullptr
} };

void Ccontrol::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Ccontrol *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->change((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 1: _t->VspChange(); break;
        case 2: _t->pbButtonPressed(); break;
        case 3: _t->chkBoxStateChanged((*reinterpret_cast< std::add_pointer_t<Qt::CheckState>>(_a[1]))); break;
        case 4: _t->comboBoxChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (Ccontrol::*)(QString );
            if (_t _q_method = &Ccontrol::change; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject *Ccontrol::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Ccontrol::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSCcontrolENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int Ccontrol::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void Ccontrol::change(QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSCpanelENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSCpanelENDCLASS = QtMocHelpers::stringData(
    "Cpanel",
    "pbButtonPressed",
    "",
    "slotDialogClosed"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSCpanelENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   26,    2, 0x08,    1 /* Private */,
       3,    0,   27,    2, 0x08,    2 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject Cpanel::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_CLASSCpanelENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSCpanelENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSCpanelENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<Cpanel, std::true_type>,
        // method 'pbButtonPressed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'slotDialogClosed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void Cpanel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Cpanel *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->pbButtonPressed(); break;
        case 1: _t->slotDialogClosed(); break;
        default: ;
        }
    }
    (void)_a;
}

const QMetaObject *Cpanel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Cpanel::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSCpanelENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int Cpanel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 2;
    }
    return _id;
}
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSCPbuttonENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSCPbuttonENDCLASS = QtMocHelpers::stringData(
    "CPbutton",
    "CPselected",
    "",
    "pbCPselectPressed"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSCPbuttonENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   26,    2, 0x06,    1 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       3,    0,   29,    2, 0x08,    3 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    2,

 // slots: parameters
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject CPbutton::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_CLASSCPbuttonENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSCPbuttonENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSCPbuttonENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<CPbutton, std::true_type>,
        // method 'CPselected'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'pbCPselectPressed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void CPbutton::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CPbutton *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->CPselected((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 1: _t->pbCPselectPressed(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (CPbutton::*)(QString );
            if (_t _q_method = &CPbutton::CPselected; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject *CPbutton::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CPbutton::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSCPbuttonENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int CPbutton::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void CPbutton::CPselected(QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSLightWidgetENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSLightWidgetENDCLASS = QtMocHelpers::stringData(
    "LightWidget",
    "turnOff",
    "",
    "turnOn",
    "on"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSLightWidgetENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       1,   28, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   26,    2, 0x0a,    2 /* Public */,
       3,    0,   27,    2, 0x0a,    3 /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,

 // properties: name, type, flags
       4, QMetaType::Bool, 0x00015103, uint(-1), 0,

       0        // eod
};

Q_CONSTINIT const QMetaObject LightWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_CLASSLightWidgetENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSLightWidgetENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSLightWidgetENDCLASS_t,
        // property 'on'
        QtPrivate::TypeAndForceComplete<bool, std::true_type>,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<LightWidget, std::true_type>,
        // method 'turnOff'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'turnOn'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void LightWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<LightWidget *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->turnOff(); break;
        case 1: _t->turnOn(); break;
        default: ;
        }
    } else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<LightWidget *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< bool*>(_v) = _t->isOn(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        auto *_t = static_cast<LightWidget *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setOn(*reinterpret_cast< bool*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    } else if (_c == QMetaObject::BindableProperty) {
    }
    (void)_a;
}

const QMetaObject *LightWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *LightWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSLightWidgetENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int LightWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 2;
    }else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    return _id;
}
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSStatusLightENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSStatusLightENDCLASS = QtMocHelpers::stringData(
    "StatusLight"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSStatusLightENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

Q_CONSTINIT const QMetaObject StatusLight::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_CLASSStatusLightENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSStatusLightENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSStatusLightENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<StatusLight, std::true_type>
    >,
    nullptr
} };

void StatusLight::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject *StatusLight::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *StatusLight::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSStatusLightENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int StatusLight::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSTextMessageENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSTextMessageENDCLASS = QtMocHelpers::stringData(
    "TextMessage"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSTextMessageENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

Q_CONSTINIT const QMetaObject TextMessage::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_CLASSTextMessageENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSTextMessageENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSTextMessageENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<TextMessage, std::true_type>
    >,
    nullptr
} };

void TextMessage::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject *TextMessage::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TextMessage::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSTextMessageENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int TextMessage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSTableENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSTableENDCLASS = QtMocHelpers::stringData(
    "Table",
    "change",
    "",
    "scriptName",
    "tableChange",
    "row",
    "column"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSTableENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   26,    2, 0x06,    1 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       4,    2,   29,    2, 0x0a,    3 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,

 // slots: parameters
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    5,    6,

       0        // eod
};

Q_CONSTINIT const QMetaObject Table::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_CLASSTableENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSTableENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSTableENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<Table, std::true_type>,
        // method 'change'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'tableChange'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>
    >,
    nullptr
} };

void Table::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Table *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->change((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 1: _t->tableChange((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (Table::*)(QString );
            if (_t _q_method = &Table::change; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject *Table::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Table::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSTableENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int Table::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void Table::change(QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSSliderENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSSliderENDCLASS = QtMocHelpers::stringData(
    "Slider",
    "change",
    "",
    "scriptName",
    "sliderChange",
    "value"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSSliderENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   26,    2, 0x06,    1 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       4,    1,   29,    2, 0x0a,    3 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    5,

       0        // eod
};

Q_CONSTINIT const QMetaObject Slider::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_CLASSSliderENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSSliderENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSSliderENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<Slider, std::true_type>,
        // method 'change'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'sliderChange'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>
    >,
    nullptr
} };

void Slider::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Slider *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->change((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 1: _t->sliderChange((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (Slider::*)(QString );
            if (_t _q_method = &Slider::change; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject *Slider::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Slider::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSSliderENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int Slider::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void Slider::change(QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSControlPanelENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSControlPanelENDCLASS = QtMocHelpers::stringData(
    "ControlPanel",
    "DialogClosed",
    "",
    "NextCP",
    "pbSD",
    "pbSE",
    "pbSave",
    "pbLoad",
    "pbMIPScomms",
    "CloseMIPScomms",
    "scriptShutdown",
    "DCBgroupDisable",
    "DCBgroupEnable",
    "slotDataAcquired",
    "filepath",
    "slotDataFileDefined",
    "pbScript",
    "popupHelp",
    "slotComments",
    "slotCommentsFinished",
    "slotSaveCP",
    "slotGeneralHelp",
    "slotMIPScommands",
    "slotScriptHelp",
    "slotThisControlPanelHelp",
    "tcpCommand",
    "pbARBcompressor",
    "slotLogStatusBarMessage",
    "slotOpenLogFile",
    "slotCloseLogFile",
    "slotCPselected",
    "slotPlotDialogClosed",
    "Plot*",
    "controlChange",
    "slotExtProcessClosed",
    "slotExternalProcessChange",
    "Save",
    "Filename",
    "Load",
    "GetLine",
    "MIPSname",
    "SendString",
    "message",
    "SendCommand",
    "SendMess",
    "SystemEnable",
    "SystemShutdown",
    "isShutDown",
    "Acquire",
    "filePath",
    "isAcquiring",
    "DismissAcquire",
    "statusMessage",
    "popupMessage",
    "popupYesNoMessage",
    "popupUserInput",
    "title",
    "sysUpdating",
    "UpdateHalted",
    "stop",
    "SelectFile",
    "fType",
    "Title",
    "Ext",
    "ReadCSVfile",
    "fileName",
    "delimiter",
    "ReadCSVentry",
    "line",
    "entry",
    "Command",
    "cmd",
    "CreatePlot",
    "Yaxis",
    "Xaxis",
    "NumPlots",
    "PlotCommand",
    "isComms",
    "MakePathNameUnique",
    "path",
    "GenerateUniqueFileName",
    "MakeFileNameUnique",
    "AppendToFile",
    "record",
    "ReadFileLine",
    "ReadFile",
    "elapsedTime",
    "init",
    "tcpSocket",
    "arg1",
    "arg2",
    "setValue",
    "key",
    "value",
    "getValue",
    "CreateProcess",
    "name",
    "program"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSControlPanelENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      67,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,  416,    2, 0x06,    1 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       4,    0,  419,    2, 0x0a,    3 /* Public */,
       5,    0,  420,    2, 0x0a,    4 /* Public */,
       6,    0,  421,    2, 0x0a,    5 /* Public */,
       7,    0,  422,    2, 0x0a,    6 /* Public */,
       8,    0,  423,    2, 0x0a,    7 /* Public */,
       9,    0,  424,    2, 0x0a,    8 /* Public */,
      10,    0,  425,    2, 0x0a,    9 /* Public */,
      11,    0,  426,    2, 0x0a,   10 /* Public */,
      12,    0,  427,    2, 0x0a,   11 /* Public */,
      13,    1,  428,    2, 0x0a,   12 /* Public */,
      15,    1,  431,    2, 0x0a,   14 /* Public */,
      16,    0,  434,    2, 0x0a,   16 /* Public */,
      17,    1,  435,    2, 0x0a,   17 /* Public */,
      18,    0,  438,    2, 0x0a,   19 /* Public */,
      19,    0,  439,    2, 0x0a,   20 /* Public */,
      20,    0,  440,    2, 0x0a,   21 /* Public */,
      21,    0,  441,    2, 0x0a,   22 /* Public */,
      22,    0,  442,    2, 0x0a,   23 /* Public */,
      23,    0,  443,    2, 0x0a,   24 /* Public */,
      24,    0,  444,    2, 0x0a,   25 /* Public */,
      25,    0,  445,    2, 0x0a,   26 /* Public */,
      26,    0,  446,    2, 0x0a,   27 /* Public */,
      27,    1,  447,    2, 0x0a,   28 /* Public */,
      28,    0,  450,    2, 0x0a,   30 /* Public */,
      29,    0,  451,    2, 0x0a,   31 /* Public */,
      30,    1,  452,    2, 0x0a,   32 /* Public */,
      31,    1,  455,    2, 0x0a,   34 /* Public */,
      33,    1,  458,    2, 0x0a,   36 /* Public */,
      34,    1,  461,    2, 0x0a,   38 /* Public */,
      35,    1,  464,    2, 0x0a,   40 /* Public */,
      36,    1,  467,    2, 0x0a,   42 /* Public */,
      38,    1,  470,    2, 0x0a,   44 /* Public */,
      39,    1,  473,    2, 0x0a,   46 /* Public */,
      41,    2,  476,    2, 0x0a,   48 /* Public */,
      43,    2,  481,    2, 0x0a,   51 /* Public */,
      44,    2,  486,    2, 0x0a,   54 /* Public */,
      45,    0,  491,    2, 0x0a,   57 /* Public */,
      46,    0,  492,    2, 0x0a,   58 /* Public */,
      47,    0,  493,    2, 0x0a,   59 /* Public */,
      48,    1,  494,    2, 0x0a,   60 /* Public */,
      50,    0,  497,    2, 0x0a,   62 /* Public */,
      51,    0,  498,    2, 0x0a,   63 /* Public */,
      52,    1,  499,    2, 0x0a,   64 /* Public */,
      53,    1,  502,    2, 0x0a,   66 /* Public */,
      54,    1,  505,    2, 0x0a,   68 /* Public */,
      55,    2,  508,    2, 0x0a,   70 /* Public */,
      57,    0,  513,    2, 0x0a,   73 /* Public */,
      58,    1,  514,    2, 0x0a,   74 /* Public */,
      60,    3,  517,    2, 0x0a,   76 /* Public */,
      64,    2,  524,    2, 0x0a,   80 /* Public */,
      67,    2,  529,    2, 0x0a,   83 /* Public */,
      70,    1,  534,    2, 0x0a,   86 /* Public */,
      72,    4,  537,    2, 0x0a,   88 /* Public */,
      76,    1,  546,    2, 0x0a,   93 /* Public */,
      77,    0,  549,    2, 0x0a,   95 /* Public */,
      78,    1,  550,    2, 0x0a,   96 /* Public */,
      80,    0,  553,    2, 0x0a,   98 /* Public */,
      81,    1,  554,    2, 0x0a,   99 /* Public */,
      82,    2,  557,    2, 0x0a,  101 /* Public */,
      84,    2,  562,    2, 0x0a,  104 /* Public */,
      85,    1,  567,    2, 0x0a,  107 /* Public */,
      86,    1,  570,    2, 0x0a,  109 /* Public */,
      88,    3,  573,    2, 0x0a,  111 /* Public */,
      91,    2,  580,    2, 0x0a,  115 /* Public */,
      94,    1,  585,    2, 0x0a,  118 /* Public */,
      95,    2,  588,    2, 0x0a,  120 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,

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
    QMetaType::Void, QMetaType::QString,   14,
    QMetaType::Void, QMetaType::QString,   14,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QPoint,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, 0x80000000 | 32,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::QString, QMetaType::QString,   37,
    QMetaType::QString, QMetaType::QString,   37,
    QMetaType::QString, QMetaType::QString,   40,
    QMetaType::Bool, QMetaType::QString, QMetaType::QString,   40,   42,
    QMetaType::Bool, QMetaType::QString, QMetaType::QString,   40,   42,
    QMetaType::QString, QMetaType::QString, QMetaType::QString,   40,   42,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Bool,
    QMetaType::Void, QMetaType::QString,   49,
    QMetaType::Bool,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   42,
    QMetaType::Void, QMetaType::QString,   42,
    QMetaType::Bool, QMetaType::QString,   42,
    QMetaType::QString, QMetaType::QString, QMetaType::QString,   56,   42,
    QMetaType::Bool,
    QMetaType::Bool, QMetaType::Bool,   59,
    QMetaType::QString, QMetaType::QString, QMetaType::QString, QMetaType::QString,   61,   62,   63,
    QMetaType::Int, QMetaType::QString, QMetaType::QString,   65,   66,
    QMetaType::QString, QMetaType::Int, QMetaType::Int,   68,   69,
    QMetaType::QString, QMetaType::QString,   71,
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::QString, QMetaType::Int,   62,   73,   74,   75,
    QMetaType::Void, QMetaType::QString,   71,
    QMetaType::Bool,
    QMetaType::QString, QMetaType::QString,   79,
    QMetaType::QString,
    QMetaType::QString, QMetaType::QString,   65,
    QMetaType::QString, QMetaType::QString, QMetaType::QString,   65,   83,
    QMetaType::QString, QMetaType::QString, QMetaType::Int,   65,   68,
    QMetaType::QString, QMetaType::QString,   65,
    QMetaType::Int, QMetaType::Bool,   87,
    QMetaType::QString, QMetaType::QString, QMetaType::QString, QMetaType::QString,   71,   89,   90,
    QMetaType::Void, QMetaType::QString, QMetaType::QVariant,   92,   93,
    QMetaType::QVariant, QMetaType::QString,   92,
    QMetaType::Bool, QMetaType::QString, QMetaType::QString,   96,   97,

       0        // eod
};

Q_CONSTINIT const QMetaObject ControlPanel::staticMetaObject = { {
    QMetaObject::SuperData::link<QDialog::staticMetaObject>(),
    qt_meta_stringdata_CLASSControlPanelENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSControlPanelENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSControlPanelENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<ControlPanel, std::true_type>,
        // method 'DialogClosed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'pbSD'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'pbSE'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'pbSave'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'pbLoad'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'pbMIPScomms'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'CloseMIPScomms'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'scriptShutdown'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'DCBgroupDisable'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'DCBgroupEnable'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'slotDataAcquired'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'slotDataFileDefined'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'pbScript'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'popupHelp'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QPoint, std::false_type>,
        // method 'slotComments'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'slotCommentsFinished'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'slotSaveCP'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'slotGeneralHelp'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'slotMIPScommands'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'slotScriptHelp'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'slotThisControlPanelHelp'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'tcpCommand'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'pbARBcompressor'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'slotLogStatusBarMessage'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'slotOpenLogFile'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'slotCloseLogFile'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'slotCPselected'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'slotPlotDialogClosed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<Plot *, std::false_type>,
        // method 'controlChange'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'slotExtProcessClosed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'slotExternalProcessChange'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'Save'
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'Load'
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'GetLine'
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'SendString'
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'SendCommand'
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'SendMess'
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'SystemEnable'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'SystemShutdown'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'isShutDown'
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'Acquire'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'isAcquiring'
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'DismissAcquire'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'statusMessage'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'popupMessage'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'popupYesNoMessage'
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'popupUserInput'
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'sysUpdating'
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'UpdateHalted'
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'SelectFile'
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'ReadCSVfile'
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'ReadCSVentry'
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'Command'
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'CreatePlot'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'PlotCommand'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'isComms'
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'MakePathNameUnique'
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'GenerateUniqueFileName'
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'MakeFileNameUnique'
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'AppendToFile'
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'ReadFileLine'
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'ReadFile'
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'elapsedTime'
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'tcpSocket'
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'setValue'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QVariant &, std::false_type>,
        // method 'getValue'
        QtPrivate::TypeAndForceComplete<QVariant, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'CreateProcess'
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>
    >,
    nullptr
} };

void ControlPanel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ControlPanel *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->DialogClosed((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 1: _t->pbSD(); break;
        case 2: _t->pbSE(); break;
        case 3: _t->pbSave(); break;
        case 4: _t->pbLoad(); break;
        case 5: _t->pbMIPScomms(); break;
        case 6: _t->CloseMIPScomms(); break;
        case 7: _t->scriptShutdown(); break;
        case 8: _t->DCBgroupDisable(); break;
        case 9: _t->DCBgroupEnable(); break;
        case 10: _t->slotDataAcquired((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 11: _t->slotDataFileDefined((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 12: _t->pbScript(); break;
        case 13: _t->popupHelp((*reinterpret_cast< std::add_pointer_t<QPoint>>(_a[1]))); break;
        case 14: _t->slotComments(); break;
        case 15: _t->slotCommentsFinished(); break;
        case 16: _t->slotSaveCP(); break;
        case 17: _t->slotGeneralHelp(); break;
        case 18: _t->slotMIPScommands(); break;
        case 19: _t->slotScriptHelp(); break;
        case 20: _t->slotThisControlPanelHelp(); break;
        case 21: _t->tcpCommand(); break;
        case 22: _t->pbARBcompressor(); break;
        case 23: _t->slotLogStatusBarMessage((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 24: _t->slotOpenLogFile(); break;
        case 25: _t->slotCloseLogFile(); break;
        case 26: _t->slotCPselected((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 27: _t->slotPlotDialogClosed((*reinterpret_cast< std::add_pointer_t<Plot*>>(_a[1]))); break;
        case 28: _t->controlChange((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 29: _t->slotExtProcessClosed((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 30: _t->slotExternalProcessChange((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 31: { QString _r = _t->Save((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 32: { QString _r = _t->Load((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 33: { QString _r = _t->GetLine((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 34: { bool _r = _t->SendString((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 35: { bool _r = _t->SendCommand((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 36: { QString _r = _t->SendMess((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 37: _t->SystemEnable(); break;
        case 38: _t->SystemShutdown(); break;
        case 39: { bool _r = _t->isShutDown();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 40: _t->Acquire((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 41: { bool _r = _t->isAcquiring();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 42: _t->DismissAcquire(); break;
        case 43: _t->statusMessage((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 44: _t->popupMessage((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 45: { bool _r = _t->popupYesNoMessage((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 46: { QString _r = _t->popupUserInput((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 47: { bool _r = _t->sysUpdating();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 48: { bool _r = _t->UpdateHalted((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 49: { QString _r = _t->SelectFile((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[3])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 50: { int _r = _t->ReadCSVfile((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = std::move(_r); }  break;
        case 51: { QString _r = _t->ReadCSVentry((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 52: { QString _r = _t->Command((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 53: _t->CreatePlot((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[4]))); break;
        case 54: _t->PlotCommand((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 55: { bool _r = _t->isComms();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 56: { QString _r = _t->MakePathNameUnique((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 57: { QString _r = _t->GenerateUniqueFileName();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 58: { QString _r = _t->MakeFileNameUnique((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 59: { QString _r = _t->AppendToFile((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 60: { QString _r = _t->ReadFileLine((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 61: { QString _r = _t->ReadFile((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 62: { int _r = _t->elapsedTime((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = std::move(_r); }  break;
        case 63: { QString _r = _t->tcpSocket((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[3])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 64: _t->setValue((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QVariant>>(_a[2]))); break;
        case 65: { QVariant _r = _t->getValue((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast< QVariant*>(_a[0]) = std::move(_r); }  break;
        case 66: { bool _r = _t->CreateProcess((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 27:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< Plot* >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ControlPanel::*)(QString );
            if (_t _q_method = &ControlPanel::DialogClosed; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject *ControlPanel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ControlPanel::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSControlPanelENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int ControlPanel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 67)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 67;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 67)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 67;
    }
    return _id;
}

// SIGNAL 0
void ControlPanel::DialogClosed(QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
