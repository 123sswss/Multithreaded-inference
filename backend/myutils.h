#ifndef MYUTILS_H
#define MYUTILS_H

#include <QObject>
#include <opencv2/opencv.hpp>

#include <MNN/Tensor.hpp>

#include "protocol.h"

class myutils : public QObject
{
    Q_OBJECT
public:
    explicit myutils(QObject *parent = nullptr);
public:
   static void letterbox(cv::Mat& img_ptr, float& ratio, cv::Point2f& pad, float input_height, float input_width);
   static std::vector<Detection> non_max_suppression(const MNN::Tensor* outputTensor, float conf_threshold, float nms_threshold);
   static void scale_coords(std::vector<Detection>& detections, const afterInfer& infer_result);

};

#endif // MYUTILS_H
