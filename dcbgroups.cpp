// =============================================================================
// dcbgroups.cpp
//
// DCBiasGroups — DC bias channel group definition widget for the MIPS custom
// control panel. Extracted from controlpanel.cpp/.h during Phase 3 refactoring.
//
// Depends on:  dcbgroups.h, Utilities.h
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Revised:     March 2026 — Phase 3 extraction
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================

#include "dcbgroups.h"
#include "Utilities.h"

// -----------------------------------------------------------------------------
// Constructor — stores parent widget and position parameters.
// -----------------------------------------------------------------------------
DCBiasGroups::DCBiasGroups(QWidget *parent, int x, int y)
    : QWidget(parent)
{
    p = parent;
    X = x;
    Y = y;
}

// Show — creates the group box, combo box, and enable checkbox; installs
// event filters so the widget can be repositioned by dragging.
void DCBiasGroups::Show(void)
{
    gbDCBgroups = new QGroupBox("Define DC bias channel groups", p);
    gbDCBgroups->setGeometry(X, Y, 251, 86);
    gbDCBgroups->setToolTip("DC bias groups are sets of DC bias channels that will track, so when you change one channel all other channels in the group will change by the same about.");

    comboGroups = new QComboBox(gbDCBgroups);
    comboGroups->setGeometry(5, 25, 236, 26);
    comboGroups->setEditable(true);
    comboGroups->setToolTip("Define a group by entering a list of channel names separated by a comma. You can define many groups.");
    comboGroups->installEventFilter(this);

    DCBenaGroups = new QCheckBox(gbDCBgroups);
    DCBenaGroups->setGeometry(5, 55, 116, 20);
    DCBenaGroups->setText("Enable groups");
    DCBenaGroups->setToolTip("Check enable to process the groups and apply.");
    connect(DCBenaGroups, &QCheckBox::clicked, this, &DCBiasGroups::slotEnableChange);

    gbDCBgroups->installEventFilter(this);
    gbDCBgroups->setMouseTracking(true);
}

// slotEnableChange — emits disable() or enable() to match the new checkbox state.
void DCBiasGroups::slotEnableChange(void)
{
    if (!DCBenaGroups->isChecked()) emit disable();
    if (DCBenaGroups->isChecked())  emit enable();
}

// SetValues — restores the combo box entries from a semicolon-separated string.
// One string is passed with all combo box entries; a semicolon separates entries.
bool DCBiasGroups::SetValues(QString strVals)
{
    DCBenaGroups->setChecked(false);
    comboGroups->clear();
    QStringList resList = strVals.split(";");
    for (int i = 0; i < resList.count(); i++)
    {
        comboGroups->addItem(resList[i]);
    }
    return true;
}

// Report — returns all combo box entries joined by semicolons, for save/restore.
QString DCBiasGroups::Report(void)
{
    QString res = "";

    for (int i = 0; i < comboGroups->count(); i++)
    {
        if (res != "") res += ";";
        res += comboGroups->itemText(i);
    }
    return res;
}

// eventFilter — handles widget drag (moveWidget) and Shift+Backspace to remove
// the current combo box item.
bool DCBiasGroups::eventFilter(QObject *obj, QEvent *event)
{
    if (moveWidget(obj, gbDCBgroups, gbDCBgroups, event)) return true;
    if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if ((keyEvent->key() == Qt::Key::Key_Backspace) && (keyEvent->modifiers() == Qt::ShiftModifier))
        {
            auto combobox = dynamic_cast<QComboBox *>(obj);
            if (combobox)
            {
                combobox->removeItem(combobox->currentIndex());
                return true;
            }
        }
    }
    return QObject::eventFilter(obj, event);
}
