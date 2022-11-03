#ifndef LOCALLOGGER_H
#define LOCALLOGGER_H

#include <QObject>
#include <QtDebug>
#include <QTextBrowser>
#include <QDateTime>
class LocalLogger : public QObject
{
    Q_OBJECT
public:
    explicit LocalLogger(QObject *parent = nullptr);

signals:

};
void logger(QTextBrowser *t, QString s);
#endif // LOCALLOGGER_H
