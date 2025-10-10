#include "videoWidget.h"

VideoWidget::VideoWidget(QWidget *parent) : QWidget(parent)
{

}

void VideoWidget::setImage(const QImage &img)
{
    if (!img.isNull())
    {
        m_currentImage = img;
        m_scaledImage = m_currentImage.scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
    else
    {
        m_currentImage = QImage();
        m_scaledImage = QImage();
    }
    update();
}

void VideoWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    if (!m_scaledImage.isNull())
    {
        int x = (width() - m_scaledImage.width()) / 2;
        int y = (height() - m_scaledImage.height()) / 2;

        painter.drawImage(x, y, m_scaledImage);
    }
    else
    {
        painter.fillRect(rect(), Qt::black);
    }
}

void VideoWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    if (!m_currentImage.isNull())
    {
        m_scaledImage = m_currentImage.scaled(event->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
}
