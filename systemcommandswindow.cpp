#include "systemcommandswindow.h"
#include "ui_systemcommandswindow.h"
#include <QtNetwork>
#include <QErrorMessage>
#include <QMessageBox>

#include "QProgressIndicator.h"
#include "qjson4/QJsonObject.h"
#include "qjson4/QJsonDocument.h"
#include "qjson4/QJsonArray.h"
#include "qjson4/QJsonParseError.h"

SystemCommandsWindow::SystemCommandsWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SystemCommandsWindow)
{
    ui->setupUi(this);

    QTextStream(stdout) << "Hello systemcommands dialog!" << endl;

    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(actionLoadCommands()));
    connect(ui->pushButton_2, SIGNAL(clicked()), this, SLOT(actionRun()));
    connect(ui->buttonBox, SIGNAL(close()), this, SLOT(closeDialog()));


    QTimer *timer3 = new QTimer(this);
    connect(timer3, SIGNAL(timeout()), this, SLOT(actionLoadCommands()));
    timer3->setSingleShot(true);
    timer3->start(1000);
}

SystemCommandsWindow::~SystemCommandsWindow()
{
    delete ui;
}

void SystemCommandsWindow::closeDialog()
{
    close();
}

void SystemCommandsWindow::setBaseUrl(QString url)
{
    baseUrl = url;
    QTextStream(stdout) << "baseUrl \""  << baseUrl.toUtf8() << "\"" << endl;
}

void SystemCommandsWindow::setApiKey(QString key)
{
    apiKey = key;
    QTextStream(stdout) << "apiKey \""  << apiKey.toUtf8() << "\"" << endl;
}


void SystemCommandsWindow::startRequest(QUrl url, QString command)
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

    pi = new QProgressIndicator();
    pi->startAnimation();
    ui->gridLayout->addWidget(pi);
}

void SystemCommandsWindow::httpFinished()
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
}

void SystemCommandsWindow::actionRun()
{
    QListWidgetItem *item = ui->listWidget->currentItem();

    if( ui->listWidget->selectedItems().size() != 0)
    {
        QString url = item->data(Qt::UserRole).toString();
        QString confirm = item->data(Qt::ToolTipRole).toString();

        QTextStream(stdout) << "URL: \""  << url << "\"" << endl;
        QTextStream(stdout) << "confirm: \""  << confirm << "\"" << endl;

        QWidget *np = 0;
        if ( QMessageBox::question(np, "Confirm", confirm, QMessageBox::Yes|QMessageBox::No, QMessageBox::No) == QMessageBox::Yes )
        {
            startRequest(url, command);
        }

        //if ( QMessageBox::question(qp, confirm, QMessageBox::Yes|QMessageBox::No, QMessageBox::No) == QMessageBox::Yes )
        //{

        //}
    } else {
        QErrorMessage *emessage = new QErrorMessage(this);
        emessage->showMessage("You have to select the file first!");
    }
}



void SystemCommandsWindow::actionLoadCommands()
{
    url = baseUrl + "/api/system/commands";

    startRequest2(url);
}


void SystemCommandsWindow::startRequest2(QUrl url)
{
    if ( !rfinished2 )
    {
        return;
    }

    QNetworkRequest request(url);
    request.setRawHeader(QByteArray("X-Api-Key"), apiKey.toUtf8());
    QTextStream(stdout) << "Get: \""  << url.toString() << "\"" << endl;

    reply2 = qnam2.get(request);
    rfinished2 = false;
    fileListInfo = "";

    connect(reply2, SIGNAL(finished()), this, SLOT(httpFinished2()));
    connect(reply2, SIGNAL(readyRead()), this, SLOT(httpReadyRead2()));

    pi = new QProgressIndicator();
    pi->startAnimation();
    ui->gridLayout->addWidget(pi);
}

void SystemCommandsWindow::httpReadyRead2()
{
    QTextStream(stdout) << "httpReadyRead2()" << endl;

    fileListInfo += reply2->readAll();
    return;
}

void SystemCommandsWindow::httpFinished2()
{
    if (reply2 == NULL || reply2->error()) {
        //pi->stopAnimation();

        QErrorMessage *emessage = new QErrorMessage(this);
        emessage->showMessage("Download failed " + reply2->errorString());
        QTextStream(stdout) << "Download failed " << reply2->errorString() << endl;
        fileListInfo = "";
    }

    if ( fileListInfo != "" )
    {
        QString answer = fileListInfo;

        QTextStream(stdout) << "Answer2o: " << answer << endl;

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
            QTextStream(stdout) << "Printer error2o:" << json["error"].toString() << endl;
            return;
        }



        //QJsonArray all_commands += json["custom"].toArray();
        //QJsonArray all_commands += json["plugins"].toArray();


        //QJsonArray core_commands = json["core"].toArray();
        //QJsonArray custom_commands = json["custom"].toArray();
        //QJsonArray plugins_commands = json["plugins"].toArray();


        ui->listWidget->clear();

        foreach(const QString& key, json.keys()) {
            QJsonArray commands = json[key].toArray();

            if ( commands.isEmpty()) {
                QTextStream(stdout) << key + "Array is empty." << endl;
                continue;
            }

            QJsonValue value = json.value(key);
            qDebug() << "Key = " << key << ", Value = " << value.toString();


            int idx = 0;
            for(const QJsonValue& val: commands)
            {
                QJsonObject loopObj = val.toObject();

                //Adding to listWidget
                QListWidgetItem *item =new QListWidgetItem();

                QTextStream(stdout) << "[" << idx << " " << loopObj["action"].toString() << "] name    : " << loopObj["name"].toString().trimmed();
                //QTextStream(stdout) << "[" << idx << "] resource    : " << refsObj["resource"].toString().trimmed();
                QTextStream(stdout) << "" << endl;


                item->setText(loopObj["name"].toString());
                item->setData(Qt::UserRole, loopObj["resource"].toString() );
                if ( loopObj["confirm"].toString().length() > 0 )
                {
                    item->setData(Qt::ToolTipRole, loopObj["confirm"].toString() );
                } else {
                    item->setData(Qt::ToolTipRole, tr("Are You sure you want to run %1 ?").arg(loopObj["name"].toString()) );
                }

                ui->listWidget->addItem(item);

                ++idx;
            }
        }
    }


    pi->stopAnimation();
    reply2->deleteLater();
    rfinished2 = true;

    if ( closeAfterRequest )
    {
        QTextStream(stdout) << "Close" << endl;
        close();
    }
}
