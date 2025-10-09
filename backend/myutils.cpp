#include "myutils.h"

myutils::myutils(QObject *parent) : QObject(parent)
{

}

void myutils::letterbox(cv::Mat& img_ptr, float& ratio, cv::Point2f& pad, float input_height, float input_width)
{
    cv::Mat& src = img_ptr;
    int src_w = src.cols;
    int src_h = src.rows;

    // r 的计算不变喵
    ratio = std::min((float)input_height / src_h, (float)input_width / src_w);
    int new_unpad_w = static_cast<int>(round(src_w * ratio));
    int new_unpad_h = static_cast<int>(round(src_h * ratio));

    float dw = (input_width - new_unpad_w) / 2.0f;
    float dh = (input_height - new_unpad_h) / 2.0f;

    // 把 dw 和 dh 存到 pad 里传出去喵！
    pad.x = dw;
    pad.y = dh;

    cv::Mat resized_img;
    cv::resize(src, resized_img, cv::Size(new_unpad_w, new_unpad_h));

    int top = static_cast<int>(round(dh - 0.1f));
    int bottom = static_cast<int>(round(dh + 0.1f));
    int left = static_cast<int>(round(dw - 0.1f));
    int right = static_cast<int>(round(dw + 0.1f));

    cv::Mat out_img;
    cv::copyMakeBorder(resized_img, out_img, top, bottom, left, right, cv::BORDER_CONSTANT, cv::Scalar(114, 114, 114));

    img_ptr = out_img;
}

std::vector<Detection> myutils::non_max_suppression(const MNN::Tensor* outputTensor, float conf_threshold, float nms_threshold)
{
    std::vector<Detection> detections;
    auto tensor_data = outputTensor->host<float>();

    // 获取输出的形状信息
    int num_proposals = outputTensor->shape()[1]; // 25200
    int num_attrs = outputTensor->shape()[2];    // 85

    std::vector<cv::Rect> boxes;
    std::vector<float> confs;
    std::vector<int> class_ids;

    for (int i = 0; i < num_proposals; ++i)
    {
        float* row = tensor_data + i * num_attrs;
        float confidence = row[4];

        if (confidence < conf_threshold)
        {
            continue;
        }

        // 找到得分最高的类别
        auto max_score_iter = std::max_element(row + 5, row + num_attrs);
        float class_score = *max_score_iter;
        int class_id = std::distance(row + 5, max_score_iter);

        if (class_score * confidence < conf_threshold)
        {
            continue;
        }

        float cx = row[0];
        float cy = row[1];
        float w = row[2];
        float h = row[3];

        int left = static_cast<int>(cx - w / 2.0f);
        int top = static_cast<int>(cy - h / 2.0f);

        boxes.emplace_back(left, top, static_cast<int>(w), static_cast<int>(h));
        confs.emplace_back(confidence * class_score);
        class_ids.emplace_back(class_id);
    }

    std::vector<int> indices;
    cv::dnn::NMSBoxes(boxes, confs, conf_threshold, nms_threshold, indices);

    for (int idx : indices) {
        detections.push_back({ boxes[idx], confs[idx], class_ids[idx] });
    }

    return detections;
}

void myutils::scale_coords(std::vector<Detection>& detections, const afterInfer& infer_result)
{
    cv::Size original_shape = infer_result.oImg.size();
    float r = infer_result.ratio;
    cv::Point2f pad = infer_result.pad;

    for (auto& det : detections)
    {
        // 这里的 pad.x 就是 dw, pad.y 就是 dh 喵
        det.box.x = static_cast<int>((det.box.x - pad.x) / r);
        det.box.y = static_cast<int>((det.box.y - pad.y) / r);
        det.box.width = static_cast<int>(det.box.width / r);
        det.box.height = static_cast<int>(det.box.height / r);

        // 裁剪坐标
        det.box.x = std::max(0, std::min(det.box.x, original_shape.width));
        det.box.y = std::max(0, std::min(det.box.y, original_shape.height));
        det.box.width = std::max(0, std::min(det.box.width, original_shape.width - det.box.x));
        det.box.height = std::max(0, std::min(det.box.height, original_shape.height - det.box.y));
    }
}
