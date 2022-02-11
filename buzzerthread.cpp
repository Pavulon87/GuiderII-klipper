#include "buzzerthread.h"
#include <QDebug>
#include <QFile>
#include <QStringList>

BuzzerThread::BuzzerThread(QString s) : sequence(s)
{
}

// We overrides the QThread's run() method here
// run() will be called when a thread starts
// the code will be shared by all threads

void BuzzerThread::run()
{
    //M300 S660 P300;\n M300 S880 P150;\n M300 S660 P300
    //QString seq = this->sequence;

    QStringList list = sequence.split(";", QString::SkipEmptyParts);

    //QStringList lines = input.split('\n');

    foreach (const QString &seq, list) {

        //QStringList tokens = seq.split(',', QString::SkipEmptyParts);

        QString S;
        QString P;

        QRegExp srx("S([0-9]+)");
        if ( srx.indexIn(seq) != -1 ) {
            S = srx.cap(1);
        }

        QRegExp prx("P([0-9]+)");
        if ( prx.indexIn(seq) != -1 ) {
            P = prx.cap(1);
        }

        int period = ( S.length() > 0 ? S.toInt() : 350 ) * 1000;
        int duty = period / 2;
        int duration = ( P.length() > 0 ? S.toInt() : 250 );

        //m_data.insert(tokens.at(0), tokens.at(1));

        writeToFile("/sys/class/pwm/pwmchip0/pwm0/period", period);
        writeToFile("/sys/class/pwm/pwmchip0/pwm0/duty_cycle",  duty);
        writeToFile("/sys/class/pwm/pwmchip0/pwm0/enable", 1);
        QThread::msleep(duration);
        writeToFile("/sys/class/pwm/pwmchip0/pwm0/enable", 0);
    }
}

void BuzzerThread::writeToFile(QString filename, int val)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly))
    {
        return;
    }

    file.write(QString::number(val).toUtf8());

    file.close();
}
