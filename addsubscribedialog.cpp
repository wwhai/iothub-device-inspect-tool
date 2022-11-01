#include "addsubscribedialog.h"
#include "ui_addsubscribedialog.h"
#include <QDebug>
AddSubscribeDialog::AddSubscribeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddSubscribeDialog)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose, true);
}

AddSubscribeDialog::~AddSubscribeDialog()
{
    qDebug()<< "AddSubscribeDialog destoryed";
    delete ui;
}

void AddSubscribeDialog::on_ensureSubscribeButton_clicked()
{
    this->ui->topicEdit->text();
    emit getSubscribeTopicEntry(this->ui->topicEdit->text(), this->ui->qosSpin->value());
}

