#include "locallogger.h"

LocalLogger::LocalLogger(QObject *parent)
    : QObject{parent}
{

}

void logger(QTextBrowser *t, QString s)
{
    qDebug() << s;
    t->append(QString("[<strong><font color=\"#FF0000\">%1</font></strong>] -> <font color=\"#00FFD1\">%2</font>")
              .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss:zzz"))
              .arg(s));
}
