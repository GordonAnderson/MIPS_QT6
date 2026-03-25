// =============================================================================
// cdirselectiondlg.h
//
// Class declaration for CDirSelectionDlg.
// See cdirselectiondlg.cpp for full documentation.
//
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Revised:     March 2026 — documented for host app v2.22
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================
#pragma once

#include <QDialog>
#include <QDir>

class QTreeView;
class QFileSystemModel;
class QLineEdit;
class QPushButton;

// -----------------------------------------------------------------------------
// CDirSelectionDlg — directory-only browse dialog. Presents a QTreeView
// filtered to show folders only. The user may also type a new folder name
// into the edit field; selectedPath() returns the combined result.
// -----------------------------------------------------------------------------
class CDirSelectionDlg : public QDialog
{
    Q_OBJECT

public:
    CDirSelectionDlg(const QString initialPath, QWidget *parent = NULL);
    void    setTitle(QString title);
    QString selectedPath(void);
    QDir    directory() const;

private slots:
    void accept(void);
    void reject(void);
    void onCurrentChanged();

private:
    QTreeView        *m_treeView;
    QFileSystemModel *m_model;
    QLineEdit        *m_folderName;
    QPushButton      *m_OKbutton;
    QPushButton      *m_Cancelbutton;
    QString           m_initialPath;
};
