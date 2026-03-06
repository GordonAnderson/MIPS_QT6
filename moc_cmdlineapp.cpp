/****************************************************************************
** Meta object code from reading C++ file 'cmdlineapp.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.7.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "cmdlineapp.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'cmdlineapp.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASScmdlineappENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASScmdlineappENDCLASS = QtMocHelpers::stringData(
    "cmdlineapp",
    "DialogClosed",
    "",
    "AppCompleted",
    "Ready",
    "readProcessOutput",
    "readMessage",
    "AppFinished",
    "sendNO",
    "messageProcessor"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASScmdlineappENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   62,    2, 0x06,    1 /* Public */,
       3,    0,   63,    2, 0x06,    2 /* Public */,
       4,    0,   64,    2, 0x06,    3 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       5,    0,   65,    2, 0x08,    4 /* Private */,
       6,    0,   66,    2, 0x08,    5 /* Private */,
       7,    0,   67,    2, 0x08,    6 /* Private */,
       8,    0,   68,    2, 0x08,    7 /* Private */,
       9,    0,   69,    2, 0x08,    8 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject cmdlineapp::staticMetaObject = { {
    QMetaObject::SuperData::link<QDialog::staticMetaObject>(),
    qt_meta_stringdata_CLASScmdlineappENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASScmdlineappENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASScmdlineappENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<cmdlineapp, std::true_type>,
        // method 'DialogClosed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'AppCompleted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'Ready'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'readProcessOutput'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'readMessage'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'AppFinished'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'sendNO'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'messageProcessor'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void cmdlineapp::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<cmdlineapp *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->DialogClosed(); break;
        case 1: _t->AppCompleted(); break;
        case 2: _t->Ready(); break;
        case 3: _t->readProcessOutput(); break;
        case 4: _t->readMessage(); break;
        case 5: _t->AppFinished(); break;
        case 6: _t->sendNO(); break;
        case 7: _t->messageProcessor(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (cmdlineapp::*)();
            if (_t _q_method = &cmdlineapp::DialogClosed; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (cmdlineapp::*)();
            if (_t _q_method = &cmdlineapp::AppCompleted; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (cmdlineapp::*)();
            if (_t _q_method = &cmdlineapp::Ready; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
    }
    (void)_a;
}

const QMetaObject *cmdlineapp::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *cmdlineapp::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASScmdlineappENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int cmdlineapp::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void cmdlineapp::DialogClosed()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void cmdlineapp::AppCompleted()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void cmdlineapp::Ready()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}
QT_WARNING_POP
