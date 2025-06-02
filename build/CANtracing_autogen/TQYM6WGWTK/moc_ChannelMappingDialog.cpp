/****************************************************************************
** Meta object code from reading C++ file 'ChannelMappingDialog.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.9.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/ChMap/ChannelMappingDialog.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ChannelMappingDialog.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN20ChannelMappingDialogE_t {};
} // unnamed namespace

template <> constexpr inline auto ChannelMappingDialog::qt_create_metaobjectdata<qt_meta_tag_ZN20ChannelMappingDialogE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "ChannelMappingDialog",
        "configurationChanged",
        "",
        "refreshChannelList",
        "applyModeFromConfig",
        "onItemChanged",
        "QTreeWidgetItem*",
        "item",
        "onFileChanged",
        "path"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'configurationChanged'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'refreshChannelList'
        QtMocHelpers::SlotData<void()>(3, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'applyModeFromConfig'
        QtMocHelpers::SlotData<void()>(4, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onItemChanged'
        QtMocHelpers::SlotData<void(QTreeWidgetItem *)>(5, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 6, 7 },
        }}),
        // Slot 'onFileChanged'
        QtMocHelpers::SlotData<void(const QString &)>(8, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 9 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<ChannelMappingDialog, qt_meta_tag_ZN20ChannelMappingDialogE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject ChannelMappingDialog::staticMetaObject = { {
    QMetaObject::SuperData::link<QDialog::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN20ChannelMappingDialogE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN20ChannelMappingDialogE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN20ChannelMappingDialogE_t>.metaTypes,
    nullptr
} };

void ChannelMappingDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<ChannelMappingDialog *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->configurationChanged(); break;
        case 1: _t->refreshChannelList(); break;
        case 2: _t->applyModeFromConfig(); break;
        case 3: _t->onItemChanged((*reinterpret_cast< std::add_pointer_t<QTreeWidgetItem*>>(_a[1]))); break;
        case 4: _t->onFileChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (ChannelMappingDialog::*)()>(_a, &ChannelMappingDialog::configurationChanged, 0))
            return;
    }
}

const QMetaObject *ChannelMappingDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ChannelMappingDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN20ChannelMappingDialogE_t>.strings))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int ChannelMappingDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void ChannelMappingDialog::configurationChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
QT_WARNING_POP
