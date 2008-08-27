//
//   This file is part of the KVIrc irc client distribution
//   Copyright (C) 1999-2002 Andrea Parrella (yap@kvirc.net)
//
//   This program is FREE software. You can redistribute it and/or
//   modify it under the terms of the GNU General Public License
//   as published by the Free Software Foundation; either version 2
//   of the License, or (at your opinion) any later version.
//
//   This program is distributed in the HOPE that it will be USEFUL,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//   See the GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program. If not, write to the Free Software Foundation,
//   Inc. ,59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//

#include "libkviurl.h"
#include "icons.h"

#include "kvi_module.h"
#include "kvi_app.h"
#include "kvi_frame.h"
#include "kvi_menubar.h"
#include "kvi_internalcmd.h"
#include "kvi_iconmanager.h"
#include "kvi_action.h"
#include "kvi_actionmanager.h"
#include "kvi_taskbar.h"
#include "kvi_pointerlist.h"
#include "kvi_kvs_eventmanager.h"
#include "kvi_tal_popupmenu.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QCursor>
#include <QDateTime>
#include <QTextStream>

static QPixmap * g_pUrlIconPixmap = 0;
static KviUrlAction * g_pUrlAction = 0;

typedef struct _UrlDlgList
{
	UrlDialog *dlg;
	int menu_id;
} UrlDlgList;

const char *g_pUrlListFilename = "/list.kviurl";
const char *g_pBanListFilename = "/list.kviban";

KviPointerList<KviUrl> *g_pList;
KviPointerList<UrlDlgList> *g_pUrlDlgList;
KviPointerList<KviStr> *g_pBanList;
ConfigDialog *g_pConfigDialog;

QString szConfigPath;

void saveUrlList();
void loadUrlList();
void saveBanList();
void loadBanList();
UrlDlgList * findFrame();
bool urllist();
void url_module_help();


#define KVI_URL_EXTENSION_NAME "URL module extenstion"

static KviModuleExtension * url_extension_alloc(KviModuleExtensionAllocStruct * s)
{
	urllist();
	return 0;
}


KviUrlAction::KviUrlAction(QObject * pParent)
: KviKvsAction(pParent,
	"url.list",
	"url.list",
	__tr2qs("Show URL List"),
	__tr2qs("Shows the URL list window"),
	KviActionManager::categoryGeneric())
{
	m_pBigIcon = new QPixmap(url_toolbar_xpm);
	m_pSmallIcon = new QPixmap(url_icon_xpm);
}

KviUrlAction::~KviUrlAction()
{
	delete m_pBigIcon;
	delete m_pSmallIcon;
}

QPixmap * KviUrlAction::bigIcon()
{
	return m_pBigIcon;
}

QPixmap * KviUrlAction::smallIcon()
{
	return m_pSmallIcon;
}

// ---------------------------- CLASS URLDIALOG ------------------------begin //

UrlDialog::UrlDialog(KviPointerList<KviUrl> *g_pList)
:KviWindow(KVI_WINDOW_TYPE_TOOL,g_pFrame,"URL List")
{
	m_pMenuBar = new KviTalMenuBar(this,"url menu");
	m_pUrlList = new KviTalListView(this);
	//m_pUrlList = new KviListView(this,"list");
	KviConfig cfg(szConfigPath,KviConfig::Read);

	KviTalPopupMenu *pop;

	pop = new KviTalPopupMenu(this);
	pop->insertItem(__tr2qs("&Configure"),this,SLOT(config()));
	pop->insertItem(__tr2qs("&Help"),this,SLOT(help()));
	pop->insertItem(__tr2qs("Clo&se"),this,SLOT(close_slot()));
	m_pMenuBar->insertItem(__tr2qs("&Module"),pop);

	pop = new KviTalPopupMenu(this);
	pop->insertItem(__tr2qs("&Load"),this,SLOT(loadList()));
	pop->insertItem(__tr2qs("&Save"),this,SLOT(saveList()));
	pop->insertItem(__tr2qs("&Clear"),this,SLOT(clear()));
	m_pMenuBar->insertItem(__tr2qs("&List"),pop);

	m_pUrlList->setShowSortIndicator(true);
	m_pUrlList->addColumn(__tr2qs("URL"));
	m_pUrlList->addColumn(__tr2qs("Window"));
	m_pUrlList->addColumn(__tr2qs("Count"));
	m_pUrlList->addColumn(__tr2qs("Timestamp"));

	cfg.setGroup("colsWidth");
	m_pUrlList->setColumnWidth(0,cfg.readIntEntry("Url",170));
	m_pUrlList->setColumnWidth(1,cfg.readIntEntry("Window",130));
	m_pUrlList->setColumnWidth(2,cfg.readIntEntry("Count",70));
	m_pUrlList->setColumnWidth(3,cfg.readIntEntry("Timestamp",70));

	connect(m_pUrlList,SIGNAL(doubleClicked(KviTalListViewItem *)),SLOT(dblclk_url(KviTalListViewItem *)));
	connect(m_pUrlList,SIGNAL(rightButtonPressed(KviTalListViewItem *, const QPoint &, int)),SLOT(popup(KviTalListViewItem *, const QPoint &, int)));

//	setFocusHandlerNoChildren(m_pUrlList);
	m_pUrlList->setFocusPolicy(Qt::StrongFocus);
	m_pUrlList->setFocus();
}

void UrlDialog::config()
{
	if (!g_pConfigDialog) g_pConfigDialog = new ConfigDialog();
}

void UrlDialog::help()
{
//#warning "help"
//	m_pFrm->requestHelpOn("doc_plugin_url.kvihelp");
}

void UrlDialog::saveList()
{
	saveUrlList();
}

void UrlDialog::loadList()
{
	loadUrlList();
}

void UrlDialog::clear()
{
	g_pList->clear();
	for (UrlDlgList *tmpitem=g_pUrlDlgList->first();tmpitem;tmpitem=g_pUrlDlgList->next()) {
		if (tmpitem->dlg) tmpitem->dlg->m_pUrlList->clear();
	}
}

/*
void UrlDialog::saveProperties()
{

	KviWindowProperty p;
	p.rect = externalGeometry();
	p.isDocked = isAttached();
	p.splitWidth1 = 0;
	p.splitWidth2 = 0;
	p.timestamp = 0;
	p.imagesVisible = 0;
	KviWindow * w = m_pFrm->activeWindow();
	p.isMaximized = isAttached() && w ? w->isMaximized() : isMaximized();
	p.topSplitWidth1 = 0;
	p.topSplitWidth2 = 0;
	p.topSplitWidth3 = 0;
	g_pOptions->m_pWinPropertiesList->setProperty(caption(),&p);
}
*/

void UrlDialog::close_slot()
{
	close();
}

void UrlDialog::remove()
{
	if (!m_pUrlList->currentItem()) {
		QMessageBox::warning(0,__tr2qs("Warning - KVIrc"),__tr2qs("Select an URL."),QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton);
		return;
	}

	for(KviUrl *tmp=g_pList->first();tmp;tmp=g_pList->next())
	{
		if (tmp->url == m_pUrlList->currentItem()->text(0)) {
			g_pList->removeRef(tmp);
			m_pUrlList->takeItem(m_pUrlList->currentItem());
			return;
		}
	}
}

void UrlDialog::findtext()
{
//#warning "find text"
/*
	if (!m_pUrlList->currentItem()) {
		kvirc_plugin_warning_box(__tr("Select an URL"));
		return;
	}
	for(KviUrl *tmp=g_pList->first();tmp;tmp=g_pList->next())
	{
		if (tmp->url == KviStr(m_pUrlList->currentItem()->text(0))) {
			g_pList->find(tmp);
			KviStr ft="findtext %";
			ft.replaceAll('%',tmp->url.ptr());
			KviWindow *wnd = m_pFrm->findWindow(tmp->window.ptr());
			if (wnd) {
				if (kvirc_plugin_execute_command(wnd,ft.ptr())) {
					if (wnd->mdiParent()) m_pFrm->m_pMdi->setTopChild(wnd->mdiParent(),true);
				}
			} else kvirc_plugin_warning_box(__tr("Window not found"));
		}

	}
*/
}

void UrlDialog::dblclk_url(KviTalListViewItem *item)
{
	QString cmd="openurl ";
	cmd.append(item->text(0));
	KviKvsScript::run(cmd,this);
}

void UrlDialog::popup(KviTalListViewItem *item, const QPoint &point, int col)
{
	if (col == 0) {
		m_szUrl = item->text(0);
		KviTalPopupMenu p(0,"menu");
		p.insertItem(__tr2qs("&Remove"),this,SLOT(remove()));
		p.insertItem(__tr2qs("&Find Text"),this,SLOT(findtext()));
		p.insertSeparator();
		m_pListPopup = new KviTalPopupMenu(0,"list");
		int i=0;
		for(KviWindow *w=g_pFrame->windowList()->first();w;w=g_pFrame->windowList()->next()){
			if ((w->type() <= 2) || (w->type() == 2) || (w->type() == 6)) {	// values defined in kvi_define.h (console,channel,query,chat,uwindow)
				m_pListPopup->insertItem(QString(w->plainTextCaption()),i);
				m_pListPopup->connectItem(i,this,SLOT(sayToWin(int)));
				i++;
			}
		}
		p.insertItem(__tr2qs("&Say to Window"),m_pListPopup);
		p.exec(QCursor::pos());
	}
}

void UrlDialog::sayToWin(int itemID)
{
	KviWindow *wnd = g_pApp->findWindowByCaption(m_pListPopup->text(itemID).toUtf8().data());
	QString say=QString("PRIVMSG %1 %2").arg(wnd->windowName()).arg(m_szUrl.ptr());
	if (wnd) {
		KviKvsScript::run(say,wnd);
		wnd->raise();
		wnd->setActiveWindow();
		wnd->setFocus();
	} else QMessageBox::warning(0,__tr2qs("Warning - KVIrc"),__tr2qs("Window not found."),QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton);
}

QPixmap *UrlDialog::myIconPtr()
{
	//QPixmap *icon = new QPixmap(url_icon_xpm);
	//return icon;
	return g_pUrlIconPixmap;
}

void UrlDialog::addUrl(QString url, QString window, QString count, QString timestamp)
{
	KviTalListViewItem *UrlItem = new KviTalListViewItem(m_pUrlList);

	UrlItem->setText(0, url);
	UrlItem->setText(1, window);
	UrlItem->setText(2, count);
	UrlItem->setText(3, timestamp);
}

void UrlDialog::resizeEvent(QResizeEvent *)
{
	int hght = m_pMenuBar->heightForWidth(width());
	m_pMenuBar->setGeometry(0,0,width(),hght);
	m_pUrlList->setGeometry(0,hght,width(),height() - hght);
}

UrlDialog::~UrlDialog()
{
	KviConfig cfg(szConfigPath,KviConfig::Write);
	cfg.setGroup("ConfigDialog");
	if (cfg.readBoolEntry("SaveColumnWidthOnClose",false)) {
		cfg.setGroup("ColsWidth");
		cfg.writeEntry("Url",m_pUrlList->columnWidth(0));
		cfg.writeEntry("Window",m_pUrlList->columnWidth(1));
		cfg.writeEntry("Count",m_pUrlList->columnWidth(2));
		cfg.writeEntry("Timestamp",m_pUrlList->columnWidth(3));
	}

	delete m_pUrlList;
/*	if (m_pListPopup) delete m_pListPopup;
	m_pListPopup = 0;
	if (m_pMenuBar) delete m_pMenuBar;
	m_pMenuBar = 0;*/
	UrlDlgList *tmpitem = findFrame();
	tmpitem->dlg = 0;
}

// ----------------------------- CLASS URLDIALOG -------------------------end //

// --------------------------- CLASS CONFIGDIALOG ----------------------begin //

ConfigDialog::ConfigDialog()
:QDialog()
{
	setWindowTitle(__tr2qs("URL Module Configuration"));

	QGridLayout *g = new QGridLayout(this);

	KviConfig *cfg = new KviConfig(szConfigPath,KviConfig::Read);
	cfg->setGroup("ConfigDialog");

	cb[0] = new QCheckBox(__tr2qs("Save URL list on module unload"),this);
	cb[0]->setChecked(cfg->readBoolEntry("SaveUrlListOnUnload",false));
	g->addMultiCellWidget(cb[0],0,0,0,1);

	cb[1] = new QCheckBox(__tr2qs("Save columns width on URL list close"),this);
	cb[1]->setChecked(cfg->readBoolEntry("SaveColumnWidthOnClose",false));
	g->addMultiCellWidget(cb[1],1,1,0,1);

	bool tmp = cfg->readBoolEntry("BanEnabled",false);
	delete cfg;

	m_pBanFrame = new BanFrame(this,"banlist",tmp);
	g->addMultiCellWidget(m_pBanFrame,3,3,0,1);

	QPushButton *b;
	// configure buttons
        b = new QPushButton(__tr2qs("&Cancel"),this,"discard");
	connect(b,SIGNAL(clicked()),this,SLOT(discardbtn()));
	g->addWidget(b,4,0);

	b = new QPushButton(__tr2qs("&OK"),this,"accept");
	connect(b,SIGNAL(clicked()),this,SLOT(acceptbtn()));
	g->addWidget(b,4,1);

	show();
}

void ConfigDialog::discardbtn()
{
	delete this;
}

void ConfigDialog::acceptbtn()
{
	if (m_pBanFrame) m_pBanFrame->saveBans();

	KviConfig *cfg = new KviConfig(szConfigPath,KviConfig::Write);
	cfg->setGroup("ConfigDialog");
	cfg->writeEntry("SaveUrlListOnUnload",cb[0]->isChecked());
	cfg->writeEntry("SaveColumnWidthOnClose",cb[1]->isChecked());
	delete cfg;

	delete this;
}

void ConfigDialog::closeEvent(QCloseEvent *)
{
	delete this;
}

ConfigDialog::~ConfigDialog()
{
	for(int i=0;i<cbnum;i++) delete cb[i];
	g_pConfigDialog = 0;
}

// --------------------------- CLASS CONFIGDIALOG ------------------------end //

// ---------------------------- CLASS BANFRAME ------------------------begin //

BanFrame::BanFrame(QWidget *parent, const char *name, bool banEnabled)
:QFrame(parent,name)
{
	setFrameStyle(QFrame::Panel | QFrame::Raised);

	QGridLayout *g = new QGridLayout(this);

	m_pEnable = new QCheckBox(__tr2qs("Enable URL ban list"),this);
	connect(m_pEnable,SIGNAL(clicked()),this,SLOT(enableClicked()));
	m_pEnable->setChecked(banEnabled);
	g->addMultiCellWidget(m_pEnable,0,0,0,1);

	m_pBanList = new QListWidget(this);
	m_pBanList->setMinimumHeight(100);
	loadBanList();
	for(KviStr *tmp=g_pBanList->first();tmp;tmp=g_pBanList->next()) {
		m_pBanList->addItem(tmp->ptr()); // load ban list into listbox
	}

	m_pBanList->setEnabled(m_pEnable->isChecked());
	g->addMultiCellWidget(m_pBanList,1,1,0,1);

	m_pAddBtn = new QPushButton(__tr2qs("&Add Ban"),this,"add");
	connect(m_pAddBtn,SIGNAL(clicked()),this,SLOT(addBan()));
	m_pAddBtn->setEnabled(m_pEnable->isChecked());
	g->addWidget(m_pAddBtn,2,0);

	m_pRemoveBtn = new QPushButton(__tr2qs("&Remove Selected"),this,"remove");
	connect(m_pRemoveBtn,SIGNAL(clicked()),this,SLOT(removeBan()));
	m_pRemoveBtn->setEnabled(m_pEnable->isChecked());
	g->addWidget(m_pRemoveBtn,2,1);
}

void BanFrame::enableClicked()
{
	m_pBanList->setEnabled(m_pEnable->isChecked());
	m_pAddBtn->setEnabled(m_pEnable->isChecked());
	m_pRemoveBtn->setEnabled(m_pEnable->isChecked());
}

void BanFrame::addBan()
{
	bool ok = false;
	KviStr *text = new KviStr(QInputDialog::getText(__tr2qs("URL Ban List"),__tr2qs("Add"),QLineEdit::Normal,QString::null,&ok,this));
	if (ok && !text->isEmpty()) {
		g_pBanList->append(text);
		m_pBanList->addItem(text->ptr());
	}
}

void BanFrame::removeBan()
{
	if (!m_pBanList->currentItem()->isSelected()) {
		QMessageBox::warning(0,__tr2qs("Warning - KVIrc"),__tr2qs("Select a ban."),QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton);
		return;
	}
	KviStr item(m_pBanList->currentItem()->text());
	for(KviStr *tmp=g_pBanList->first();tmp;tmp=g_pBanList->next())
	{
		if (*tmp == item)
		{
			g_pBanList->removeCurrent();
			return;
		}
	}

	m_pBanList->removeItemWidget(m_pBanList->currentItem());

}

void BanFrame::saveBans()
{
	if (m_pEnable->isChecked()) saveBanList();
	KviConfig *cfg = new KviConfig(szConfigPath,KviConfig::Write);
	cfg->setGroup("ConfigDialog");
	cfg->writeEntry("BanEnabled",m_pEnable->isChecked());
	delete cfg;
}

BanFrame::~BanFrame()
{

}

// ---------------------------- CLASS URLTOOLBAR -------------------------end //

void saveUrlList()
{
	QString urllist;
	g_pApp->getLocalKvircDirectory(urllist,KviApp::ConfigPlugins);
	urllist += g_pUrlListFilename;
	QFile file;
	file.setName(urllist);
	file.open(IO_WriteOnly);

	QTextStream stream(&file);

	stream << g_pList->count() << endl;

	for(KviUrl *tmp=g_pList->first();tmp;tmp=g_pList->next())
	{
		stream << tmp->url << endl;
		stream << tmp->window << endl;
		stream << tmp->count << endl;
		stream << tmp->timestamp << endl;
	}
	file.flush();
	file.close();
}

void loadUrlList()
{
	QString urllist;
	g_pApp->getLocalKvircDirectory(urllist,KviApp::ConfigPlugins);
	urllist += g_pUrlListFilename;
	QFile file;
	file.setName(urllist);
	if (!file.open(IO_ReadOnly))return;

	QTextStream stream(&file);

	g_pList->clear();

	for (UrlDlgList *tmpitem=g_pUrlDlgList->first();tmpitem;tmpitem=g_pUrlDlgList->next()) {
		if (tmpitem->dlg) tmpitem->dlg->m_pUrlList->clear();
	}
	KviUrl *tmp;
	int i=0;
	int num = stream.readLine().toInt();
	while ((!stream.atEnd()) && (i<num)){
		tmp = new KviUrl();
		tmp->url = stream.readLine();
		tmp->window = stream.readLine();
		tmp->count = stream.readLine().toInt();
		tmp->timestamp = stream.readLine();

		g_pList->append(tmp);

		for (UrlDlgList *tmpitem=g_pUrlDlgList->first();tmpitem;tmpitem=g_pUrlDlgList->next()) {
			if (tmpitem->dlg) {
				QString tmpCount;
				tmpCount.setNum(tmp->count);
				tmpitem->dlg->addUrl(QString(tmp->url), QString(tmp->window), tmpCount, QString(tmp->timestamp));
			}
		}
		i++;
	}
	file.close();
}

void saveBanList()
{
	QString banlist;
	g_pApp->getLocalKvircDirectory(banlist,KviApp::ConfigPlugins);
	banlist += g_pBanListFilename;
	QFile file;
	file.setName(banlist);
	file.open(IO_WriteOnly);

	QTextStream stream(&file);

	stream << g_pBanList->count() << endl;
	for(KviStr *tmp=g_pBanList->first();tmp;tmp=g_pBanList->next())
	{
		stream << tmp->ptr() << endl;
	}
	file.flush();
	file.close();
}

void loadBanList()
{
	QString banlist;
	g_pApp->getLocalKvircDirectory(banlist,KviApp::ConfigPlugins);
	banlist += g_pBanListFilename;
	QFile file;
	file.setName(banlist);
	if (!file.open(IO_ReadOnly))return;

	QTextStream stream(&file);

	g_pBanList->clear();

	int i=0;
	int num = stream.readLine().toInt();
	while ((!stream.atEnd()) && (i<num)){
		KviStr *tmp = new KviStr(stream.readLine());
		g_pBanList->append(tmp);
		i++;
	}
	file.close();
}


/*
 	@doc: url.list
	@type:
		command
	@title:
		url.list
	@short:
		Opens url list
	@syntax:
		url.list
	@description:
		This command opens a window containing the urls' list.
		In the list there is other information:[br]
		[U]Window[/U] : window where the url has been shown last[br]
		[U]Count[/U] : number of urls shown[br]
		[U]Timestamp[/U] : date/time when the url has been shown first[br]
		Clicking right on the url column of the list a menu will popup, through it
		you can remove the selected item, find the url in the window it appeared last, and
		say it to: [I]@Console, Channels, Querys, DCC Chats[/I] and [I]User windows[/I].[br]
		The list is saved to file when you click on the menu item or when you unload the plugin
		on condition that you have checked the relative checkbox in configuration dialog.[br]
		You can also open the url in your web browser double clicking on it in the url list window.[br][br]

  */

static bool url_kvs_cmd_list(KviKvsModuleCommandCall * c)
{
	urllist();
	return true;
}

UrlDlgList *findFrame()
{
	UrlDlgList *tmpitem = g_pUrlDlgList->first();
	if (!tmpitem) {
		UrlDlgList *udl = new UrlDlgList();
		udl->dlg = 0;
		udl->menu_id = -1;
		g_pUrlDlgList->append(udl);
		tmpitem = g_pUrlDlgList->current();
	}
	return tmpitem;
}

bool urllist()
{
	UrlDlgList *tmpitem = findFrame();
	if (tmpitem->dlg) return false;

	tmpitem->dlg = new UrlDialog(g_pList);
	g_pFrame->addWindow(tmpitem->dlg);

	for(KviUrl *tmp=g_pList->first();tmp;tmp=g_pList->next())
	{
		QString tmpCount;
		tmpCount.setNum(tmp->count);
		tmpitem->dlg->addUrl(QString(tmp->url), QString(tmp->window), tmpCount, QString(tmp->timestamp));
	}
	return true;
}


/*
 	@doc: url.config
	@type:
		command
	@title:
		url.config
	@short:
		Loads URL list module
	@syntax:
		url.config
	@description:
		This command opens a configuration window where it is possible
		to setup plugin's parameters. You can also open this window by
		using popup menu in the url list window or by clicking on the "configure plugin" button
		in plugins options.<BR><BR>
		<H3>Configure dialog options:</H3>
		There is also a ban list widget, which allows to have a list of words that plugin mustn't catch.<BR><BR>
		<I>E.g.<BR>
		<blockquote>if the word "ftp" is inserted in the ban list and if in a window there is an output like "ftp.kvirc.net",
		the url will not be catched.</blockquote></I>
		<HR>
*/

static bool url_kvs_cmd_config(KviKvsModuleCommandCall * c)
{
	if (!g_pConfigDialog) g_pConfigDialog = new ConfigDialog();
	return true;
}

int check_url(KviWindow *w,const QString &szUrl) // return 0 if no occurence of the url were found
{
	int tmp = 0;

	for(KviStr *tmpi=g_pBanList->first();tmpi;tmpi=g_pBanList->next())
	{
		if (szUrl.indexOf(QString(tmpi->ptr()),Qt::CaseInsensitive) != -1) tmp++;
	}
	if (tmp > 0) return tmp;

	for(KviUrl *u = g_pList->first();u;u=g_pList->next())
	{
		if (u->url == szUrl) {
			u->window = w->plainTextCaption();
			u->count++;
			tmp++;
		}
	}

	for (UrlDlgList *tmpitem=g_pUrlDlgList->first();tmpitem;tmpitem=g_pUrlDlgList->next()) {
		if (tmpitem->dlg) {
			KviTalListViewItemIterator lvi(tmpitem->dlg->m_pUrlList);
			for(;lvi.current();++lvi)
			{
				if (lvi.current()->text(0) == szUrl) {
					int tmpCount = lvi.current()->text(2).toInt();
					tmpCount++;
					QString tmpStr;
					tmpStr.setNum(tmpCount);
					lvi.current()->setText(2,tmpStr);
					lvi.current()->setText(1,w->plainTextCaption());
				}
			}
		}
	}
	return tmp;
}

bool urllist_module_event_onUrl(KviKvsModuleEventCall * c)
{
	KviKvsVariant * vUrl = c->firstParam();
	QString szUrl;
	if(vUrl)vUrl->asString(szUrl);

	if (check_url(c->window(),szUrl) == 0)
	{

		KviUrl *tmp = new KviUrl;
		KviStr tmpTimestamp;
		QDate d = QDate::currentDate();
		KviStr date(KviStr::Format,"%d-%d%d-%d%d",d.year(),d.month() / 10,d.month() % 10,d.day() / 10,d.day() % 10);
		tmpTimestamp = "["+date+"]"+" [";
		tmpTimestamp += QTime::currentTime().toString()+"]";
		tmp->url = szUrl;
		tmp->window = c->window()->plainTextCaption();
		tmp->count = 1;
		tmp->timestamp = tmpTimestamp;

		g_pList->append(tmp);
		for (UrlDlgList *tmpitem=g_pUrlDlgList->first();tmpitem;tmpitem=g_pUrlDlgList->next()) {
			if (tmpitem->dlg) {
				QString tmpCount;
				tmpCount.setNum(tmp->count);
				tmpitem->dlg->addUrl(QString(tmp->url), QString(tmp->window), tmpCount, QString(tmp->timestamp));
				tmpitem->dlg->taskBarItem()->highlight(false);
			}
		}
	}
	return true;
}


/*	@doc: url.load
	@type:
		command
	@title:
		url.load
	@short:
		Loads URL list module
	@syntax:
		url.load
	@description:
		Loads the URL list module which keeps track of all urls shown in kvirc windows.
*/


static bool url_module_init(KviModule *m)
{
	KviModuleExtensionDescriptor * d = m->registerExtension("tool",
							KVI_URL_EXTENSION_NAME,
							__tr2qs("View URL list"),
							url_extension_alloc);
	if(d)d->setIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_URL)));

	g_pList = new KviPointerList<KviUrl>;
	g_pList->setAutoDelete(true);

	g_pUrlDlgList = new KviPointerList<UrlDlgList>;
	g_pUrlDlgList->setAutoDelete(true);

	g_pBanList = new KviPointerList<KviStr>;
	g_pBanList->setAutoDelete(true);

	g_pUrlIconPixmap = new QPixmap(url_icon_xpm);

	KVSM_REGISTER_SIMPLE_COMMAND(m,"list",url_kvs_cmd_list);
	KVSM_REGISTER_SIMPLE_COMMAND(m,"config",url_kvs_cmd_config);
	g_pUrlAction = new KviUrlAction(KviActionManager::instance());
	KviActionManager::instance()->registerAction(g_pUrlAction);

	m->kvsRegisterAppEventHandler(KviEvent_OnUrl,urllist_module_event_onUrl);

	g_pApp->getLocalKvircDirectory(szConfigPath,KviApp::ConfigPlugins,"url.conf");

	loadUrlList();
	loadBanList();

	UrlDlgList *udl = new UrlDlgList();
	udl->dlg = 0;
	g_pUrlDlgList->append(udl);

	return true;
}

static bool url_module_cleanup(KviModule *m)
{
	KviConfig cfg(szConfigPath,KviConfig::Read);
	cfg.setGroup("ConfigDialog");
	if (cfg.readBoolEntry("SaveUrlListOnUnload",false) == true) saveUrlList();
	for (UrlDlgList *tmpitem=g_pUrlDlgList->first();tmpitem;tmpitem=g_pUrlDlgList->next()) {
		if (tmpitem->dlg) tmpitem->dlg->close();
	}

	delete g_pList;
	g_pList = 0;
	delete g_pBanList;
	g_pBanList = 0;

	delete g_pUrlDlgList;
	g_pUrlDlgList = 0;
	delete g_pUrlAction;
	g_pUrlAction = 0;


	delete g_pUrlIconPixmap;
	g_pUrlIconPixmap = 0;

	return true;
}

static bool url_module_can_unload(KviModule *m)
{
	// FIXME: really ugly :/
	return false;
}

void url_module_config()
{
	if (!g_pConfigDialog) g_pConfigDialog = new ConfigDialog();
}

/*
	@doc: url
	@type:
		module
	@short:
		The URL list module: keeps track of all urls shown in kvirc windows
	@title:
		The URL list module
	@body:
		This plugin keeps track of all urls shown in kvirc windows.
		<H3>Exported commands:</H3>
		<B>/url.list</B> : this command opens a window containing the urls' list.
		In the list there is other information:<BR>
		<U>Window</U> : window where the url has been shown last<BR>
		<U>Count</U> : number of urls shown<BR>
		<U>Timestamp</U> : date/time when the url has been shown first<BR>
		Clicking right on the url column of the list a menu will popup, through it
		you can remove the selected item, find the url in the window it appeared last, and
		say it to: <I>@Console, Channels, Querys, DCC Chats</I> and <I>User windows</I>.<BR>
		The list is saved to file when you click on the menu item or when you unload the plugin
		on condition that you have checked the relative checkbox in configuration dialog.<BR>
		You can also open the url in your web browser double clicking on it in the url list window.<BR><BR>

		Mail me if you have any suggestion or you want to notice a bug.<BR>
		<B>Andrea 'YaP' Parrella</B> &lt;anandrea@iname.com&gt;<BR><BR>
		Thanks to:<BR>
		<B>Szymon Stefanek</B> and <B>Till Bush</B> for their help.<BR>
		<B>Ted Podgurski</B> for toolbar's icons.
*/


KVIRC_MODULE(
	"URL",
	"4.0.0" ,
	"Copyright (C) 2002 Andrea Parrella <yap@yapsoft.it>",
	"url list module for KVIrc",
	url_module_init,
	url_module_can_unload,
	0,
	url_module_cleanup
)

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "libkviurl.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
