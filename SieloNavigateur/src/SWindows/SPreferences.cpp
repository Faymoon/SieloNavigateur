#include "includes/SWindows/SPreferences.hpp"
#include "includes/SWidgets/STabWidget.hpp"
#include "includes/SWidgets/SWebView.hpp"
#include "includes/SApplication.hpp"

#include <QWebEngineSettings>
#include <QDir>
#include <QMessageBox>
#include <QMenuBar>

GeneralPageWidget::GeneralPageWidget(QWidget *parent) :
	QWidget(parent)
{
	// Fill the main layout
	m_layout->addWidget(m_closeBox);
	m_layout->addWidget(m_personalisationBox);
	m_layout->addWidget(m_homePagebox);

	QStringList loadingBarStyleList{};
	loadingBarStyleList << "Fin" << "Flat";

	// Set widgets attributes
    m_homePageArea->setText(mApp->settings()->value("preferences/homePage", "http://google.com").toString());
    m_showMenuBar->setChecked(mApp->settings()->value("preferences/showMenuBar", false).toBool());
	m_loadingBarStyleComboBox->addItems(loadingBarStyleList);

    if(mApp->settings()->value("preferences/saveTabs", false).toBool())
		m_saveTabRButton->setChecked(true);
	else
		m_homePageRButton->setChecked(true);

    if (mApp->settings()->value("preferences/loadingBarStyle", "fineBar").toString() == "fineBar")
		m_loadingBarStyleComboBox->setCurrentText("Fin");
    else if (mApp->settings()->value("preferences/loadingBarStyle", "fineBar").toString() == "flatBar")
		m_loadingBarStyleComboBox->setCurrentText("Flat");
	else
		m_loadingBarStyleComboBox->setCurrentText("Fin");

    if (mApp->settings()->value("preferences/openWidgetInTab", true).toBool())
		m_prefereOpenInNewTab->setChecked(true);
	else
		m_prefereOpenInNewWin->setChecked(true);

	m_closeBox->setTitle(tr("Option d'ouverture des fenêtres"));
	m_personalisationBox->setTitle(tr("Personnalisation"));
	m_homePagebox->setTitle(tr("Page d'accueil"));

	// Fill others layouts
	m_closeLayout->addWidget(m_homePageRButton);
	m_closeLayout->addWidget(m_saveTabRButton);
	m_personalisationLayout->addWidget(m_showMenuBar, 0, 0, 1, 2);
	m_personalisationLayout->addWidget(m_loadingBarStyleLabel, 1, 0);
	m_personalisationLayout->addWidget(m_loadingBarStyleComboBox, 1, 1);
	m_personalisationLayout->addWidget(m_prefereOpenInNewTab, 2, 0);
	m_personalisationLayout->addWidget(m_prefereOpenInNewWin, 3, 0);
	m_homePageLayout->addWidget(m_homePageArea);
}

GeneralPageWidget::~GeneralPageWidget()
{

}

void GeneralPageWidget::save()
{
	if(m_saveTabRButton->isChecked())
        mApp->settings()->setValue("preferences/saveTabs", true);
	else
        mApp->settings()->setValue("preferences/saveTabs", false);

	if (m_loadingBarStyleComboBox->currentText() == "Fin")
        mApp->settings()->setValue("preferences/loadingBarStyle", "fineBar");
	else if (m_loadingBarStyleComboBox->currentText() == "Flat")
        mApp->settings()->setValue("preferences/loadingBarStyle", "flatBar");

	if (m_prefereOpenInNewTab->isChecked())
        mApp->settings()->setValue("preferences/openWidgetInTab", true);
	else
        mApp->settings()->setValue("preferences/openWidgetInTab", false);

    mApp->settings()->setValue("preferences/showMenuBar", m_showMenuBar->isChecked());
    mApp->settings()->setValue("preferences/homePage", m_homePageArea->text());
}

BrowsPageWidget::BrowsPageWidget(SPreferencesWindow *parent) :
	QWidget(parent),
	m_parent(parent)
{
	m_layout->addWidget(m_webBox);
	m_layout->addWidget(m_cookiesBox);

    m_pluginCheckBox->setChecked(mApp->settings()->value("preferences/enablePlugins", true).toBool());
    m_javascripCheckBox->setChecked(mApp->settings()->value("preferences/enableJavascript", true).toBool());
    m_cookiesCheckBox->setChecked(mApp->settings()->value("preferences/enableCookies", true).toBool());

	m_webBox->setTitle(tr("Options de navigation"));
	m_cookiesBox->setTitle(tr("Options pour les cookies"));

	m_webLayout->addWidget(m_pluginCheckBox);
	m_webLayout->addWidget(m_javascripCheckBox);
	m_cookiesLayout->addWidget(m_cookiesCheckBox);
	m_cookiesLayout->addWidget(m_deleteAllCookies);

	connect(m_deleteAllCookies, &QPushButton::clicked, this, &BrowsPageWidget::deleteAllCookies);
}

BrowsPageWidget::~BrowsPageWidget()
{

}

void BrowsPageWidget::save()
{
    mApp->settings()->setValue("preferences/enablePlugins", m_pluginCheckBox->isChecked());
    mApp->settings()->setValue("preferences/enableJavascript", m_javascripCheckBox->isChecked());
    mApp->settings()->setValue("preferences/enableCookies", m_cookiesCheckBox->isChecked());
}

void BrowsPageWidget::deleteAllCookies()
{
	for (int i{ 0 }; i < m_parent->getParent()->getTabs()->count() - 1; ++i) 
		m_parent->getParent()->getTabs()->widget(i)->findChild<SWebView*>()->page()->profile()->cookieStore()->deleteAllCookies();

	QMessageBox::information(this, tr("Cookies"), tr("Tous les cookies ont bien été suprimés"));
}

SPreferencesWindow::SPreferencesWindow(SMainWindow *parent) :
	QDialog(parent),
	m_parent(parent)
{
	// Set window attributes
	resize(657, 270);
	setAttribute(Qt::WA_DeleteOnClose);

	// Fill the layout
	m_layout->addWidget(m_tab);
	m_layout->addWidget(m_boxBtn);

	// Add pages to the tab widget
	m_tab->addTab(m_generalPageWidget, tr("Page d'accueil"));
	m_tab->addTab(m_browsPageWidget, tr("Options de Navigation"));

	// Connections
	connect(m_boxBtn, &QDialogButtonBox::accepted, this, &SPreferencesWindow::accept);
	connect(m_boxBtn, &QDialogButtonBox::rejected, this, &SPreferencesWindow::close);

}

SPreferencesWindow::~SPreferencesWindow()
{
	// Empty
}

void SPreferencesWindow::accept()
{
	// Save all settings
	m_generalPageWidget->save();
	m_browsPageWidget->save();

	m_parent->menuBar()->setVisible(m_generalPageWidget->getShowMenuBar()->isChecked());

	close();
}
