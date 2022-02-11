#ifndef BUZZERTHREAD_H
#define BUZZERTHREAD_H

#include <QThread>
#include <QString>

class BuzzerThread : public QThread
{
public:
    // constructor
    // set name using initializer
    void writeToFile(QString filename, int val);
    explicit BuzzerThread(QString s);

    // overriding the QThread's run() method
    void run();
private:
    QString sequence;
};

#endif // BUZZERTHREAD_H
