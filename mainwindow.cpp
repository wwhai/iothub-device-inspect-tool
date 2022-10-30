#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mqttwindow.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_actionMQTT_triggered()
{
    MqttWindow *MQTTMainWindow = new MqttWindow(this);
    this->MQTTWindowMap[MQTTMainWindow->id.toString()]=MQTTMainWindow;
    MQTTMainWindow->show();
}

void MainWindow::on_actionAbout_triggered(){
    QMessageBox::about(this, "About", "IoT test tool, <a href=\"https://www.github.com/i4de/rulex\">https://www.github.com/i4de</a>");
}
