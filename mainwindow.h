#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QUrl>
#include "QProgressIndicator.h"
#include "simpleserver.h"
#include "buttonthread.h"
#include <QSignalMapper>


namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    static int const EXIT_CODE_REBOOT = -123456789;

    ~MainWindow();
    void on_pushButton_clicked();
    void startRequest(QUrl url, QString command);
    void startRequest2(QUrl url);
    void startRequest3(QUrl url);
    void getOctoprintSettings();
    void disableMoveButtons();
    void enableMoveButtons();
    QString formatSeconds(int total);
    void sendFilamentStatus();


public slots:
    void onPrompShow(QString title, QStringList choices);
    void onErrorMessage(QString msg);
    //void pushButtonClicked();

signals:
    void clicked();

private slots:
    void onStatusLcd(QString msg);
    void onPromptClicked(int i);
    void onClosePrompt();
    void checkFilamentSensor();
    //void checkHomeButtonPressed();
    void onHomeButtonPressed(bool status);
    void clearCache();
    void restartProcess();
    void slotReboot();


    void showTime();
    void httpFinished();
    void httpFinished2();
    void httpFinished3();
    void httpReadyRead2();
    void httpReadyRead3();
    void actionHome();
    void actionPrinterInfo();
    void actionPrintJobInfo();
    void actionStart();
    void actionPause();
    void actionStop();
    void actionConnect();
    void actionRestart();
    void actionFirmwareRestart();
    void actionBedTemp();
    void actionExtTemp();
    void actionToggleLight();
    void actionFanSpeed();
    void actionBackFan();
    void actionFlow();
    void actionSpeed();
    void actionDisableSteppers();
    void actionEstop();
    void actionZoffsetP();
    void actionZoffsetM();
    void actionApllyZoffset();
    void actionSaveConfig();

    void actionFilamentChange();

    void actionPromptReopen();

    void actionMoveXm();
    void actionMoveXp();
    void actionMoveYm();
    void actionMoveYp();
    void actionMoveZm();
    void actionMoveZp();
    void actionMoveC();
    void actionMoveEm();
    void actionMoveEp();


    void lenSliderChanged(int);
    void bedSliderChanged(int);
    void extSliderChanged(int);
    void fanSliderChanged(int);
    void flowSliderChanged(int);
    void speedSliderChanged(int);

    void displayCalibrationDialog();
    void displayOpenFileDialog();
    void displaySystemCommandsDialog();



private:
    Ui::MainWindow *ui;
    SimpleServer *server;
    QDialog *promptDialog = 0;

    QAction* actionReboot;

    bool filamentLoaded = true;
    bool filamentInited = false;

    QUrl url;
    QNetworkAccessManager qnam;
    QNetworkAccessManager qnam2;
    QNetworkAccessManager qnam3;
    QNetworkReply *reply = 0;
    QNetworkReply *reply2 = 0;
    QNetworkReply *reply3 = 0;
    bool rfinished = true;
    bool rfinished2 = true;
    bool rfinished3 = true;
    bool firstRun = true;
    QString answer;
    QProgressIndicator *pi;
    QString baseUrl = "";
    QString apiKey = "";
    QString command = "{}";
    QString status = "";
    QSignalMapper *signalMapper;
    ButtonThread* butt;
    QThread workerThread;

    int httpGetId;
    bool httpRequestAborted;
};

#endif // MAINWINDOW_H
