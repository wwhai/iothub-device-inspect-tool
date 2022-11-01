#ifndef LOCALLOGGER_H
#define LOCALLOGGER_H

#include <QObject>

class LocalLogger : public QObject
{
    Q_OBJECT
public:
    explicit LocalLogger(QObject *parent = nullptr);

signals:

};

#endif // LOCALLOGGER_H
