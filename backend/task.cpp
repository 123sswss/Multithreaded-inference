#include "task.h"
#include <QDebug>

Task::Task(QObject *parent) : QObject(parent)
{

}

Task::~Task()
{

}

void Task::newTask(Worker* worker)
{
    QThread* thread = new QThread();
    worker->moveToThread(thread);
    connect(thread, &QThread::started, worker, &Worker::doWork);
    connect(worker, &Worker::finished, thread, &QThread::quit);
    connect(worker, &Worker::finished, worker, &QObject::deleteLater);
    connect(thread, &QThread::finished, thread, &QObject::deleteLater);
    taskList.push_back(std::make_pair(thread, worker));
    thread->start();
}

void Task::deleteTask(int index)
{
    if (index < 0 || index >= (int)taskList.size())
        return;
    auto& task = taskList[index];
    QThread* thread = task.first;
    Worker* worker = task.second;
    // 让worker安全结束
    QMetaObject::invokeMethod(worker, "stopWork", Qt::QueuedConnection);
    thread->quit();
    thread->wait();
    taskList.erase(taskList.begin() + index);
}

void Task::pause(int index, bool val)
{
    if (index < 0 || index >= (int)taskList.size())
        return;
    auto& task = taskList[index];
//    QThread* thread = task.first;
    Worker* worker = task.second;
    QMetaObject::invokeMethod(worker, val ? "pauseWork" : "resumeWork", Qt::QueuedConnection);
}
