#ifndef SYSTEMCOMMANDSWINDOW_H
#define SYSTEMCOMMANDSWINDOW_H

#include <QDialog>
#include <QNetworkAccessManager>
#include <QUrl>
#include "QProgressIndicator.h"

namespace Ui {
class SystemCommandsWindow;
}

class SystemCommandsWindow : public QDialog
{
    Q_OBJECT

public:
    explicit SystemCommandsWindow(QWidget *parent = 0);
    ~SystemCommandsWindow();

    void startRequest(QUrl url, QString command);
    void startRequest2(QUrl url);
    void setBaseUrl(QString url);
    void setApiKey(QString key);

private slots:
    void httpFinished();
    void httpFinished2();
    void httpReadyRead2();
    void actionLoadCommands();
    void actionRun();
    void closeDialog();

private:
    Ui::SystemCommandsWindow *ui;

    QUrl url;
    QNetworkAccessManager qnam;
    QNetworkAccessManager qnam2;
    QNetworkReply *reply = 0;
    QNetworkReply *reply2 = 0;
    bool rfinished = true;
    bool rfinished2 = true;

    QString answer;
    QProgressIndicator* pi;
    QString baseUrl = "";
    QString apiKey = "";
    QString command = "{}";
    QString status = "";
    QString fileListInfo = "";
    int httpGetId;
    bool httpRequestAborted;
    bool closeAfterRequest = false;
};

#endif // SYSTEMCOMMANDSWINDOW_H
