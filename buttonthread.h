#ifndef BUTTONTHREAD_H
#define BUTTONTHREAD_H

#include <QThread>

class ButtonThread : public QThread
{
    Q_OBJECT
    QThread workerThread;

public:
    ButtonThread();
    //void doWork();
    // overriding the QThread's run() method
    //void run();

signals:
    void signalErrorMsg(QString msg);
    void signalButtonPressed(bool status);

private slots:
    void checkHomeButtonPressed();

private:
    bool homeButtonPressed = false;
    bool homeButtonInited = false;

};

#endif // BUTTONTHREAD_H
