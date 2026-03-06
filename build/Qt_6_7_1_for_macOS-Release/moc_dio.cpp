/****************************************************************************
** Meta object code from reading C++ file 'dio.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.7.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../dio.h"
#include <QtGui/qtextcursor.h>
#include <QtGui/qscreen.h>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'dio.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASSDIOENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSDIOENDCLASS = QtMocHelpers::stringData(
    "DIO",
    "UpdateDIO",
    "",
    "DOUpdated",
    "TrigHigh",
    "TrigLow",
    "TrigPulse",
    "RFgenerate",
    "SetFreq",
    "SetWidth",
    "RemoteNavigation",
    "RemoteNavSmallUP",
    "RemoteNavLargeUP",
    "RemoteNavSmallDown",
    "RemoteNavLargeDown",
    "RemoteNavSelect"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSDIOENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      14,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   98,    2, 0x08,    1 /* Private */,
       3,    0,   99,    2, 0x08,    2 /* Private */,
       4,    0,  100,    2, 0x08,    3 /* Private */,
       5,    0,  101,    2, 0x08,    4 /* Private */,
       6,    0,  102,    2, 0x08,    5 /* Private */,
       7,    0,  103,    2, 0x08,    6 /* Private */,
       8,    0,  104,    2, 0x08,    7 /* Private */,
       9,    0,  105,    2, 0x08,    8 /* Private */,
      10,    0,  106,    2, 0x08,    9 /* Private */,
      11,    0,  107,    2, 0x08,   10 /* Private */,
      12,    0,  108,    2, 0x08,   11 /* Private */,
      13,    0,  109,    2, 0x08,   12 /* Private */,
      14,    0,  110,    2, 0x08,   13 /* Private */,
      15,    0,  111,    2, 0x08,   14 /* Private */,

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

       0        // eod
};

Q_CONSTINIT const QMetaObject DIO::staticMetaObject = { {
    QMetaObject::SuperData::link<QDialog::staticMetaObject>(),
    qt_meta_stringdata_CLASSDIOENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSDIOENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSDIOENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<DIO, std::true_type>,
        // method 'UpdateDIO'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'DOUpdated'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'TrigHigh'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'TrigLow'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'TrigPulse'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'RFgenerate'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'SetFreq'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'SetWidth'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'RemoteNavigation'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'RemoteNavSmallUP'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'RemoteNavLargeUP'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'RemoteNavSmallDown'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'RemoteNavLargeDown'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'RemoteNavSelect'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void DIO::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<DIO *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->UpdateDIO(); break;
        case 1: _t->DOUpdated(); break;
        case 2: _t->TrigHigh(); break;
        case 3: _t->TrigLow(); break;
        case 4: _t->TrigPulse(); break;
        case 5: _t->RFgenerate(); break;
        case 6: _t->SetFreq(); break;
        case 7: _t->SetWidth(); break;
        case 8: _t->RemoteNavigation(); break;
        case 9: _t->RemoteNavSmallUP(); break;
        case 10: _t->RemoteNavLargeUP(); break;
        case 11: _t->RemoteNavSmallDown(); break;
        case 12: _t->RemoteNavLargeDown(); break;
        case 13: _t->RemoteNavSelect(); break;
        default: ;
        }
    }
    (void)_a;
}

const QMetaObject *DIO::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DIO::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSDIOENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int DIO::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
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
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSDIOchannelENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSDIOchannelENDCLASS = QtMocHelpers::stringData(
    "DIOchannel",
    "DIOChange",
    ""
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSDIOchannelENDCLASS[] = {

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
       1,    1,   20,    2, 0x08,    1 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::Bool,    2,

       0        // eod
};

Q_CONSTINIT const QMetaObject DIOchannel::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_CLASSDIOchannelENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSDIOchannelENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSDIOchannelENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<DIOchannel, std::true_type>,
        // method 'DIOChange'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>
    >,
    nullptr
} };

void DIOchannel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<DIOchannel *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->DIOChange((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject *DIOchannel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DIOchannel::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSDIOchannelENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int DIOchannel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
QT_WARNING_POP
