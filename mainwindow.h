#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QDebug>
#include <QMessageBox>
#include <QMutex>
#include <QWaitCondition>
#include <QTimer>

#include "MNN/Interpreter.hpp"
#include "MNN/MNNDefine.h"
#include "MNN/Tensor.hpp"
#include "MNN/ImageProcess.hpp"

#include <opencv2/opencv.hpp>

#include "backend/protocol.h"
#include "backend/task.h"
#include "backend/preprocess.h"
#include "backend/infer.h"
#include "backend/afterprocess.h"
#include "backend/videoWidget.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private:
    void init();
public:
    // ///////////////////////////////
    // 流水线专用 //
    QVector<inputImg> imgList;
    QVector<afterPreprocess> inputTensorList;
    QVector<afterInfer> outPutTensorList;
    // 线程管理专用 //
    QMutex sharedMutex;
    QWaitCondition sharedCondition;  
    // MNN专用
    int numThread;
    MNN::Tensor host_input_tensor;
    MNN::Tensor host_output_tensor;
    // ///////////////////////////////
private:
    Ui::MainWindow *ui;
    QTimer* timer;
    cv::VideoCapture cap;
    Task* task;
    cv::Mat copy; // copy防止源被释放
    QString MNNdir;
    bool infering = 0;
    bool taskMode = LIST_MODE;
private slots:
    void selectImg();
    void selectFolder();
    void selectVideo();
    void mainThread();
    void visualizeResult(cv::Mat r);
    void selectMNN();

    void update_camera_display();
    void initCap();
    void deleteCap();
signals:
    void modeChanged(bool mode);
};

#endif // MAINWINDOW_H
