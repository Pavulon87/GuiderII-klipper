#ifndef SIMPLESERVER_H
#define SIMPLESERVER_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include "buzzerthread.h"

class SimpleServer : public QTcpServer
{
    Q_OBJECT
public:
    SimpleServer();
    //~SimpleServer();
    void writeToFile(QString filename, int val);

signals:
    void signalErrorMsg(QString msg);
    void signalStatusLcd(QString text);
    void signalPromptShow(QString title, QStringList choices);


public slots:
    void onNewConnection();
    void onSocketStateChanged(QAbstractSocket::SocketState socketState);
    void clientDisconnected();
    void onReadyRead();
    void buzzer(QString seq);
    void displayLcd(QString text);
    void setupPrompt(QString text);

private:
    QTcpServer _server;
    QList<QTcpSocket*>  _sockets;
    BuzzerThread* buzzt;

    QString prompt_title = "";
    QStringList prompt_choices = {};
};

#endif // SIMPLESERVER_H
