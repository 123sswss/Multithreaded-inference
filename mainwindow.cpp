#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ///////////////////////////////////////////////
    /// 变量设置，以后考虑加入ui或者lua脚本
    numThread = 4;
    ///////////////////////////////////////////////
    init();
}

MainWindow::~MainWindow()
{
    if (cap.isOpened())
    {
        cap.release();
    }
    delete ui;
}

void MainWindow::init()
{
    qRegisterMetaType<cv::Mat>("cv::Mat");
    connect(ui->selectImg, &QPushButton::clicked, this, &MainWindow::selectImg);
    connect(ui->selectFolder, &QPushButton::clicked, this, &MainWindow::selectFolder);
    connect(ui->infer, &QPushButton::clicked, this, &MainWindow::mainThread);
    connect(ui->selectMNN, &QPushButton::clicked, this, &MainWindow::selectMNN);
}

void MainWindow::initCap()
{
    cap.open(0); // 打开默认的摄像头
    if (!cap.isOpened())
    {
        QMessageBox::warning(this, "错误", "摄像头启动失败！");
        return;
    }
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::update_camera_display);
    timer->start(int(1000.0f/Protocol::FPS));
}

// 选择单个文件
void MainWindow::selectImg()
{
    QString filePath = QFileDialog::getOpenFileName(this, tr("选择图片文件"),
                                                    "E:/AAA24105033/project/qt/yoloInfer/weights",
                                                    tr("图片文件 (*.png *.jpg *.jpeg)"));
    inputImg img;
    if (!filePath.isEmpty())
    {
        qDebug() << "选择了文件:" << filePath;
        try
        {
            img.oImg = cv::imread(filePath.toStdString());
        }
        catch (...)
        {
            QMessageBox::warning(this, "错误", "读取图片文件时出现错误");
            return;
        }
    }
    if (img.fastCheck())
    {
        imgList.append(img);
        img.fastVisual();
        if (infering) sharedCondition.wakeAll();
    }
    else
    {
        QMessageBox::warning(this, "错误", "存在空数据，读取错误。");
    }
    qDebug()<<"imgList.length()"<<imgList.length();
}

// 选择文件夹并返回该文件夹一级目录下的所有图片文件路径
void MainWindow::selectFolder()
{
    QString dirPath = QFileDialog::getExistingDirectory(this, tr("选择文件夹"),
                                                        "E:/AAA24105033/project/qt/yoloInfer/weights",
                                                        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    QStringList imgFiles;
    if (!dirPath.isEmpty())
    {
        QDir dir(dirPath);
        QStringList nameFilters;
        nameFilters << "*.png" << "*.jpg" << "*.jpeg";
        QFileInfoList fileList = dir.entryInfoList(nameFilters, QDir::Files | QDir::NoDotAndDotDot);
        for (const QFileInfo &fileInfo : qAsConst(fileList))
        {
            imgFiles << fileInfo.absoluteFilePath();
        }
    }
    if (imgFiles.length())
    {
        for (const QString &imgPath : qAsConst(imgFiles))
        {
            inputImg img;
            try
            {
                img.oImg = cv::imread(imgPath.toStdString());
            }
            catch (...)
            {
                QMessageBox::warning(this, "错误", "读取图片文件时出现错误");
                return;
            }
            if (img.fastCheck())
            {
                imgList.append(img);
                 img.fastVisual();
            }
            else
            {
                QMessageBox::warning(this, "错误", "存在空数据，读取错误。");
            }
        }
    }
    qDebug()<<"imgList.length()"<<imgList.length();
}

void MainWindow::selectMNN()
{
    MNNdir = QFileDialog::getOpenFileName(this, tr("选择MNN模型文件"),
                                                    "E:/AAA24105033/project/qt/yoloInfer/yoloInfer/model",
                                                    tr("模型文件 (*.mnn)"));
    ui->MNNdir->setText(MNNdir);
}

void MainWindow::mainThread()
{
    ui->infer->setDisabled(1);
    infering = 1;
    ui->MNNdir->setReadOnly(1);
    qDebug()<<"加载MNN模型...";
    auto interpreter = std::shared_ptr<MNN::Interpreter>(MNN::Interpreter::createFromFile(MNNdir.toStdString().c_str()));
    qDebug()<<"create interpreter done";
    if (interpreter == nullptr)
    {
        QMessageBox::warning(this, "错误", "模型加载失败，请检查路径和模型文件后重新初始化MNN！");
        ui->infer->setDisabled(0);
        return;
    }
    MNN::ScheduleConfig config;
    config.numThread = numThread; // 可以设置线程数
    MNN::Session* session = nullptr;
    session = interpreter->createSession(config);
    qDebug()<<"create Session done";
    if (session == nullptr)
    {
        QMessageBox::warning(this, "错误", "创建会话失败，请检查相关设置后重新初始化MNN！");
        ui->infer->setDisabled(0);
        return;
    }
    QMessageBox::about(this, "成功", "会话创建成功！");
    task = new Task;
    // 前处理
    Preprocess* preprocessor = new Preprocess(&imgList,
                                              &inputTensorList,
                                              &sharedMutex,
                                              &sharedCondition);
    task->newTask(preprocessor);
    qDebug()<<"create preprocessor done";
    // 推理
    Infer* infer = new Infer(&inputTensorList,
                             &outPutTensorList,
                             interpreter,
                             session,
                             &sharedMutex,
                             &sharedCondition);
    task->newTask(infer);
    qDebug()<<"create infer done";
    // 后处理
    Afterprocess* afterProcessor = new Afterprocess(&outPutTensorList,
                                                    &sharedMutex,
                                                    &sharedCondition);
    task->newTask(afterProcessor);
    qDebug()<<"create afterProcessor done";
    connect(afterProcessor, &Afterprocess::resultImg, this, &MainWindow::visualizeResult);

}

void MainWindow::visualizeResult(const cv::Mat o, const cv::Mat r)
{
    cv::cvtColor(o, copy, cv::COLOR_BGR2RGB);
    QImage oImg(copy.data, copy.cols, copy.rows, copy.step, QImage::Format_RGB888);
    ui->inImg->setPixmap(QPixmap::fromImage(oImg));
    copy.release();
    cv::cvtColor(r, copy, cv::COLOR_BGR2RGB);
    QImage rImg(copy.data, copy.cols, copy.rows, copy.step, QImage::Format_RGB888);
    ui->outImg->setPixmap(QPixmap::fromImage(rImg));
    copy.release();
}

void MainWindow::update_camera_display()
{
    cv::Mat frame;
    cap >> frame; // 从摄像头抓一帧画面

    if (!frame.empty())
    {
        inputImg img;
        img.oImg = frame;
        imgList.append(img);

        // 颜色频道转换 BGR -> RGB
        cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);
        // 把 cv::Mat 转成 QImage
        QImage qimg(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);
        ui->inImg->setPixmap(QPixmap::fromImage(qimg));
        ui->inImg->setScaledContents(true); // 让图像自适应QLabel大小
    }
}








































