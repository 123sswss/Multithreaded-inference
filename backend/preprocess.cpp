#include "preprocess.h"

Preprocess::Preprocess(QVector<inputImg>* imgList,
                       QVector<afterPreprocess>* tensorList,
                       QMutex* mutex,
                       QWaitCondition* cond,
                       QObject* parent)
    : Worker(parent),
      m_imgListPtr(imgList),
      m_tensorListPtr(tensorList),
      m_mutexPtr(mutex),
      m_condPtr(cond),
      m_stopped(false)
{

}

void Preprocess::stop()
{
    QMutexLocker locker(m_mutexPtr);
    m_stopped = true;
    m_condPtr->wakeOne(); // 叫醒可能正在睡觉的线程，让它看到 m_stopped 变成 true 了然后自己退出喵
}

void Preprocess::doWork()
{
    while (1)
    {
        m_mutexPtr->lock();

        while (m_imgListPtr->isEmpty() && !m_stopped)
        {
            // qDebug() << "没有图片了";
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

        inputImg currentImg = m_imgListPtr->first();
        // currentImg.fastVisual();

        m_imgListPtr->removeFirst();
        m_mutexPtr->unlock();

        // 开始处理图片 //
        auto ooImg = currentImg.oImg.clone();
        afterPreprocess tmp;
        // tmp.hostInputTensor = MNN::Tensor::create<float>({1, 3, (int)INPUT_HEIGHT, (int)INPUT_WIDTH}, NULL);

        // 调用新的 letterbox 函数喵！
        myutils::letterbox(currentImg.oImg, tmp.ratio, tmp.pad, float(INPUT_HEIGHT), float(INPUT_WIDTH));
        cv::Mat processed_image;
        cv::cvtColor(currentImg.oImg, processed_image, cv::COLOR_BGR2RGB);
        processed_image.convertTo(processed_image, CV_32FC3, 1.0f / 255.0f);

        if (!processed_image.isContinuous())
        {
            processed_image = processed_image.clone();
        }

        // HWC to CHW
        std::vector<float> data(INPUT_WIDTH * INPUT_HEIGHT * 3);
        const float* source_data = processed_image.ptr<float>(0);
        for (int i = 0; i < INPUT_HEIGHT * INPUT_WIDTH; ++i)
        {
            for (int c = 0; c < 3; ++c)
            {
                data[c * (INPUT_WIDTH * INPUT_HEIGHT) + i] = source_data[i * 3 + c];
            }
        }
        // memcpy(tmp.hostInputTensor->host<float>(), data.data(), data.size() * sizeof(float));
        tmp.inputData = std::move(data);
        tmp.oImg = ooImg;

        // 处理图片结束  //
        m_mutexPtr->lock();
        m_tensorListPtr->append(tmp);
        m_mutexPtr->unlock();
        m_condPtr->wakeOne();
    }
    emit finished();
}
