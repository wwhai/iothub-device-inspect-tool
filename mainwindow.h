#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "mqttwindow.h"
#include <QMap>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    QMap<QString, MqttWindow*> MQTTWindowMap;
    ~MainWindow();

private slots:
    void on_actionMQTT_triggered();
    void on_actionAbout_triggered();
private:
    Ui::MainWindow *ui;

};
#endif // MAINWINDOW_H
