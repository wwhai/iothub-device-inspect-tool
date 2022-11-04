#include "mqttwindow.h"
#include "mainwindow.h"
#include "ui_mqttwindow.h"
#include <QDebug>
#include <QStandardItemModel>
#include <QMetaEnum>
#include <QThread>
#include <QMessageBox>
#include "locallogger.h"
#include <QMenu>
MqttWindow::MqttWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MqttWindow)
{
    ui->setupUi(this);
    this->setParent(parent);
    this->id = QUuid::createUuid();
    this->setAttribute(Qt::WA_DeleteOnClose, true);
    this->setFixedSize( this->width(), this->height());
    this->setWindowTitle("MQTT Client: " + this->id.toString());
    this->model = new QStandardItemModel(this);
    //
    this->contextMenu = new QMenu(this->ui->msgTextBrowser);
    this->clearAction = new QAction("ClearContent", this->ui->msgTextBrowser);
    this->contextMenu->addAction(clearAction);
    QObject::connect(this->clearAction, SIGNAL(triggered(bool)), this, SLOT(clearText(bool)));
    this->ui->subscribeListView->setModel(model);
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
    //
    QObject::connect(client, SIGNAL(pingresp()), this, SLOT(pingresp()));
    QObject::connect(client, SIGNAL(subscribed(const QString &, const quint8)), this, SLOT(subscribed(const QString &,
                     const quint8)));
    QObject::connect(client, SIGNAL(unsubscribed(const QString &)), this, SLOT(unsubscribed(const QString &)));
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
QString parseTopic(QString i)
{
    // "QOS  |  Topic"
    return i.mid(i.indexOf("|") + 3, i.length());
}
int parseQos(QString i)
{
    if(i[0] == 0) {
        return 0;
    }
    if(i[1] == 1) {
        return 1;
    }
    if(i[1] == 1) {
        return 2;
    }
    return  0;
}
void MqttWindow::on_deleteSubscribeButton_clicked()
{
    if(!ui->subscribeListView->model()->itemData(this->currentSelectedItem).empty()) {
        if (this->client->isConnectedToHost()) {
            QStandardItemModel *model = dynamic_cast<QStandardItemModel *>(ui->subscribeListView->model());
            QStandardItem *item = model->itemFromIndex(this->currentSelectedItem);
            this->client->unsubscribe(parseTopic(item->text()));
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
    QThread::msleep(100);
    client->connectToHost();
}
void MqttWindow::connected()
{
    logger(ui->logTextBrowser, "Success connect to: " + ui->hostEdit->text() + ":" + QString::number(
               ui->portSpin->value()));
    ui->connectButton->setStyleSheet("background-color:#32CD32;");
    ui->connectButton->setText("Connected");
    ui->connectButton->setDisabled(false);

    for (int i = 0; i < ui->subscribeListView->model()->rowCount(); ++i) {
        QStandardItemModel *model = dynamic_cast<QStandardItemModel *>(ui->subscribeListView->model());
        QStandardItem *item = model->item(i);
        this->client->subscribe(parseTopic(item->text()), parseQos(item->text()));
    }

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
    logger(ui->logTextBrowser, QString("Subscribed: %1, QOS: %2").arg(topic).arg(qos));
}
void MqttWindow::unsubscribed(const QString &topic)
{
    logger(ui->logTextBrowser, QString("Unsubscribed: %1").arg(topic));
}

void MqttWindow::pingresp()
{

}
static QString msgRecvTemplate = "<html>"
                                 "<body>"
                                 "<div style=\"width:100%; height: 100%;\">"
                                 "<table border=\"1\" style=\"border-collapse: collapse; border-spacing: 0;width: 100%;border-color: aqua;\">"
                                 "<tr>"
                                 "<td style=\"background-color:#b800e6;color: aliceblue;\">TOPIC</td>"
                                 "<td style=\"background-color:#28353d;color: aliceblue;text-align: center;\" colspan=\"5\">%1</td>"
                                 "</tr>"
                                 "<tr>"
                                 "<td style=\"background-color:#00eeff;color: rgb(0, 0, 0);\">Ts</td>"
                                 "<td style=\"background-color:#28353d ;color: aliceblue;text-align: center;\">%2</td>"

                                 "<td style=\"background-color:#5d5ffc;color: aliceblue;\">Retain</td>"
                                 "<td style=\"background-color:#28353d ;color: aliceblue;text-align: center;\">%3</td>"

                                 "<td style=\"background-color:#fdad00;color: rgb(0, 0, 0);\">QOS</td>"
                                 "<td style=\"background-color:#28353d ;color: aliceblue;text-align: center;\">%4</td>"
                                 "</tr>"
                                 "<tr>"
                                 "<td style=\"background-color:#000000;color: rgb(166, 255, 0);margin: 2px;padding: 2px;\" colspan=\"6\">"
                                 "%5"
                                 "</td>"
                                 "</tr>"
                                 "</table>"
                                 "</div>"
                                 "</body>"
                                 "</html>";
void MqttWindow::received(const QMQTT::Message &message)
{
    // T TS R Qo Msg

    logger(ui->msgTextBrowser, msgRecvTemplate.arg(message.topic())
           .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"))
           .arg(QString::number(message.retain()))
           .arg(QString::number(message.qos()))
           .arg(QString::fromUtf8(message.payload())));

}
void MqttWindow::published(const QMQTT::Message &message, quint16 msgid)
{
    logger(ui->msgTextBrowser,
           QString("Published success: Topic: %1, QoS: %2, Id: %3, Paylad: %4")
           .arg(message.topic())
           .arg(QString::number(message.qos()))
           .arg(QString::number(msgid))
           .arg(QString::fromUtf8(message.payload())));
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

void MqttWindow::on_addSubscribeButton_clicked()
{
    this->addSubscribeDialog = new AddSubscribeDialog(this);
    QObject::connect(this->addSubscribeDialog, SIGNAL(getSubscribeTopicEntry(QString, int)),
                     this, SLOT(getSubscribeTopicEntry(QString, int)));
    this->addSubscribeDialog->show();
}


void MqttWindow::on_publishMsgButton_clicked()
{
    if (!this->client->isConnectedToHost()) {
        QMessageBox::warning(this, "Warning", "Client has disconnected from broker.");
        return;
    }
    if(ui->publishTopicEdit->text().isEmpty()) {
        QMessageBox::warning(this, "Warning", "Required Topic");
        return;
    }
    if(ui->pubMsgEdit->toPlainText().isEmpty()) {
        QMessageBox::warning(this, "Warning", "Required Message");
        return;
    }

    int qos = ui->publishQosSpin->value();
    QMQTT::Message message(0,
                           ui->publishTopicEdit->text(),
                           ui->pubMsgEdit->toPlainText().toUtf8(),
                           qos,
                           ui->retainCheck->isChecked(),
                           ui->dupCheck->isChecked());
    if (this->client->isConnectedToHost()) {
        this->client->publish(message);
    }
}

void MqttWindow::on_clearContentButton_clicked()
{
    ui->pubMsgEdit->setText("");
}

void MqttWindow::on_msgTextBrowser_customContextMenuRequested(const QPoint &pos)
{
    this->contextMenu->exec(QCursor::pos());
}

void MqttWindow::clearText(bool t)
{
    ui->msgTextBrowser->setText("");
}
