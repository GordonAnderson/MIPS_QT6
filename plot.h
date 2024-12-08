#ifndef PLOT_H
#define PLOT_H

#include <QDialog>
#include <QStatusBar>
#include <QMenu>
#include <QDebug>
#include <QKeyEvent>
#include "qcustomplot.h"

namespace Ui {
class Plot;
}

typedef struct
{
   int    np;
   float  h;
   QList<float> an;
} SGcoeff;

// This structure contains one set of data points for a graph
typedef struct
{
    int point;
    float X;
    QList<float *> Y;
} DataPoint;

// This structure contains one set of graphs
typedef struct
{
    int NumScans;           // Number of coadds, only Y values are coadded
    QList<DataPoint *> Vec;
} PlotGraph;

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
    QAction *FilterOption;
    QAction *TrackOption;
    QAction *ClipboardOption;
    QAction *CommentOption;
    QAction *HeatOption;

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

protected:
    bool eventFilter(QObject *obj, QEvent *event);
};

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
//private:
    Plot *plot;
private slots:
protected:
    //bool eventFilter(QObject *obj, QEvent *event);
};

#endif // PLOT_H
