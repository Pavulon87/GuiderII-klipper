#include "mainwindow.h"
#include "dialogwindow.h"
#include "openfilewindow.h"
#include "systemcommandswindow.h"
#include "simpleserver.h"
#include "buttonthread.h"

#include "ui_mainwindow.h"
#include "ui_dialogwindow.h"
#include "ui_openfilewindow.h"
#include <QTextStream>
#include <QtNetwork>
#include <QFile>
#include <QErrorMessage>
#include <QMessageBox>
#include <QDialog>
#include <QSignalMapper>
#include <QProcess>

#include "QProgressIndicator.h"
#include "qjson4/QJsonObject.h"
#include "qjson4/QJsonDocument.h"
#include "qjson4/QJsonArray.h"
#include "qjson4/QJsonParseError.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QTextStream(stdout) << "Hello World!" << endl;

    actionReboot = new QAction( this );
    actionReboot->setText( tr("Restart") );
    actionReboot->setStatusTip( tr("Restarts the application") );
    connect( actionReboot, SIGNAL (triggered()),this, SLOT (slotReboot()));

    connect(ui->pushButton_29, SIGNAL(clicked()), this, SLOT(restartProcess()));


    getOctoprintSettings();

    disableMoveButtons();

    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(actionHome()));
    connect(ui->pushButton_2, SIGNAL(clicked()), this, SLOT(actionStart()));
    connect(ui->pushButton_3, SIGNAL(clicked()), this, SLOT(actionPause()));
    connect(ui->pushButton_4, SIGNAL(clicked()), this, SLOT(actionStop()));
    connect(ui->pushButton_5, SIGNAL(clicked()), this, SLOT(actionConnect()));
    connect(ui->pushButton_6, SIGNAL(clicked()), this, SLOT(actionRestart()));
    connect(ui->pushButton_7, SIGNAL(clicked()), this, SLOT(actionFirmwareRestart()));

    connect(ui->pushButton_8, SIGNAL(clicked()), this, SLOT(actionMoveXm()));
    connect(ui->pushButton_9, SIGNAL(clicked()), this, SLOT(actionMoveXp()));

    connect(ui->pushButton_10, SIGNAL(clicked()), this, SLOT(actionMoveYm()));
    connect(ui->pushButton_11, SIGNAL(clicked()), this, SLOT(actionMoveYp()));

    connect(ui->pushButton_12, SIGNAL(clicked()), this, SLOT(actionMoveZm()));
    connect(ui->pushButton_13, SIGNAL(clicked()), this, SLOT(actionMoveZp()));

    connect(ui->pushButton_16, SIGNAL(clicked()), this, SLOT(actionMoveEp()));
    connect(ui->pushButton_17, SIGNAL(clicked()), this, SLOT(actionMoveEm()));

    connect(ui->pushButton_14, SIGNAL(clicked()), this, SLOT(actionBedTemp()));
    connect(ui->pushButton_15, SIGNAL(clicked()), this, SLOT(actionExtTemp()));

    connect(ui->pushButton_22, SIGNAL(clicked()), this, SLOT(actionMoveC()));

    connect(ui->pushButton_18, SIGNAL(clicked()), this, SLOT(actionToggleLight()));
    connect(ui->pushButton_23, SIGNAL(clicked()), this, SLOT(actionDisableSteppers()));
    connect(ui->pushButton_24, SIGNAL(clicked()), this, SLOT(actionEstop()));


    connect(ui->pushButton_19, SIGNAL(clicked()), this, SLOT(actionFanSpeed()));
    connect(ui->pushButton_20, SIGNAL(clicked()), this, SLOT(actionSpeed()));
    connect(ui->pushButton_21, SIGNAL(clicked()), this, SLOT(actionFlow()));
    connect(ui->pushButton_32, SIGNAL(clicked()), this, SLOT(actionBackFan()));

    connect(ui->pushButton_25, SIGNAL(clicked()), this, SLOT(actionZoffsetM()));
    connect(ui->pushButton_26, SIGNAL(clicked()), this, SLOT(actionZoffsetP()));
    connect(ui->pushButton_27, SIGNAL(clicked()), this, SLOT(actionFilamentChange()));
    connect(ui->pushButton_33, SIGNAL(clicked()), this, SLOT(actionApllyZoffset()));
    connect(ui->pushButton_34, SIGNAL(clicked()), this, SLOT(actionSaveConfig()));

    connect(ui->pushButton_28, SIGNAL(clicked()), this, SLOT(actionPromptReopen()));

    connect(ui->pushButton_31, SIGNAL(clicked()), this, SLOT(displayCalibrationDialog()));
    connect(ui->pushButton_35, SIGNAL(clicked()), this, SLOT(displayOpenFileDialog()));
    connect(ui->pushButton_36, SIGNAL(clicked()), this, SLOT(displaySystemCommandsDialog()));

    connect(ui->dial, SIGNAL(valueChanged(int)), this, SLOT(lenSliderChanged(int)));
    lenSliderChanged(ui->dial->value());

    connect(ui->bedSlider, SIGNAL(valueChanged(int)), this, SLOT(bedSliderChanged(int)));
    bedSliderChanged(ui->bedSlider->value());

    connect(ui->extSlider, SIGNAL(valueChanged(int)), this, SLOT(extSliderChanged(int)));
    extSliderChanged(ui->extSlider->value());

    connect(ui->fanSlider, SIGNAL(valueChanged(int)), this, SLOT(fanSliderChanged(int)));
    fanSliderChanged(ui->fanSlider->value());

    connect(ui->speedSlider, SIGNAL(valueChanged(int)), this, SLOT(speedSliderChanged(int)));
    speedSliderChanged(ui->speedSlider->value());

    connect(ui->flowSlider, SIGNAL(valueChanged(int)), this, SLOT(flowSliderChanged(int)));
    flowSliderChanged(ui->flowSlider->value());

    pi = new QProgressIndicator();
    ui->gridLayout->addWidget(pi);

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(showTime()));
    timer->start(1000);

    QTimer *timer2 = new QTimer(this);
    connect(timer2, SIGNAL(timeout()), this, SLOT(actionPrinterInfo()));
    timer2->start(5000);

    QTimer *timer3 = new QTimer(this);
    connect(timer3, SIGNAL(timeout()), this, SLOT(actionPrintJobInfo()));
    timer3->start(5000);

    QTimer *timer4 = new QTimer(this);
    connect(timer4, SIGNAL(timeout()), this, SLOT(checkFilamentSensor()));
    timer4->start(5000);


    QTimer *timer5 = new QTimer(this);
    connect(timer5, SIGNAL(timeout()), this, SLOT(clearCache()));
    timer5->start(30*60*1000);
    clearCache();

    butt = new ButtonThread;
    butt->moveToThread(&workerThread);
    connect(&workerThread, SIGNAL(finished()), butt, SLOT(deleteLater()));

    //connect(this, SIGNAL(operate(QString)), butt, SLOT(doWork(QString)));
    connect(butt, SIGNAL(signalErrorMsg(QString)), this, SLOT(onErrorMessage(QString)));
    connect(butt, SIGNAL(signalButtonPressed(bool)), this, SLOT(onHomeButtonPressed(bool)));

    workerThread.start();

    //butt = new ButtonThread();
    //butt->exec();
    //butt->run();
    //butt->moveToThread(this);

    //connect(butt, SIGNAL(signalErrorMsg(QString)), this, SLOT(onErrorMessage(QString)));
    //connect(butt, SIGNAL(signalButtonPressed(bool)), this, SLOT(onHomeButtonPressed(bool)));



    actionPrinterInfo();
    actionPrintJobInfo();

    server = new SimpleServer();

    connect(server, SIGNAL(signalErrorMsg(QString)), this, SLOT(onErrorMessage(QString)));
    connect(server, SIGNAL(signalStatusLcd(QString)), this, SLOT(onStatusLcd(QString)));
    connect(server, SIGNAL(signalPromptShow(QString,QStringList)), this, SLOT(onPrompShow(QString,QStringList)));
    connect(server, SIGNAL(signalZoffset(double)), this, SLOT(setZoffset(double)));

    QTimer::singleShot(5000, this, SLOT(getHomePosition()));

    /*QTimer *timerX = new QTimer(this);
    connect(timerX, SIGNAL(timeout()), this, SLOT(displaySystemCommandsDialog()));
    timerX->setSingleShot(true);
    timerX->start(5000);*/

    //displayOpenFileDialog();
}

void MainWindow::clearCache()
{
    QProcess::startDetached("/bin/sync");

    QFile file("/proc/sys/vm/drop_caches");

    if ( !file.open(QIODevice::WriteOnly) )
    {
        return;
    }

    file.write("1");

    file.close();
}

void MainWindow::onHomeButtonPressed(bool status)
{
    if ( status )
    {
        QTextStream(stdout) << "Button pressed" << endl;
        url = baseUrl + "/api/printer/command";
        command = "{ \"command\": \"M112\" }";
        startRequest(url, command);
    } else {
        QTextStream(stdout) << "Button released" << endl;
    }
}

void MainWindow::slotReboot()
{
    qDebug() << "Performing application reboot...";
    qApp->exit( MainWindow::EXIT_CODE_REBOOT );
}

void MainWindow::restartProcess()
{
    //qApp->exit(1337);

    reply->close();
    reply2->close();
    reply3->close();
    qApp->exit( 1337 );
    actionReboot->trigger();
}

/*void MainWindow::checkHomeButtonPressed()
{
    QFile file("/sys/class/gpio/gpio172/value");
    bool currentStatus = homeButtonPressed;

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream(stdout) << "Could not open file /sys/class/gpio/gpio256/value" << endl;
        onErrorMessage("Could not open file /sys/class/gpio/gpio256/value");

        return;
    }

    //QTextStream in(&file);
    QByteArray line = file.readLine();
    if ( line == "0" )
    {
        if ( !currentStatus )
        {
            homeButtonPressed = true;
            QTextStream(stdout) << "Button pressed" << endl;
        }
    } else {
        if ( currentStatus )
        {
            homeButtonPressed = false;
            QTextStream(stdout) << "Button released" << endl;
        }
    }

    file.close();
}*/

void MainWindow::setZoffset(double offset)
{
    zoffset = offset;
    ui->label_20->setText(QString::number(zoffset, 'f', 2));
}

void MainWindow::checkFilamentSensor()
{
    QFile file("/sys/class/gpio/gpio256/value");

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream(stdout) << "Could not open file /sys/class/gpio/gpio256/value" << endl;
        onErrorMessage("Could not open file /sys/class/gpio/gpio256/value");

        return;
    }


    QString line = QString::fromUtf8(file.readLine());

    if ( line.startsWith("1") )
    {
        if ( !filamentLoaded || !filamentInited )
        {
            filamentLoaded = true;
            ui->radioButton->setChecked(true);

            if ( filamentInited )
            {
                QTextStream(stdout) << "Filament loaded" << endl;
                sendFilamentStatus();
            }
        }
    } else {
        if ( filamentLoaded || !filamentInited )
        {
            filamentLoaded = false;
            ui->radioButton->setChecked(false);

            if ( filamentInited )
            {
                QTextStream(stdout) << "Filament unloaded" << endl;
                sendFilamentStatus();
            }
        }
    }

    file.close();

    if ( ! filamentInited )
        filamentInited = true;
}

void MainWindow::sendFilamentStatus()
{
    if ( status != "Printing" )
    {
        return;
    }

    url = baseUrl + "/api/printer/command";
    if ( !filamentLoaded )
    {
        command = "{ \"command\": \"M118 action:runout_detected\" }";
    } else {
        command = "{ \"command\": \"M118 action:filament_loaded\" }";
    }

    startRequest(url, command);
}


void MainWindow::onErrorMessage(QString msg)
{
    QErrorMessage *emessage = new QErrorMessage(this);
    emessage->showMessage( msg );
    emessage->deleteLater();
}

void MainWindow::onStatusLcd(QString msg)
{
    ui->statusBar->showMessage("     "+msg);
}


void MainWindow::onPrompShow(QString title, QStringList choices)
{
    QTextStream(stdout) << "New Prompt: " << title << endl;

    /*int it = 0;
    foreach (const QString &choice, choices)
    {
        QTextStream(stdout) << tr("Choice[%1]: %2").arg(QString::number(it), choice) << endl;
        it++;
    }*/

    //QDialog *promptDialog = new QDialog();
    if ( promptDialog )
    {
        promptDialog->close();
        promptDialog->deleteLater();
    }
    promptDialog = new QDialog();
    Qt::WindowFlags flags(Qt::WindowTitleHint);

    promptDialog->setWindowFlags(flags);

    promptDialog->setGeometry(QRect(200, 100, 400, 300));
    promptDialog->setWindowTitle("Message from printer");

    QVBoxLayout *vboxlayout = new QVBoxLayout(promptDialog);
    vboxlayout->setSpacing(6);
    vboxlayout->setContentsMargins(11, 11, 11, 11);

    QLabel *label = new QLabel();
    label->setGeometry(QRect(10, 10, 380, 200));
    QFont font;
    font.setPointSize(12);
    label->setFont(font);
    label->setAlignment(Qt::AlignJustify|Qt::AlignVCenter);
    label->setText(title);
    label->setWordWrap(true);
    vboxlayout->addWidget(label);

    QHBoxLayout *hboxlayout = new QHBoxLayout();
    hboxlayout->setSpacing(10);
    hboxlayout->setContentsMargins(11, 11, 11, 11);

    vboxlayout->addLayout(hboxlayout);

    signalMapper = new QSignalMapper(this);

    int i = 0;
    foreach (const QString &choice, choices)
    {
        QPushButton *pushButton = new QPushButton();
        pushButton->setObjectName(QString::fromUtf8("pushButton_")+QString::number(i));
        pushButton->setText(choice);

        connect(pushButton, SIGNAL(clicked()), signalMapper, SLOT(map()));
        signalMapper->setMapping(pushButton, i);

        hboxlayout->addWidget(pushButton);

        QTextStream(stdout) << tr("Choice[%1]: %2").arg(QString::number(i), choice) << endl;
        i++;
    }
    connect(signalMapper, SIGNAL(mapped(int)), this, SLOT(onPromptClicked(int)));

    QDialogButtonBox *buttonBox = new QDialogButtonBox();
    buttonBox->setGeometry(QRect(205, 250, 81, 25));
    buttonBox->setStandardButtons(QDialogButtonBox::Close);
    vboxlayout->addWidget(buttonBox);

    connect(buttonBox, SIGNAL(rejected()), this, SLOT(onClosePrompt()));
    connect(promptDialog, SIGNAL(closed()), this, SLOT(onClosePrompt()));

    promptDialog->show();
    ui->pushButton_28->setEnabled(false);
}


void MainWindow::actionPromptReopen()
{
    if ( promptDialog )
    {
        promptDialog->show();
        ui->pushButton_28->setEnabled(false);
    }
}


void MainWindow::onClosePrompt()
{
    promptDialog->close();

    ui->pushButton_28->setEnabled(true);
}

void MainWindow::onPromptClicked(int i)
{
    url = baseUrl + "/api/printer/command";
    command = "{ \"command\": \"M876 S" + QString::number(i) + "\" }";

    startRequest(url, command);
}

void MainWindow::getOctoprintSettings()
{
    baseUrl = "http://127.0.0.1:5000/";
    apiKey = "";

    QFile file("/root/octoprint.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream(stdout) << "Could not open file /root/octoprint.txt" << endl;
        onErrorMessage("Could not open file /root/octoprint.txt");

        return;
    }

    QTextStream in(&file);
    QString line = in.readLine();
    if (!line.isNull()) {
        baseUrl = line;

        line = in.readLine();
        if (!line.isNull()) {
            apiKey = line;
        }
    }
    file.close();

    QTextStream(stdout) << "Octoprint baseURL: \""  << baseUrl << "\"" << endl;
    QTextStream(stdout) << "apiKey: "  << apiKey  << "\"" << endl;
}

void MainWindow::actionToggleLight()
{
    url = baseUrl + "/api/printer/command";
    command = "{ \"command\": \"LIGHTS_TOGGLE\" }";

    startRequest(url, command);
}

void MainWindow::actionDisableSteppers()
{
    url = baseUrl + "/api/printer/command";
    command = "{ \"command\": \"M18\" }";

    startRequest(url, command);
}


void MainWindow::actionZoffsetM()
{
    url = baseUrl + "/api/printer/command";
    command = "{ \"command\": \"SET_GCODE_OFFSET Z_ADJUST=-0.01 MOVE=1\" }";

    startRequest(url, command);

    QTimer::singleShot(500, this, SLOT(getHomePosition()));
}

void MainWindow::actionZoffsetP()
{
    url = baseUrl + "/api/printer/command";
    command = "{ \"command\": \"SET_GCODE_OFFSET Z_ADJUST=0.01 MOVE=1\" }";

    startRequest(url, command);

    QTimer::singleShot(5000, this, SLOT(getHomePosition()));
}

void MainWindow::getHomePosition()
{
    url = baseUrl + "/api/printer/command";
    command = "{ \"command\": \"GET_POSITION\" }";

    startRequest(url, command);
}

void MainWindow::actionApllyZoffset()
{
    if ( QMessageBox::question(this, "Confirm", "Are you sure you want to run Z_OFFSET_APPLY_PROBE?", QMessageBox::Yes|QMessageBox::No, QMessageBox::No) == QMessageBox::Yes )
    {
        url = baseUrl + "/api/printer/command";
        command = "{ \"command\": \"Z_OFFSET_APPLY_PROBE\" }";

        startRequest(url, command);
    }
}

void MainWindow::actionSaveConfig()
{
    if ( QMessageBox::question(this, "Confirm", "Are you sure you want to SAVE_CONFIG?", QMessageBox::Yes|QMessageBox::No, QMessageBox::No) == QMessageBox::Yes )
    {
        url = baseUrl + "/api/printer/command";
        command = "{ \"command\": \"SAVE_CONFIG\" }";

        startRequest(url, command);
    }
}

void MainWindow::actionFilamentChange()
{
    if ( QMessageBox::question(this, "Confirm", "Are you sure you want to change filament?", QMessageBox::Yes|QMessageBox::No, QMessageBox::No) == QMessageBox::Yes )
    {
        url = baseUrl + "/api/printer/command";
        command = "{ \"command\": \"M600\" }";
        startRequest(url, command);
    }
}

void MainWindow::actionEstop()
{
    url = baseUrl + "/api/printer/command";
    command = "{ \"command\": \"M112\" }";

    startRequest(url, command);
}

void MainWindow::actionMoveXp()
{
    url = baseUrl + "/api/printer/printhead";
    command = "{ \"command\": \"jog\", \"absolute\": false, \"x\": " + QVariant(ui->dial->value()).toString() + "  }";

    startRequest(url, command);
}

void MainWindow::actionMoveXm()
{
    url = baseUrl + "/api/printer/printhead";
    command = "{ \"command\": \"jog\", \"absolute\": false, \"x\": -" + QVariant(ui->dial->value()).toString() + "  }";

    startRequest(url, command);
}

void MainWindow::actionMoveYp()
{
    url = baseUrl + "/api/printer/printhead";
    command = "{ \"command\": \"jog\", \"absolute\": false, \"y\": " + QVariant(ui->dial->value()).toString() + "  }";

    startRequest(url, command);
}

void MainWindow::actionMoveYm()
{
    url = baseUrl + "/api/printer/printhead";
    command = "{ \"command\": \"jog\", \"absolute\": false, \"y\": -" + QVariant(ui->dial->value()).toString() + "  }";

    startRequest(url, command);
}

void MainWindow::actionMoveZp()
{
    url = baseUrl + "/api/printer/printhead";
    command = "{ \"command\": \"jog\", \"absolute\": false, \"z\": " + QVariant(ui->dial->value()).toString() + "  }";

    startRequest(url, command);
}

void MainWindow::actionMoveZm()
{
    url = baseUrl + "/api/printer/printhead";
    command = "{ \"command\": \"jog\", \"absolute\": false, \"z\": -" + QVariant(ui->dial->value()).toString() + "  }";

    startRequest(url, command);
}

void MainWindow::actionMoveC()
{
    url = baseUrl + "/api/printer/printhead";
    command = "{ \"command\": \"jog\", \"absolute\": true, \"x\": 140, \"y\": 125  }";

    startRequest(url, command);
}

void MainWindow::actionMoveEp()
{
    url = baseUrl + "/api/printer/tool";
    command = "{ \"command\": \"extrude\", \"amount\": " + QVariant(ui->dial->value()).toString() + "  }";

    startRequest(url, command);
}

void MainWindow::actionMoveEm()
{
    url = baseUrl + "/api/printer/tool";
    command = "{ \"command\": \"extrude\", \"amount\": -" + QVariant(ui->dial->value()).toString() + "  }";

    startRequest(url, command);
}

void MainWindow::lenSliderChanged(int value)
{
    ui->label_9->setText( QVariant(value).toString()+"mm" );
}

void MainWindow::bedSliderChanged(int value)
{
    ui->label_10->setText( QVariant(value).toString()+QString::fromUtf8("°C") );
}

void MainWindow::extSliderChanged(int value)
{
    ui->label_11->setText( QVariant(value).toString()+QString::fromUtf8("°C") );
}

void MainWindow::fanSliderChanged(int value)
{
    ui->label_12->setText( QVariant(value).toString()+"%" );
}

void MainWindow::speedSliderChanged(int value)
{
    ui->label_14->setText( QVariant(value).toString()+"%" );
}

void MainWindow::flowSliderChanged(int value)
{
    ui->label_16->setText( QVariant(value).toString()+"%" );
}

void MainWindow::actionBedTemp()
{
    url = baseUrl + "/api/printer/bed";
    command = "{ \"command\": \"target\", \"target\": " + QVariant(ui->bedSlider->value()).toString() + " }";

    startRequest(url, command);
}

void MainWindow::actionExtTemp()
{
    url = baseUrl + "/api/printer/tool";
    command = "{ \"command\": \"target\", \"targets\": { \"tool0\": " + QVariant(ui->extSlider->value()).toString()  + " } }";

    startRequest(url, command);
}

void MainWindow::actionFanSpeed()
{
    url = baseUrl + "/api/printer/command";
    command = "{ \"command\": \"M106 S" + QVariant(ui->fanSlider->value()*2.55).toString()  + "\" }";

    startRequest(url, command);
}


void MainWindow::actionBackFan()
{
    url = baseUrl + "/api/printer/command";
    command = "{ \"command\": \"backfan_toggle\" }";

    startRequest(url, command);
}


void MainWindow::actionSpeed()
{
    url = baseUrl + "/api/printer/command";
    //command = "{ \"command\": \"feedrate\", \"factor\": " + QVariant(ui->speedSlider->value()/100).toString()  + " }";
    command = "{ \"command\": \"M220 S" + QVariant(ui->speedSlider->value()).toString()  + "\" }";

    startRequest(url, command);
}

void MainWindow::actionFlow()
{
    url = baseUrl + "/api/printer/command";
    //command = "{ \"command\": \"flowrate\", \"factor\": " + QVariant(ui->flowSlider->value()/100).toString()  + " }";
    command = "{ \"command\": \"M221 S" + QVariant(ui->flowSlider->value()).toString()  + "\" }";

    startRequest(url, command);
}

void MainWindow::showTime()
{
    QTime time = QTime::currentTime();
    QString text = time.toString("hh:mm:ss");

    ui->time->setText(text);
}

void MainWindow::actionHome()
{
    url = baseUrl + "/api/printer/printhead";
    command = "{ \"command\": \"home\", \"axes\": [\"x\", \"y\", \"z\"] }";

    startRequest(url, command);
}

void MainWindow::actionStart()
{
    url = baseUrl + "/api/job";
    command = "{ \"command\": \"start\" }";

    startRequest(url, command);
}

void MainWindow::actionPause()
{
    if ( QMessageBox::question(this, "Confirm", "Are you sure you want to Pause/Resume?", QMessageBox::Yes|QMessageBox::No, QMessageBox::No) == QMessageBox::Yes )
    {
        url = baseUrl + "/api/job";
        command = "{ \"command\": \"pause\", \"action\": \"toggle\" }";

        startRequest(url, command);
    } else {
        QTextStream(stdout) << "Aborted Pause" << endl;
    }
}

void MainWindow::actionStop()
{
    if ( QMessageBox::question(this, "Confirm", "Are you sure you want to Abort?", QMessageBox::Yes|QMessageBox::No, QMessageBox::No) == QMessageBox::Yes )
    {
        url = baseUrl + "/api/job";
        command = "{ \"command\": \"cancel\" }";
        startRequest(url, command);
    } else {
        QTextStream(stdout) << "Aborted Stop" << endl;
    }
}

void MainWindow::actionConnect()
{
    url = baseUrl + "/api/connection";
    command = "{ \"command\": \"connect\" }";

    startRequest(url, command);

    QTimer::singleShot(500, this, SLOT(getHomePosition()));
}

void MainWindow::actionRestart()
{
    if ( QMessageBox::question(this, "Confirm", "Are you sure you want to Restart?", QMessageBox::Yes|QMessageBox::No, QMessageBox::No) == QMessageBox::Yes )
    {
        url = baseUrl + "/api/printer/command";
        command = "{ \"command\": \"RESTART\" }";
        startRequest(url, command);
    }

    QTimer::singleShot(500, this, SLOT(getHomePosition()));
}

void MainWindow::actionFirmwareRestart()
{
    if ( QMessageBox::question(this, "Confirm", "Are you sure you want to Firmware Restart?", QMessageBox::Yes|QMessageBox::No, QMessageBox::No) == QMessageBox::Yes )
    {
        url = baseUrl + "/api/printer/command";
        command = "{ \"command\": \"FIRMWARE_RESTART\" }";
        startRequest(url, command);
    }
}

void MainWindow::displayCalibrationDialog()
{
    DialogWindow dlg;
    Qt::WindowFlags flags(Qt::WindowTitleHint);

    dlg.setWindowFlags(flags);
    dlg.setApiKey(apiKey);
    dlg.setBaseUrl(baseUrl);

    //Ui::Dialog uid;
    //uid.setupUi(&dlg);
    //dlg.adjustSize();

    //dlg.show();
    dlg.setFixedSize(dlg.size());
    //...

    dlg.exec();

    /*if ( dlg.exec() == QDialog::close() ) {
        QTextStream(stdout) << "Calibration dialog closed" << endl;
    }*/
}


void MainWindow::displayOpenFileDialog()
{
    OpenFileWindow dlg;
    Qt::WindowFlags flags(Qt::WindowTitleHint);

    dlg.setWindowFlags(flags);
    dlg.setApiKey(apiKey);
    dlg.setBaseUrl(baseUrl);

    dlg.setFixedSize(dlg.size());

    dlg.exec();
}

void MainWindow::displaySystemCommandsDialog()
{
    SystemCommandsWindow dlg;
    Qt::WindowFlags flags(Qt::WindowTitleHint);

    dlg.setWindowFlags(flags);
    dlg.setApiKey(apiKey);
    dlg.setBaseUrl(baseUrl);

    dlg.setFixedSize(dlg.size());

    dlg.exec();
}


/*DialogForm::DialogForm(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
}*/

void MainWindow::startRequest(QUrl url, QString command)
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

void MainWindow::httpFinished()
{
    rfinished = true;
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
}

void MainWindow::actionPrinterInfo()
{
    QUrl url2 = baseUrl + "/api/printer";

    startRequest2(url2);
}

void MainWindow::startRequest2(QUrl url)
{
    if ( !rfinished2 )
    {
        return;
    }

    QNetworkRequest request(url);
    request.setRawHeader(QByteArray("X-Api-Key"), apiKey.toUtf8());

    //QTextStream(stdout) << "Get: \""  << url.toString() << "\"" << endl;

    reply2 = qnam2.get(request);
    rfinished2 = false;

    connect(reply2, SIGNAL(finished()), this, SLOT(httpFinished2()));
    connect(reply2, SIGNAL(readyRead()), this, SLOT(httpReadyRead2()));
}

void MainWindow::httpFinished2()
{
    rfinished2 = true;

    if (reply2 == NULL || reply2->error()) {
        //QTextStream(stdout) << "Download failed " << reply2->errorString() << endl;
        //ui->label_8->setText( "Error: " + reply2->errorString() );
    }

    //
    reply2->deleteLater();
}

void MainWindow::httpReadyRead2()
{
    QString answer = reply2->readAll();

    QTextStream(stdout) << "Answer2: " << answer << endl;

    QJsonDocument doc(QJsonDocument::fromJson(answer.toUtf8()));

    if (doc.isNull()) {
        QTextStream(stdout) << "Failed to create JSON doc." << endl;
        return;
    }
    if (!doc.isObject()) {
        QTextStream(stdout) << "JSON is not an object." << endl;
        return;
    }

    QJsonObject json = doc.object();

    if (json.isEmpty()) {
        QTextStream(stdout) << "JSON object is empty." << endl;
        return;
    }

    if ( json["error"].toString() != "" ) {
        ui->label_8->setText( json["error"].toString() );
        QTextStream(stdout) << "Printer error:" << json["error"].toString() << endl;

        if ( firstRun )
        {
            actionConnect();
        }

        return;
    }

    QJsonObject state = json["state"].toObject();

    status = state["text"].toString();

    if ( status == "Printing" )
    {
        ui->progressBar->setEnabled(true);
    } else {
        if ( status == "Finishing" )
        {
            ui->progressBar->setEnabled(true);
            ui->progressBar->setValue(100);
        } else {
            ui->progressBar->setEnabled(false);
            ui->progressBar->setValue(0);
        }
        ui->label_8->setText( status );
    }

    if ( status == "Operational" )
    {
        enableMoveButtons();
        //ui->label_22->setText( "" );
    } else {
        disableMoveButtons();
    }

    QJsonObject flags = state["flags"].toObject();

    if ( flags["paused"].toBool() )
    {
        ui->pushButton_3->setText("RESUME");
    } else {
        ui->pushButton_3->setText("PAUSE");
    }

    QJsonObject temps = json["temperature"].toObject();
    if (temps.isEmpty()) {
        QTextStream(stdout) << "temps is empty." << endl;
        return;
    }

    QJsonObject tool0 = temps["tool0"].toObject();
    if (tool0.isEmpty()) {
        QTextStream(stdout) << "tool0 is empty." << endl;
        return;
    }
    QJsonObject bed = temps["bed"].toObject();
    if (bed.isEmpty()) {
        QTextStream(stdout) << "bed is empty." << endl;
        return;
    }

    if ( firstRun && status == "Operational" && tool0["actual"].toDouble() == 0 )
    {
        url = baseUrl + "/api/printer/command";
        command = "{ \"command\": \"RESTART\" }";
        startRequest(url, command);
        firstRun = false;
    }

    ui->label_2->setText( QString::number(tool0["actual"].toDouble()) + " / " + QString::number(tool0["target"].toDouble()) );
    ui->label_4->setText( QString::number(bed["actual"].toDouble()) + " / " + QString::number(bed["target"].toDouble()) );
}


void MainWindow::actionPrintJobInfo()
{
    QUrl url3 = baseUrl + "/api/job";

    if ( status == "Printing" )
    {
        startRequest3(url3);
    }
}


void MainWindow::startRequest3(QUrl url)
{
    if ( !rfinished3 )
    {
        return;
    }

    QNetworkRequest request(url);
    request.setRawHeader(QByteArray("X-Api-Key"), apiKey.toUtf8());

    QTextStream(stdout) << "Get: \""  << url.toString() << "\"" << endl;

    reply3 = qnam3.get(request);
    rfinished3 = false;

    connect(reply3, SIGNAL(finished()), this, SLOT(httpFinished3()));
    connect(reply3, SIGNAL(readyRead()), this, SLOT(httpReadyRead3()));
}


void MainWindow::httpFinished3()
{
    rfinished3 = true;

    if (reply3 && reply3->error()) {
        //QTextStream(stdout) << "Download failed " << reply3->errorString() << endl;
        //ui->label_8->setText( "Error: " + reply3->errorString() );
    }

    reply3 = 0;
}

QString MainWindow::formatSeconds(int total)
{
    QString output = "";

    int days = 0;
    int hours = 0;
    int minutes = 0;
    int seconds = 0;

    if ( total > (24*60*60) )
    {
        days = total / (24*60*60);
        total -= days * (24*60*60);
        output += QString::number(days)+"d ";
    }
    if ( total > (60*60) )
    {
        hours = total / (60*60);
        total -= hours * (60*60);
    }
    output += QString::number(hours).rightJustified(2,'0')+":";

    if ( total > 60 )
    {
        minutes = total / 60;
        total -= minutes * 60;
    }
    output += QString::number(minutes).rightJustified(2,'0')+":";

    seconds = total;

    output += QString::number(seconds).rightJustified(2,'0');

    return output;
}

void MainWindow::httpReadyRead3()
{
    QString answer = reply3->readAll();

    //QTextStream(stdout) << "Answer3 " << answer << endl;

    QJsonDocument doc(QJsonDocument::fromJson(answer.toUtf8()));

    if (doc.isNull()) {
        QTextStream(stdout) << "Failed to create JSON doc." << endl;
        return;
    }
    if (!doc.isObject()) {
        QTextStream(stdout) << "JSON is not an object." << endl;
        return;
    }

    QJsonObject json = doc.object();

    if (json.isEmpty()) {
        QTextStream(stdout) << "JSON object is empty." << endl;
        return;
    }

    if ( json["error"].toString() != "" ) {
        ui->label_22->setText( json["error"].toString() );
        QTextStream(stdout) << "Printer error:" << json["error"].toString() << endl;
        return;
    }

    if ( json["state"].toString() != "Printing" )
    {
        return;
    }
    ui->progressBar->setEnabled(true);

    QJsonObject job = json["job"].toObject();

    QJsonObject file = job["file"].toObject();

    QJsonObject progress = json["progress"].toObject();

    QString output = json["state"].toString() + " " + file["name"].toString();

    QString output2 = "Print Time: " + formatSeconds( progress["printTime"].toInt() ) + " | Finished: " + QString::number(progress["completion"].toDouble(), 'f', 2)+"% / ";

    ui->progressBar->setValue(static_cast<int>(progress["completion"].toDouble()));

    output2 += formatSeconds( progress["printTimeLeft"].toInt() ) + "left";

    ui->label_8->setText( output );
    ui->label_22->setText( output2 );

    //QTextStream(stdout) << "Output: " << output << " " << output2 << endl;
}


void MainWindow::disableMoveButtons()
{
    ui->pushButton->setEnabled(false);
    ui->pushButton_8->setEnabled(false);
    ui->pushButton_9->setEnabled(false);
    ui->pushButton_10->setEnabled(false);
    ui->pushButton_11->setEnabled(false);
    ui->pushButton_12->setEnabled(false);
    ui->pushButton_13->setEnabled(false);
    ui->pushButton_16->setEnabled(false);
    ui->pushButton_17->setEnabled(false);
    ui->pushButton_22->setEnabled(false);
    ui->pushButton_23->setEnabled(false);
    ui->pushButton_31->setEnabled(false);
    ui->pushButton_33->setEnabled(false);
    ui->pushButton_34->setEnabled(false);
}

void MainWindow::enableMoveButtons()
{
    ui->pushButton->setEnabled(true);
    ui->pushButton_8->setEnabled(true);
    ui->pushButton_9->setEnabled(true);
    ui->pushButton_10->setEnabled(true);
    ui->pushButton_11->setEnabled(true);
    ui->pushButton_12->setEnabled(true);
    ui->pushButton_13->setEnabled(true);
    ui->pushButton_16->setEnabled(true);
    ui->pushButton_17->setEnabled(true);
    ui->pushButton_22->setEnabled(true);
    ui->pushButton_23->setEnabled(true);
    ui->pushButton_31->setEnabled(true);
    ui->pushButton_33->setEnabled(true);
    ui->pushButton_34->setEnabled(true);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete server;
    workerThread.quit();
    workerThread.wait();
}
