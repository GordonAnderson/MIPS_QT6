/****************************************************************************
** Meta object code from reading C++ file 'psg.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.7.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "psg.h"
#include <QtGui/qtextcursor.h>
#include <QtGui/qscreen.h>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'psg.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASSPSGENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSPSGENDCLASS = QtMocHelpers::stringData(
    "PSG",
    "on_pbDownload_pressed",
    "",
    "on_pbViewTable_pressed",
    "on_pbLoadFromFile_pressed",
    "on_pbCreateNew_pressed",
    "on_pbSaveToFile_pressed",
    "on_pbEditCurrent_pressed",
    "on_leSequenceNumber_textEdited",
    "on_chkAutoAdvance_clicked",
    "on_pbTrigger_pressed",
    "on_pbRead_pressed",
    "on_pbWrite_pressed",
    "on_pbVisPulseSequence_pressed",
    "on_pbExitTableMode_pressed"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSPSGENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   92,    2, 0x08,    1 /* Private */,
       3,    0,   93,    2, 0x08,    2 /* Private */,
       4,    0,   94,    2, 0x08,    3 /* Private */,
       5,    0,   95,    2, 0x08,    4 /* Private */,
       6,    0,   96,    2, 0x08,    5 /* Private */,
       7,    0,   97,    2, 0x08,    6 /* Private */,
       8,    0,   98,    2, 0x08,    7 /* Private */,
       9,    0,   99,    2, 0x08,    8 /* Private */,
      10,    0,  100,    2, 0x08,    9 /* Private */,
      11,    0,  101,    2, 0x08,   10 /* Private */,
      12,    0,  102,    2, 0x08,   11 /* Private */,
      13,    0,  103,    2, 0x08,   12 /* Private */,
      14,    0,  104,    2, 0x08,   13 /* Private */,

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

       0        // eod
};

Q_CONSTINIT const QMetaObject PSG::staticMetaObject = { {
    QMetaObject::SuperData::link<QDialog::staticMetaObject>(),
    qt_meta_stringdata_CLASSPSGENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSPSGENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSPSGENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<PSG, std::true_type>,
        // method 'on_pbDownload_pressed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_pbViewTable_pressed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_pbLoadFromFile_pressed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_pbCreateNew_pressed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_pbSaveToFile_pressed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_pbEditCurrent_pressed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_leSequenceNumber_textEdited'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_chkAutoAdvance_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_pbTrigger_pressed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_pbRead_pressed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_pbWrite_pressed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_pbVisPulseSequence_pressed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_pbExitTableMode_pressed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void PSG::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<PSG *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->on_pbDownload_pressed(); break;
        case 1: _t->on_pbViewTable_pressed(); break;
        case 2: _t->on_pbLoadFromFile_pressed(); break;
        case 3: _t->on_pbCreateNew_pressed(); break;
        case 4: _t->on_pbSaveToFile_pressed(); break;
        case 5: _t->on_pbEditCurrent_pressed(); break;
        case 6: _t->on_leSequenceNumber_textEdited(); break;
        case 7: _t->on_chkAutoAdvance_clicked(); break;
        case 8: _t->on_pbTrigger_pressed(); break;
        case 9: _t->on_pbRead_pressed(); break;
        case 10: _t->on_pbWrite_pressed(); break;
        case 11: _t->on_pbVisPulseSequence_pressed(); break;
        case 12: _t->on_pbExitTableMode_pressed(); break;
        default: ;
        }
    }
    (void)_a;
}

const QMetaObject *PSG::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PSG::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSPSGENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int PSG::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 13)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 13;
    }
    return _id;
}
QT_WARNING_POP
