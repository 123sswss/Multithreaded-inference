#ifndef VIDEOWIDGET_H
#define VIDEOWIDGET_H

#include <QObject>
#include <QWidget>
#include <QPainter>
#include <QDebug>
#include <QResizeEvent>

class VideoWidget : public QWidget
{
    Q_OBJECT
public:
    explicit VideoWidget(QWidget *parent = nullptr);
    void setImage(const QImage &img);

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    QImage m_currentImage;
    QImage m_scaledImage;

};

#endif // VIDEOWIDGET_H
