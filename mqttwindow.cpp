#include "mqttwindow.h"
#include "mainwindow.h"
#include "ui_mqttwindow.h"
#include <QDebug>
#include <QStandardItemModel>
#include <QMetaEnum>
#include <QThread>
#include <QMessageBox>
void logger(QTextBrowser *t,QString s){
    qDebug()<< s;
    t->append(QString( "[%1]-> %2")
                  .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss:zzz"))
                  .arg(s));
}

MqttWindow::MqttWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MqttWindow)
{
    ui->setupUi(this);
    this->setParent(parent);
    this->id = QUuid::createUuid();
    this->setAttribute(Qt::WA_DeleteOnClose, true);
    this->setWindowTitle("MQTT Client: "+this->id.toString());
    QListView *subscribeListView= this->findChild<QListView *>("subscribeListView");
    QStandardItemModel *model = new QStandardItemModel(this);
    QStandardItem *item1 = new QStandardItem("Topic: /user/group/devid/1, QOS: 1");
    model->appendRow(item1);
    QStandardItem *item2 = new QStandardItem("Topic: /user/group/devid/2, QOS: 1");
    model->appendRow(item2);
    subscribeListView->setModel(model);
    qDebug()<< "Mqtt window created, id: " << this->id.toString();
    //
    ui->clientIdEdit->setText(this->id.toString());
    ui->connectButton->setStyleSheet("background-color:#FF4500;");
    //
    client = new QMQTT::Client();
    QObject::connect(client, SIGNAL(disconnected()),this,SLOT(disconnected()));
    QObject::connect(client, SIGNAL(error(QMQTT::ClientError)),this,SLOT(error(QMQTT::ClientError)));
    QObject::connect(client, SIGNAL(connected()),this,SLOT(connected()));
    QObject::connect(client, SIGNAL(received(QMQTT::Message)),this,SLOT(received(QMQTT::Message)));
    QObject::connect(client, SIGNAL(pingresp()),this,SLOT(pingresp()));

}

MqttWindow::~MqttWindow()
{
    qDebug()<< "Mqtt window destoryed, id: " << this->id.toString();
    delete ui;
    delete client;
}

void MqttWindow::closeEvent(QCloseEvent *){
    qDebug()<< "Mqtt window closed, id: " << this->id.toString();
    MainWindow* mainWindow=( MainWindow*)this->parent();
    mainWindow->MQTTWindowMap.remove(this->id.toString());
}

void MqttWindow::on_deleteSubscribeButton_clicked()
{
    qDebug()<< "deleteSubscribe: " << this->currentSelectedItem;
    if(!ui->subscribeListView->model()->itemData(this->currentSelectedItem).empty()){
        ui->subscribeListView->model()->removeRow(this->currentSelectedItem.row());
    }
}


void MqttWindow::on_subscribeListView_activated(const QModelIndex &index)
{
    qDebug()<< "Current row activated: " << index.row();
}


void MqttWindow::on_subscribeListView_clicked(const QModelIndex &index)
{
    this->currentSelectedItem = index;
    qDebug()<< "Current row clicked: " << index.row();
}


void MqttWindow::on_connectButton_clicked()
{

    if(client->isConnectedToHost()){
        switch(QMessageBox::information(this,  "Disconnect",
                                         "Disconnect from broker?",
                                         QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes) )
        {
        case QMessageBox::Yes:
            client->disconnectFromHost();
            logger(ui->logTextBrowser,"client try to disconnect");
            return;
        case QMessageBox::No:
            return;
        default:
            return;
        }
    }
    //
    client->setCleanSession(true);
    client->setHostName(this->ui->hostEdit->text());
    client->setPort(this->ui->portSpin->value());
    client->setClientId(this->ui->clientIdEdit->text());
    client->setUsername(this->ui->usernameEdit->text());
    client->setPassword(this->ui->passwordEdit->text().toUtf8());
    QThread::sleep(1);
    client->connectToHost();
}
void MqttWindow::connected(){
    logger(ui->logTextBrowser,"client connected");
    ui->connectButton->setStyleSheet("background-color:#32CD32;");
    ui->connectButton->setText("Connected");
    ui->connectButton->setDisabled(false);
}
void MqttWindow::disconnected(){
    logger(ui->logTextBrowser,"client disconnected");
    ui->connectButton->setDisabled(false);
    ui->connectButton->setStyleSheet("background-color:#FF4500;");
    ui->connectButton->setText("Connect");
}
void MqttWindow::error(const QMQTT::ClientError error){
    QMetaEnum errors = QMetaEnum::fromType<QMQTT::ClientError>();
    logger(ui->logTextBrowser, errors.valueToKey(error));
    ui->connectButton->setDisabled(false);
    ui->connectButton->setStyleSheet("background-color:#FF4500;");
    ui->connectButton->setText("Connect");
}

void MqttWindow::subscribed(const QString& topic, const quint8 qos ){
    logger(ui->logTextBrowser,QString("client subscribed:%1 with QOS:%2").arg(topic).arg(qos));
}
void MqttWindow::unsubscribed(const QString& topic){
    logger(ui->logTextBrowser,QString("client unsubscribed:%1").arg(topic));
}

void MqttWindow::pingresp(){

}
void MqttWindow::received(const QMQTT::Message& message){

}
void MqttWindow::published(const QMQTT::Message& message, quint16 msgid){

}

void MqttWindow::timeout(){

    logger(ui->logTextBrowser,"timeout");
}
