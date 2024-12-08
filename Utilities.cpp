#include "Utilities.h"

bool moveWidget(QObject *obj, QWidget *frame, QObject *hook , QEvent *event)
{
    if((obj == hook) && (event->type() == QEvent::MouseButtonPress))
    {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::RightButton)
        {
            if(mouseEvent->position().rx() > 25) return false;
            if(mouseEvent->position().ry() > 25) return false;
            frame->raise();
            if(obj->property("moving").toBool())obj->setProperty("moving", false);
            else obj->setProperty("moving", true);
            return true;
        }
    }
    if((obj == hook) && (event->type() == QEvent::MouseMove))
    {
        if(obj->property("moving").toBool())
        {
            frame->raise();
            QMouseEvent *mouse = static_cast<QMouseEvent *>(event);
            frame->setGeometry(frame->pos().x() + mouse->pos().rx() - 10, frame->pos().y() + mouse->pos().ry() - 10, frame->width(),frame->height());
            return true;
        }
    }
    return false;
}

bool adjustValue(QObject *obj,QLineEdit *Vsp, QEvent *event,float multiplier)
{
    float delta = 0;

    if (((obj == Vsp) && (event->type() == QEvent::KeyPress)) || ((obj == Vsp) && (event->type() == QEvent::Wheel)))
    {
        if(event->type() == QEvent::KeyPress)
        {
            QKeyEvent *key = static_cast<QKeyEvent *>(event);
            if(key->key() == 16777235) delta = 0.1;
            if(key->key() == 16777237) delta = -0.1;
            if((QApplication::queryKeyboardModifiers() & 0xA000000) != 0) delta *= 0.1;
            if((QApplication::queryKeyboardModifiers() & 0x2000000) != 0) delta *= 10;
            if((QApplication::queryKeyboardModifiers() & 0x8000000) != 0) delta *= 100;
        }
        else if(event->type() == QEvent::Wheel)
        {
            QWheelEvent *wheel = static_cast<QWheelEvent *>(event);
            delta = (float)wheel->angleDelta().ry()/10.0;
        }
        if(delta != 0)
        {
            QString myString;
            if(abs(multiplier) <= 0.01) myString = myString.asprintf("%3.3f", Vsp->text().toFloat() + delta * multiplier);
            //else if(abs(multiplier) <= 0.1) myString = myString.asprintf("%3.2f", Vsp->text().toFloat() + delta * multiplier);
            //else if(abs(multiplier) <= 1) myString = myString.asprintf("%3.1f", Vsp->text().toFloat() + delta * multiplier);
            else myString = myString.asprintf("%3.2f", Vsp->text().toFloat() + delta * multiplier);
            Vsp->setText(myString);
            Vsp->setModified(true);
            emit Vsp->editingFinished();
            return true;
        }
    }
    return false;
}

