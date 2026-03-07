// =============================================================================
// mipscomms.cpp
//
// Implements the MIPScomms dialog — a simple terminal window that lets the
// user select a MIPS system by name and send raw commands to it. All incoming
// data from every connected Comms object is forwarded to the text display.
//
// Depends on:  ui_mipscomms.h, comms.h
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Revised:     March 2026 — documented for host app v2.22
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================
#include "mipscomms.h"
#include "ui_mipscomms.h"

MIPScomms::MIPScomms(QWidget *parent, QList<Comms*> S) :
    QDialog(parent),
    ui(new Ui::MIPScomms)
{
    ui->setupUi(this);
    Systems = S;

    // Populate the system selector with MIPS names
    ui->comboTermMIPS->clear();
    for(int i = 0; i < Systems.count(); i++)
        ui->comboTermMIPS->addItem(Systems[i]->MIPSname);

    if(ui->comboTermCMD->lineEdit())
        connect(ui->comboTermCMD->lineEdit(), SIGNAL(returnPressed()), this, SLOT(CommandEntered()));

    for(int i = 0; i < Systems.count(); i++)
        connect(Systems.at(i), SIGNAL(DataReady()), this, SLOT(readData2Console()));
}

MIPScomms::~MIPScomms()
{
    for(int i = 0; i < Systems.count(); i++)
        disconnect(Systems.at(i), SIGNAL(DataReady()), 0, 0);
    delete ui;
}

void MIPScomms::reject()
{
    emit DialogClosed();
    delete this;
}

// CommandEntered — sends the typed command to the selected MIPS system,
// then clears the input combo box.
void MIPScomms::CommandEntered(void)
{
    QString res = ui->comboTermCMD->currentText() + "\n";
    for(int i = 0; i < Systems.count(); i++)
        Systems.at(i)->SendString(ui->comboTermMIPS->currentText(), res);
    ui->comboTermCMD->setCurrentText("");
}

// readData2Console — reads any available data from all connected Comms objects
// and appends it to the terminal display.
void MIPScomms::readData2Console(void)
{
    for(int i = 0; i < Systems.count(); i++)
    {
        QByteArray data = Systems.at(i)->readall();
        ui->txtTerm->insertPlainText(QString(data));
        ui->txtTerm->ensureCursorVisible();
    }
}
