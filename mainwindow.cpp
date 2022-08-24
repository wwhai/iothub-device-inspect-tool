#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qmqtt/include/QtQmqtt/qmqtt.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    new QMQTT::Client(QHostAddress("192.168.1.10"),1883);
}

MainWindow::~MainWindow()
{
    delete ui;
}

