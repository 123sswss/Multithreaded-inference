#include "infer.h"

Infer::Infer(QVector<afterPreprocess>* inputTensorList,
             QVector<afterInfer>* outPutTensorList,
             std::shared_ptr<MNN::Interpreter> interpreter,
             MNN::Session* session,
             QMutex* mutex,
             QWaitCondition* cond,
             QObject* parent)
    : Worker(parent),
      m_inputTensorPtr(inputTensorList),
      m_outputTensorPtr(outPutTensorList),
      m_interpreter(interpreter),
      m_session(session),
      m_mutexPtr(mutex),
      m_condPtr(cond),
      m_stopped(false)
{

}

void Infer::stop()
{
    QMutexLocker locker(m_mutexPtr);
    m_stopped = true;
    m_condPtr->wakeOne(); // 叫醒可能正在睡觉的线程，让它看到 m_stopped 变成 true 了然后自己退出喵
}

void Infer::doWork()
{
    while (1)
    {
        m_mutexPtr->lock();

        while (m_inputTensorPtr->isEmpty() && !m_stopped)
        {
            // qDebug() << "没有tensor了";
            // 如果列表是空的，就调用 wait()，它会：
            // a. 自动解开 m_mutexPtr 这个锁
            // b. 让线程在这里睡觉
            // c. 被唤醒后，会自动重新锁上 m_mutexPtr，然后继续执行
            m_condPtr->wait(m_mutexPtr);
        }
        if (m_stopped) {
            m_mutexPtr->unlock();
            break;
        }

        afterPreprocess currentTensor = m_inputTensorPtr->first();

        m_inputTensorPtr->removeFirst();
        m_mutexPtr->unlock();
        afterInfer tmp;
        // 开始推理 //
        auto input_tensor = m_interpreter->getSessionInput(m_session, nullptr);
        auto hostInputTensor = new MNN::Tensor(input_tensor, MNN::Tensor::CAFFE);
        // 把预处理传过来的数据拷贝进去
        memcpy(hostInputTensor->host<float>(), currentTensor.inputData.data(), currentTensor.inputData.size() * sizeof(float));
        input_tensor->copyFromHostTensor(hostInputTensor);
        // input_tensor->copyFromHostTensor(currentTensor.hostInputTensor);
        m_interpreter->runSession(m_session);

        // auto output_tensor = m_interpreter->getSessionOutput(m_session, nullptr);
        // tmp.hostOutputTensor = new MNN::Tensor(output_tensor, MNN::Tensor::CAFFE, true);
        // output_tensor->copyToHostTensor(tmp.hostOutputTensor);
        auto output_tensor = m_interpreter->getSessionOutput(m_session, nullptr);
        tmp.hostOutputTensor = new MNN::Tensor(output_tensor, MNN::Tensor::CAFFE);
        output_tensor->copyToHostTensor(tmp.hostOutputTensor);
        tmp.oImg = currentTensor.oImg;
        tmp.ratio = currentTensor.ratio;
        tmp.pad = currentTensor.pad;
        // 推理结束 //
        m_mutexPtr->lock();
        m_outputTensorPtr->append(tmp);
        m_mutexPtr->unlock();
        delete hostInputTensor;
        m_condPtr->wakeOne();
    }
    m_interpreter->releaseSession(m_session);
    emit finished();
}
















