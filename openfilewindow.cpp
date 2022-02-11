#include "openfilewindow.h"
#include "ui_openfilewindow.h"
#include <QtNetwork>
#include <QErrorMessage>
#include <QMessageBox>

#include "QProgressIndicator.h"
#include "qjson4/QJsonObject.h"
#include "qjson4/QJsonDocument.h"
#include "qjson4/QJsonArray.h"
#include "qjson4/QJsonParseError.h"

OpenFileWindow::OpenFileWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OpenFileWindow)
{
    ui->setupUi(this);
    QTextStream(stdout) << "Hello openfile dialog!" << endl;


    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(actionLoadFiles()));
    connect(ui->pushButton_2, SIGNAL(clicked()), this, SLOT(actionSelect()));
    connect(ui->pushButton_3, SIGNAL(clicked()), this, SLOT(actionStart()));
    connect(ui->buttonBox, SIGNAL(close()), this, SLOT(closeDialog()));

    QTimer *timer3 = new QTimer(this);
    connect(timer3, SIGNAL(timeout()), this, SLOT(actionLoadFiles()));
    timer3->setSingleShot(true);
    timer3->start(1000);
}

OpenFileWindow::~OpenFileWindow()
{
    delete ui;
}

void OpenFileWindow::closeDialog()
{
    close();
}

void OpenFileWindow::setBaseUrl(QString url)
{
    baseUrl = url;
    QTextStream(stdout) << "baseUrl \""  << baseUrl.toUtf8() << "\"" << endl;
}

void OpenFileWindow::setApiKey(QString key)
{
    apiKey = key;
    QTextStream(stdout) << "apiKey \""  << apiKey.toUtf8() << "\"" << endl;

}

void OpenFileWindow::actionStart()
{
    url = baseUrl + "/api/job";
    command = "{ \"command\": \"start\" }";

    startRequest(url, command);
}


void OpenFileWindow::startRequest(QUrl url, QString command)
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

void OpenFileWindow::httpFinished()
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

void OpenFileWindow::actionSelect()
{
    QListWidgetItem *item = ui->listWidget->currentItem();

    if( ui->listWidget->selectedItems().size() != 0)
    {
        QString url = item->data(Qt::UserRole).toString();
        QString type = item->data(Qt::ToolTipRole).toString();

        QTextStream(stdout) << "URL: \""  << url << "\"" << endl;
        QTextStream(stdout) << "type: \""  << type << "\"" << endl;

        if ( type == "folder" )
        {
             startRequest2(url);
        } else {
             command = "{ \"command\": \"select\" }";
             startRequest(url, command);
        }
    } else {
        QErrorMessage *emessage = new QErrorMessage(this);
        emessage->showMessage("You have to select the file first!");
    }
}



void OpenFileWindow::actionLoadFiles()
{

    if ( ui->listWidget->count() )
    {
        QWidget *np = 0;
        if ( QMessageBox::question(np, "Confirm", "Are you sure, you want to reload file list?", QMessageBox::Yes|QMessageBox::No, QMessageBox::No) != QMessageBox::Yes )
        {
            return;
        }
    }

    url = baseUrl + "/api/files/local";

    startRequest2(url);
}


void OpenFileWindow::startRequest2(QUrl url)
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

void OpenFileWindow::httpReadyRead2()
{
    QTextStream(stdout) << "httpReadyRead2()" << endl;

    fileListInfo += reply2->readAll();
    return;
}


QString OpenFileWindow::formatSeconds(int total)
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
        output += QString::number(hours).rightJustified(2,'0')+":";
    }

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

void OpenFileWindow::httpFinished2()
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

        QJsonArray files = json["files"].toArray();
        if ( files.isEmpty()) {
            files = json["children"].toArray();
        }

        //QJsonObject ar1 = array.at(0).toObject();
        //QTextStream(stdout) << "name" << ar1["name"].toString();

        QTextStream(stdout) << "Sorting files..." << endl;

        std::sort(files.begin(), files.end(), [](const QJsonValue &v1, const QJsonValue &v2) {

            if ( v1.toObject()["type"].toString() == "folder" )
            {
                if ( v2.toObject()["type"].toString() != "folder" )
                {
                    return true;
                }
            } else if ( v2.toObject()["type"].toString() == "folder" )
            {
                return false;
            }


            return v1.toObject()["display"].toString().toLower() < v2.toObject()["display"].toString().toLower();
        });


        ui->listWidget->clear();

        int idx = 0;
        for(const QJsonValue& val: files)
        {
            QJsonObject loopObj = val.toObject();

            QJsonObject refsObj = loopObj["refs"].toObject();

            //Adding to listWidget
            QListWidgetItem *item =new QListWidgetItem();

            QTextStream(stdout) << "[" << idx << " " << loopObj["type"].toString() << "] display    : " << loopObj["display"].toString().trimmed();
            //QTextStream(stdout) << "[" << idx << "] resource    : " << refsObj["resource"].toString().trimmed();
            QTextStream(stdout) << "" << endl;

            if ( loopObj["type"] == "folder" )
            {
                QFont fnt = item->font();
                fnt.setWeight(QFont::Bold);
                item->setFont(fnt);
            }

            QJsonObject analysis = loopObj["gcodeAnalysis"].toObject();
            int aTime = analysis["estimatedPrintTime"].toDouble();
            if ( aTime > 0 )
            {
                QString sTime = formatSeconds(static_cast<int>(aTime));

                item->setText(loopObj["display"].toString()+" - "+sTime);
            } else {

                item->setText(loopObj["display"].toString());
            }



            item->setData(Qt::UserRole, refsObj["resource"].toString() );
            item->setData(Qt::ToolTipRole, loopObj["type"].toString() );

            ui->listWidget->addItem(item);

            //QString filePath =   listWidget->currentItem()->data(1); //note::: role equals 1

            //ui->listWidget->addItem( loopObj["display"].toString() );
            ++idx;
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
