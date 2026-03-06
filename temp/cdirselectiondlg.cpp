/*
   This C++ code defines a custom Qt dialog named CDirSelectionDlg which is designed specifically
   for selecting a directory (folder), similar to a simplified "Browse Folder" dialog.

   The code uses the Model-View-Controller architecture provided by Qt, specifically QFileSystemModel
   and QTreeView, to efficiently display the file system hierarchy.
*/
#include "cdirselectiondlg.h"       // Declaration for the custom dialog class.
//#include "scriptingconsole.h"       // Included, but its use is not visible in this snippet.

#include <QLabel>                   // For displaying static text (e.g., "Folder:").
#include <QBoxLayout>               // For layout management (QHBoxLayout, QVBoxLayout).
#include <QDialogButtonBox>         // For standard buttons (OK, Cancel).
#include <QTreeView>                // The visual component to display the directory structure.
#include <QFileSystemModel>         // The data model that interfaces with the actual file system.
#include <QPushButton>              // Base class for buttons (used to reference OK/Cancel).
#include <QLineEdit>                // For displaying/editing the selected folder name.
#include <qdebug.h>                 // For debugging output (not used in this snippet, but included).

// Constructor: Initializes the dialog.
CDirSelectionDlg::CDirSelectionDlg(const QString initialPath, QWidget *parent)
    : QDialog(parent), m_initialPath(initialPath) // Initialize base class and store the starting path.
{
    // --- 1. Dialog Setup ---
    setMinimumSize(200, 300); // Sets the minimum window size.
    resize(600, 430);         // Sets the default window size.

    // --- 2. File System Model (Data Layer) ---
    m_model = new QFileSystemModel(this);
    // Set the filter to show only directories (important for a folder selection dialog).
    m_model->setFilter(QDir::AllDirs | QDir::NoDotAndDotDot);

    // Set the root path that the model will display.
    auto rootIdx = m_model->setRootPath(m_initialPath);

    // --- 3. Tree View (Presentation Layer) ---
    m_treeView = new QTreeView(this);
    m_treeView->setModel(m_model);                          // Attach the file system data model.
    m_treeView->setSelectionMode(QAbstractItemView::SingleSelection); // Only one item can be selected at a time.
    m_treeView->setHeaderHidden(true);                      // Hide the column headers (e.g., Name, Size, Type).
    m_treeView->setSortingEnabled(true);
    m_treeView->sortByColumn(0, Qt::AscendingOrder);        // Sorts items alphabetically by name.

    // Hide all columns except the name column (column 0).
    for(int i = 1; i < m_model->columnCount(); i ++)
        m_treeView->setColumnHidden(i, true);

    // Scroll the view to ensure the root directory is visible.
    m_treeView->scrollTo(rootIdx);

    // Select and focus on the initial path's index.
    m_treeView->selectionModel()->setCurrentIndex(rootIdx, QItemSelectionModel::Current | QItemSelectionModel::Select);

    // Connect the selection change signal to the custom slot for updating the dialog state.
    connect(m_treeView->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &CDirSelectionDlg::onCurrentChanged);

    // --- 4. Button Box Setup ---
    // Create standard OK and Cancel buttons.
    auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    // Connect standard button signals to the dialog's standard slots.
    connect(buttonBox, &QDialogButtonBox::accepted, this, &CDirSelectionDlg::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &CDirSelectionDlg::reject);

    // Store pointers to the actual QPushButton widgets for later state control (enabling/disabling OK).
    m_OKbutton = buttonBox->button(QDialogButtonBox::Ok);
    m_Cancelbutton = buttonBox->button(QDialogButtonBox::Cancel);

    // --- 5. Folder Name Display Setup (QLineEdit) ---
    auto label = new QLabel(tr("Folder:"));
    m_folderName = new QLineEdit(this);
    m_folderName->setReadOnly(false); // Allows the user to type in a custom folder name.

    // Set the QLineEdit to display the filename component of the initial path.
    m_folderName->setText(QFileInfo(m_initialPath).fileName());

    // Layout for the folder name display (horizontal: Label | Text Field)
    auto pathLayout = new QHBoxLayout();
    pathLayout->addWidget(label);
    pathLayout->addSpacing(10);
    pathLayout->addWidget(m_folderName);

    // --- 6. Main Dialog Layout (Vertical) ---
    auto mainLayout = new QVBoxLayout();
    mainLayout->addWidget(m_treeView);     // 1. Directory Tree
    mainLayout->addSpacing(10);
    mainLayout->addLayout(pathLayout);     // 2. Folder Name Input
    mainLayout->addSpacing(10);
    mainLayout->addWidget(buttonBox);      // 3. OK/Cancel Buttons
    setLayout(mainLayout);                 // Apply the main layout to the dialog.
}

// Slot called when the OK button is pressed (or Enter key).
void CDirSelectionDlg::accept(void)
{
    this->setResult(1); // Set the dialog result code to indicate acceptance (success).
    this->hide();       // Close the dialog.
}

// Slot called when the Cancel button is pressed (or Esc key).
void CDirSelectionDlg::reject(void)
{
    this->setResult(0); // Set the dialog result code to indicate rejection (cancellation).
    this->hide();       // Close the dialog.
}

// Public method to set the window title of the dialog.
void CDirSelectionDlg::setTitle(QString title)
{
    this->setWindowTitle(title);
}

// Custom slot executed when the selection in the QTreeView changes.
void CDirSelectionDlg::onCurrentChanged()
{
    // Get the QFileInfo object for the currently selected item.
    auto fileInfo = m_model->fileInfo(m_treeView->selectionModel()->currentIndex());

    // Update the QLineEdit with the file/folder name of the selected item.
    m_folderName->setText(fileInfo.fileName());

    // Check if the selected item is a directory (folder).
    // The OK button is only enabled if a directory is selected.
    m_OKbutton->setEnabled(fileInfo.isDir());

    // Also set the OK button as the default button only if a directory is selected.
    m_OKbutton->setDefault(fileInfo.isDir());
}

// Public method to retrieve the user's selected path after the dialog is accepted.
QString CDirSelectionDlg::selectedPath(void)
{
    // 1. Get the path to the selected directory (the canonical path up to the selected item).
    auto fileInfo = m_model->fileInfo(m_treeView->selectionModel()->currentIndex());

    // 2. Combine the canonical path with the text entered in the QLineEdit
    // (allowing the user to rename the folder on selection).
    return fileInfo.canonicalPath() + "/" + m_folderName->text();
}

// Public method to retrieve the selected directory as a QDir object.
QDir CDirSelectionDlg::directory() const
{
    // Returns the QDir object corresponding to the currently selected file path in the tree view.
    return QDir(m_model->fileInfo(m_treeView->selectionModel()->currentIndex()).absoluteFilePath());
}
