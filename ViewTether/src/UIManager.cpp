#include "UIManager.h"

void openLink(const wchar_t * link) {
	ShellExecute(0, 0, link, 0, 0 , SW_SHOW );
}





UIManager::UIManager() {
	m_mainWidget = new Ui_MainWidget();
	QWidget* widget = new QWidget();

	m_mainWidget->setupUi(widget);

	//initialize shits here

	widget->show();
}

UIManager::UIManager(const UIManager&) {	

}

UIManager::~UIManager() {

}

void UIManager::setVideoFrame(std::shared_ptr<unsigned char> pixeldata) {
	std::shared_ptr<QImage> notSwappedImage = std::shared_ptr<QImage>(new QImage(pixeldata.get(), 1920, 1080, QImage::Format_RGBA8888));
	//std::shared_ptr<QImage> image = std::shared_ptr<QImage>(new QImage(notSwappedImage->rgbSwapped()));
	m_videoWidget->updateImage(notSwappedImage);

}

void UIManager::setAddress(QString ip, QString port) {
	m_mainWidget->ip_label->setText("IP: " + ip);
	m_mainWidget->port_label->setText("PORT: " + port); 
}

void UIManager::setConnectionStatus(bool isConnected) {
	if (isConnected) {
		m_mainWidget->connected_status_label->setText("Current Status: Connected");
	}
	else {
		m_mainWidget->connected_status_label->setText("Current Status: Not Connected");
	}
}

void UIManager::setDisplayInformation(DisplayInformationManager displayInformationManager) {
	m_displayInformationManager = displayInformationManager;
	initialize();
}

void UIManager::streamSwitch() 
{
	if (isActive) {
		isActive = false;

		m_mainWidget->startButton->setText("Start Streaming");
		m_mainWidget->adapterComboBox->setDisabled(false);
		m_mainWidget->outputComboBox->setDisabled(false);

		m_videoWidget->hide();

	}
	else {
		isActive = true;

		m_mainWidget->startButton->setText("Stop Streaming");
		m_mainWidget->adapterComboBox->setDisabled(true);
		m_mainWidget->outputComboBox->setDisabled(true);

		m_videoWidget->show();
	}
	emit activeStatusChanged(isActive);
}

void UIManager::initialize() {
	initializeVideoWidget();
	initializeButtons();
	initializeComboBoxes();
}

void UIManager::initializeVideoWidget()
{
	m_videoWidget = new VideoWidget(m_mainWidget->previewContainer);
	m_videoWidget->show();
	QHBoxLayout* layout = new QHBoxLayout(m_mainWidget->previewContainer);
	layout->addWidget(m_videoWidget);
}

void UIManager::initializeButtons() {
	connect(m_mainWidget->startButton, &QPushButton::clicked, this, &UIManager::streamSwitch);

	connect(m_mainWidget->settingsButton, &QPushButton::clicked, this, [this] {m_mainWidget->appRouter->setCurrentWidget(m_mainWidget->settingsPage); });
	connect(m_mainWidget->backToStreaming, &QPushButton::clicked, this, [this] {m_mainWidget->appRouter->setCurrentWidget(m_mainWidget->streamingPage); });

	connect(m_mainWidget->aboutSettingsButton, &QPushButton::clicked, this, [this] {m_mainWidget->settingsRouter->setCurrentWidget(m_mainWidget->aboutSettings); });
	connect(m_mainWidget->virtualMonitorSettingsButton, &QPushButton::clicked, this, [this] {m_mainWidget->settingsRouter->setCurrentWidget(m_mainWidget->virtualMonitorSettings); });

	connect(m_mainWidget->githubRepoButton, &QPushButton::clicked, this, [] {openLink(L"https://github.com/0CottonBuds/Screen-Capture"); });
	connect(m_mainWidget->clientGithubRepoButton, &QPushButton::clicked, this, [] {openLink(L"https://github.com/0CottonBuds/Screen-Capture-Android-Client"); });
	connect(m_mainWidget->linkedinButton, &QPushButton::clicked, this, [] {openLink(L"https://www.linkedin.com/in/emil-john-llanes-187214231/"); });
	connect(m_mainWidget->githubButton, &QPushButton::clicked, this, [] {openLink(L"https://github.com/0CottonBuds"); });
	connect(m_mainWidget->facebookButton, &QPushButton::clicked, this, [] {openLink(L"https://www.facebook.com/0CottonBuds"); });

	connect(m_mainWidget->amyuniInstallButton, &QPushButton::clicked, this, &UIManager::driverInstallClicked);
	connect(m_mainWidget->amyuniUninstallButton, &QPushButton::clicked, this, &UIManager::driverUninstallClicked);
	connect(m_mainWidget->amyuniAddMonitorButton, &QPushButton::clicked, this, &UIManager::addVirtualMonitorClicked);
	connect(m_mainWidget->AmyuniRemoveMonitorButton, &QPushButton::clicked, this,&UIManager::removeVirtualMonitorClicked);
}

void UIManager::initializeComboBoxes() {
	connect(m_mainWidget->frameRateComboBox, &QComboBox::currentIndexChanged, this, [this] {emit fpsChanged(m_mainWidget->frameRateComboBox->currentText().toInt());});
	connect(m_mainWidget->adapterComboBox, &QComboBox::currentIndexChanged, this, [this] {emit screenChanged(m_mainWidget->adapterComboBox->currentIndex(), m_mainWidget->outputComboBox->currentIndex());});
	connect(m_mainWidget->outputComboBox, &QComboBox::currentIndexChanged, this, [this] {emit screenChanged(m_mainWidget->adapterComboBox->currentIndex(), m_mainWidget->outputComboBox->currentIndex());});

	std::vector<DisplayProvider> displayProviders = m_displayInformationManager.getDisplayProviders();
	for (int i = 0; i < displayProviders.size(); i++) {
		DisplayProvider currProvider = displayProviders[i];
		m_mainWidget->adapterComboBox->addItem(currProvider.name.c_str());
	}

	m_mainWidget->adapterComboBox->setCurrentIndex(0);

	m_mainWidget->outputComboBox->setCurrentIndex(0);
	int adapterIndex = m_mainWidget->adapterComboBox->currentIndex();
	DisplayProvider selectedDisplayProvider = m_displayInformationManager.getDisplayProvider(adapterIndex);

	m_mainWidget->outputComboBox->clear();

	for (int i = 0; i < selectedDisplayProvider.displayInformations.size(); i++) {
		m_mainWidget->outputComboBox->addItem(std::to_string(i).c_str());
	}
	m_mainWidget->outputComboBox->setCurrentIndex(0);

}


