// =============================================================================
// plot.h
//
// Class declarations for Plot and PlotData, plus the PlotGraph / DataPoint /
// SGcoeff data structures and the free-standing binary/CSV helper functions.
// See plot.cpp for full documentation.
//
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Created:     2021
// Revised:     March 2026 — documented for host app v2.22
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================
#ifndef PLOT_H
#define PLOT_H

#include <QDialog>
#include <QStatusBar>
#include <QMenu>
#include <QDebug>
#include <QKeyEvent>
#include <QList>
#include "qcustomplot.h"

namespace Ui {
class Plot;
}

// Savitzky-Golay filter coefficient table entry
typedef struct
{
    int          np;  // number of points in the filter window
    float        h;   // normalisation divisor
    QList<float> an;  // convolution coefficients
} SGcoeff;

// One data point in a scan: X axis value and one Y value per graph channel
typedef struct
{
    int           point;  // sequential point index
    float         X;      // X axis value
    QList<float*> Y;      // Y values (heap-allocated); one entry per graph channel
} DataPoint;

// One complete scan: a vector of DataPoints accumulated over NumScans coadds
typedef struct
{
    int                NumScans;  // coadd count; Y values are divided by this when painted
    QList<DataPoint*>  Vec;
} PlotGraph;

// -----------------------------------------------------------------------------
// Plot — QDialog-based plot window driven by PlotCommand() string API.
// Supports 1 or 2 line graphs, optional dual heatmap views, Savitzky-Golay
// smoothing, zoom history undo, binary/CSV save/load, and clipboard capture.
// All commands processed by PlotCommand() are appended to the Comments block
// so a saved .plot file can fully recreate the session on Load().
// -----------------------------------------------------------------------------
class Plot : public QDialog
{
    Q_OBJECT
signals:
    void DialogClosed(Plot *plot);
public:
    explicit Plot(QWidget *parent, QString Title, QString Yaxis, QString Xaxis, int NumPlots);
    ~Plot();
    QString PlotCommand(QString cmd, bool PlotOnly = false);
    void PaintGraphs(PlotGraph *pg);
    void Save(QString filename);
    void Load(QString filename);
    void FreeAllData(void);
    void ZoomSelect(void);
    void SavitzkyGolayFilter(int order, QList<float> Y, QList<float> *Yf);
    void addAverageGraph(void);
    QCPColorMap *colorMap1,*colorMap2;
    QCPColorScale *colorScale1,*colorScale2;
    QString     PlotTitle;
    QString     Comments;
    QString     Label1;
    QString     Label2;
    QString     Scan;
    bool        plotOnly = false;
    QString     fileName = "";
    QStatusBar  *statusBar;

private:
    QWidget *p;
    QCPTextElement *plotFile = NULL;
    Ui::Plot *ui;
    int  CurrentIndex;
    int  Filter;
    float m,b;
    QList<PlotGraph *> plotGraphs;    // All graphs
    QMenu   *popupMenu;
    QAction *SaveOption;
    QAction *ExportOption;
    QAction *LoadOption;
    QAction *XaxisZoomOption;
    QAction *YaxisZoomOption;
    QAction *ZoomOutFullOption;
    QAction *ZoomOutOneLevelOption;
    QAction *FilterOption;
    QAction *TrackOption;
    QAction *ClipboardOption;
    QAction *CommentOption;
    QAction *HeatOption;
    QStack<QCPRange> m_xAxisRangeHistory;
    QStack<QCPRange> m_yAxisRangeHistory;
    bool m_isUndoingZoom = false; // Flag to prevent infinite loop when setting range from history

protected:
    void resizeEvent(QResizeEvent *event); // override;
    void closeEvent (QCloseEvent *event);

public slots:
    void mousePressed(QMouseEvent*);
    void mousePressedHM(QMouseEvent*);
    void slotSaveMenu(void);
    void slotExportMenu(void);
    void slotLoadMenu(void);
    void slotCommentMenu(void);
    void slotCloseComments(void);
    void slotXaxisZoomOption(void);
    void slotYaxisZoomOption(void);
    void slotFilterOption(void);
    void slotTrackOption(void);
    void slotClipBoard(void);
    void slotHeatMap(void);
    void mouseMove(QMouseEvent*event);
    void slotSaveRangeHistory(QCPRange newRange);
    void slotZoomOut(void);
    void slotZoomOutOneLevel(void);
    void slotRescaleYToDataInRange(QCPRange newXRange);

protected:
    bool eventFilter(QObject *obj, QEvent *event);
};

// -----------------------------------------------------------------------------
// PlotData — draggable embedded widget that wraps a Plot dialog inside a
// QVBoxLayout. Used by the scripting system to embed plots in control panels.
// ProcessCommand() and SetValues() forward commands to Plot::PlotCommand().
// -----------------------------------------------------------------------------
class PlotData : public QWidget
{
    Q_OBJECT
public:
    PlotData(QWidget *parent, QString name, QString Yname, QString Xname, int numGraphs, int width, int height, int x, int y);
    void    Show(void);
    //void    Update(QString sVals = "");
    QString Report(void);
    bool    SetValues(QString strVals);
    QString ProcessCommand(QString cmd);
    QWidget               *p;
    QString               Title;
    int                   X,Y;
    QWidget               *w;
    QVBoxLayout *layout;
private:
    Plot *plot;
private slots:
protected:
           //bool eventFilter(QObject *obj, QEvent *event);
};

// Prototypes
void clearPlotGraphs(QList<PlotGraph *>& graphs);
void clearPlotGraph(PlotGraph *graph);
void clearDataPoint(DataPoint *dataPoint);

#endif // PLOT_H
