/********************************************************************************
** Form generated from reading UI file 'MainWindowckOkSJ.ui'
**
** Created by: Qt User Interface Compiler version 6.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef MAINWINDOWCKOKSJ_H
#define MAINWINDOWCKOKSJ_H

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
    QPushButton *pushButton;
    QLabel *label;
    QLabel *ip_label;
    QLabel *port_label;
    QLabel *connected_status_label;
    QLabel *label_5;
    QComboBox *comboBox;

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
        pushButton = new QPushButton(controlContainers);
        pushButton->setObjectName("pushButton");
        pushButton->setGeometry(QRect(430, 40, 141, 61));
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
        comboBox = new QComboBox(controlContainers);
        comboBox->addItem(QString());
        comboBox->addItem(QString());
        comboBox->addItem(QString());
        comboBox->setObjectName("comboBox");
        comboBox->setGeometry(QRect(910, 20, 69, 22));
        comboBox->setEditable(false);

        verticalLayout->addWidget(controlContainers);


        retranslateUi(MainWidget);

        QMetaObject::connectSlotsByName(MainWidget);
    } // setupUi

    void retranslateUi(QWidget *MainWidget)
    {
        MainWidget->setWindowTitle(QCoreApplication::translate("MainWidget", "Form", nullptr));
        pushButton->setText(QCoreApplication::translate("MainWidget", "Start Preview", nullptr));
        label->setText(QCoreApplication::translate("MainWidget", "Connect from client with these address:", nullptr));
        ip_label->setText(QCoreApplication::translate("MainWidget", "IP: N/A", nullptr));
        port_label->setText(QCoreApplication::translate("MainWidget", "PORT: N/A", nullptr));
        connected_status_label->setText(QCoreApplication::translate("MainWidget", "Current Status: Not Connected", nullptr));
        label_5->setText(QCoreApplication::translate("MainWidget", "Frame Rate:", nullptr));
        comboBox->setItemText(0, QCoreApplication::translate("MainWidget", "60", nullptr));
        comboBox->setItemText(1, QCoreApplication::translate("MainWidget", "30", nullptr));
        comboBox->setItemText(2, QCoreApplication::translate("MainWidget", "15", nullptr));

    } // retranslateUi

};

namespace Ui {
    class MainWidget: public Ui_MainWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // MAINWINDOWCKOKSJ_H
