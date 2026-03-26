// =============================================================================
// aboutdialog.cpp
//
// Implements the AboutDialog — a fixed-size dialog showing application
// version, Qt build info, copyright, author contact, and two clickable
// hyperlinks (GitHub source and GAA Google Drive documentation).
//
// Depends on:  ui_aboutdialog.h  (generated from aboutdialog.ui)
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Created:     March 2026
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================
#include "aboutdialog.h"
#include <ui_aboutdialog.h>

// AboutDialog — constructor. Populates all dynamic labels from compile-time
// macros (APP_VERSION, QT_VERSION_STR, __DATE__, __TIME__) and sets the
// two hyperlink labels using Qt rich text so they open in the system browser.
AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);
    setFixedSize(440, 360);

    // ── Logo ─────────────────────────────────────────────────────────────
    // GAACElogo.ico is registered in files.qrc under the "/" prefix.
    QPixmap logo(":/GAACElogo.ico");
    if (!logo.isNull())
        ui->lblLogo->setPixmap(logo.scaled(64, 64,
                                           Qt::KeepAspectRatio,
                                           Qt::SmoothTransformation));

    // ── Version ──────────────────────────────────────────────────────────
    // APP_VERSION is defined in MIPS.pro:
    //   DEFINES += APP_VERSION=\\\"2.22\\\"
#ifdef APP_VERSION
    ui->lblVersion->setText(QString("Version %1").arg(APP_VERSION));
#else
    ui->lblVersion->setText("Version (unknown — define APP_VERSION in MIPS.pro)");
#endif

    // ── Build info ────────────────────────────────────────────────────────
    ui->lblBuildInfo->setText(
        QString("Built with Qt %1   |   %2  %3")
            .arg(QT_VERSION_STR)
            .arg(__DATE__)
            .arg(__TIME__)
    );

    // ── Hyperlinks ────────────────────────────────────────────────────────
    // QLabel renders rich text and opens URLs in the system browser when
    // setOpenExternalLinks(true) is set. Both are also set in the .ui file
    // but we set them here too for clarity.
    ui->lblGitHub->setText(
        "<a href=\"https://github.com/GordonAnderson/MIPS_QT6\">"
        "https://github.com/GordonAnderson/MIPS_QT6</a>"
    );
    ui->lblGitHub->setOpenExternalLinks(true);

    ui->lblDocs->setText(
        "<a href=\"https://drive.google.com/drive/folders/"
        "0B3IchPRNYqYIcjZhdGFVMVR1VzQ\">"
        "GAA Google Drive — documentation &amp; install files</a>"
    );
    ui->lblDocs->setOpenExternalLinks(true);
}

// ~AboutDialog — destructor. Releases the UI form object.
AboutDialog::~AboutDialog()
{
    delete ui;
}

// on_pbClose_clicked — accepts the dialog (same as pressing Enter, since
// pbClose is the default button).
void AboutDialog::on_pbClose_clicked(void)
{
    accept();
}
