// =============================================================================
// cdirselectiondlg.cpp
//
// Implements CDirSelectionDlg — a directory-only browse dialog built with
// QFileSystemModel and QTreeView. Shows only folders (no files), hides all
// columns except the name column, and lets the user optionally type a new
// folder name into the QLineEdit before accepting.
//
// The OK button is enabled only when a directory is selected. selectedPath()
// returns the canonical parent path combined with the text in the QLineEdit,
// so the user can rename the target folder at selection time.
//
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Revised:     March 2026 — documented for host app v2.22
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================
#include "cdirselectiondlg.h"

#include <QLabel>
#include <QBoxLayout>
#include <QDialogButtonBox>
#include <QTreeView>
#include <QFileSystemModel>
#include <QPushButton>
#include <QLineEdit>

CDirSelectionDlg::CDirSelectionDlg(const QString initialPath, QWidget *parent)
    : QDialog(parent), m_initialPath(initialPath)
{
    setMinimumSize(200, 300);
    resize(600, 430);

    // File system model — directories only
    m_model = new QFileSystemModel(this);
    m_model->setFilter(QDir::AllDirs | QDir::NoDotAndDotDot);
    auto rootIdx = m_model->setRootPath(m_initialPath);

    // Tree view — single selection, name column only, sorted ascending
    m_treeView = new QTreeView(this);
    m_treeView->setModel(m_model);
    m_treeView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_treeView->setHeaderHidden(true);
    m_treeView->setSortingEnabled(true);
    m_treeView->sortByColumn(0, Qt::AscendingOrder);
    for(int i = 1; i < m_model->columnCount(); i++)
        m_treeView->setColumnHidden(i, true);
    m_treeView->scrollTo(rootIdx);
    m_treeView->selectionModel()->setCurrentIndex(rootIdx,
        QItemSelectionModel::Current | QItemSelectionModel::Select);
    connect(m_treeView->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &CDirSelectionDlg::onCurrentChanged);

    // OK / Cancel buttons
    auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &CDirSelectionDlg::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &CDirSelectionDlg::reject);
    m_OKbutton     = buttonBox->button(QDialogButtonBox::Ok);
    m_Cancelbutton = buttonBox->button(QDialogButtonBox::Cancel);

    // Folder name display / edit field
    auto label    = new QLabel(tr("Folder:"));
    m_folderName  = new QLineEdit(this);
    m_folderName->setReadOnly(false);
    m_folderName->setText(QFileInfo(m_initialPath).fileName());

    auto pathLayout = new QHBoxLayout();
    pathLayout->addWidget(label);
    pathLayout->addSpacing(10);
    pathLayout->addWidget(m_folderName);

    auto mainLayout = new QVBoxLayout();
    mainLayout->addWidget(m_treeView);
    mainLayout->addSpacing(10);
    mainLayout->addLayout(pathLayout);
    mainLayout->addSpacing(10);
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);
}

void CDirSelectionDlg::accept(void)
{
    this->setResult(1);
    this->hide();
}

void CDirSelectionDlg::reject(void)
{
    this->setResult(0);
    this->hide();
}

void CDirSelectionDlg::setTitle(QString title)
{
    this->setWindowTitle(title);
}

// onCurrentChanged — updates the folder name field and enables OK only when
// the selected item is a directory.
void CDirSelectionDlg::onCurrentChanged()
{
    auto fileInfo = m_model->fileInfo(m_treeView->selectionModel()->currentIndex());
    m_folderName->setText(fileInfo.fileName());
    m_OKbutton->setEnabled(fileInfo.isDir());
    m_OKbutton->setDefault(fileInfo.isDir());
}

// selectedPath — returns the canonical parent path joined with the folder name
// text, allowing the user to specify a new folder name at selection time.
QString CDirSelectionDlg::selectedPath(void)
{
    auto fileInfo = m_model->fileInfo(m_treeView->selectionModel()->currentIndex());
    return fileInfo.canonicalPath() + "/" + m_folderName->text();
}

QDir CDirSelectionDlg::directory() const
{
    return QDir(m_model->fileInfo(m_treeView->selectionModel()->currentIndex()).absoluteFilePath());
}
