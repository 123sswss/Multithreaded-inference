#ifndef VIDEOWIDGET_H
#define VIDEOWIDGET_H

#include <QObject>
#include <QWidget>
#include <QPainter>
#include <QDebug>

class VideoWidget : public QWidget
{
    Q_OBJECT
public:
    explicit VideoWidget(QWidget *parent = nullptr);
    void paintEvent(QPaintEvent *event);

public:
    void setImage(const QImage &img);
private:
    QImage m_currentImage;

};

#endif // VIDEOWIDGET_H
