#include "dialogwindow.h"
#include "ui_dialogwindow.h"
#include <QtNetwork>
#include "QProgressIndicator.h"
#include <QErrorMessage>

DialogWindow::DialogWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogWindow)
{
    ui->setupUi(this);
    QTextStream(stdout) << "Hello calibration dialog!" << endl;

    //baseUrl = MainWindow->apiKey();
    //apiKey = MainWindow::returnApiKey();

    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(actionHome()));
    connect(ui->pushButton_2, SIGNAL(clicked()), this, SLOT(actionStartCalibration()));
    connect(ui->pushButton_3, SIGNAL(clicked()), this, SLOT(actionTestAm()));
    connect(ui->pushButton_4, SIGNAL(clicked()), this, SLOT(actionTestBm()));
    connect(ui->pushButton_5, SIGNAL(clicked()), this, SLOT(actionTestCm()));
    connect(ui->pushButton_6, SIGNAL(clicked()), this, SLOT(actionTestAp()));
    connect(ui->pushButton_7, SIGNAL(clicked()), this, SLOT(actionTestBp()));
    connect(ui->pushButton_8, SIGNAL(clicked()), this, SLOT(actionTestCp()));
    connect(ui->pushButton_9, SIGNAL(clicked()), this, SLOT(actionAccept()));
    connect(ui->pushButton_10, SIGNAL(clicked()), this, SLOT(actionAbort()));
    connect(ui->pushButton_11, SIGNAL(clicked()), this, SLOT(actionSaveConfig()));
    connect(ui->pushButton_12, SIGNAL(clicked()), this, SLOT(actionDeploy()));
    connect(ui->pushButton_13, SIGNAL(clicked()), this, SLOT(actionStow()));
    connect(ui->buttonBox, SIGNAL(close()), this, SLOT(closeDialog()));
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(closeDialog()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(closeDialog()));

    pi = new QProgressIndicator();
    ui->gridLayout->addWidget(pi);
}

DialogWindow::~DialogWindow()
{
    delete ui;
}

void DialogWindow::closeDialog()
{
    QTextStream(stdout) << "Close" << endl;
    close();
}

void DialogWindow::setBaseUrl(QString url)
{
    baseUrl = url;
}

void DialogWindow::setApiKey(QString key)
{
    apiKey = key;
}

void DialogWindow::actionHome()
{
    url = baseUrl + "/api/printer/printhead";
    command = "{ \"command\": \"home\", \"axes\": [\"x\", \"y\", \"z\"] }";

    startRequest(url, command);
}

void DialogWindow::actionDeploy()
{
    url = baseUrl + "/api/printer/command";
    command = "{ \"command\": \"DEPLOY\" }";

    startRequest(url, command);
}


void DialogWindow::actionStow()
{
    url = baseUrl + "/api/printer/command";
    command = "{ \"command\": \"STOW\" }";

    startRequest(url, command);
}


void DialogWindow::actionAccept()
{
    url = baseUrl + "/api/printer/command";
    command = "{ \"command\": \"ACCEPT\" }";

    startRequest(url, command);
}


void DialogWindow::actionAbort()
{
    url = baseUrl + "/api/printer/command";
    command = "{ \"command\": \"ABORT\" }";

    startRequest(url, command);
}


void DialogWindow::actionSaveConfig()
{
    url = baseUrl + "/api/printer/command";
    command = "{ \"command\": \"SAVE_CONFIG\" }";

    closeAfterRequest = true;

    startRequest(url, command);
}


void DialogWindow::actionStartCalibration()
{
    url = baseUrl + "/api/printer/command";
    command = "{ \"command\": \"PROBE_CALIBRATE\" }";

    startRequest(url, command);
}

void DialogWindow::actionTestAm()
{
    url = baseUrl + "/api/printer/command";
    command = "{ \"command\": \"TESTZ Z=-1\" }";

    startRequest(url, command);
}


void DialogWindow::actionTestBm()
{
    url = baseUrl + "/api/printer/command";
    command = "{ \"command\": \"TESTZ Z=-0.1\" }";

    startRequest(url, command);
}


void DialogWindow::actionTestCm()
{
    url = baseUrl + "/api/printer/command";
    command = "{ \"command\": \"TESTZ Z=-0.01\" }";

    startRequest(url, command);
}

void DialogWindow::actionTestAp()
{
    url = baseUrl + "/api/printer/command";
    command = "{ \"command\": \"TESTZ Z=1\" }";

    startRequest(url, command);
}


void DialogWindow::actionTestBp()
{
    url = baseUrl + "/api/printer/command";
    command = "{ \"command\": \"TESTZ Z=0.1\" }";

    startRequest(url, command);
}


void DialogWindow::actionTestCp()
{
    url = baseUrl + "/api/printer/command";
    command = "{ \"command\": \"TESTZ Z=0.01\" }";

    startRequest(url, command);
}

void DialogWindow::startRequest(QUrl url, QString command)
{
    if ( !rfinished )
    {
        return;
    }

    QNetworkRequest request(url);
    request.setRawHeader(QByteArray("X-Api-Key"), apiKey.toUtf8());
    request.setRawHeader(QByteArray("Content-Type"), QByteArray("application/json"));

    QTextStream(stdout) << "Post: \""  << url.toString() << "\" \"" << command << "\"" << endl;

    reply = qnam.post(request, command.toUtf8());
    rfinished = false;

    connect(reply, SIGNAL(finished()), this, SLOT(httpFinished()));

    pi->startAnimation();
}

void DialogWindow::httpFinished()
{
    if (reply == NULL || reply->error()) {
        //pi->stopAnimation();
        //QTextStream(stdout) << "Download failed " << reply->errorString() << endl;
    } else {
        if ( reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() != 204 )
        {
            QTextStream(stdout) << "Download finished " << reply->readAll() << endl;
        }
    }

    pi->stopAnimation();
    reply->deleteLater();
    rfinished = true;

    if ( closeAfterRequest )
    {
        QTextStream(stdout) << "Close" << endl;
        close();
    }
}
