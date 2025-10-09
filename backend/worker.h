#ifndef WORKER_H
#define WORKER_H

#include <QObject>

class Worker : public QObject
{
    Q_OBJECT
public:
    explicit Worker(QObject *parent = nullptr) : QObject(parent) {}
    virtual ~Worker() {}
public slots:
    virtual void doWork(){};
signals:
    void finished();
};

#endif // WORKER_H

