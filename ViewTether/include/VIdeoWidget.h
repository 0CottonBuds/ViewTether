#pragma once
#include <QWidget>
#include <QPainter>

class VideoWidget : public QWidget { 
public:
    VideoWidget(QWidget *parent = nullptr) : QWidget(parent) {
    }

public slots:
    void removeCurrentImage() {
        currentImage = blankImage;
        update();
    }

    void updateImage(std::shared_ptr<QImage> image) {
        currentImage = std::move(image);
        update();
    }

protected:
    void paintEvent(QPaintEvent *event) override {
		QPainter painter = QPainter(this);
        if (currentImage && !currentImage->isNull()) {
			painter.drawImage(rect(), *currentImage, currentImage->rect());
        }
        else {
			painter.drawImage(rect(), *blankImage, blankImage->rect());
        }
    }

private:
    std::shared_ptr<QImage> currentImage;
    std::shared_ptr<QImage> blankImage = std::shared_ptr<QImage>(new QImage());
};

