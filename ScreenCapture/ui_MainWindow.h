/********************************************************************************
** Form generated from reading UI file 'MainWindowcHBJrw.ui'
**
** Created by: Qt User Interface Compiler version 6.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef MAINWINDOWCHBJRW_H
#define MAINWINDOWCHBJRW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWidget
{
public:
    QVBoxLayout *verticalLayout;
    QStackedWidget *appRouter;
    QWidget *streamingPage;
    QVBoxLayout *verticalLayout_2;
    QWidget *previewContainer;
    QWidget *controlContainers;
    QPushButton *startButton;
    QLabel *instructionLabel;
    QLabel *ip_label;
    QLabel *port_label;
    QLabel *connected_status_label;
    QLabel *frameRateLabel;
    QComboBox *frameRateComboBox;
    QLabel *adapterLabel;
    QLabel *outputLabel;
    QComboBox *adapterComboBox;
    QComboBox *outputComboBox;
    QPushButton *startButton_2;
    QLabel *connected_status_label_2;
    QWidget *settingsPage;
    QHBoxLayout *horizontalLayout;
    QFrame *frame;
    QLabel *label_4;
    QPushButton *backToStreaming;
    QPushButton *aboutSettingsButton;
    QPushButton *virtualMonitorSettingsButton;
    QStackedWidget *settingsRouter;
    QWidget *aboutSettings;
    QLabel *aboutLabel;
    QLabel *label;
    QLabel *aboutLabel_2;
    QPushButton *githubRepoButton;
    QPushButton *clientGithubRepoButton;
    QPushButton *linkedinButton;
    QPushButton *githubButton;
    QPushButton *facebookButton;
    QPushButton *websiteButton;
    QWidget *virtualMonitorSettings;
    QLabel *VirtualMonitorLabel;
    QLabel *virtualMonitorNote;
    QPushButton *amyuniInstallButton;
    QPushButton *amyuniUninstallButton;
    QPushButton *amyuniAddMonitorButton;
    QPushButton *AmyuniRemoveMonitorButton;
    QLabel *virtualMonitorNote_2;
    QLabel *virtualMonitorNote_3;
    QLabel *virtualMonitorNote_4;
    QLabel *virtualMonitorNote_5;

    void setupUi(QWidget *MainWidget)
    {
        if (MainWidget->objectName().isEmpty())
            MainWidget->setObjectName("MainWidget");
        MainWidget->resize(1000, 700);
        MainWidget->setMinimumSize(QSize(1000, 700));
        MainWidget->setMaximumSize(QSize(1000, 700));
        verticalLayout = new QVBoxLayout(MainWidget);
        verticalLayout->setObjectName("verticalLayout");
        appRouter = new QStackedWidget(MainWidget);
        appRouter->setObjectName("appRouter");
        streamingPage = new QWidget();
        streamingPage->setObjectName("streamingPage");
        verticalLayout_2 = new QVBoxLayout(streamingPage);
        verticalLayout_2->setSpacing(0);
        verticalLayout_2->setObjectName("verticalLayout_2");
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        previewContainer = new QWidget(streamingPage);
        previewContainer->setObjectName("previewContainer");

        verticalLayout_2->addWidget(previewContainer);

        controlContainers = new QWidget(streamingPage);
        controlContainers->setObjectName("controlContainers");
        controlContainers->setMinimumSize(QSize(0, 150));
        controlContainers->setMaximumSize(QSize(16777215, 150));
        startButton = new QPushButton(controlContainers);
        startButton->setObjectName("startButton");
        startButton->setGeometry(QRect(410, 30, 141, 61));
        instructionLabel = new QLabel(controlContainers);
        instructionLabel->setObjectName("instructionLabel");
        instructionLabel->setGeometry(QRect(10, 20, 401, 16));
        ip_label = new QLabel(controlContainers);
        ip_label->setObjectName("ip_label");
        ip_label->setGeometry(QRect(10, 40, 391, 16));
        port_label = new QLabel(controlContainers);
        port_label->setObjectName("port_label");
        port_label->setGeometry(QRect(10, 50, 381, 20));
        connected_status_label = new QLabel(controlContainers);
        connected_status_label->setObjectName("connected_status_label");
        connected_status_label->setGeometry(QRect(10, 120, 341, 16));
        frameRateLabel = new QLabel(controlContainers);
        frameRateLabel->setObjectName("frameRateLabel");
        frameRateLabel->setGeometry(QRect(840, 20, 61, 16));
        frameRateComboBox = new QComboBox(controlContainers);
        frameRateComboBox->addItem(QString());
        frameRateComboBox->addItem(QString());
        frameRateComboBox->addItem(QString());
        frameRateComboBox->setObjectName("frameRateComboBox");
        frameRateComboBox->setGeometry(QRect(910, 20, 69, 22));
        frameRateComboBox->setEditable(false);
        adapterLabel = new QLabel(controlContainers);
        adapterLabel->setObjectName("adapterLabel");
        adapterLabel->setGeometry(QRect(840, 50, 61, 21));
        outputLabel = new QLabel(controlContainers);
        outputLabel->setObjectName("outputLabel");
        outputLabel->setGeometry(QRect(840, 80, 61, 21));
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
        startButton_2 = new QPushButton(controlContainers);
        startButton_2->setObjectName("startButton_2");
        startButton_2->setGeometry(QRect(560, 30, 51, 61));
        connected_status_label_2 = new QLabel(controlContainers);
        connected_status_label_2->setObjectName("connected_status_label_2");
        connected_status_label_2->setGeometry(QRect(840, 120, 141, 31));
        QFont font;
        font.setPointSize(7);
        connected_status_label_2->setFont(font);

        verticalLayout_2->addWidget(controlContainers);

        appRouter->addWidget(streamingPage);
        settingsPage = new QWidget();
        settingsPage->setObjectName("settingsPage");
        horizontalLayout = new QHBoxLayout(settingsPage);
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        frame = new QFrame(settingsPage);
        frame->setObjectName("frame");
        frame->setMinimumSize(QSize(225, 0));
        frame->setMaximumSize(QSize(225, 16777215));
        frame->setFrameShape(QFrame::Shape::StyledPanel);
        frame->setFrameShadow(QFrame::Shadow::Raised);
        label_4 = new QLabel(frame);
        label_4->setObjectName("label_4");
        label_4->setGeometry(QRect(60, 10, 91, 31));
        QFont font1;
        font1.setPointSize(14);
        label_4->setFont(font1);
        backToStreaming = new QPushButton(frame);
        backToStreaming->setObjectName("backToStreaming");
        backToStreaming->setGeometry(QRect(10, 10, 41, 31));
        aboutSettingsButton = new QPushButton(frame);
        aboutSettingsButton->setObjectName("aboutSettingsButton");
        aboutSettingsButton->setGeometry(QRect(20, 110, 151, 31));
        virtualMonitorSettingsButton = new QPushButton(frame);
        virtualMonitorSettingsButton->setObjectName("virtualMonitorSettingsButton");
        virtualMonitorSettingsButton->setGeometry(QRect(20, 70, 151, 31));

        horizontalLayout->addWidget(frame);

        settingsRouter = new QStackedWidget(settingsPage);
        settingsRouter->setObjectName("settingsRouter");
        aboutSettings = new QWidget();
        aboutSettings->setObjectName("aboutSettings");
        aboutLabel = new QLabel(aboutSettings);
        aboutLabel->setObjectName("aboutLabel");
        aboutLabel->setGeometry(QRect(10, 40, 151, 31));
        aboutLabel->setFont(font1);
        label = new QLabel(aboutSettings);
        label->setObjectName("label");
        label->setGeometry(QRect(10, 90, 681, 61));
        aboutLabel_2 = new QLabel(aboutSettings);
        aboutLabel_2->setObjectName("aboutLabel_2");
        aboutLabel_2->setGeometry(QRect(10, 250, 71, 31));
        aboutLabel_2->setFont(font1);
        githubRepoButton = new QPushButton(aboutSettings);
        githubRepoButton->setObjectName("githubRepoButton");
        githubRepoButton->setGeometry(QRect(10, 160, 151, 31));
        clientGithubRepoButton = new QPushButton(aboutSettings);
        clientGithubRepoButton->setObjectName("clientGithubRepoButton");
        clientGithubRepoButton->setGeometry(QRect(10, 200, 151, 31));
        linkedinButton = new QPushButton(aboutSettings);
        linkedinButton->setObjectName("linkedinButton");
        linkedinButton->setGeometry(QRect(10, 290, 151, 31));
        githubButton = new QPushButton(aboutSettings);
        githubButton->setObjectName("githubButton");
        githubButton->setGeometry(QRect(10, 330, 151, 31));
        facebookButton = new QPushButton(aboutSettings);
        facebookButton->setObjectName("facebookButton");
        facebookButton->setGeometry(QRect(10, 370, 151, 31));
        websiteButton = new QPushButton(aboutSettings);
        websiteButton->setObjectName("websiteButton");
        websiteButton->setGeometry(QRect(10, 410, 151, 31));
        settingsRouter->addWidget(aboutSettings);
        virtualMonitorSettings = new QWidget();
        virtualMonitorSettings->setObjectName("virtualMonitorSettings");
        VirtualMonitorLabel = new QLabel(virtualMonitorSettings);
        VirtualMonitorLabel->setObjectName("VirtualMonitorLabel");
        VirtualMonitorLabel->setGeometry(QRect(10, 40, 151, 31));
        VirtualMonitorLabel->setFont(font1);
        virtualMonitorNote = new QLabel(virtualMonitorSettings);
        virtualMonitorNote->setObjectName("virtualMonitorNote");
        virtualMonitorNote->setGeometry(QRect(10, 80, 731, 31));
        virtualMonitorNote->setTextFormat(Qt::TextFormat::AutoText);
        amyuniInstallButton = new QPushButton(virtualMonitorSettings);
        amyuniInstallButton->setObjectName("amyuniInstallButton");
        amyuniInstallButton->setGeometry(QRect(20, 180, 151, 31));
        amyuniUninstallButton = new QPushButton(virtualMonitorSettings);
        amyuniUninstallButton->setObjectName("amyuniUninstallButton");
        amyuniUninstallButton->setGeometry(QRect(20, 260, 151, 31));
        amyuniAddMonitorButton = new QPushButton(virtualMonitorSettings);
        amyuniAddMonitorButton->setObjectName("amyuniAddMonitorButton");
        amyuniAddMonitorButton->setGeometry(QRect(20, 340, 151, 31));
        AmyuniRemoveMonitorButton = new QPushButton(virtualMonitorSettings);
        AmyuniRemoveMonitorButton->setObjectName("AmyuniRemoveMonitorButton");
        AmyuniRemoveMonitorButton->setGeometry(QRect(20, 420, 151, 31));
        virtualMonitorNote_2 = new QLabel(virtualMonitorSettings);
        virtualMonitorNote_2->setObjectName("virtualMonitorNote_2");
        virtualMonitorNote_2->setGeometry(QRect(20, 150, 391, 31));
        virtualMonitorNote_2->setTextFormat(Qt::TextFormat::AutoText);
        virtualMonitorNote_3 = new QLabel(virtualMonitorSettings);
        virtualMonitorNote_3->setObjectName("virtualMonitorNote_3");
        virtualMonitorNote_3->setGeometry(QRect(20, 230, 391, 31));
        virtualMonitorNote_3->setTextFormat(Qt::TextFormat::AutoText);
        virtualMonitorNote_4 = new QLabel(virtualMonitorSettings);
        virtualMonitorNote_4->setObjectName("virtualMonitorNote_4");
        virtualMonitorNote_4->setGeometry(QRect(20, 310, 411, 31));
        virtualMonitorNote_4->setTextFormat(Qt::TextFormat::AutoText);
        virtualMonitorNote_5 = new QLabel(virtualMonitorSettings);
        virtualMonitorNote_5->setObjectName("virtualMonitorNote_5");
        virtualMonitorNote_5->setGeometry(QRect(20, 390, 431, 31));
        virtualMonitorNote_5->setTextFormat(Qt::TextFormat::AutoText);
        settingsRouter->addWidget(virtualMonitorSettings);

        horizontalLayout->addWidget(settingsRouter);

        appRouter->addWidget(settingsPage);

        verticalLayout->addWidget(appRouter);


        retranslateUi(MainWidget);

        appRouter->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWidget);
    } // setupUi

    void retranslateUi(QWidget *MainWidget)
    {
        MainWidget->setWindowTitle(QCoreApplication::translate("MainWidget", "Form", nullptr));
        startButton->setText(QCoreApplication::translate("MainWidget", "Start Streaming", nullptr));
        instructionLabel->setText(QCoreApplication::translate("MainWidget", "Connect from client with these address:", nullptr));
        ip_label->setText(QCoreApplication::translate("MainWidget", "IP: N/A", nullptr));
        port_label->setText(QCoreApplication::translate("MainWidget", "PORT: N/A", nullptr));
        connected_status_label->setText(QCoreApplication::translate("MainWidget", "Current Status: Not Connected", nullptr));
        frameRateLabel->setText(QCoreApplication::translate("MainWidget", "Frame Rate:", nullptr));
        frameRateComboBox->setItemText(0, QCoreApplication::translate("MainWidget", "60", nullptr));
        frameRateComboBox->setItemText(1, QCoreApplication::translate("MainWidget", "30", nullptr));
        frameRateComboBox->setItemText(2, QCoreApplication::translate("MainWidget", "15", nullptr));

        adapterLabel->setText(QCoreApplication::translate("MainWidget", "Adapter: ", nullptr));
        outputLabel->setText(QCoreApplication::translate("MainWidget", "Output:", nullptr));
        adapterComboBox->setCurrentText(QString());
        adapterComboBox->setPlaceholderText(QCoreApplication::translate("MainWidget", "NULL", nullptr));
        outputComboBox->setPlaceholderText(QCoreApplication::translate("MainWidget", "NULL", nullptr));
        startButton_2->setText(QCoreApplication::translate("MainWidget", "Settings", nullptr));
        connected_status_label_2->setText(QCoreApplication::translate("MainWidget", "Tip: Add virtual Monitor at settings", nullptr));
        label_4->setText(QCoreApplication::translate("MainWidget", "Settings", nullptr));
        backToStreaming->setText(QCoreApplication::translate("MainWidget", "Back", nullptr));
        aboutSettingsButton->setText(QCoreApplication::translate("MainWidget", "About", nullptr));
        virtualMonitorSettingsButton->setText(QCoreApplication::translate("MainWidget", "Virtual Monitors", nullptr));
        aboutLabel->setText(QCoreApplication::translate("MainWidget", "About", nullptr));
        label->setText(QCoreApplication::translate("MainWidget", "Screen Duplicator is a software that streams a virtual second scrreen to another device in partner with Screen Duplicator Client.\n"
"This program uses c++, qt for UI, dxgi for screen duplication, and ffmpeg for compression. The project is Open-Source so\n"
" feel free to contribute to the github repo linked below.", nullptr));
        aboutLabel_2->setText(QCoreApplication::translate("MainWidget", "Socials", nullptr));
        githubRepoButton->setText(QCoreApplication::translate("MainWidget", "Github Repo", nullptr));
        clientGithubRepoButton->setText(QCoreApplication::translate("MainWidget", "Client Github Repo", nullptr));
        linkedinButton->setText(QCoreApplication::translate("MainWidget", "Linkedin", nullptr));
        githubButton->setText(QCoreApplication::translate("MainWidget", "Github", nullptr));
        facebookButton->setText(QCoreApplication::translate("MainWidget", "Facebook Page", nullptr));
        websiteButton->setText(QCoreApplication::translate("MainWidget", "Website", nullptr));
        VirtualMonitorLabel->setText(QCoreApplication::translate("MainWidget", "Virtual Monitors", nullptr));
        virtualMonitorNote->setText(QCoreApplication::translate("MainWidget", "NOTE: This software uses unmodified Amyuni USB Mobile Monitor Virtual Display drivers.   ", nullptr));
        amyuniInstallButton->setText(QCoreApplication::translate("MainWidget", "Install ", nullptr));
        amyuniUninstallButton->setText(QCoreApplication::translate("MainWidget", "Uninstall", nullptr));
        amyuniAddMonitorButton->setText(QCoreApplication::translate("MainWidget", "Add Virtual Monitor", nullptr));
        AmyuniRemoveMonitorButton->setText(QCoreApplication::translate("MainWidget", "Remove Virtual Monitor", nullptr));
        virtualMonitorNote_2->setText(QCoreApplication::translate("MainWidget", "Install Amyuni USB Mobile Monitor Virtual Display", nullptr));
        virtualMonitorNote_3->setText(QCoreApplication::translate("MainWidget", "Uninstall Amyuni USB Mobile Monitor Virtual Display", nullptr));
        virtualMonitorNote_4->setText(QCoreApplication::translate("MainWidget", "Create a Virtual Monitor using Amyuni USB Mobile Monitor Virtual Display", nullptr));
        virtualMonitorNote_5->setText(QCoreApplication::translate("MainWidget", "Remove a Virtual Monitor using Amyuni USB Mobile Monitor Virtual Display", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWidget: public Ui_MainWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // MAINWINDOWCHBJRW_H
