#ifndef PREPROCESS_H
#define PREPROCESS_H

#include <QObject>
#include <QMutex>
#include <QWaitCondition>

#include "MNN/MNNDefine.h"
#include "MNN/Interpreter.hpp"

#include "protocol.h"
#include "worker.h"
#include "myutils.h"

class Preprocess : public Worker
{
    Q_OBJECT
public:
    Preprocess(QVector<inputImg>* imgList,
               QVector<afterPreprocess>* tensorList,
               QMutex* mutex,
               QWaitCondition* cond,
               QObject* parent=nullptr);

public slots:
    void doWork() override;
    void stop();
    void handleModeChange(bool mode);
private:
    QVector<inputImg>* m_imgListPtr;           // 指向原始图片列表的指针
    QVector<afterPreprocess>* m_tensorListPtr; // 指向处理后结果列表的指针
    QMutex* m_mutexPtr;                        // 指向互斥锁的指针
    QWaitCondition* m_condPtr;                 // 指向等待条件的指针
    bool m_stopped;                            // 控制循环是否继续的开关
    bool localMode;                            // 控制实时模式还是序列模式
};

#endif // PREPROCESS_H
