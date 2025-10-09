#ifndef AFTERPROCESS_H
#define AFTERPROCESS_H

#include <QObject>
#include <QMutex>
#include <QWaitCondition>

#include "MNN/MNNDefine.h"
#include "MNN/Interpreter.hpp"

#include "protocol.h"
#include "worker.h"
#include "myutils.h"

class Afterprocess : public Worker
{
    Q_OBJECT
public:
    explicit Afterprocess(QVector<afterInfer>* outPutTensorList,
                          QMutex* mutex,
                          QWaitCondition* cond,
                          QObject *parent = nullptr);
public slots:
    void doWork() override;
    void stop();
private:
    QVector<afterInfer>* m_outputTensorPtr;         // 指向推理后结果列表的指针
    QMutex* m_mutexPtr;                             // 指向互斥锁的指针
    QWaitCondition* m_condPtr;                      // 指向等待条件的指针
    bool m_stopped;                                 // 控制循环是否继续的开关
signals:
    void resultImg(cv::Mat o, cv::Mat r);
};

#endif // AFTERPROCESS_H
