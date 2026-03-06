/****************************************************************************
** Meta object code from reading C++ file 'pse.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.7.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../pse.h"
#include <QtGui/qtextcursor.h>
#include <QtGui/qscreen.h>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'pse.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASSpseDialogENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSpseDialogENDCLASS = QtMocHelpers::stringData(
    "pseDialog",
    "on_DIO_checked",
    "",
    "on_pbNext_pressed",
    "on_pbPrevious_pressed",
    "on_DCBIAS_edited",
    "on_pbInsert_pressed",
    "on_pbDelete_pressed",
    "on_leName_textChanged",
    "arg1",
    "on_leClocks_textChanged",
    "on_leCycles_textChanged",
    "on_chkLoop_clicked",
    "checked",
    "on_comboLoop_currentIndexChanged",
    "InsertPulse",
    "MakeRamp",
    "RampCancel"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSpseDialogENDCLASS[] = {

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
       8,    1,  104,    2, 0x08,    7 /* Private */,
      10,    1,  107,    2, 0x08,    9 /* Private */,
      11,    1,  110,    2, 0x08,   11 /* Private */,
      12,    1,  113,    2, 0x08,   13 /* Private */,
      14,    1,  116,    2, 0x08,   15 /* Private */,
      15,    0,  119,    2, 0x08,   17 /* Private */,
      16,    0,  120,    2, 0x08,   18 /* Private */,
      17,    0,  121,    2, 0x08,   19 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    9,
    QMetaType::Void, QMetaType::QString,    9,
    QMetaType::Void, QMetaType::QString,    9,
    QMetaType::Void, QMetaType::Bool,   13,
    QMetaType::Void, QMetaType::QString,    9,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject pseDialog::staticMetaObject = { {
    QMetaObject::SuperData::link<QDialog::staticMetaObject>(),
    qt_meta_stringdata_CLASSpseDialogENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSpseDialogENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSpseDialogENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<pseDialog, std::true_type>,
        // method 'on_DIO_checked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_pbNext_pressed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_pbPrevious_pressed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_DCBIAS_edited'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_pbInsert_pressed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_pbDelete_pressed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_leName_textChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'on_leClocks_textChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'on_leCycles_textChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'on_chkLoop_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'on_comboLoop_currentIndexChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'InsertPulse'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'MakeRamp'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'RampCancel'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void pseDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<pseDialog *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->on_DIO_checked(); break;
        case 1: _t->on_pbNext_pressed(); break;
        case 2: _t->on_pbPrevious_pressed(); break;
        case 3: _t->on_DCBIAS_edited(); break;
        case 4: _t->on_pbInsert_pressed(); break;
        case 5: _t->on_pbDelete_pressed(); break;
        case 6: _t->on_leName_textChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 7: _t->on_leClocks_textChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 8: _t->on_leCycles_textChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 9: _t->on_chkLoop_clicked((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 10: _t->on_comboLoop_currentIndexChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 11: _t->InsertPulse(); break;
        case 12: _t->MakeRamp(); break;
        case 13: _t->RampCancel(); break;
        default: ;
        }
    }
}

const QMetaObject *pseDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *pseDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSpseDialogENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int pseDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
QT_WARNING_POP
