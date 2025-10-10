#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <QString>
#include <QImage>
#include <QDebug>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>

#include "MNN/MNNDefine.h"
#include "MNN/Interpreter.hpp"

#define INPUT_HEIGHT    640.0f
#define INPUT_WIDTH     640.0f
#define CONF_THRESHOLD  0.1f
#define NMS_THRESHOLD   0.35f
#define MAX_LIST_LEN    16

using namespace cv;

class Protocol
{
public:
    inline static const QVector<std::string> class_names =
    {
        "person", "bicycle", "car", "motorcycle", "airplane", "bus", "train", "truck", "boat", "traffic light",
        "fire hydrant", "stop sign", "parking meter", "bench", "bird", "cat", "dog", "horse", "sheep", "cow",
        "elephant", "bear", "zebra", "giraffe", "backpack", "umbrella", "handbag", "tie", "suitcase", "frisbee",
        "skis", "snowboard", "sports ball", "kite", "baseball bat", "baseball glove", "skateboard", "surfboard",
        "tennis racket", "bottle", "wine glass", "cup", "fork", "knife", "spoon", "bowl", "banana", "apple",
        "sandwich", "orange", "broccoli", "carrot", "hot dog", "pizza", "donut", "cake", "chair", "couch",
        "potted plant", "bed", "dining table", "toilet", "tv", "laptop", "mouse", "remote", "keyboard",
        "cell phone", "microwave", "oven", "toaster", "sink", "refrigerator", "book", "clock", "vase",
        "scissors", "teddy bear", "hair drier", "toothbrush"
    };
    static const int FPS = 30;
};

struct inputImg
{
    cv::Mat oImg;

    bool resized = 0;

    bool fastCheck() const
    {
        if (oImg.empty())
            return false;
        return true;
    }

    void fastVisual()
    {
        qDebug()<<"img.size:"<<oImg.size().height<<oImg.size().width;
    }  
};

struct afterPreprocess
{
    // MNN::Tensor* hostInputTensor=nullptr;
    std::vector<float> inputData;
    cv::Mat oImg;
    float ratio;
    cv::Point2f pad;

    bool fastCheck() const
    {
        if (!inputData.size())
            return false;
        if (oImg.empty())
            return false;
        return true;
    }

    void fastVisual()
    {
        qDebug()<<"inputData.length():"<<inputData.size();
        qDebug()<<"oImg.size:"<<oImg.size().height<<oImg.size().width;
    }
};

struct afterInfer
{
    MNN::Tensor* hostOutputTensor=nullptr;
    cv::Mat oImg;
    float ratio;
    cv::Point2f pad;

    bool fastCheck() const
    {
        if (hostOutputTensor==nullptr)
            return false;
        if (oImg.empty())
            return false;
        return true;
    }

    void fastVisual()
    {
        qDebug()<<"hostTensor.shape:"<<hostOutputTensor->shape();
        qDebug()<<"oImg.size:"<<oImg.size().height<<oImg.size().width;
    }
};

struct Detection
{
    cv::Rect box;
    float confidence;
    int classId;
};
#endif // PROTOCOL_H


