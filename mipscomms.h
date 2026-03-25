// =============================================================================
// mipscomms.h
//
// Class declaration for MIPScomms.
// See mipscomms.cpp for full documentation.
//
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Revised:     March 2026 — documented for host app v2.22
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================
#ifndef MIPSCOMMS_H
#define MIPSCOMMS_H

#include <QDialog>
#include <QInputDialog>
#include "comms.h"

namespace Ui {
class MIPScomms;
}

// -----------------------------------------------------------------------------
// MIPScomms — raw terminal dialog. Lets the user pick a MIPS system by name
// and send/receive unformatted command strings. All connected Comms objects
// forward their DataReady() signals to the terminal display.
// -----------------------------------------------------------------------------
class MIPScomms : public QDialog
{
    Q_OBJECT

signals:
    void DialogClosed(void);

public:
    explicit MIPScomms(QWidget *parent, QList<Comms*> S);
    ~MIPScomms();
    virtual void reject();

    QList<Comms*> Systems;

private:
    Ui::MIPScomms *ui;

private slots:
    void CommandEntered(void);
    void readData2Console(void);
};

#endif // MIPSCOMMS_H
