#ifndef WORKER_H
#define WORKER_H

#include <QObject>

class Worker : public QObject
{
    Q_OBJECT

public:
    Worker();
    ~Worker();

public slots:
    void process();

signals:
    void finished();
    void error(QString err);

private:
    bool _stop;
};

#endif // WORKER_H
