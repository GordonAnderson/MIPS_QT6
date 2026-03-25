// =============================================================================
// statuslight.h
//
// StatusLight — traffic-light style status indicator for the MIPS custom
// control panel. Extracted from controlpanel.cpp/.h during Phase 3 refactoring.
//
// Also defines LightWidget and TrafficLightWidget, which are implementation
// helpers used only by StatusLight.
//
// Depends on:  Utilities.h
// Author:      Gordon Anderson, GAA Custom Electronics, LLC
// Revised:     March 2026 — Phase 3 extraction
//
// Copyright 2026 GAA Custom Electronics, LLC. All rights reserved.
// =============================================================================
#ifndef STATUSLIGHT_H
#define STATUSLIGHT_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QPainter>

// -----------------------------------------------------------------------------
// LightWidget
//
// Paints a single circular indicator in one of three colours. When off, the
// indicator is drawn in the dark variant of the configured colour. Supports
// turnOn()/turnOff() slots and an isOn()/setOn() property.
// -----------------------------------------------------------------------------
class LightWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(bool on READ isOn WRITE setOn)
public:
    LightWidget(const QColor &color, QWidget *parent = nullptr)
        : QWidget(parent), m_color(color), m_on(false) {}

    bool isOn() const { return m_on; }
    void setOn(bool on)
    {
        if (on == m_on) return;
        m_on = on;
        update();
    }

public slots:
    void turnOff() { setOn(false); }
    void turnOn()  { setOn(true); }

protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        if (!m_on)
        {
            QColor offc;
            if (m_color == Qt::red)    offc = Qt::darkRed;
            if (m_color == Qt::yellow) offc = Qt::darkYellow;
            if (m_color == Qt::green)  offc = Qt::darkGreen;
            painter.setBrush(offc);
        }
        else
        {
            painter.setBrush(m_color);
        }
        painter.drawEllipse(0, 0, width(), height());
    }

private:
    QColor m_color;
    bool   m_on;
};

// -----------------------------------------------------------------------------
// TrafficLightWidget
//
// Stacks three LightWidgets (red, yellow, green) vertically inside a gray
// background widget.
// -----------------------------------------------------------------------------
class TrafficLightWidget : public QWidget
{
public:
    TrafficLightWidget(QWidget *parent = nullptr)
        : QWidget(parent)
    {
        QVBoxLayout *vbox = new QVBoxLayout(this);
        m_red    = new LightWidget(Qt::red);
        vbox->addWidget(m_red);
        m_yellow = new LightWidget(Qt::yellow);
        vbox->addWidget(m_yellow);
        m_green  = new LightWidget(Qt::green);
        vbox->addWidget(m_green);
        QPalette pal = palette();
        pal.setColor(QPalette::Window, Qt::gray);
        setPalette(pal);
        setAutoFillBackground(true);
    }

    LightWidget *redLight()    const { return m_red; }
    LightWidget *yellowLight() const { return m_yellow; }
    LightWidget *greenLight()  const { return m_green; }

private:
    LightWidget *m_red;
    LightWidget *m_yellow;
    LightWidget *m_green;
};

// -----------------------------------------------------------------------------
// StatusLight
//
// Displays a TrafficLightWidget with a title label and a text message below.
// The active light colour and message text are set via ProcessCommand().
// The widget can be dragged to a new position on the control panel background.
// -----------------------------------------------------------------------------
class StatusLight : public QWidget
{
    Q_OBJECT
public:
    StatusLight(QWidget *parent, QString name, int x, int y);
    void Show(void);
    void ShowMessage(void);
    QString ProcessCommand(QString cmd);

    QWidget *p;
    QString  Title;
    QString  Status;
    QString  Mode;
    int      X, Y;
    QLabel  *label;

private:
    QLabel             *TL;
    QLabel             *Message;
    TrafficLightWidget *widget;

protected:
    bool eventFilter(QObject *obj, QEvent *event);
};

#endif // STATUSLIGHT_H
