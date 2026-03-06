/****************************************************************************
** Meta object code from reading C++ file 'comms.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.7.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../comms.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'comms.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASSCommsENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSCommsENDCLASS = QtMocHelpers::stringData(
    "Comms",
    "lineAvalible",
    "",
    "DataReady",
    "ADCrecordingDone",
    "ADCvectorReady",
    "readData2RingBuffer",
    "handleError",
    "QSerialPort::SerialPortError",
    "error",
    "readData2ADCBuffer",
    "connected",
    "disconnected",
    "slotAboutToClose",
    "slotKeepAlive",
    "slotReconnect",
    "pollLoop"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSCommsENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   92,    2, 0x06,    1 /* Public */,
       3,    0,   93,    2, 0x06,    2 /* Public */,
       4,    0,   94,    2, 0x06,    3 /* Public */,
       5,    0,   95,    2, 0x06,    4 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       6,    0,   96,    2, 0x0a,    5 /* Public */,
       7,    1,   97,    2, 0x08,    6 /* Private */,
      10,    0,  100,    2, 0x08,    8 /* Private */,
      11,    0,  101,    2, 0x08,    9 /* Private */,
      12,    0,  102,    2, 0x08,   10 /* Private */,
      13,    0,  103,    2, 0x08,   11 /* Private */,
      14,    0,  104,    2, 0x08,   12 /* Private */,
      15,    0,  105,    2, 0x08,   13 /* Private */,
      16,    0,  106,    2, 0x08,   14 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 8,    9,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject Comms::staticMetaObject = { {
    QMetaObject::SuperData::link<QDialog::staticMetaObject>(),
    qt_meta_stringdata_CLASSCommsENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSCommsENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSCommsENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<Comms, std::true_type>,
        // method 'lineAvalible'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'DataReady'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'ADCrecordingDone'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'ADCvectorReady'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'readData2RingBuffer'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'handleError'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QSerialPort::SerialPortError, std::false_type>,
        // method 'readData2ADCBuffer'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'connected'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'disconnected'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'slotAboutToClose'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'slotKeepAlive'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'slotReconnect'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'pollLoop'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void Comms::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Comms *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->lineAvalible(); break;
        case 1: _t->DataReady(); break;
        case 2: _t->ADCrecordingDone(); break;
        case 3: _t->ADCvectorReady(); break;
        case 4: _t->readData2RingBuffer(); break;
        case 5: _t->handleError((*reinterpret_cast< std::add_pointer_t<QSerialPort::SerialPortError>>(_a[1]))); break;
        case 6: _t->readData2ADCBuffer(); break;
        case 7: _t->connected(); break;
        case 8: _t->disconnected(); break;
        case 9: _t->slotAboutToClose(); break;
        case 10: _t->slotKeepAlive(); break;
        case 11: _t->slotReconnect(); break;
        case 12: _t->pollLoop(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (Comms::*)();
            if (_t _q_method = &Comms::lineAvalible; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (Comms::*)();
            if (_t _q_method = &Comms::DataReady; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (Comms::*)();
            if (_t _q_method = &Comms::ADCrecordingDone; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (Comms::*)();
            if (_t _q_method = &Comms::ADCvectorReady; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
    }
}

const QMetaObject *Comms::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Comms::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSCommsENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int Comms::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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

// SIGNAL 0
void Comms::lineAvalible()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void Comms::DataReady()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void Comms::ADCrecordingDone()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void Comms::ADCvectorReady()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}
QT_WARNING_POP
