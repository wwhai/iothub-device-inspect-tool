#ifndef ADDSUBSCRIBEDIALOG_H
#define ADDSUBSCRIBEDIALOG_H

#include <QDialog>

namespace Ui {
class AddSubscribeDialog;
}

class AddSubscribeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddSubscribeDialog(QWidget *parent = nullptr);
    ~AddSubscribeDialog();
signals:
    void getSubscribeTopicEntry(QString topic, int qos);

private slots:
    void on_ensureSubscribeButton_clicked();

private:
    Ui::AddSubscribeDialog *ui;
};

#endif // ADDSUBSCRIBEDIALOG_H
