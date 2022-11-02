#include "mqttwindow.h"
#include "mainwindow.h"
#include "ui_mqttwindow.h"
#include <QDebug>
#include <QStandardItemModel>
#include <QMetaEnum>
#include <QThread>
#include <QMessageBox>
void logger(QTextBrowser *t, QString s)
{
    qDebug() << s;
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
    this->setWindowTitle("MQTT Client: " + this->id.toString());
    QListView *subscribeListView = this->findChild<QListView *>("subscribeListView");
    this->model = new QStandardItemModel(this);
    subscribeListView->setModel(model);
    qDebug() << "Mqtt window created, id: " << this->id.toString();
    //
    ui->clientIdEdit->setText(this->id.toString());
    ui->connectButton->setStyleSheet("background-color:#FF4500;");
    //
    client = new QMQTT::Client();
    QObject::connect(client, SIGNAL(disconnected()), this, SLOT(disconnected()));
    QObject::connect(client, SIGNAL(error(QMQTT::ClientError)), this, SLOT(error(QMQTT::ClientError)));
    QObject::connect(client, SIGNAL(connected()), this, SLOT(connected()));
    QObject::connect(client, SIGNAL(received(QMQTT::Message)), this, SLOT(received(QMQTT::Message)));
    QObject::connect(client, SIGNAL(pingresp()), this, SLOT(pingresp()));

}

MqttWindow::~MqttWindow()
{
    qDebug() << "Mqtt window destoryed, id: " << this->id.toString();
    delete ui;
    delete client;
}

void MqttWindow::closeEvent(QCloseEvent *)
{
    qDebug() << "Mqtt window closed, id: " << this->id.toString();
    MainWindow *mainWindow = ( MainWindow *)this->parent();
    mainWindow->MQTTWindowMap.remove(this->id.toString());
}

void MqttWindow::on_deleteSubscribeButton_clicked()
{
    if(!ui->subscribeListView->model()->itemData(this->currentSelectedItem).empty()) {
        if (this->client->isConnectedToHost()) {
            QStandardItemModel *model = dynamic_cast<QStandardItemModel *>(ui->subscribeListView->model());
            QStandardItem *item = model->itemFromIndex(this->currentSelectedItem);
            int index = item->text().indexOf("|"); //"0  |  t"
            this->client->unsubscribe(item->text().mid(index, item->text().length()));

            qDebug() << "Unsubscribe topic: " << item->text().mid(index + 3, item->text().length());
        }
        ui->subscribeListView->model()->removeRow(this->currentSelectedItem.row());
    }
}

void MqttWindow::on_subscribeListView_clicked(const QModelIndex &index)
{
    this->currentSelectedItem = index;
    qDebug() << "Current row clicked: " << index.row();
}


void MqttWindow::on_connectButton_clicked()
{

    if(client->isConnectedToHost()) {
        switch(QMessageBox::information(this,  "Disconnect",
                                        "Disconnect from broker?",
                                        QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes) ) {
            case QMessageBox::Yes:
                client->disconnectFromHost();
                logger(ui->logTextBrowser, "client try to disconnect");
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
void MqttWindow::connected()
{
    logger(ui->logTextBrowser, "client connected");
    ui->connectButton->setStyleSheet("background-color:#32CD32;");
    ui->connectButton->setText("Connected");
    ui->connectButton->setDisabled(false);
}
void MqttWindow::disconnected()
{
    logger(ui->logTextBrowser, "client disconnected");
    ui->connectButton->setDisabled(false);
    ui->connectButton->setStyleSheet("background-color:#FF4500;");
    ui->connectButton->setText("Connect");
}
void MqttWindow::error(const QMQTT::ClientError error)
{
    QMetaEnum errors = QMetaEnum::fromType<QMQTT::ClientError>();
    logger(ui->logTextBrowser, errors.valueToKey(error));
    ui->connectButton->setDisabled(false);
    ui->connectButton->setStyleSheet("background-color:#FF4500;");
    ui->connectButton->setText("Connect");
}

void MqttWindow::subscribed(const QString &topic, const quint8 qos )
{
    logger(ui->logTextBrowser, QString("client subscribed:%1 with QOS:%2").arg(topic).arg(qos));
}
void MqttWindow::unsubscribed(const QString &topic)
{
    logger(ui->logTextBrowser, QString("client unsubscribed:%1").arg(topic));
}

void MqttWindow::pingresp()
{

}

void MqttWindow::received(const QMQTT::Message &message)
{
    logger(ui->msgTextBrowser, QString("Received data: %1").arg(QString::fromUtf8(message.payload())));
}
void MqttWindow::published(const QMQTT::Message &message, quint16 msgid)
{

}

void MqttWindow::getSubscribeTopicEntry(QString topic, int qos)
{
    if(!this->client->isConnectedToHost()) {
        QMessageBox::warning(this, "Warning", "Subscribe topic failure! Client has disconnected from broker.");
        return;
    }
    if(topic.isEmpty()) {
        logger(ui->logTextBrowser, "Topic cant empty");
        QMessageBox::warning(this, "Warning", "Topic can't empty");
        return;
    }
    //
    for (int i = 0; i < ui->subscribeListView->model()->rowCount(); ++i) {
        QStandardItemModel *model = dynamic_cast<QStandardItemModel *>(ui->subscribeListView->model());
        QStandardItem *item = model->item(i);
        if(item->text().contains(topic)) {
            QMessageBox::warning(this, "Warning", "Duplicate Topic, Please Remove Befor Subscribe Another Same Topic");
            return;
        }
    }

    qDebug() << "getSubscribeTopicEntry: " << topic << ",qos: " << qos;
    QStandardItem *item = new QStandardItem(QString::number(qos) + "  |  " + topic);
    model->appendRow(item);
    if (this->client->isConnectedToHost()) {
        this->client->subscribe(topic, qos);
    }
}
void MqttWindow::timeout()
{

}

void MqttWindow::on_addSubscribeButton_clicked()
{
    this->addSubscribeDialog = new AddSubscribeDialog(this);
    QObject::connect(this->addSubscribeDialog, SIGNAL(getSubscribeTopicEntry(QString, int)),
                     this, SLOT(getSubscribeTopicEntry(QString, int)));
    this->addSubscribeDialog->show();
}

