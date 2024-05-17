/********************************************************************************
** Form generated from reading UI file 'MainWindowPUJzTE.ui'
**
** Created by: Qt User Interface Compiler version 5.12.12
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef MAINWINDOWPUJZTE_H
#define MAINWINDOWPUJZTE_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
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
    QLineEdit *lineEdit;
    QLabel *label_5;

    void setupUi(QWidget *MainWidget)
    {
        if (MainWidget->objectName().isEmpty())
            MainWidget->setObjectName(QString::fromUtf8("MainWidget"));
        MainWidget->resize(1000, 700);
        MainWidget->setMinimumSize(QSize(1000, 700));
        MainWidget->setMaximumSize(QSize(1000, 700));
        verticalLayout = new QVBoxLayout(MainWidget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        previewContainer = new QWidget(MainWidget);
        previewContainer->setObjectName(QString::fromUtf8("previewContainer"));

        verticalLayout->addWidget(previewContainer);

        controlContainers = new QWidget(MainWidget);
        controlContainers->setObjectName(QString::fromUtf8("controlContainers"));
        controlContainers->setMinimumSize(QSize(0, 150));
        controlContainers->setMaximumSize(QSize(16777215, 150));
        pushButton = new QPushButton(controlContainers);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setGeometry(QRect(430, 40, 141, 61));
        label = new QLabel(controlContainers);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(10, 20, 191, 16));
        ip_label = new QLabel(controlContainers);
        ip_label->setObjectName(QString::fromUtf8("ip_label"));
        ip_label->setGeometry(QRect(10, 40, 47, 13));
        port_label = new QLabel(controlContainers);
        port_label->setObjectName(QString::fromUtf8("port_label"));
        port_label->setGeometry(QRect(10, 50, 71, 20));
        connected_status_label = new QLabel(controlContainers);
        connected_status_label->setObjectName(QString::fromUtf8("connected_status_label"));
        connected_status_label->setGeometry(QRect(10, 120, 151, 16));
        lineEdit = new QLineEdit(controlContainers);
        lineEdit->setObjectName(QString::fromUtf8("lineEdit"));
        lineEdit->setGeometry(QRect(860, 40, 113, 20));
        label_5 = new QLabel(controlContainers);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(800, 40, 61, 16));

        verticalLayout->addWidget(controlContainers);


        retranslateUi(MainWidget);

        QMetaObject::connectSlotsByName(MainWidget);
    } // setupUi

    void retranslateUi(QWidget *MainWidget)
    {
        MainWidget->setWindowTitle(QApplication::translate("MainWidget", "Form", nullptr));
        pushButton->setText(QApplication::translate("MainWidget", "Start Preview", nullptr));
        label->setText(QApplication::translate("MainWidget", "Connect from client with these address:", nullptr));
        ip_label->setText(QApplication::translate("MainWidget", "IP: N/A", nullptr));
        port_label->setText(QApplication::translate("MainWidget", "PORT: N/A", nullptr));
        connected_status_label->setText(QApplication::translate("MainWidget", "Current Status: Not Connected", nullptr));
        lineEdit->setPlaceholderText(QApplication::translate("MainWidget", "60", nullptr));
        label_5->setText(QApplication::translate("MainWidget", "Frame Rate:", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWidget: public Ui_MainWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // MAINWINDOWPUJZTE_H
