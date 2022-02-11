#ifndef OPENFILEWINDOW_H
#define OPENFILEWINDOW_H

#include <QDialog>
#include <QNetworkAccessManager>
#include <QUrl>
#include "QProgressIndicator.h"

namespace Ui {
class OpenFileWindow;
}

class OpenFileWindow : public QDialog
{
    Q_OBJECT

public:
    explicit OpenFileWindow(QWidget *parent = 0);
    ~OpenFileWindow();
    void startRequest(QUrl url, QString command);
    void startRequest2(QUrl url);
    void setBaseUrl(QString url);
    void setApiKey(QString key);

private slots:
    void closeDialog();
    void httpFinished();
    void httpFinished2();
    void httpReadyRead2();
    void actionLoadFiles();
    void actionStart();
    void actionSelect();

private:
    QString formatSeconds(int total);
    Ui::OpenFileWindow *ui;

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

#endif // OPENFILEWINDOW_H
