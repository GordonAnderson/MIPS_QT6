// =============================================================================
// aboutdialog.h
//
// Class declaration for AboutDialog.
// See aboutdialog.cpp for full documentation.
//
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Created:     March 2026
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================
#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>

namespace Ui { class AboutDialog; }

// -----------------------------------------------------------------------------
// AboutDialog — fixed-size About box displaying application version, build
// info, copyright, author contact, and clickable hyperlinks to the GitHub
// repository and GAA Google Drive documentation folder.
//
// APP_VERSION must be defined as a compile-time string macro. Add to MIPS.pro:
//   DEFINES += APP_VERSION=\\\"2.22\\\"
// Then bumping the version in the .pro is the only change needed at release.
// -----------------------------------------------------------------------------
class AboutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AboutDialog(QWidget *parent = 0);
    ~AboutDialog();

private slots:
    void on_pbClose_clicked(void);

private:
    Ui::AboutDialog *ui;
};

#endif // ABOUTDIALOG_H
