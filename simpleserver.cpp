#include "simpleserver.h"
#include <mainwindow.h>
#include "buzzerthread.h"

#include <QDebug>
#include <QHostAddress>
#include <QAbstractSocket>
#include <QErrorMessage>
#include <QFile>
#include <QThread>



SimpleServer::SimpleServer()
{
    if ( !_server.listen(QHostAddress::Any, 4422) ) {

        QTextStream(stdout) << "Unable to start the server: \"" << _server.errorString() << endl;
        emit signalErrorMsg( tr("Unable to start the server: %1.").arg(_server.errorString()) );

        close();
        return;
    }

    connect(&_server, SIGNAL(newConnection()), this, SLOT(onNewConnection()));
}

void SimpleServer::onNewConnection()
{
    QTcpSocket *clientSocket = _server.nextPendingConnection();
    //QTextStream(stdout) << clientSocket->peerAddress().toString() << " connected to server!" << endl;

    connect(clientSocket, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
    connect(clientSocket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(onSocketStateChanged(QAbstractSocket::SocketState)));

    connect(clientSocket, SIGNAL(disconnected()), this, SLOT(clientDisconnected() ));


    clientSocket->write("HELLO "+clientSocket->peerAddress().toString().toUtf8());
    clientSocket->flush();

    _sockets.push_back(clientSocket);
    /*for (QTcpSocket* socket : _sockets) {
    socket->write(clientSocket->peerAddress().toString().toUtf8() + "\n");
    QTextStream(stdout) << socket->peerAddress().toString() << " connected to server!" << endl;
    }*/
}

void SimpleServer::clientDisconnected()
{
    QTcpSocket* sender = static_cast<QTcpSocket*>(QObject::sender());
    //QTextStream(stdout) << sender->peerAddress().toString() << " disconnectedA!" << endl;
    _sockets.removeOne(sender);
    sender->deleteLater();
}

void SimpleServer::onSocketStateChanged(QAbstractSocket::SocketState socketState)
{
    if (socketState == QAbstractSocket::UnconnectedState)
    {
        QTcpSocket* sender = static_cast<QTcpSocket*>(QObject::sender());
        //QTextStream(stdout) << sender->peerAddress().toString() << " disconnectedB!" << endl;
        _sockets.removeOne(sender);
        sender->deleteLater();
    }
}

void SimpleServer::onReadyRead()
{
    QTcpSocket* sender = static_cast<QTcpSocket*>(QObject::sender());
    QString data = QString::fromUtf8(sender->readAll()).trimmed();
    QTextStream(stdout) << "Received: \"" << data << "\"" << endl;

    QStringList lines = data.split('\n');
    //lines[0] = lines[0].toLower();

    //QTextStream(stdout) << "Lines[0]: \"" << lines[0] << "\"" << endl;

    foreach (const QString &seq, lines)
    {
        if ( seq.startsWith("m300", Qt::CaseInsensitive) || seq.startsWith("buzzer", Qt::CaseInsensitive))
        {
            QTextStream(stdout) << "Launching buzzer!" << endl;
            buzzer(seq);
        } else if ( seq.startsWith("quit", Qt::CaseInsensitive) )
        {
            sender->close();
            sender->deleteLater();
        } else if ( seq.startsWith("m117", Qt::CaseInsensitive) || seq.startsWith("m118", Qt::CaseInsensitive) || seq.startsWith("status", Qt::CaseInsensitive) )
        {
            displayLcd(seq);
        } else if ( seq.startsWith("error", Qt::CaseInsensitive) )
        {
            displayLcd(seq);
        } else if ( seq.startsWith("prompt", Qt::CaseInsensitive) )
        {
            setupPrompt(seq);
        } else if ( seq.startsWith("gcode", Qt::CaseInsensitive) )
        {
            gcodePosition(seq);
        }
    }
}

void SimpleServer::gcodePosition(QString text)
{
    if ( text.indexOf("base") == -1 )
    {
        return;
    }

    text.remove(0, text.indexOf(QString::fromUtf8(" "))+1);
    text.remove(0, text.indexOf(QString::fromUtf8("Z:"))+2);
    text = text.left(text.indexOf(QString::fromUtf8(" ")));
    //int tmp = text.indexOf(QString::fromUtf8(" "));
    //text.remove(tmp, text.length()-tmp);

    emit signalZoffset(text.toDouble());
}

void SimpleServer::buzzer(QString seq)
{
    //buzzt = new BuzzerThread("660,300;880,150;660,300");

    seq.remove(0, seq.indexOf(QString::fromUtf8(" "))+1);

    buzzt = new BuzzerThread(seq);
    buzzt->start();
    return;
}

void SimpleServer::displayLcd(QString text)
{
    text.remove(0, text.indexOf(QString::fromUtf8(" "))+1);
    emit signalStatusLcd( text );
}

void SimpleServer::setupPrompt(QString text)
{
    if (text.startsWith("prompt_begin"))
    {
        text.remove(0, text.indexOf(QString::fromUtf8(" "))+1);
        prompt_title = text;
        prompt_choices.clear();
        //clear previous choices
    } else if (text.startsWith("prompt_choice"))
    {
        text.remove(0, text.indexOf(QString::fromUtf8(" "))+1);
        prompt_choices.append(text);
        //add choice
    } else if (text.startsWith("prompt_show"))
    {
        //show prompt
        emit signalPromptShow(prompt_title, prompt_choices);
    }

    //emit signalStatusLcd( text );
}
