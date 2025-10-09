#include "afterprocess.h"

Afterprocess::Afterprocess(QVector<afterInfer>* outPutTensorList,
                           QMutex* mutex,
                           QWaitCondition* cond,
                           QObject *parent)
    : Worker(parent),
      m_outputTensorPtr(outPutTensorList),
      m_mutexPtr(mutex),
      m_condPtr(cond),
      m_stopped(false)
{

}

void Afterprocess::stop()
{
    QMutexLocker locker(m_mutexPtr);
    m_stopped = true;
    m_condPtr->wakeOne(); // 叫醒可能正在睡觉的线程，让它看到 m_stopped 变成 true 了然后自己退出喵
}

void Afterprocess::doWork()
{
    while (1)
    {
        m_mutexPtr->lock();

        while (m_outputTensorPtr->isEmpty() && !m_stopped)
        {
            qDebug() << "没有tensor了";
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

        afterInfer currentResult = m_outputTensorPtr->first();
        currentResult.fastVisual();

        cv::Mat ooImg = currentResult.oImg.clone();
        m_outputTensorPtr->removeFirst();
        m_mutexPtr->unlock();

        // 开始后处理 //
        auto detections = myutils::non_max_suppression(currentResult.hostOutputTensor,
                                                       CONF_THRESHOLD,
                                                       NMS_THRESHOLD);
        if (!detections.empty())
        {
            myutils::scale_coords(detections, currentResult);

            qDebug() << "检测到 " << detections.size() << " 个目标。";

            for (const auto& det : detections)
            {
                qDebug() << "坐标：" << det.box.x << det.box.y << det.box.width << det.box.height;
                cv::rectangle(currentResult.oImg, det.box, cv::Scalar(0, 255, 0), 2);
                std::string label = Protocol::class_names[det.classId] + " " + cv::format("%.2f", det.confidence);
                cv::putText(currentResult.oImg, label, cv::Point(det.box.x, det.box.y - 10), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 0), 2);
            }
        }
        else
        {
            qDebug() << "在当前阈值下，未检测到任何目标。";
        }

        // 后处理结束 //
        delete currentResult.hostOutputTensor;
        emit resultImg(ooImg, currentResult.oImg);
        m_condPtr->wakeAll();
    }
    emit finished();
}
