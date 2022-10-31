#ifndef MQTTWINDOW_H
#define MQTTWINDOW_H

#include <QMainWindow>
#include <QUuid>
#include <QModelIndex>
#include <QTimer>
#include <qmqtt.h>
#include <addsubscribedialog.h>
#include <QStandardItemModel>
namespace Ui {
class MqttWindow;
}
// Mqtt subscribe list item
typedef struct SubscribeTopic
{
    QString topic;
    qint8 qos;
}SubscribeTopic;
Q_DECLARE_METATYPE(SubscribeTopic);
//
class MqttWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MqttWindow(QWidget *parent = nullptr);
    QUuid id;
    void closeEvent(QCloseEvent *);
    ~MqttWindow();

private slots:
    void on_deleteSubscribeButton_clicked();
    void on_subscribeListView_activated(const QModelIndex &index);
    void on_subscribeListView_clicked(const QModelIndex &index);
    void on_connectButton_clicked();
    // Mqtt
    void connected();
    void disconnected();
    //
    void error(const QMQTT::ClientError error);
    //
    void subscribed(const QString& topic, const quint8 qos = 0);
    void unsubscribed(const QString& topic);
    void pingresp();
    //
    void received(const QMQTT::Message& message);
    void published(const QMQTT::Message& message, quint16 msgid = 0);
    //
    void timeout();
    //
    void on_addSubscribeButton_clicked();
    //
    void getSubscribeTopicEntry(QString topic, int qos);

private:
    Ui::MqttWindow *ui;
    QString clientId, username, password;
    QString host;
    qint32 port;
    //
    QModelIndex currentSelectedItem;
    QMQTT::Client *client;
    QTimer timer;
    //
    AddSubscribeDialog *addSubscribeDialog;
    QStandardItemModel *model;

};

#endif // MQTTWINDOW_H
