#ifndef UTILITIES_H
#define UTILITIES_H

#include <QObject>
#include <QEvent>
#include <QMouseEvent>
#include <qframe.h>
#include <QGroupBox>
#include <QPushButton>
#include <QTabWidget>
#include <QLineEdit>
#include <QApplication>
#include <QWidget>

bool moveWidget(QObject *obj, QWidget *frame, QObject *hook , QEvent *event);
bool adjustValue(QObject *obj,QLineEdit *Vsp, QEvent *event,float multiplier);

#endif // UTILITIES_H
