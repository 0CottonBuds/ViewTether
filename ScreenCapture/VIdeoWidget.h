#pragma once
#include <QWidget>
#include <QPainter>

class VideoWidget : public QWidget { 
public:
    VideoWidget(QWidget *parent = nullptr) : QWidget(parent) {
    }

    void removeCurrentImage() {
        currentImage = blankImage;
        update();
    }

    void updateImage(const QImage *image) {
        currentImage = *image;
        update(); // Schedule a repaint
    }

protected:
    void paintEvent(QPaintEvent *event) override {
		QPainter painter = QPainter(this);
        painter.drawImage(rect(), currentImage, currentImage.rect());
    }

private:
    QImage currentImage;
    QImage blankImage = QImage();
};

