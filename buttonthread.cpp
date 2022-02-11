#include "buttonthread.h"
#include <QFile>
#include <QTimer>
#include <QTextStream>

ButtonThread::ButtonThread()
{
    //QTextStream(stdout) << "Hello buttonThread0" << endl;
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(checkHomeButtonPressed()));
    timer->start(250);
}

/*void ButtonThread::run()
{
    QTextStream(stdout) << "Hello buttonThread1" << endl;

}

void ButtonThread::doWork()
{

    QTextStream(stdout) << "Hello buttonThread2" << endl;
}*/

void ButtonThread::checkHomeButtonPressed()
{
    QFile file("/sys/class/gpio/gpio172/value");

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream(stdout) << "Could not open file /sys/class/gpio/gpio256/value" << endl;
        emit signalErrorMsg("Could not open file /sys/class/gpio/gpio256/value");

        return;
    }

    QString line = QString::fromUtf8(file.readLine());
    if ( line.startsWith("0") )
    {
        if ( !homeButtonPressed || !homeButtonInited )
        {
            homeButtonPressed = true;

            if ( homeButtonInited )
            {
                emit signalButtonPressed(true);
                //emit signalErrorMsg("Button pressed");
            }
        }
    } else {
        if ( homeButtonPressed || !homeButtonInited )
        {
            homeButtonPressed = false;

            if ( homeButtonInited )
            {
                emit signalButtonPressed(false);
                //emit signalErrorMsg("Button released");
            }
        }
    }

    file.close();

    if ( !homeButtonInited )
        homeButtonInited = true;
}
