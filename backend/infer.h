#ifndef INFER_H
#define INFER_H

#include <QObject>
#include <QMutex>
#include <QWaitCondition>

#include "MNN/MNNDefine.h"
#include "MNN/Interpreter.hpp"

#include "protocol.h"
#include "worker.h"
#include "myutils.h"

class Infer : public Worker
{
    Q_OBJECT
public:
    Infer(QVector<afterPreprocess>* inputTensorList,
          QVector<afterInfer>* outPutTensorList,
          std::shared_ptr<MNN::Interpreter> interpreter,
          MNN::Session* session,
          QMutex* mutex,
          QWaitCondition* cond,
          QObject* parent=nullptr);
public slots:
    void doWork() override;
    void stop();
private:
    QVector<afterPreprocess>* m_inputTensorPtr;     // 指向处理后结果列表的指针
    QVector<afterInfer>* m_outputTensorPtr;         // 指向推理后结果列表的指针
    std::shared_ptr<MNN::Interpreter> m_interpreter;// 指向解释器的共享指针
    MNN::Session* m_session;                        // 指向会话的指针
    QMutex* m_mutexPtr;                             // 指向互斥锁的指针
    QWaitCondition* m_condPtr;                      // 指向等待条件的指针
    bool m_stopped;                                 // 控制循环是否继续的开关


};

#endif // INFER_H
