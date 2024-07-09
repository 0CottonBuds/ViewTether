/********************************************************************************
** Form generated from reading UI file 'MainWindowlTIffX.ui'
**
** Created by: Qt User Interface Compiler version 6.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef MAINWINDOWLTIFFX_H
#define MAINWINDOWLTIFFX_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWidget
{
public:
    QVBoxLayout *verticalLayout;
    QWidget *previewContainer;
    QWidget *controlContainers;
    QPushButton *startButton;
    QLabel *label;
    QLabel *ip_label;
    QLabel *port_label;
    QLabel *connected_status_label;
    QLabel *label_5;
    QComboBox *frameRateComboBox;
    QLabel *label_2;
    QLabel *label_3;
    QComboBox *adapterComboBox;
    QComboBox *outputComboBox;

    void setupUi(QWidget *MainWidget)
    {
        if (MainWidget->objectName().isEmpty())
            MainWidget->setObjectName("MainWidget");
        MainWidget->resize(1000, 700);
        MainWidget->setMinimumSize(QSize(1000, 700));
        MainWidget->setMaximumSize(QSize(1000, 700));
        verticalLayout = new QVBoxLayout(MainWidget);
        verticalLayout->setObjectName("verticalLayout");
        previewContainer = new QWidget(MainWidget);
        previewContainer->setObjectName("previewContainer");

        verticalLayout->addWidget(previewContainer);

        controlContainers = new QWidget(MainWidget);
        controlContainers->setObjectName("controlContainers");
        controlContainers->setMinimumSize(QSize(0, 150));
        controlContainers->setMaximumSize(QSize(16777215, 150));
        startButton = new QPushButton(controlContainers);
        startButton->setObjectName("startButton");
        startButton->setGeometry(QRect(430, 40, 141, 61));
        label = new QLabel(controlContainers);
        label->setObjectName("label");
        label->setGeometry(QRect(10, 20, 401, 16));
        ip_label = new QLabel(controlContainers);
        ip_label->setObjectName("ip_label");
        ip_label->setGeometry(QRect(10, 40, 391, 16));
        port_label = new QLabel(controlContainers);
        port_label->setObjectName("port_label");
        port_label->setGeometry(QRect(10, 50, 381, 20));
        connected_status_label = new QLabel(controlContainers);
        connected_status_label->setObjectName("connected_status_label");
        connected_status_label->setGeometry(QRect(10, 120, 341, 16));
        label_5 = new QLabel(controlContainers);
        label_5->setObjectName("label_5");
        label_5->setGeometry(QRect(840, 20, 61, 16));
        frameRateComboBox = new QComboBox(controlContainers);
        frameRateComboBox->addItem(QString());
        frameRateComboBox->addItem(QString());
        frameRateComboBox->addItem(QString());
        frameRateComboBox->setObjectName("frameRateComboBox");
        frameRateComboBox->setGeometry(QRect(910, 20, 69, 22));
        frameRateComboBox->setEditable(false);
        label_2 = new QLabel(controlContainers);
        label_2->setObjectName("label_2");
        label_2->setGeometry(QRect(840, 50, 61, 21));
        label_3 = new QLabel(controlContainers);
        label_3->setObjectName("label_3");
        label_3->setGeometry(QRect(840, 80, 61, 21));
        adapterComboBox = new QComboBox(controlContainers);
        adapterComboBox->setObjectName("adapterComboBox");
        adapterComboBox->setGeometry(QRect(910, 50, 69, 22));
        adapterComboBox->setEditable(false);
        adapterComboBox->setDuplicatesEnabled(true);
        outputComboBox = new QComboBox(controlContainers);
        outputComboBox->setObjectName("outputComboBox");
        outputComboBox->setGeometry(QRect(910, 80, 69, 22));
        outputComboBox->setEditable(false);
        outputComboBox->setDuplicatesEnabled(true);

        verticalLayout->addWidget(controlContainers);


        retranslateUi(MainWidget);

        QMetaObject::connectSlotsByName(MainWidget);
    } // setupUi

    void retranslateUi(QWidget *MainWidget)
    {
        MainWidget->setWindowTitle(QCoreApplication::translate("MainWidget", "Form", nullptr));
        startButton->setText(QCoreApplication::translate("MainWidget", "Start Streaming", nullptr));
        label->setText(QCoreApplication::translate("MainWidget", "Connect from client with these address:", nullptr));
        ip_label->setText(QCoreApplication::translate("MainWidget", "IP: N/A", nullptr));
        port_label->setText(QCoreApplication::translate("MainWidget", "PORT: N/A", nullptr));
        connected_status_label->setText(QCoreApplication::translate("MainWidget", "Current Status: Not Connected", nullptr));
        label_5->setText(QCoreApplication::translate("MainWidget", "Frame Rate:", nullptr));
        frameRateComboBox->setItemText(0, QCoreApplication::translate("MainWidget", "60", nullptr));
        frameRateComboBox->setItemText(1, QCoreApplication::translate("MainWidget", "30", nullptr));
        frameRateComboBox->setItemText(2, QCoreApplication::translate("MainWidget", "15", nullptr));

        label_2->setText(QCoreApplication::translate("MainWidget", "Adapter: ", nullptr));
        label_3->setText(QCoreApplication::translate("MainWidget", "Outputs:", nullptr));
        adapterComboBox->setCurrentText(QString());
        adapterComboBox->setPlaceholderText(QCoreApplication::translate("MainWidget", "NULL", nullptr));
        outputComboBox->setPlaceholderText(QCoreApplication::translate("MainWidget", "NULL", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWidget: public Ui_MainWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // MAINWINDOWLTIFFX_H
