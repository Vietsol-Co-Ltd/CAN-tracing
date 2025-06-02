/****************************************************************************
** Meta object code from reading C++ file 'SimulationWindow.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.9.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../include/SimulationWindow.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'SimulationWindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.9.0. It"
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
struct qt_meta_tag_ZN16SimulationWindowE_t {};
} // unnamed namespace

template <> constexpr inline auto SimulationWindow::qt_create_metaobjectdata<qt_meta_tag_ZN16SimulationWindowE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "SimulationWindow",
        "onSystemTreeContextMenu",
        "",
        "pos",
        "showRightPanelContextMenu",
        "handleTreeItemDoubleClick",
        "QTreeWidgetItem*",
        "item",
        "column",
        "saveConfig"
    };

    QtMocHelpers::UintData qt_methods {
        // Slot 'onSystemTreeContextMenu'
        QtMocHelpers::SlotData<void(const QPoint &)>(1, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QPoint, 3 },
        }}),
        // Slot 'showRightPanelContextMenu'
        QtMocHelpers::SlotData<void(const QPoint &)>(4, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QPoint, 3 },
        }}),
        // Slot 'handleTreeItemDoubleClick'
        QtMocHelpers::SlotData<void(QTreeWidgetItem *, int)>(5, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 6, 7 }, { QMetaType::Int, 8 },
        }}),
        // Slot 'saveConfig'
        QtMocHelpers::SlotData<void()>(9, 2, QMC::AccessPrivate, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<SimulationWindow, qt_meta_tag_ZN16SimulationWindowE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject SimulationWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN16SimulationWindowE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN16SimulationWindowE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN16SimulationWindowE_t>.metaTypes,
    nullptr
} };

void SimulationWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<SimulationWindow *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->onSystemTreeContextMenu((*reinterpret_cast< std::add_pointer_t<QPoint>>(_a[1]))); break;
        case 1: _t->showRightPanelContextMenu((*reinterpret_cast< std::add_pointer_t<QPoint>>(_a[1]))); break;
        case 2: _t->handleTreeItemDoubleClick((*reinterpret_cast< std::add_pointer_t<QTreeWidgetItem*>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 3: _t->saveConfig(); break;
        default: ;
        }
    }
}

const QMetaObject *SimulationWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SimulationWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN16SimulationWindowE_t>.strings))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int SimulationWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 4;
    }
    return _id;
}
QT_WARNING_POP
