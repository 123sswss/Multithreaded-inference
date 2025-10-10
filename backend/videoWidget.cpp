#include "videoWidget.h"

VideoWidget::VideoWidget(QWidget *parent) : QWidget(parent)
{

}

void VideoWidget::setImage(const QImage &img)
{
    m_currentImage = img;
    update();
}

void VideoWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    if (!m_currentImage.isNull())
    {
        QImage scaledImage = m_currentImage.scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        int x = (width() - scaledImage.width()) / 2;
        int y = (height() - scaledImage.height()) / 2;

        painter.drawImage(x, y, scaledImage);
    }
    else
    {
        painter.fillRect(rect(), Qt::black);
    }
}
