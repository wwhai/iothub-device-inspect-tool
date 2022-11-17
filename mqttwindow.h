#ifndef MQTTWINDOW_H
#define MQTTWINDOW_H

#include <QMainWindow>
#include <QUuid>
#include <QModelIndex>
#include <QTimer>
#include <qmqtt.h>
#include <addsubscribedialog.h>
#include <QStandardItemModel>
namespace Ui
{
class MqttWindow;
}
// Mqtt subscribe list item
typedef struct SubscribeTopic {
    QString topic;
    qint8 qos;
} SubscribeTopic;
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
    void on_subscribeListView_clicked(const QModelIndex &index);
    void on_connectButton_clicked();
    // Mqtt
    void connected();
    void disconnected();
    //
    void error(const QMQTT::ClientError error);
    //
    void subscribed(const QString &topic, const quint8 qos = 0);
    void unsubscribed(const QString &topic);
    void pingresp();
    //
    void received(const QMQTT::Message &message);
    void published(const QMQTT::Message &message, quint16 msgid = 0);
    //
    void on_addSubscribeButton_clicked();
    //
    void getSubscribeTopicEntry(QString topic, int qos);

    void on_publishMsgButton_clicked();

    void on_clearContentButton_clicked();

    void on_msgTextBrowser_customContextMenuRequested(const QPoint &pos);
    //
    void clearText(bool t);
    void on_dupCheck_stateChanged(int arg1);

    void on_retainCheck_stateChanged(int arg1);

private:
    Ui::MqttWindow *ui;
    QString clientId, username, password;
    QString host;
    qint32 port;
    //
    quint64 msgId;
    //
    QModelIndex currentSelectedItem;
    QMQTT::Client *client;
    //
    AddSubscribeDialog *addSubscribeDialog;
    QStandardItemModel *model;
    //
    QMenu *contextMenu;
    QAction *clearAction;
};

#endif // MQTTWINDOW_H
