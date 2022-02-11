#ifndef DIALOGWINDOW_H
#define DIALOGWINDOW_H

#include <QDialog>
#include <QNetworkAccessManager>
#include <QUrl>
#include "QProgressIndicator.h"


namespace Ui {
    class DialogWindow;
}

class DialogWindow : public QDialog
{
    Q_OBJECT

public:
    explicit DialogWindow(QWidget *parent = 0);
    void startRequest(QUrl url, QString command);
    ~DialogWindow();
    void setBaseUrl(QString url);
    void setApiKey(QString key);

private slots:
    void closeDialog();
    void actionHome();
    void actionDeploy();
    void actionStow();
    void actionStartCalibration();
    void actionAccept();
    void actionAbort();
    void actionSaveConfig();

    void actionTestAm();
    void actionTestAp();
    void actionTestBm();
    void actionTestBp();
    void actionTestCm();
    void actionTestCp();

    void httpFinished();

private:
    Ui::DialogWindow *ui;

    QUrl url;
    QNetworkAccessManager qnam;
    //QNetworkAccessManager qnam2;
    QNetworkReply *reply = 0;
    //QNetworkReply *reply2 = 0;
    bool rfinished = true;
    QString answer;
    QProgressIndicator* pi;
    QString baseUrl = "";
    QString apiKey = "";
    QString command = "{}";
    QString status = "";
    int httpGetId;
    bool httpRequestAborted;
    bool closeAfterRequest = false;
};

#endif // DIALOGWINDOW_H
