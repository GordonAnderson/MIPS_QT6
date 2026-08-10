// =============================================================================
// plot.cpp
//
// Generic plot dialog used by the MIPS host scripting system. Hosts one or
// two line graphs plus optional heatmap views of multi-scan data.
//
// Key capabilities:
//   - PlotCommand() — string-driven API; all commands are also appended to the
//     comment block so a saved .plot file can fully recreate the session
//   - Multi-scan accumulation via NewGraph / AddPoint / AddPoints commands
//   - Savitzky-Golay smoothing (5, 7, 9, or 11-point)
//   - Heatmap view (QCPColorMap) for two-channel ion mobility data
//   - Zoom history stack with one-level and full undo
//   - Binary (.plot) and CSV save/load, clipboard image capture
//   - PlotData — draggable embedded widget wrapper around Plot
//
// PlotCommand string API is documented inline above the function body.
//
// Depends on:  ui_plot.h, qcustomplot.h
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Created:     2021
// Revised:     March 2026 — documented for host app v2.22
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================
#include "plot.h"
#include "ui_plot.h"
#include "properties.h"
#include <QScreen>

#include <QFile>
#include <QDataStream>
#include <QTextStream>
#include <QList>

// --- Binary Save/Load Functions for a single PlotGraph ---

/**
 * @brief Saves a single PlotGraph structure to a binary file stream.
 * @param out The QDataStream to write to.
 * @param graph The PlotGraph structure to save.
 */
bool saveSinglePlotGraph(QDataStream& out, const PlotGraph& graph) {
    // Write the main structure's data
    out << graph.NumScans;
    out << (qint32)graph.Vec.size(); // Save the number of DataPoint objects

    // Iterate through the list of DataPoint pointers using an index-based loop
    for (int i = 0; i < graph.Vec.size(); ++i) {
        const auto& dataPoint = graph.Vec.at(i);
        if (!dataPoint) {
            // Handle null pointer, e.g., by writing a special value
            continue;
        }

        // Write the DataPoint's data
        out << dataPoint->point;
        out << dataPoint->X;
        out << (qint32)dataPoint->Y.size(); // Save the number of Y values

        // Iterate through the list of float pointers and write the values
        for (int j = 0; j < dataPoint->Y.size(); ++j) {
            const auto& yValuePtr = dataPoint->Y.at(j);
            if (yValuePtr) {
                out << *yValuePtr; // Dereference the pointer to save the actual float value
            } else {
                out << 0.0f; // Handle null float pointer
            }
        }
    }
    return true;
}

/**
 * @brief Loads a single PlotGraph structure from a binary file stream.
 * @param in The QDataStream to read from.
 * @param graph The PlotGraph structure to load data into.
 */
bool loadSinglePlotGraph(QDataStream& in, PlotGraph& graph) {
    // Read the main structure's data
    in >> graph.NumScans;

    qint32 vecSize;
    in >> vecSize; // Read the number of DataPoint objects

    // Loop to read and reconstruct the DataPoint objects
    for (qint32 i = 0; i < vecSize; ++i) {
        DataPoint* dataPoint = new DataPoint;
        in >> dataPoint->point;
        in >> dataPoint->X;

        qint32 ySize;
        in >> ySize; // Read the number of Y values

        // Loop to read and reconstruct the float values
        for (qint32 j = 0; j < ySize; ++j) {
            float* yValuePtr = new float;
            in >> *yValuePtr; // Read the value into the newly allocated memory
            dataPoint->Y.append(yValuePtr);
        }

        graph.Vec.append(dataPoint);
    }
    return true;
}

// --- Binary Save/Load Functions for a QList of PlotGraphs ---

/**
 * @brief Saves a list of PlotGraph structures to a binary file.
 * @param filePath The path to the file.
 * @param graphs The list of PlotGraph structures to save.
 * @return True if the save was successful, false otherwise.
 */
bool savePlotGraphs(const QString& filePath, const QList<PlotGraph *>& graphs) {
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }

    QDataStream out(&file);
    out.setVersion(QDataStream::Qt_5_15);

    // First, save the number of graphs in the list
    out << (qint32)graphs.size();

    // Then, save each individual graph
    for (const auto& graph : graphs) {
        if (!graph) continue;
        saveSinglePlotGraph(out, *graph);
    }

    file.close();
    return true;
}

/**
 * @brief Loads a list of PlotGraph structures from a binary file.
 * @param filePath The path to the file.
 * @param graphs The list to load the data into.
 * @return True if the load was successful, false otherwise.
 */
bool loadPlotGraphs(const QString& filePath, QList<PlotGraph *>& graphs) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    QDataStream in(&file);
    in.setVersion(QDataStream::Qt_5_15);

    // Clear any existing data
    clearPlotGraphs(graphs);

    // Read the number of graphs to load
    qint32 numGraphs;
    in >> numGraphs;

    for (qint32 i = 0; i < numGraphs; ++i) {
        PlotGraph* newGraph = new PlotGraph;
        loadSinglePlotGraph(in, *newGraph);
        graphs.append(newGraph);
    }

    file.close();
    return true;
}

// --- CSV Save/Load Functions for a QList of PlotGraphs ---

/**
 * @brief Saves a list of PlotGraph structures to a CSV text file.
 * The format is:
 * [Num Graphs]
 * NumScans_Graph1
 * [Num DataPoints_Graph1]
 * point,X,[Num Y Values],Y1,Y2,Y3,...
 * NumScans_Graph2
 * [Num DataPoints_Graph2]
 * ...
 * @param filePath The path to the file.
 * @param graphs The list of PlotGraph structures to save.
 * @return True if the save was successful, false otherwise.
 */
bool savePlotGraphsCsv(const QString& filePath, const QList<PlotGraph *>& graphs) {
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }

    QTextStream out(&file);

    // Write the number of graphs
    out << graphs.size() << "\n";

    for (const auto& graph : graphs) {
        if (!graph) continue;

        out << graph->NumScans << "\n";
        out << graph->Vec.size() << "\n";

        for (int i = 0; i < graph->Vec.size(); ++i) {
            const auto& dataPoint = graph->Vec.at(i);
            if (!dataPoint) {
                continue;
            }

            out << dataPoint->point << "," << dataPoint->X << "," << dataPoint->Y.size();

            for (int j = 0; j < dataPoint->Y.size(); ++j) {
                const auto& yValuePtr = dataPoint->Y.at(j);
                if (yValuePtr) {
                    out << "," << *yValuePtr;
                } else {
                    out << "," << 0.0f;
                }
            }
            out << "\n";
        }
    }

    file.close();
    return true;
}

/**
 * @brief Loads a list of PlotGraph structures from a CSV text file.
 * @param filePath The path to the file.
 * @param graphs The list to load the data into.
 * @return True if the load was successful, false otherwise.
 */
bool loadPlotGraphsCsv(const QString& filePath, QList<PlotGraph *>& graphs) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

    QTextStream in(&file);

    clearPlotGraphs(graphs);

    QString line = in.readLine();
    if (line.isEmpty()) {
        file.close();
        return false;
    }
    int numGraphs = line.toInt();

    for (int k = 0; k < numGraphs; ++k) {
        PlotGraph* newGraph = new PlotGraph;

        line = in.readLine();
        if (line.isEmpty()) { clearPlotGraphs(graphs); file.close(); return false; }
        newGraph->NumScans = line.toInt();

        line = in.readLine();
        if (line.isEmpty()) { clearPlotGraphs(graphs); file.close(); return false; }
        int vecSize = line.toInt();

        for (int i = 0; i < vecSize; ++i) {
            line = in.readLine();
            if (line.isEmpty()) { clearPlotGraphs(graphs); file.close(); return false; }

            QStringList values = line.split(',');

            if (values.size() < 3) { clearPlotGraphs(graphs); file.close(); return false; }

            DataPoint* dataPoint = new DataPoint;
            dataPoint->point = values.at(0).toInt();
            dataPoint->X = values.at(1).toFloat();
            int ySize = values.at(2).toInt();

            for (int j = 0; j < ySize; ++j) {
                if (3 + j < values.size()) {
                    float* yValuePtr = new float;
                    *yValuePtr = values.at(3 + j).toFloat();
                    dataPoint->Y.append(yValuePtr);
                } else {
                    delete dataPoint; clearPlotGraphs(graphs); file.close(); return false;
                }
            }
            newGraph->Vec.append(dataPoint);
        }
        graphs.append(newGraph);
    }

    file.close();
    return true;
}

// --- Memory Management Functions ---

/**
 * @brief Clears all dynamically allocated memory in a DataPoint structure.
 * @param dataPoint A pointer to the DataPoint to clear.
 */
void clearDataPoint(DataPoint *dataPoint) {
    if (!dataPoint) {
        return;
    }
    for (int i = 0; i < dataPoint->Y.size(); ++i) {
        delete dataPoint->Y.at(i);
    }
    dataPoint->Y.clear();
}

/**
 * @brief Clears all dynamically allocated memory in a single PlotGraph structure.
 * @param graph A pointer to the PlotGraph to clear.
 */
void clearPlotGraph(PlotGraph *graph) {
    if (!graph) {
        return;
    }
    for (int i = 0; i < graph->Vec.size(); ++i) {
        DataPoint* dataPoint = graph->Vec.at(i);
        clearDataPoint(dataPoint);
        delete dataPoint;
    }
    graph->Vec.clear();
}

/**
 * @brief Clears all dynamically allocated memory in a list of PlotGraph structures.
 * @param graphs A reference to the QList to clear.
 */
void clearPlotGraphs(QList<PlotGraph *>& graphs) {
    for (int i = 0; i < graphs.size(); ++i) {
        PlotGraph* graph = graphs.at(i);
        clearPlotGraph(graph);
        delete graph;
    }
    graphs.clear();
}


/*! @brief The Savitzky-Golay coefficients for different orders.
 * This array contains the coefficients used for the Savitzky-Golay filter.
 * Each entry corresponds to a specific polynomial order and contains the
 * coefficients and normalization factor.
 */
SGcoeff SG[4] = {{5,35,QList<float>() << -3 << 12 << 17 << 12 << -3},
                 {7,21,QList<float>() << -2 << 3 << 6 << 7 << 6 << 3 << -2},
                 {9,231,QList<float>() << -21 << 14 << 39 << 54 << 59 << 54 <<39 << 14 << -21},
                 {11,429,QList<float>() << -36 << 9 << 44 << 69 << 84 << 89 <<84 << 69 << 44 << 9 << -36}};

/*!
 * @brief Constructor for the Plot class.
 * Initializes the plot with the specified title, Y-axis label, X-axis label,
 * and number of plots. Sets up the UI and connects signals and slots.
 *
 * @param parent Pointer to the parent widget.
 * @param Title The title of the plot.
 * @param Yaxis The label for the Y-axis.
 * @param Xaxis The label for the X-axis.
 * @param NumPlots The number of plots to create.
 */
Plot::Plot(QWidget *parent, QString Title, QString Yaxis, QString Xaxis, int NumPlots) :
    QDialog(parent),
    ui(new Ui::Plot)
{
    p = parent;
    ui->setupUi(this);
    m=1;
    b=0;
    Filter = -1;
    m_xAxisRangeHistory.clear();
    m_yAxisRangeHistory.clear();
    m_isUndoingZoom = false;
    ui->txtComments->appendPlainText("CreatePlot," + Title.replace(",","_") + "," + Yaxis.replace(",","_") + "," + Xaxis.replace(",","_") + "," + QString::number(NumPlots));
    Comments = ui->txtComments->toPlainText();
    PlotTitle = Title;
    Plot::setWindowTitle(Title);
    this->installEventFilter(this);
    ui->txtComments->setVisible(false);
    ui->pbCloseComments->setVisible(false);
    plotGraphs.clear();
    // Setup the statusbar
    statusBar = new QStatusBar(this);
    statusBar->setGeometry(0,this->height()-18,this->width(),18);
    statusBar->raise();
    // Hide the heat maps
    ui->HeatMap1->setVisible(false);
    ui->HeatMap2->setVisible(false);
    colorScale1 = new QCPColorScale(ui->HeatMap1);
    colorMap1   = new QCPColorMap(ui->HeatMap1->xAxis, ui->HeatMap1->yAxis);
    colorScale2 = new QCPColorScale(ui->HeatMap2);
    colorMap2   = new QCPColorMap(ui->HeatMap2->xAxis, ui->HeatMap2->yAxis);
    // give the axes some labels:
    ui->Graph->xAxis->setLabel(Xaxis);
    ui->Graph->yAxis->setLabel(Yaxis);
    // create the plots
    for(int i=0; i<NumPlots; i++)
    {
        ui->Graph->addGraph();
        if(i == 1) ui->Graph->graph(1)->setPen(QPen(Qt::red));
    }
    ui->Graph->plotLayout()->insertRow(0);
    plotFile = new QCPTextElement(ui->Graph, "");
    if(p == 0) plotFile->setText("Not saved!");
    else plotFile->setText(Title);
    if(plotFile != NULL) ui->Graph->plotLayout()->addElement(0, 0, plotFile);
    ui->Graph->replot();
    connect(ui->Graph, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(mousePressed(QMouseEvent*)));
    connect(ui->Graph, SIGNAL(mouseMove(QMouseEvent*)), this, SLOT(mouseMove(QMouseEvent*)));
    connect(ui->HeatMap1, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(mousePressedHM(QMouseEvent*)));
    connect(ui->HeatMap2, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(mousePressedHM(QMouseEvent*)));
    connect(ui->pbCloseComments, SIGNAL(pressed()), this, SLOT(slotCloseComments()));
    if((pProps != nullptr) && pProps->LassoZoom)
    {
        connect(ui->Graph->xAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(slotSaveRangeHistory(QCPRange)));
        connect(ui->Graph->yAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(slotSaveRangeHistory(QCPRange)));
    }
    // -----------------------
    // Plot popup menu options
    SaveOption = new QAction("Save", this);
    connect(SaveOption, SIGNAL(triggered()), this, SLOT(slotSaveMenu()));
    ExportOption = new QAction("Export", this);
    connect(ExportOption, SIGNAL(triggered()), this, SLOT(slotExportMenu()));
    LoadOption = new QAction("Load", this);
    connect(LoadOption, SIGNAL(triggered()), this, SLOT(slotLoadMenu()));
    CommentOption  = new QAction("Comments", this);
    connect(CommentOption, SIGNAL(triggered()), this, SLOT(slotCommentMenu()));
    XaxisZoomOption = new QAction("Xaxis zoom", this);
    XaxisZoomOption->setCheckable(true);
    connect(XaxisZoomOption, SIGNAL(triggered()), this, SLOT(slotXaxisZoomOption()));
    YaxisZoomOption = new QAction("Yaxis zoom", this);
    YaxisZoomOption->setCheckable(true);
    connect(YaxisZoomOption, SIGNAL(triggered()), this, SLOT(slotYaxisZoomOption()));
    if((pProps != nullptr) && pProps->LassoZoom)
    {
        ZoomOutFullOption = new QAction("Zoom Out (Full View)", this); // Renamed for clarity
        connect(ZoomOutFullOption, SIGNAL(triggered()), this, SLOT(slotZoomOut()));
        ZoomOutOneLevelOption = new QAction("Zoom Out (One Level)", this);
        connect(ZoomOutOneLevelOption, SIGNAL(triggered()), this, SLOT(slotZoomOutOneLevel()));
        XaxisZoomOption->setChecked(true);
        ZoomSelect();
    }
    FilterOption = new QAction("Filter", this);
    connect(FilterOption, SIGNAL(triggered()), this, SLOT(slotFilterOption()));
    TrackOption = new QAction("Track", this);
    TrackOption->setCheckable(true);
    connect(TrackOption, SIGNAL(triggered()), this, SLOT(slotTrackOption()));
    ClipboardOption = new QAction("Clipboard", this);
    connect(ClipboardOption, SIGNAL(triggered()), this, SLOT(slotClipBoard()));
    HeatOption = new QAction("Heatmap", this);
    HeatOption->setCheckable(true);
    connect(HeatOption, SIGNAL(triggered()), this, SLOT(slotHeatMap()));
    if((pProps != nullptr) && pProps->LassoZoom)
    {
        connect(ui->Graph, &QCustomPlot::mousePress, this, [this](QMouseEvent *event) {
            if(event->button() == Qt::LeftButton) {
                if(ui->Graph->selectionRectMode() == QCP::srmNone) {
                    ui->Graph->setSelectionRectMode(QCP::srmZoom);
                    if(ui->Graph->selectionRect()) {
                        ui->Graph->selectionRect()->setVisible(true);
                    }
                }
            }
        });
    }
}

/*!
 * @brief Frees all data associated with the plot.
 * This function iterates through all PlotGraph objects and their DataPoint objects,
 * deleting dynamically allocated memory to prevent memory leaks.
 */
void Plot::FreeAllData(void)
{
    for(int i=0;i<plotGraphs.count();i++)
    {
        for(int j=0;j<plotGraphs[i]->Vec.count();j++)
        {
            for(int k=0;k<plotGraphs[i]->Vec[j]->Y.count();k++)
            {
                delete plotGraphs[i]->Vec[j]->Y[k];
            }
            delete plotGraphs[i]->Vec[j]; // Delete DataPoint
        }
        delete plotGraphs[i]; // Delete PlotGraph
    }
    plotGraphs.clear();
}

/*!
 * @brief Destructor for the Plot class.
 * Cleans up resources by freeing all data and deleting the UI.
 */
Plot::~Plot()
{
    FreeAllData();
    delete ui;
}

// closeEvent — emits DialogClosed(this) so the owner can delete the Plot object.
void Plot::closeEvent (QCloseEvent *)
{
    emit DialogClosed(this);
}

/*!
 * @brief Resize event handler for the Plot widget.
 * This function adjusts the geometry of the graph, heat maps, status bar,
 * and comments text area when the widget is resized.
 *
 * @param event The resize event containing the new size information.
 */

void Plot::resizeEvent(QResizeEvent *event)
{
    ui->Graph->setGeometry(0,0,this->width(),this->height()-18);
    if(ui->Graph->graphCount() < 2) ui->HeatMap1->setGeometry(0,0,this->width(),this->height()-18);
    else  ui->HeatMap1->setGeometry(0,0,this->width()/2,this->height()-18);
    ui->HeatMap2->setGeometry(this->width()/2,0,this->width()/2,this->height()-18);
    statusBar->setGeometry(0,this->height()-18,this->width(),18);
    ui->txtComments->setGeometry(0,0,this->width()-ui->pbCloseComments->width(),this->height()-18);
    ui->pbCloseComments->setGeometry(this->width()-ui->pbCloseComments->width(),this->height()-18-ui->pbCloseComments->height(),ui->pbCloseComments->width(),ui->pbCloseComments->height());
    ui->Graph->replot();
    QWidget::resizeEvent(event);
}

// eventFilter — handles keyboard navigation through stored scans (arrow keys,
// Shift+J to jump by number). Passes all other events to the base class.
bool Plot::eventFilter(QObject *obj, QEvent *event)
{

    if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent *key = static_cast<QKeyEvent *>(event);
        if(key->key() == 16777236) CurrentIndex++;
        else if(key->key() == 16777235) CurrentIndex += 10;
        else if(key->key() == 16777234)  CurrentIndex--;
        else if(key->key() == 16777237)  CurrentIndex -= 10;
        else if((key->key() == 74) && (key->modifiers() & Qt::ShiftModifier))
        {
            // Shift j will get you to this point, allows jump to scan
            bool ok;
            QString text = QInputDialog::getText(0, "MIPS", "Enter desided scan number:", QLineEdit::Normal,"", &ok );
            if(ok && !text.isEmpty()) CurrentIndex = text.toInt() - 1;
        }
        else return QObject::eventFilter(obj, event);
        if(CurrentIndex < 0) CurrentIndex = 0;
        if(CurrentIndex >= plotGraphs.count()) CurrentIndex = plotGraphs.count()-1;
        if(CurrentIndex >= 0)
        {
            PaintGraphs(plotGraphs[CurrentIndex]);
            Plot::setWindowTitle(PlotTitle + ", " + "number: " + QString::number(CurrentIndex+1));
            ui->Graph->replot();
        }
        return true;
    }
    return QObject::eventFilter(obj, event);
}

// mousePressedHM — right-click handler for heat map widgets. Shows the
// heatmap context menu with the Heatmap toggle action.
void Plot::mousePressedHM(QMouseEvent* event)
{
    if (event->button() == Qt::RightButton)
    {
        popupMenu = new QMenu(tr("Plot options"), this);
        popupMenu->addAction(HeatOption);
        popupMenu->exec(event->globalPosition().toPoint());
    }
}

// mouseMove — updates the status bar with X/Y coordinates when Track mode is active.
void Plot::mouseMove(QMouseEvent*event)
{
    if(TrackOption->isChecked())
    {
        double x = ui->Graph->xAxis->pixelToCoord(event->pos().x());
        double y = ui->Graph->yAxis->pixelToCoord(event->pos().y());
        statusBar->showMessage("X = " + QString::number(x) + ", Y = " + QString::number(y));
    }
}

// mousePressed — right-click handler for the main graph. Shows the full
// context menu with save, export, load, zoom, filter, and heatmap options.
void Plot::mousePressed(QMouseEvent* event)
{
    if (event->button() == Qt::RightButton)
    {
        popupMenu = new QMenu(tr("Plot options"), this);
        popupMenu->addAction(SaveOption);
        popupMenu->addAction(ExportOption);
        popupMenu->addAction(LoadOption);
        popupMenu->addAction(XaxisZoomOption);
        popupMenu->addAction(YaxisZoomOption);
        popupMenu->addAction(ZoomOutFullOption); // Full View
        popupMenu->addAction(ZoomOutOneLevelOption); // <-- NEW: One Level Zoom Out
        popupMenu->addAction(FilterOption);
        popupMenu->addAction(TrackOption);
        popupMenu->addAction(ClipboardOption);
        popupMenu->addAction(CommentOption);
        popupMenu->addAction(HeatOption);
        popupMenu->exec(event->globalPosition().toPoint());
    }
}

// Save — writes the entire comment/command block to filename as a .plot text file
// and updates the plotFile title element.
void Plot::Save(QString filename)
{
    if(filename.isEmpty()) return;
    QFile file(filename);
    if(file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream stream(&file);
        stream << ui->txtComments->toPlainText();
        file.close();
        PlotCommand("plotFile," + QDir(filename).dirName(), true);
    }
}

// slotSaveMenu — opens a file-save dialog and calls Save() with the chosen path.
void Plot::slotSaveMenu(void)
{
    QString fileName = QFileDialog::getSaveFileName(this, "Save plot data","","Plot (*.plot);;All files (*.*)");
    Save(fileName);
}

// slotExportMenu — opens dialogs to select a CSV file and a plot number, then
// exports that scan's X/Y data with optional Savitzky-Golay filtering.
void Plot::slotExportMenu(void)
{
    QList<float> I,A;
    bool ok;
    QString rec;

    if(plotGraphs.count() <= 0) return;
    QString fileName = QFileDialog::getSaveFileName(this, "Export plot data","","Plot (*.csv);;All files (*.*)");
    if(fileName.isEmpty()) return;
    QString res = QInputDialog::getText(this, "Export", "Enter plot number to export", QLineEdit::Normal,QString(), &ok);
    if(!ok && res.isEmpty()) return;
    int plot = res.toInt() - 1;
    if((plot < 0) || (plot > plotGraphs.count()-1))
    {
        statusBar->showMessage("Invalid plot selected!");
        return;
    }
    QFile file(fileName);
    if(file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream stream(&file);
        // Write the Xaxis data row
        rec.clear();
        rec = "\t" + ui->Graph->xAxis->label();
        for(int i=0;i<plotGraphs[0]->Vec.count();i++)
        {
            rec += "\t" + QString::number(plotGraphs[0]->Vec[i]->X * m + b);
        }
        stream << rec + "\n";
        // Output scan header
        stream << "Scan\t";
        if(!Scan.isEmpty()) stream << Scan.split(",")[0];
        stream << "\n";
        // Output each scan
        for(int scan=0;scan<plotGraphs.count();scan++)
        {
            rec.clear();
            rec = QString::number(scan+1) + "\t";
            if(!Scan.isEmpty() && (Scan.split(",").count() == 3))
            {
                float sd = (Scan.split(",")[2].toFloat() - Scan.split(",")[1].toFloat())/plotGraphs.count();
                rec += QString::number(Scan.split(",")[1].toFloat() + sd * scan);
            }
            I.clear();
            for(int i=0;i<plotGraphs[0]->Vec.count();i++) I.append(*plotGraphs[scan]->Vec[i]->Y[plot]);
            SavitzkyGolayFilter(Filter,I,&A);
            for(int i=0;i<plotGraphs[0]->Vec.count();i++)
            {
                rec += "\t" + QString::number(A[i]);
            }
            stream << rec + "\n";
        }
        file.close();
        statusBar->showMessage("Data export complete!");
        return;
    }
    statusBar->showMessage("Export failed!");
}

// slotLoadMenu — opens a file-open dialog and calls Load() with the chosen path.
void Plot::slotLoadMenu(void)
{
    QString fileName = QFileDialog::getOpenFileName(this, "Load plot data","","Plot (*.plot);;All files (*.*)");
    Load(fileName);
}

/*!
 * @brief Loads plot data from a file and processes the commands.
 * This function reads the contents of a specified file, clears any existing data,
 * and processes each line as a plot command.
 *
 * @param filename The name of the file to load.
 */
void Plot::Load(QString filename)
{
    if(filename.isEmpty()) return;
    QFile file(filename);
    if(file.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        QTextStream stream(&file);
        Comments = stream.readAll();
        ui->txtComments->clear();
        ui->txtComments->appendPlainText(Comments);
        // Clear current data
        FreeAllData();
        file.close();
        // process the commands
        QStringList Lines = Comments.split("\n");
        for(int i=0;i<Lines.count();i++)
        {
            PlotCommand(Lines[i], true);
        }
        PlotCommand("plotFile," + QDir(filename).dirName(), false);
    }
}

// slotCommentMenu — shows the comments text area and hides the graph, allowing
// the user to view and edit the raw command/comment block.
void Plot::slotCommentMenu(void)
{
    if(!ui->txtComments->isVisible())
    {
        ui->Graph->setVisible(false);
        ui->txtComments->setVisible(true);
        ui->pbCloseComments->setVisible(true);
    }
}

// slotCloseComments — restores the graph view, hides the comments editor, and
// saves any edits back to the Comments string.
void Plot::slotCloseComments(void)
{
    ui->Graph->setVisible(true);
    ui->txtComments->setVisible(false);
    ui->pbCloseComments->setVisible(false);
    Comments = ui->txtComments->toPlainText();
}

/*!
 * @brief Saves the current range of the axis that triggered the signal.
 * This slot is connected to both xAxis->rangeChanged and yAxis->rangeChanged.
 * It prevents saving the range when an undo operation is currently being performed
 * to avoid saving the undo state back into the history.
 *
 * @param newRange The range that the axis has just changed to. (Not used directly
 * as we save the *previous* state which is often needed, but
 * QCustomPlot's rangeChanged is best suited to track history).
 */
void Plot::slotSaveRangeHistory(QCPRange newRange)
{
    // Only save range history when not in the middle of a zoom undo operation
    if(!m_isUndoingZoom)
    {
        QCPAxis *axis = qobject_cast<QCPAxis*>(sender());
        if(axis == ui->Graph->xAxis)
        {
            if(XaxisZoomOption->isChecked())
            {
                if(!m_xAxisRangeHistory.isEmpty() &&
                    m_xAxisRangeHistory.top().lower == newRange.lower &&
                    m_xAxisRangeHistory.top().upper == newRange.upper) return;
                if(m_xAxisRangeHistory.count() > 20) m_xAxisRangeHistory.pop_front();
                m_xAxisRangeHistory.push(newRange);
            }
        }
        else if(axis == ui->Graph->yAxis)
        {
            if(YaxisZoomOption->isChecked())
            {
                if(!m_yAxisRangeHistory.isEmpty() &&
                    m_yAxisRangeHistory.top().lower == newRange.lower &&
                    m_yAxisRangeHistory.top().upper == newRange.upper) return;
                if(m_yAxisRangeHistory.count() > 20) m_yAxisRangeHistory.pop_front();
                m_yAxisRangeHistory.push(newRange);
            }
        }
    }
}

/*!
 * @brief Handles the 'Zoom Out (Full View)' action.
 * Rescales the X and Y axes of the main graph to fit the entire range of the current data.
 */
void Plot::slotZoomOut(void)
{
    m_isUndoingZoom = true;

    ui->Graph->setSelectionRectMode(QCP::srmNone);

    if(XaxisZoomOption->isChecked())
    {
        ui->Graph->xAxis->rescale(true);
        m_xAxisRangeHistory.clear();
    }
    if(YaxisZoomOption->isChecked())
    {
        ui->Graph->yAxis->rescale(true);
        m_yAxisRangeHistory.clear();
    }

    ui->Graph->replot(QCustomPlot::rpImmediateRefresh);
    statusBar->showMessage("Zoomed out to full data range.");

    m_isUndoingZoom = false;
}

/*!
 * @brief Handles the 'Zoom Out (One Level)' action.
 * Restores the X and Y axes to the previous range stored in the history stack.
 */
void Plot::slotZoomOutOneLevel(void)
{
    ui->Graph->setSelectionRectMode(QCP::srmNone);
    if(XaxisZoomOption->isChecked())
    {
        if (!m_xAxisRangeHistory.isEmpty())
        {
            m_xAxisRangeHistory.pop();  // Remove current zoom level
            if (!m_xAxisRangeHistory.isEmpty())
            {
                ui->Graph->xAxis->setRange(m_xAxisRangeHistory.top());
                statusBar->showMessage("Zoomed out one level.");
            }
            else
            {
                ui->Graph->xAxis->rescale(true);  // Back to full range
                statusBar->showMessage("Zoomed out to full data range.");
            }
        }
        else statusBar->showMessage("No more zoom history available.");
    }

    if(YaxisZoomOption->isChecked())
    {
        if (!m_yAxisRangeHistory.isEmpty())
        {
            m_yAxisRangeHistory.pop();
            if (!m_yAxisRangeHistory.isEmpty())
            {
                ui->Graph->yAxis->setRange(m_yAxisRangeHistory.top());
                statusBar->showMessage("Zoomed out one level.");
            }
            else
            {
                ui->Graph->yAxis->rescale(true);
                statusBar->showMessage("Zoomed out to full data range.");
            }
        }
        else statusBar->showMessage("No more zoom history available.");
    }

    ui->Graph->replot(QCustomPlot::rpImmediateRefresh);
}

// SavitzkyGolayFilter — applies the Savitzky-Golay smoothing filter of the given
// order index (0–3 → 5/7/9/11-point) to Y, writing the result into *Yf.
// Pass order < 0 to copy Y unchanged.
void Plot::SavitzkyGolayFilter(int order, QList<float> Y, QList<float> *Yf)
{
    float   val;
    int     k;
    SGcoeff sg;

    (*Yf).clear();
    if(order<0)
    {
        for(int i=0;i<Y.count();i++) (*Yf).append(Y[i]);
        return;
    }
    sg = SG[order];
    for(int i=0;i<Y.count();i++)
    {
        val = 0;
        for(int j=-(sg.np-1)/2;j<=(sg.np-1)/2;j++)
        {
            k = abs(i+j);
            if(k >= Y.count()) k = (2*Y.count()-1) -k;
            val += Y[k] * sg.an[j+(sg.np-1)/2];
        }
        (*Yf).append(val/sg.h);
    }
}

/*!
 * @brief Paints the graphs in the Plot widget.
 * This function clears existing graph data, filters the data points,
 * and adds them to the graph for visualization.
 *
 * @param pg Pointer to the PlotGraph object containing the data to be plotted.
 */
void Plot::PaintGraphs(PlotGraph *pg)
{
    if(pg==NULL) return;
    if(pg->NumScans==0) return;
    for(int i=0;i<ui->Graph->graphCount();i++) ui->Graph->graph(i)->data()->clear();
    // Filter the data
    QList<float> I,A,B;
    if(ui->Graph->graphCount()>=1)
    {
        I.clear();
        A.clear();
        for(int i=0;i<pg->Vec.count();i++) if(pg->Vec[i]->Y.count() > 0) I.append(*pg->Vec[i]->Y[0]/pg->NumScans);
        if(I.count() > 0) SavitzkyGolayFilter(Filter,I,&A);
    }
    if(ui->Graph->graphCount()>1)
    {
        I.clear();
        B.clear();
        for(int i=0;i<pg->Vec.count();i++) if(pg->Vec[i]->Y.count() > 1) I.append(*pg->Vec[i]->Y[1]/pg->NumScans);
        if(I.count() > 0) SavitzkyGolayFilter(Filter,I,&B);
    }
    // Add the data points from last plotGraphs
    for(int i=0;i<pg->Vec.count();i++)
    {
        if(ui->Graph->graphCount()>=1) if(A.count() > 0) ui->Graph->graph(0)->addData((pg->Vec[i]->X - pg->Vec[0]->X)*m+b, A[i]);
        if(ui->Graph->graphCount()>1)  if(B.count() > 0) ui->Graph->graph(1)->addData((pg->Vec[i]->X - pg->Vec[0]->X)*m+b, B[i]);
    }
}

/*!
 * @brief Adds an average graph to the plot.
 * This function calculates the average of all existing graphs and adds a new graph
 * representing the average data points.
 *
 * It assumes that all graphs have the same number of data points and that each data
 * point has the same number of Y-values.
 */
void Plot::addAverageGraph(void)
{
    // Check if there are any graphs to average.
    if (plotGraphs.isEmpty()) {
        qDebug() << "Cannot average an empty list of graphs.";
        return;
    }

    // Assume all graphs have the same number of data points.
    // We will use the first graph as a template for the new average graph.
    PlotGraph *firstGraph = plotGraphs.first();
    int numPoints = firstGraph->Vec.size();
    if (numPoints == 0) {
        qDebug() << "First graph is empty, cannot average.";
        return;
    }

    // Create a new PlotGraph object to hold the averaged data.
    PlotGraph *averageGraph = new PlotGraph();
    averageGraph->NumScans = 1; // It's an average, so we'll treat it as a single scan.

    // Loop through each data point in the first graph to set up the new average graph.
    for (int i = 0; i < numPoints; ++i) {
        DataPoint *newDataPoint = new DataPoint();
        newDataPoint->point = firstGraph->Vec[i]->point;
        newDataPoint->X = firstGraph->Vec[i]->X;

        // Loop through the list of Y-values at each point.
        // We will assume that the number of Y-values is consistent across all graphs.
        int numYValues = firstGraph->Vec[i]->Y.size();
        for (int y = 0; y < numYValues; ++y) {
            float sum = 0.0f;
            int validGraphs = 0;

            // Sum the corresponding Y-values from all existing graphs.
            foreach (PlotGraph *graph, plotGraphs) {
                if (graph->Vec.size() > i && graph->Vec[i]->Y.size() > y) {
                    // Check for null pointers before dereferencing
                    if (graph->Vec[i]->Y[y] != nullptr) {
                        sum += *(graph->Vec[i]->Y[y]);
                        validGraphs++;
                    }
                }
            }

            // Calculate the average for this specific Y-value.
            float averageY = (validGraphs > 0) ? sum / validGraphs : 0.0f;

            newDataPoint->Y.append(new float(averageY));
        }

        // Add the new data point to the average graph's vector.
        averageGraph->Vec.append(newDataPoint);
    }

    // Add the newly created average graph to the list of all graphs.
    plotGraphs.append(averageGraph);

    qDebug() << "Successfully added an average graph with" << numPoints << "data points.";
}

// This function accepts plot command strings. Commands are also written
// to the comment block. Comments can be added to the comment block by sending
// commands that are not valid or just make the first character of the command a #.
//
// The following commands are supported:
// Title,name, Sets the plot title, this is the name in the title bar of
//             the plot window.
// Clear, Clears the graphs and data
// ClearComments, Clears the comment block
// NoPopup, true/false, if true no popup menu will be displayed
// Normalcursor, sets the normal cursor
// Refresh, Refreshes the plot, this is used to update the plot after
//          data has been added to the plot.
// Plot
// Scan
// Xrange, min, max
// Yrange, min, max
// Yrange,Auto, Performs auto range of displayed data, issue after data is
//              added to the plot and displayed.
// PlotOnly, true/false, if true only plot the data, do not append to comments
// NewGraph,numgraphs,points
// AddPoint,point num, X axis value, Graph1, Graph 2, ....
// AddPoints,graph num, point num,Y1,Y2, ...
// Average, Adds an average graph to the plot, this averages all the
//          data points in the plot and adds a new graph with the average
//          data points. The average graph is added to the end of the plotGraphs list.
// Plot1,label, label is the heatmap color table label for plot 1
// Plot2,label, label is the heatmap color table label for plot 2
// Delete
// Save,filename, Saves the plot data to a file, filename is the name of the file
// Load,filename, Loads the plot data from a file, filename is the name of the file
// PlotFile,name, sets the plotFile->text to the name, normally set "no saved" and
//                then set to the file name when the plot is saved.
// OpenFile, Opens a file to stream data, use a file popup dialog. All data in the
//           comment block is written to the file and then the file is closed. All
//           subsequent commands are appended to the file. The file is closed when
//           the plot is closed or the OpenFile command is issued again.
// SaveBinary,filename, Saves the plot data to a binary file, filename is the name of the file
// LoadBinary,filename, Loads the plot data from a binary file, filename is the name of the file
// Export,filename, Exports the plot data to a CSV file, filename is the name of the file
// Import,filename, Imports the plot data from a CSV file, filename is the name of the file
//
QString Plot::PlotCommand(QString cmd, bool PlotOnly)
{
    QStringList reslist = cmd.split(",");

    // The save and load binary commans and the export and import do not
    // append the command to the comments, they are used to save and load
    // the plot data to and from a binary file or CSV file.
    if((reslist[0].toUpper() == "SAVEBINARY") && (reslist.count() == 2))
    {
        if(savePlotGraphs(reslist[1], plotGraphs))
        {
            statusBar->showMessage("Plot data saved to " + fileName);
        } else
        {
            statusBar->showMessage("Failed to save plot data to " + fileName);
        }
        return "";
    }
    if((reslist[0].toUpper() == "LOADBINARY") && (reslist.count() == 2))
    {
        if(loadPlotGraphs(reslist[1], plotGraphs))
        {
            statusBar->showMessage("Plot data loaded from " + fileName);
            PaintGraphs(plotGraphs.last());
            Plot::setWindowTitle(PlotTitle + ", " + "number: " + QString::number(CurrentIndex+1));
            ui->Graph->replot();
        } else
        {
            statusBar->showMessage("Failed to load plot data from " + fileName);
        }
        return "";
    }
    if((reslist[0].toUpper() == "EXPORT") && (reslist.count() == 2))
    {
        if(savePlotGraphsCsv(reslist[1], plotGraphs))
        {
            statusBar->showMessage("Plot data saved to " + fileName);
        } else
        {
            statusBar->showMessage("Failed to save plot data to " + fileName);
        }
        return "";
    }
    if((reslist[0].toUpper() == "IMPORT") && (reslist.count() == 2))
    {
        if(loadPlotGraphsCsv(reslist[1], plotGraphs))
        {
            statusBar->showMessage("Plot data loaded from " + fileName);
            PaintGraphs(plotGraphs.last());
            Plot::setWindowTitle(PlotTitle + ", " + "number: " + QString::number(CurrentIndex+1));
            ui->Graph->replot();
        } else
        {
            statusBar->showMessage("Failed to load plot data from " + fileName);
        }
        return "";
    }
    // If the file name is defined we will save the command to a file
    if(!fileName.isEmpty())
    {
        QFile file(fileName);
        // Open file and append the command to the file
        if (file.open(QIODevice::WriteOnly | QIODevice::Append))
        {
            // File opened successfully
            QTextStream out(&file);
            out << cmd + "\n";
            file.close();
        } else
        {
            QMessageBox msgBox;
            // File could not be opened
            msgBox.setText("File could not be opened, data will not be saved to a file!");
            msgBox.setInformativeText("");
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.exec();
        }
    }
    if((!PlotOnly) && (!plotOnly))
    {
        ui->txtComments->appendPlainText(cmd);
        Comments.append(cmd + "\n");
        if(reslist[0].toUpper() == "SAVE") Save(reslist[1]);
        else if(reslist[0].toUpper() == "LOAD") Load(reslist[1]);
    }
    // Adds multiple points to the plot. Command format:
    // ADDPOINTS,graph num, point num,Y1,Y2, ...
    if((reslist[0].toUpper() == "ADDPOINTS")&&(reslist.count()>3))
    {
        if(reslist[2].toInt() > 0)
        {
            if((plotGraphs.last()->Vec.count() > 0) && (reslist[1].toInt() > 0))
            {
                if(reslist[1].toInt()-1 < plotGraphs.last()->Vec[0]->Y.count())
                {
                    if(reslist[2].toInt()-1 == 0) plotGraphs.last()->NumScans++;
                    for(int i=3;i<reslist.count();i++)
                    {
                        plotGraphs.last()->Vec[reslist[2].toInt()-1 + i - 3]->X = (reslist[2].toFloat() + i-3);
                        if(reslist[1].toInt() == 1) *plotGraphs.last()->Vec[reslist[2].toInt()-1 + i -3]->Y[0] += reslist[i].toFloat();
                        if((reslist[1].toInt() == 2) && (ui->Graph->graphCount() > 1)) *plotGraphs.last()->Vec[reslist[2].toInt()-1 + i -3]->Y[1] += reslist[i].toFloat();
                    }
                }
            }
        }
    }
    if((reslist[0].toUpper() == "ADDPOINT")&&(reslist.count()>=4))
    {
        if((plotGraphs.last()->Vec.count() > 0) && (reslist[1].toInt() > 0))
        {
            if(reslist[1].toInt()-1 < plotGraphs.last()->Vec.count())
            {
                if(reslist[1].toInt()-1 == 0) plotGraphs.last()->NumScans++;
                plotGraphs.last()->Vec[reslist[1].toInt()-1]->X = reslist[1].toFloat();
                *plotGraphs.last()->Vec[reslist[1].toInt()-1]->Y[0] += reslist[3].toFloat();
                if((reslist.count()>4) && (ui->Graph->graphCount() > 1)) *plotGraphs.last()->Vec[reslist[1].toInt()-1]->Y[1] += reslist[4].toFloat();
            }
        }
    }
    else if((cmd.startsWith("CLEARCOMMENTS",Qt::CaseInsensitive)) && (reslist.count()==1))
    {
        ui->txtComments->clear();
        Comments = ui->txtComments->toPlainText();
    }
    else if((cmd.startsWith("PLOTONLY",Qt::CaseInsensitive)) && (reslist.count()==2))
    {
        if(reslist[1].toUpper() == "TRUE") plotOnly = true;
        if(reslist[1].toUpper() == "FALSE") plotOnly = false;
    }
    else if((cmd.startsWith("OPENFILE",Qt::CaseInsensitive)) && (reslist.count()==1))
    {
        // Open a file to stream data, use a file popup dialog
        fileName = QFileDialog::getSaveFileName(this, "Select file to save plot data","","Plot (*.Plot);;All files (*.*)");
        if(fileName.isEmpty())
        {
            QMessageBox msgBox;
            msgBox.setText("File could not be opened, data will not be saved to a file!");
            msgBox.setInformativeText("");
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.exec();
        }
        QFile file(fileName);
        if(file.exists()) file.remove();
        // Open file and append the command to the file
        if (file.open(QIODevice::WriteOnly | QIODevice::Append))
        {
            // File opened successfully
            QTextStream out(&file);
            out << ui->txtComments->toPlainText() + "\n";
            out << "FileName," << fileName + "\n";
            file.close();
        }
        return "";
    }
    else if((cmd.startsWith("CREATEPLOT",Qt::CaseInsensitive)) && (reslist.count()==5))
    {
        PlotTitle = reslist[1].replace("_",",");
        ui->Graph->xAxis->setLabel(reslist[3].replace("_",","));
        ui->Graph->yAxis->setLabel(reslist[2].replace("_",","));
        ui->Graph->clearGraphs();
        for(int i=0; i<reslist[4].toInt(); i++)
        {
            ui->Graph->addGraph();
            if(i == 1) ui->Graph->graph(1)->setPen(QPen(Qt::red));
        }
    }
    else if((reslist[0].toUpper() == "NOPOPUP") && reslist.count() == 2)
    {
        if(reslist[1].toUpper() == "TRUE")
        {
            disconnect(ui->Graph, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(mousePressed(QMouseEvent*)));
        }
        if(reslist[1].toUpper() == "FALSE")
        {
            disconnect(ui->Graph, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(mousePressed(QMouseEvent*)));
            connect(ui->Graph, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(mousePressed(QMouseEvent*)));
        }
    }
    else if(reslist[0].toUpper() == "NORMALCURSOR")
    {
        QApplication::restoreOverrideCursor();
    }
    else if(reslist[0].toUpper() == "REFRESH")
    {
        if(plotGraphs.count() > 0) Plot::setWindowTitle(PlotTitle + ", " + "number: " + QString::number(CurrentIndex+1));
        else Plot::setWindowTitle(PlotTitle);
        ui->Graph->replot();
    }
    else if((cmd.startsWith("SCAN",Qt::CaseInsensitive)) && (reslist.count()==4))
    {
        Scan = reslist[1] + "," + reslist[2] + "," + reslist[3];
    }
    else if((cmd.startsWith("XRANGE",Qt::CaseInsensitive)) && (reslist.count()==2))
    {
        if(reslist[1].toUpper() == "TIME")
        {
            QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
            timeTicker->setTimeFormat("%h:%m:%s");
            ui->Graph->xAxis->setTicker(timeTicker);
            ui->Graph->axisRect()->setupFullAxesBox();

            // make left and bottom axes transfer their ranges to right and top axes:
            connect(ui->Graph->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->Graph->xAxis2, SLOT(setRange(QCPRange)));
        }
    }
    else if((cmd.startsWith("XRANGE",Qt::CaseInsensitive)) && (reslist.count()==3))
    {
        ui->Graph->xAxis->setRange(reslist[1].toDouble(), reslist[2].toDouble());
        ui->Graph->replot();
        m=1;
        b=0;
    }
    else if((cmd.startsWith("PLOT1",Qt::CaseInsensitive)) && (reslist.count()==2)) Label1 = reslist[1];
    else if((cmd.startsWith("PLOT2",Qt::CaseInsensitive)) && (reslist.count()==2)) Label2 = reslist[1];
    else if((cmd.startsWith("XRANGE",Qt::CaseInsensitive)) && (reslist.count()==5))
    {
        ui->Graph->xAxis->setRange(reslist[3].toDouble(), reslist[4].toDouble());
        ui->Graph->replot();
        m = (reslist[3].toFloat() - reslist[4].toFloat())/(reslist[1].toFloat()-reslist[2].toFloat());
        b = reslist[3].toFloat()  - reslist[1].toFloat()*m;
    }
    else if(cmd.startsWith("YRANGE",Qt::CaseInsensitive))
    {
        if(reslist.count()==2)
        {
            if(reslist[1].toUpper().trimmed()=="AUTO")
            {
                ui->Graph->yAxis->rescale(true);
                ui->Graph->replot();
            }
        }
        if(reslist.count()==3)
        {
            ui->Graph->yAxis->setRange(reslist[1].toDouble(), reslist[2].toDouble());
            ui->Graph->replot();
        }
    }
    else if(cmd.toUpper() == "CLEAR")
    {
        for(int i=0;i<ui->Graph->graphCount();i++) ui->Graph->graph(i)->data()->clear();
        plotGraphs.clear();         // added these four lines, 7/29/24
        ui->Graph->clearGraphs();   //
        Comments.clear();
        ui->txtComments->clear();
    }
    else if((reslist[0].toUpper() == "NEWGRAPH")&&(reslist.count()==3))
    {
        PlotGraph *pg = new PlotGraph;
        pg->Vec.clear();
        plotGraphs.append(pg);
        plotGraphs.last()->NumScans = 0;
        for(int i=0;i<reslist[2].toInt();i++)
        {
            DataPoint *dp = new DataPoint;
            plotGraphs.last()->Vec.append(dp);
            plotGraphs.last()->Vec.last()->point = 0;
            plotGraphs.last()->Vec.last()->X = 0;
            for(int j=0;j<reslist[1].toInt();j++)
            {
                float *f = new float;
                plotGraphs.last()->Vec.last()->Y.append(f);
                *plotGraphs.last()->Vec.last()->Y.last() = 0.0;
            }
        }
    }
    else if(cmd.toUpper() == "REMOVELAST")
    {
        if(plotGraphs.length() > 0) plotGraphs.removeLast();
    }
    else if((reslist[0].toUpper() == "PLOTPOINT")&&(reslist.count()>=3))
    {
        if(plotGraphs.count() < 1) return "";
        double key = reslist[1].toDouble();
        ui->Graph->graph(0)->addData(key, reslist[2].toDouble());
        if((reslist.count()==4) && (plotGraphs.count() > 1)) ui->Graph->graph(1)->addData(key, reslist[3].toDouble());
        ui->Graph->xAxis->setRangeUpper(key);
        ui->Graph->replot();
    }
    else if(cmd.toUpper() == "PLOT")
    {
        if(plotGraphs.count() < 1) return "";
        CurrentIndex = plotGraphs.count() - 1;
        PaintGraphs(plotGraphs.last());
    }
    else if(cmd.toUpper() == "AVERAGE")
    {
        addAverageGraph();
    }
    else if((reslist[0].toUpper() == "TITLE")&&(reslist.count()==2))
    {
        PlotTitle = reslist[1];
        Plot::setWindowTitle(PlotTitle);
    }
    else if((reslist[0].toUpper() == "PLOTFILE")&&(reslist.count()==2))
    {
        if((plotFile != NULL) && (p == 0))plotFile->setText(reslist[1]);
        ui->Graph->replot();
    }
    return "";
}

// slotXaxisZoomOption — toggles X-axis zoom mode and calls ZoomSelect() to apply it.
void Plot::slotXaxisZoomOption(void)
{
    if(XaxisZoomOption->isChecked()) XaxisZoomOption->setChecked(true);
    else XaxisZoomOption->setChecked(false);
    ZoomSelect();
}

// slotYaxisZoomOption — toggles Y-axis zoom mode and calls ZoomSelect() to apply it.
void Plot::slotYaxisZoomOption(void)
{
    if(YaxisZoomOption->isChecked()) YaxisZoomOption->setChecked(true);
    else YaxisZoomOption->setChecked(false);
    ZoomSelect();
}

// slotFilterOption — prompts the user for a Savitzky-Golay polynomial length
// (5/7/9/11 or 0 to disable), then repaints the current scan with the new filter.
void Plot::slotFilterOption(void)
{
    QMessageBox msgBox;
    bool ok;

    QString res = QInputDialog::getText(this, "Savitzky-Golay filter", "Enter Savitzky-Golay filter polynomial length, 5,7,9, or 11. \nEnter 0 for no filter.", QLineEdit::Normal,QString(), &ok);
    if(ok && !res.isEmpty())
    {
        switch(res.toInt())
        {
        case 5:
            Filter = 0;
            break;
        case 7:
            Filter = 1;
            break;
        case 9:
            Filter = 2;
            break;
        case 11:
            Filter = 3;
            break;
        case 0:
            Filter = -1;
            break;
        default:
            msgBox.setText("Invalid entry, filter will be turned off.");
            msgBox.setInformativeText("");
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.exec();
            Filter = -1;
            break;
        }
        PaintGraphs(plotGraphs[CurrentIndex]);
        ui->Graph->replot();
    }
}

/**
 * SLOT: Rescales the Y-axis to fit the visible data points on the X-axis.
 */
void Plot::slotRescaleYToDataInRange(QCPRange newXRange) {
    double minY = std::numeric_limits<double>::max();
    double maxY = -std::numeric_limits<double>::max();
    bool foundData = false;

    // Iterate through all graphs to find the visible bounds
    for (int i = 0; i < ui->Graph->graphCount(); ++i) {
        QCPGraph *graph = ui->Graph->graph(i);
        if (!graph->visible()) continue;

        // Use QCustomPlot's built-in data container to find the value range in a specific key range
        QCPRange valueRange = graph->getValueRange(foundData, QCP::sdBoth, newXRange);

        if (foundData) {
            if (valueRange.lower < minY) minY = valueRange.lower;
            if (valueRange.upper > maxY) maxY = valueRange.upper;
        }
    }

    if (foundData) {
        // Add a small margin (e.g., 10%) so the data isn't touching the top/bottom edges
        double margin = (maxY - minY) * 0.1;
        if (margin == 0) margin = 1.0; // Prevent flat line issues

        ui->Graph->yAxis->setRange(minY - margin, maxY + margin);

        // Prevent recursive history logging if this was triggered by an undo
    }
}


// ZoomSelect — configures QCustomPlot interaction mode (lasso zoom or scroll-wheel)
// based on the current X/Y zoom checkbox states.
void Plot::ZoomSelect(void)
{
    if(XaxisZoomOption->isChecked() && YaxisZoomOption->isChecked())
    {
        if((pProps != nullptr) && pProps->LassoZoom)
        {
            ui->Graph->yAxis->axisRect()->setRangeZoom(Qt::Vertical | Qt::Horizontal);
            ui->Graph->yAxis->axisRect()->setRangeDrag(Qt::Vertical | Qt::Horizontal);
            ui->Graph->axisRect()->setRangeZoomAxes(ui->Graph->xAxis, ui->Graph->yAxis);
            ui->Graph->setSelectionRectMode(QCP::srmZoom);
            //m_xAxisRangeHistory.push(ui->Graph->xAxis->range());
            //m_yAxisRangeHistory.push(ui->Graph->xAxis->range());
        }
        else
        {
            ui->Graph->xAxis->axisRect()->setRangeZoom(Qt::Vertical | Qt::Horizontal);
            ui->Graph->xAxis->axisRect()->setRangeDrag(Qt::Vertical | Qt::Horizontal);
            ui->Graph->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
        }
    }
    else if(XaxisZoomOption->isChecked())
    {
        if((pProps != nullptr) && pProps->LassoZoom)
        {
            ui->Graph->yAxis->axisRect()->setRangeZoom(Qt::Horizontal);
            ui->Graph->yAxis->axisRect()->setRangeDrag(Qt::Horizontal);
            ui->Graph->axisRect()->setRangeZoomAxes(ui->Graph->xAxis, NULL);
            ui->Graph->setSelectionRectMode(QCP::srmZoom);
            connect(ui->Graph->xAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(slotRescaleYToDataInRange(QCPRange)));
            //m_xAxisRangeHistory.push(ui->Graph->xAxis->range());
        }
        else
        {
            ui->Graph->xAxis->axisRect()->setRangeZoom(Qt::Horizontal);
            ui->Graph->xAxis->axisRect()->setRangeDrag(Qt::Horizontal);
            ui->Graph->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
        }
    }
    else if(YaxisZoomOption->isChecked())
    {
        if((pProps != nullptr) && pProps->LassoZoom)
        {
            ui->Graph->yAxis->axisRect()->setRangeZoom(Qt::Vertical);
            ui->Graph->yAxis->axisRect()->setRangeDrag(Qt::Vertical);
            ui->Graph->axisRect()->setRangeZoomAxes(NULL, ui->Graph->yAxis);
            ui->Graph->setSelectionRectMode(QCP::srmZoom);
            //m_yAxisRangeHistory.push(ui->Graph->yAxis->range());
        }
        else
        {
            ui->Graph->yAxis->axisRect()->setRangeZoom(Qt::Vertical);
            ui->Graph->yAxis->axisRect()->setRangeDrag(Qt::Vertical);
            ui->Graph->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
        }
    }
    else ui->Graph->setInteractions(QCP::iNone);
}

// slotHeatMap — toggles the dual QCPColorMap heatmap view. When enabled,
// builds heatmap data for both ion channels from plotGraphs, applies the
// Savitzky-Golay filter, and configures color scale and tick labels.
void Plot::slotHeatMap(void)
{
    if(plotGraphs.count() <= 0) return;
    if(plotGraphs[0]->Vec.count() <= 0) return;
    if(HeatOption->isChecked())
    {
        ui->HeatMap1->setVisible(true);
        // Heatmap 1 setup...
        // configure axis rect:
        colorMap1->data()->clear();
        //ui->HeatMap1->setInteractions(QCP::iRangeDrag|QCP::iRangeZoom); // this will also allow rescaling the color scale by dragging/zooming
        ui->HeatMap1->setInteractions(QCP::iRangeZoom); // this will also allow rescaling the color scale by dragging/zooming
        ui->HeatMap1->axisRect()->setupFullAxesBox(true);
        ui->HeatMap1->xAxis->setLabel(ui->Graph->xAxis->label());
        if(Scan.isEmpty()) ui->HeatMap1->yAxis->setLabel("Scan");
        else ui->HeatMap1->yAxis->setLabel(Scan.split(",")[0]);
        ui->HeatMap1->xAxis->axisRect()->setRangeDrag(Qt::Horizontal);
        ui->HeatMap1->xAxis->axisRect()->setRangeZoom(Qt::Horizontal);

        if(plotGraphs.isEmpty()) return;
        // set up the QCPColorMap:
        int nx = plotGraphs[0]->Vec.count();
        int ny = plotGraphs.count();
        colorMap1->data()->setSize(nx, ny); // we want the color map to have nx * ny data points
        colorMap1->data()->setRange(QCPRange(0, plotGraphs[0]->Vec.count()), QCPRange(0, plotGraphs.count())); // and span the coordinate range -4..4 in both key (x) and value (y) dimensions
        // now we assign some data, by accessing the QCPColorMapData instance of the color map:
        QList<float> I,A;
        for (int yIndex=0; yIndex<ny; ++yIndex)
        {
            I.clear();
            for (int xIndex=0; xIndex<nx; ++xIndex) I.append(*plotGraphs[yIndex]->Vec[xIndex]->Y[0] / plotGraphs[yIndex]->NumScans);
            SavitzkyGolayFilter(Filter,I,&A);
            for (int xIndex=0; xIndex<nx; ++xIndex)
            {
                colorMap1->data()->setCell(xIndex, yIndex, A[xIndex]);
            }
        }
        // add a color scale:
        ui->HeatMap1->plotLayout()->addElement(0, 1, colorScale1); // add it to the right of the main axis rect
        colorScale1->setType(QCPAxis::atRight); // scale shall be vertical bar with tick/axis labels right (actually atRight is already the default)
        colorMap1->setColorScale(colorScale1); // associate the color map with the color scale
        colorScale1->axis()->setLabel(Label1);
        colorScale1->setRangeDrag(true);
        colorScale1->setRangeZoom(true);

        // set the color gradient of the color map to one of the presets:
        colorMap1->setGradient(QCPColorGradient::gpPolar);

        // rescale the data dimension (color) such that all data points lie in the span visualized by the color gradient:
        colorMap1->rescaleDataRange();

        // make sure the axis rect and color scale synchronize their bottom and top margins (so they line up):
        QCPMarginGroup *marginGroup = new QCPMarginGroup(ui->HeatMap1);
        ui->HeatMap1->axisRect()->setMarginGroup(QCP::msBottom|QCP::msTop, marginGroup);
        colorScale1->setMarginGroup(QCP::msBottom|QCP::msTop, marginGroup);

        int step = 5;
        // Label the X axis ticks
        QSharedPointer<QCPAxisTickerText> textTickerX(new QCPAxisTickerText);
        for(int i=0;i<=step;i++) textTickerX->addTick(i * nx/5, QString::number(ui->Graph->xAxis->range().lower + i * (ui->Graph->xAxis->range().upper - ui->Graph->xAxis->range().lower)/step));
        ui->HeatMap1->xAxis->setTicker(textTickerX);
        // Label the y axis ticks
        QSharedPointer<QCPAxisTickerText> textTickerY(new QCPAxisTickerText);
        if(!Scan.isEmpty())
        {
            textTickerY->addTick(0,Scan.split(",")[1]);
            textTickerY->addTick(ny/3,QString::number(Scan.split(",")[1].toFloat() + (Scan.split(",")[2].toFloat()-Scan.split(",")[1].toFloat())/3.0));
            textTickerY->addTick(ny*2.0/3.0,QString::number(Scan.split(",")[1].toFloat() + 2.0*(Scan.split(",")[2].toFloat()-Scan.split(",")[1].toFloat())/3.0));
            textTickerY->addTick(ny,Scan.split(",")[2]);
            ui->HeatMap1->yAxis->setTicker(textTickerY);
        }
        // rescale the key (x) and value (y) axes so the whole color map is visible:
        ui->HeatMap1->rescaleAxes();
        ui->HeatMap1->replot();

        if(plotGraphs[0]->Vec[0]->Y.count() < 2) return;
        // Heatmap 2 setup...
        ui->HeatMap2->setVisible(true);
        // configure axis rect:
        colorMap2->data()->clear();
        //ui->HeatMap2->setInteractions(QCP::iRangeDrag|QCP::iRangeZoom); // this will also allow rescaling the color scale by dragging/zooming
        ui->HeatMap2->setInteractions(QCP::iRangeZoom); // this will also allow rescaling the color scale by dragging/zooming
        ui->HeatMap2->axisRect()->setupFullAxesBox(true);
        ui->HeatMap2->xAxis->setLabel(ui->Graph->xAxis->label());
        if(Scan.isEmpty()) ui->HeatMap2->yAxis->setLabel("Scan");
        else ui->HeatMap2->yAxis->setLabel(Scan.split(",")[0]);
        ui->HeatMap2->xAxis->axisRect()->setRangeDrag(Qt::Horizontal);
        ui->HeatMap2->xAxis->axisRect()->setRangeZoom(Qt::Horizontal);

        // set up the QCPColorMap:
        nx = plotGraphs[0]->Vec.count();
        ny = plotGraphs.count();
        colorMap2->data()->setSize(nx, ny); // we want the color map to have nx * ny data points
        colorMap2->data()->setRange(QCPRange(0, plotGraphs[0]->Vec.count()), QCPRange(0, plotGraphs.count())); // and span the coordinate range -4..4 in both key (x) and value (y) dimensions
        // now we assign some data, by accessing the QCPColorMapData instance of the color map:
        for (int yIndex=0; yIndex<ny; ++yIndex)
        {
            I.clear();
            for (int xIndex=0; xIndex<nx; ++xIndex) I.append(*plotGraphs[yIndex]->Vec[xIndex]->Y[1] / plotGraphs[yIndex]->NumScans);
            SavitzkyGolayFilter(Filter,I,&A);
            for (int xIndex=0; xIndex<nx; ++xIndex)
            {
                colorMap2->data()->setCell(xIndex, yIndex, A[xIndex]);
            }
        }
        // add a color scale:
        ui->HeatMap2->plotLayout()->addElement(0, 1, colorScale2); // add it to the right of the main axis rect
        colorScale2->setType(QCPAxis::atRight); // scale shall be vertical bar with tick/axis labels right (actually atRight is already the default)
        colorMap2->setColorScale(colorScale2); // associate the color map with the color scale
        colorScale2->axis()->setLabel(Label2);
        colorScale2->setRangeDrag(true);
        colorScale2->setRangeZoom(true);

        // set the color gradient of the color map to one of the presets:
        colorMap2->setGradient(QCPColorGradient::gpPolar);

        // rescale the data dimension (color) such that all data points lie in the span visualized by the color gradient:
        colorMap2->rescaleDataRange();

        // make sure the axis rect and color scale synchronize their bottom and top margins (so they line up):
        QCPMarginGroup *marginGroup2 = new QCPMarginGroup(ui->HeatMap2);
        ui->HeatMap2->axisRect()->setMarginGroup(QCP::msBottom|QCP::msTop, marginGroup2);
        colorScale2->setMarginGroup(QCP::msBottom|QCP::msTop, marginGroup2);

        step = 5;
        // Label the X axis ticks, drift time axis
        QSharedPointer<QCPAxisTickerText> textTickerX2(new QCPAxisTickerText);
        for(int i=0;i<=step;i++) textTickerX2->addTick(i * nx/5, QString::number(ui->Graph->xAxis->range().lower + i * (ui->Graph->xAxis->range().upper - ui->Graph->xAxis->range().lower)/step));
        ui->HeatMap2->xAxis->setTicker(textTickerX2);
        // Label the y axis ticks
        QSharedPointer<QCPAxisTickerText> textTickerY2(new QCPAxisTickerText);
        if(!Scan.isEmpty())
        {
            textTickerY2->addTick(0,Scan.split(",")[1]);
            textTickerY2->addTick(ny/3,QString::number(Scan.split(",")[1].toFloat() + (Scan.split(",")[2].toFloat()-Scan.split(",")[1].toFloat())/3.0));
            textTickerY2->addTick(ny*2.0/3.0,QString::number(Scan.split(",")[1].toFloat() + 2.0*(Scan.split(",")[2].toFloat()-Scan.split(",")[1].toFloat())/3.0));
            textTickerY2->addTick(ny,Scan.split(",")[2]);
            ui->HeatMap2->yAxis->setTicker(textTickerY2);
        }
        // rescale the key (x) and value (y) axes so the whole color map is visible:
        ui->HeatMap2->rescaleAxes();
        ui->HeatMap2->replot();
    }
    else
    {
        ui->HeatMap1->setVisible(false);
        ui->HeatMap2->setVisible(false);
    }
    this->resize(this->width()+1,this->height());
}

// slotTrackOption — toggles cursor-position tracking in the status bar.
void Plot::slotTrackOption(void)
{
    if(TrackOption->isChecked()) TrackOption->setChecked(true);
    else TrackOption->setChecked(false);
}

// slotClipBoard — copies the current graph as a pixmap to the system clipboard.
void Plot::slotClipBoard(void)
{
    // Set the clilpboard image
    QClipboard * clipboard = QApplication::clipboard();
    QPalette mypalette=this ->palette();
    mypalette.setColor(QPalette::Window,Qt::white);
    ui->Graph->setPalette(mypalette);
    QPixmap pixmap= ui->Graph->toPixmap();
    clipboard->setPixmap(pixmap);
}

// *************************************************************************************************
// PlotData         ********************************************************************************
// *************************************************************************************************

// PlotData — constructor. Creates an embedded Plot widget inside a plain QWidget
// frame at (x, y) with the given dimensions. Call Show() to make it visible.
PlotData::PlotData(QWidget *parent, QString name, QString Yname, QString Xname, int numGraphs, int width, int height, int x, int y) : QWidget(parent)
{
    p      = parent;
    Title  = name;
    X      = x;
    Y      = y;

    w = new QWidget(p);
    w->setGeometry(x,y,width,height);
    plot = new Plot(this,name,Yname,Xname,numGraphs);
    layout = new QVBoxLayout(w);
    layout->addWidget(plot);
}

// Show — makes the container and embedded plot visible, and clears any stale graph data.
void PlotData::Show(void)
{
    w->show();
    plot->show();
    plot->PlotCommand("Clear",true);
}

// Report — returns a multi-line CSV string prefixed with the widget title,
// replaying every stored PlotCommand so the state can be saved and restored.
QString PlotData::Report(void)
{
    QString res;
    QStringList resList;
    QString title;

    title.clear();
    if(p->objectName() != "") title = p->objectName() + ".";
    title += Title;
    plot->Comments += "\n";
    resList = plot->Comments.split("\n");
    res.clear();
    res = title + ",Clear\n";
    for(int i = 0;i<resList.count();i++) if(!resList[i].isEmpty()) res += title + "," + resList[i] + "\n";
    return res;
}

// SetValues — parses a "title,<PlotCommand>" CSV string and forwards the command
// to the embedded Plot. Returns false if the title does not match.
bool PlotData::SetValues(QString strVals)
{
    QStringList resList;
    QString title;

    title.clear();
    if(p->objectName() != "") title = p->objectName() + ".";
    title += Title;
    if(!strVals.startsWith(title)) return false;
    resList = strVals.split(",");
    if(resList[0] != title) return false;
    resList.removeFirst();
    plot->PlotCommand(resList.join(","));
    return true;
}

// ProcessCommand — scripting API handler. Strips the widget title prefix and
// passes the right-hand side of a "title=<PlotCommand>" string to PlotCommand().
// Returns "?" if the title does not match.
QString PlotData::ProcessCommand(QString cmd)
{
    QString title;

    title.clear();
    if(p->objectName() != "") title = p->objectName() + ".";
    title += Title;
    if(!cmd.startsWith(title)) return "?";
    QStringList resList = cmd.split("=");
    if(resList.count() > 1) return plot->PlotCommand(resList[1]);
    return "";
}
