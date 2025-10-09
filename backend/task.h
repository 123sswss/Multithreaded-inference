#ifndef TASK_H
#define TASK_H

#include <QObject>
#include <QThread>
#include <vector>

#include "worker.h"

class Task : public QObject
{
    Q_OBJECT
public:
    explicit Task(QObject *parent = nullptr);
    ~Task();

private:
    std::vector<std::pair<QThread*, Worker*>> taskList;

public:
    void newTask(Worker* worker);           ///< 新建—个task线程
    void deleteTask(int idx);               ///< 删除—个task线程
    void pause(int index, bool val);        ///< 暂停或者继续执行task

signals:

public slots:
};

#endif // TASK_H
