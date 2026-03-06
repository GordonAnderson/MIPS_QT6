/****************************************************************************
** Meta object code from reading C++ file 'plot.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.7.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../plot.h"
#include <QtGui/qtextcursor.h>
#include <QtGui/qscreen.h>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'plot.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASSPlotENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSPlotENDCLASS = QtMocHelpers::stringData(
    "Plot",
    "DialogClosed",
    "",
    "Plot*",
    "plot",
    "mousePressed",
    "QMouseEvent*",
    "mousePressedHM",
    "slotSaveMenu",
    "slotExportMenu",
    "slotLoadMenu",
    "slotCommentMenu",
    "slotCloseComments",
    "slotXaxisZoomOption",
    "slotYaxisZoomOption",
    "slotFilterOption",
    "slotTrackOption",
    "slotClipBoard",
    "slotHeatMap",
    "mouseMove",
    "event",
    "slotSaveRangeHistory",
    "QCPRange",
    "newRange",
    "slotZoomOut",
    "slotZoomOutOneLevel",
    "slotRescaleYToDataInRange",
    "newXRange"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSPlotENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      19,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,  128,    2, 0x06,    1 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       5,    1,  131,    2, 0x0a,    3 /* Public */,
       7,    1,  134,    2, 0x0a,    5 /* Public */,
       8,    0,  137,    2, 0x0a,    7 /* Public */,
       9,    0,  138,    2, 0x0a,    8 /* Public */,
      10,    0,  139,    2, 0x0a,    9 /* Public */,
      11,    0,  140,    2, 0x0a,   10 /* Public */,
      12,    0,  141,    2, 0x0a,   11 /* Public */,
      13,    0,  142,    2, 0x0a,   12 /* Public */,
      14,    0,  143,    2, 0x0a,   13 /* Public */,
      15,    0,  144,    2, 0x0a,   14 /* Public */,
      16,    0,  145,    2, 0x0a,   15 /* Public */,
      17,    0,  146,    2, 0x0a,   16 /* Public */,
      18,    0,  147,    2, 0x0a,   17 /* Public */,
      19,    1,  148,    2, 0x0a,   18 /* Public */,
      21,    1,  151,    2, 0x0a,   20 /* Public */,
      24,    0,  154,    2, 0x0a,   22 /* Public */,
      25,    0,  155,    2, 0x0a,   23 /* Public */,
      26,    1,  156,    2, 0x0a,   24 /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 6,    2,
    QMetaType::Void, 0x80000000 | 6,    2,
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
    QMetaType::Void, 0x80000000 | 6,   20,
    QMetaType::Void, 0x80000000 | 22,   23,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 22,   27,

       0        // eod
};

Q_CONSTINIT const QMetaObject Plot::staticMetaObject = { {
    QMetaObject::SuperData::link<QDialog::staticMetaObject>(),
    qt_meta_stringdata_CLASSPlotENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSPlotENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSPlotENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<Plot, std::true_type>,
        // method 'DialogClosed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<Plot *, std::false_type>,
        // method 'mousePressed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QMouseEvent *, std::false_type>,
        // method 'mousePressedHM'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QMouseEvent *, std::false_type>,
        // method 'slotSaveMenu'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'slotExportMenu'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'slotLoadMenu'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'slotCommentMenu'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'slotCloseComments'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'slotXaxisZoomOption'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'slotYaxisZoomOption'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'slotFilterOption'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'slotTrackOption'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'slotClipBoard'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'slotHeatMap'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'mouseMove'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QMouseEvent *, std::false_type>,
        // method 'slotSaveRangeHistory'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QCPRange, std::false_type>,
        // method 'slotZoomOut'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'slotZoomOutOneLevel'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'slotRescaleYToDataInRange'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QCPRange, std::false_type>
    >,
    nullptr
} };

void Plot::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Plot *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->DialogClosed((*reinterpret_cast< std::add_pointer_t<Plot*>>(_a[1]))); break;
        case 1: _t->mousePressed((*reinterpret_cast< std::add_pointer_t<QMouseEvent*>>(_a[1]))); break;
        case 2: _t->mousePressedHM((*reinterpret_cast< std::add_pointer_t<QMouseEvent*>>(_a[1]))); break;
        case 3: _t->slotSaveMenu(); break;
        case 4: _t->slotExportMenu(); break;
        case 5: _t->slotLoadMenu(); break;
        case 6: _t->slotCommentMenu(); break;
        case 7: _t->slotCloseComments(); break;
        case 8: _t->slotXaxisZoomOption(); break;
        case 9: _t->slotYaxisZoomOption(); break;
        case 10: _t->slotFilterOption(); break;
        case 11: _t->slotTrackOption(); break;
        case 12: _t->slotClipBoard(); break;
        case 13: _t->slotHeatMap(); break;
        case 14: _t->mouseMove((*reinterpret_cast< std::add_pointer_t<QMouseEvent*>>(_a[1]))); break;
        case 15: _t->slotSaveRangeHistory((*reinterpret_cast< std::add_pointer_t<QCPRange>>(_a[1]))); break;
        case 16: _t->slotZoomOut(); break;
        case 17: _t->slotZoomOutOneLevel(); break;
        case 18: _t->slotRescaleYToDataInRange((*reinterpret_cast< std::add_pointer_t<QCPRange>>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 0:
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
            using _t = void (Plot::*)(Plot * );
            if (_t _q_method = &Plot::DialogClosed; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject *Plot::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Plot::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSPlotENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int Plot::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 19)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 19;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 19)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 19;
    }
    return _id;
}

// SIGNAL 0
void Plot::DialogClosed(Plot * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSPlotDataENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSPlotDataENDCLASS = QtMocHelpers::stringData(
    "PlotData"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSPlotDataENDCLASS[] = {

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

Q_CONSTINIT const QMetaObject PlotData::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_CLASSPlotDataENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSPlotDataENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSPlotDataENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<PlotData, std::true_type>
    >,
    nullptr
} };

void PlotData::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject *PlotData::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PlotData::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSPlotDataENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int PlotData::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    return _id;
}
QT_WARNING_POP
