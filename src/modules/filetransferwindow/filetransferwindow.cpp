//==================================================================================
//
//   File : filetransferwindow.cpp
//   Creation date : Mon Apr 21 2003 23:14:12 CEST by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 1999-2001 Szymon Stefanek (pragma at kvirc dot net)
//
//   This program is FREE software. You can redistribute it and/or
//   modify it under the linkss of the GNU General Public License
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
//==================================================================================

#include "filetransferwindow.h"

#include "kvi_debug.h"
#include "kvi_iconmanager.h"
#include "kvi_ircview.h"
#include "kvi_out.h"
#include "kvi_options.h"
#include "kvi_locale.h"
#include "kvi_out.h"
#include "kvi_mirccntrl.h"
#include "kvi_themedlabel.h"
#include "kvi_input.h"
#include "kvi_qstring.h"
#include "kvi_tal_hbox.h"
#include "kvi_frame.h"

#include <QSplitter>
#include <QToolTip>
#include <QPainter>
#include <QMessageBox>
#include <QClipboard>
#include <QFileInfo>
#include <QFile>
#include <QLabel>
#include <QFontMetrics>
#include <QEvent>
#include <QKeyEvent>

#include <QHeaderView>

#ifdef COMPILE_KDE_SUPPORT
	#include <kurl.h>
	#include <krun.h>
	#include <kmimetype.h>
	#include <kmimetypetrader.h>
	#include <kiconloader.h>
#endif //COMPILE_KDE_SUPPORT

#ifdef COMPILE_PSEUDO_TRANSPARENCY
	extern KVIRC_API QPixmap * g_pShadedChildGlobalDesktopBackground;
#endif

extern KviFileTransferWindow * g_pFileTransferWindow;


KviFileTransferItem::KviFileTransferItem(KviFileTransferWidget * v,KviFileTransfer * t)
: KviTalTableWidgetItemEx(v)
{
	m_pTransfer = t;
	m_pTransfer->setDisplayItem(this);

	//create items for the second and the third column
	col1Item = new KviTalTableWidgetItem(v, row(), 1);
	col2Item = new KviTalTableWidgetItem(v, row(), 2);
	//FIXME fixed row height
	tableWidget()->setRowHeight( row(), 68 );
}

KviFileTransferItem::~KviFileTransferItem()
{
	m_pTransfer->setDisplayItem(0);
	delete col1Item;
	delete col2Item;
}

void KviFileTransferItem::displayUpdate()
{
	int dummy = (int) time(NULL);
	tableWidget()->model()->setData(tableWidget()->model()->index(row(),0), dummy, Qt::DisplayRole);
	tableWidget()->model()->setData(tableWidget()->model()->index(row(),1), dummy, Qt::DisplayRole);
	tableWidget()->model()->setData(tableWidget()->model()->index(row(),2), dummy, Qt::DisplayRole);
}

QString KviFileTransferItem::key(int column,bool bAcending) const
{
	QString ret;
	ret.setNum(m_pTransfer->id());
	if(ret.length() == 1)ret.prepend("0000000");
	else if(ret.length() == 2)ret.prepend("000000");
	else if(ret.length() == 3)ret.prepend("00000");
	else if(ret.length() == 4)ret.prepend("0000");
	else if(ret.length() == 5)ret.prepend("000");
	else if(ret.length() == 6)ret.prepend("00");
	else if(ret.length() == 7)ret.prepend("0");
	return ret;
}

void KviFileTransferWidget::paintEvent(QPaintEvent * event)
{
	QPainter *p = new QPainter(viewport());
	QStyleOptionViewItem option = viewOptions();
	QRect rect = event->rect();
	KviFileTransferItem* item;
	int r, c;

#ifdef COMPILE_PSEUDO_TRANSPARENCY
	if(g_pShadedChildGlobalDesktopBackground)
	{
		QPoint pnt = viewport()->mapToGlobal(rect.topLeft());
		p->drawTiledPixmap(rect,*g_pShadedChildGlobalDesktopBackground,pnt);
	} else {
#endif
		//FIXME this is not the treewindowlist
		p->fillRect(rect,KVI_OPTION_COLOR(KviOption_colorTreeWindowListBackground));
#ifdef COMPILE_PSEUDO_TRANSPARENCY
	}
#endif
	delete p;

	//call paint on all childrens
	KviTalTableWidget::paintEvent(event);
}

void KviFileTransferItemDelegate::paint(QPainter * p, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
	//FIXME not exactly model/view coding style.. but we need to access data on the item by now
	KviFileTransferWidget* tableWidget = (KviFileTransferWidget*)parent();
	KviFileTransferItem* item = (KviFileTransferItem*) tableWidget->itemFromIndex(index);

	KviFileTransfer* transfer = ((KviFileTransferItem*)tableWidget->item(item->row(), 0))->transfer();

	p->setFont(option.font);

	p->setPen(option.state & QStyle::State_Selected ? option.palette.highlight().color() : option.palette.base().color());

	p->drawRect(option.rect);
	p->setPen(transfer->active() ? QColor(180,180,180) : QColor(200,200,200));

	p->drawRect(option.rect.left() + 1, option.rect.top() + 1, option.rect.width() - 2,option.rect.height() - 2);
	p->fillRect(option.rect.left() + 2, option.rect.top() + 2, option.rect.width() - 4, option.rect.height() - 4,transfer->active() ? QColor(240,240,240) : QColor(225,225,225));

	transfer->displayPaint(p, index.column(), option.rect);
}

QSize KviFileTransferItemDelegate::sizeHint( const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
	// FIXME fixed width
	return QSize(((KviFileTransferWidget*)parent())->viewport()->size().width(), 68);
}
/*
void KviFileTransferItem::setHeight(int h)
{
	KviTalTableWidgetItem::setHeight(m_pTransfer->displayHeight(g_pFileTransferWindow->lineSpacing()));
}
*/

KviFileTransferWindow::KviFileTransferWindow(KviModuleExtensionDescriptor * d,KviFrame * lpFrm)
: KviWindow(KVI_WINDOW_TYPE_TOOL,lpFrm,"file transfer window",0) , KviModuleExtension(d)
{
	g_pFileTransferWindow = this;

	m_pContextPopup = 0;
	m_pLocalFilePopup = 0;
	m_pOpenFilePopup = 0;

	m_pMemPixmap = new QPixmap(1,1);

	m_pTimer = new QTimer(this);
	connect(m_pTimer,SIGNAL(timeout()),this,SLOT(heartbeat()));

	m_pInput   = new KviInput(this,0);

	m_pSplitter = new QSplitter(Qt::Horizontal,this);
	m_pSplitter->setObjectName("transferwindow_hsplitter");
	m_pVertSplitter = new QSplitter(Qt::Vertical,m_pSplitter);
	m_pVertSplitter->setObjectName("transferwindow_vsplitter");
	
	m_pTableWidget  = new KviFileTransferWidget(m_pVertSplitter);

	//ad-hoc itemdelegate for this view
	m_pItemDelegate = new KviFileTransferItemDelegate(m_pTableWidget);
	m_pTableWidget->setItemDelegate(m_pItemDelegate);

	m_pTableWidget->verticalHeader()->hide();
	m_pTableWidget->setShowGrid(false);

	m_pTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_pTableWidget->setSelectionMode(QAbstractItemView::SingleSelection);

	m_pTableWidget->setColumnCount(3);

	QStringList colHeaders;
	colHeaders.append(__tr2qs_ctx("Type","filetransferwindow"));
	colHeaders.append(__tr2qs_ctx("Information","filetransferwindow"));
	colHeaders.append(__tr2qs_ctx("Progress","filetransferwindow"));
	m_pTableWidget->setHorizontalHeaderLabels(colHeaders);
	m_pTableWidget->horizontalHeader()->setResizeMode(QHeaderView::Interactive);
	m_pTableWidget->horizontalHeader()->setMinimumSectionSize(50);

	//default column widths
	m_pTableWidget->setColumnWidth(0, 68);
	m_pTableWidget->horizontalHeader()->setResizeMode(0, QHeaderView::Fixed);
	m_pTableWidget->horizontalHeader()->setResizeMode(1, QHeaderView::Stretch);
	m_pTableWidget->horizontalHeader()->setResizeMode(2, QHeaderView::Stretch);

	KviDynamicToolTip * tp = new KviDynamicToolTip(m_pTableWidget->viewport());
	//TODO
	//connect(tp,SIGNAL(tipRequest(KviDynamicToolTip *,const QPoint &)),this,SLOT(tipRequest(KviDynamicToolTip *,const QPoint &)));

	m_pTableWidget->setFocusPolicy(Qt::NoFocus);
	m_pTableWidget->viewport()->setFocusPolicy(Qt::NoFocus);

	//connect(m_pTableWidget,SIGNAL(rightButtonPressed(KviTalTableWidgetItem *,const QPoint &,int)),
	// this,SLOT(showHostPopup(KviTalTableWidgetItem *,const QPoint &,int)));

	QFontMetrics fm(font());
	m_iLineSpacing = fm.lineSpacing();

	m_pIrcView = new KviIrcView(m_pVertSplitter,lpFrm,this);

	m_pTableWidget->installEventFilter(this);
	connect(m_pTableWidget,SIGNAL(rightButtonPressed(KviTalTableWidgetItem *,const QPoint &,int)),this,SLOT(rightButtonPressed(KviTalTableWidgetItem *,const QPoint &,int)));
	connect(m_pTableWidget,SIGNAL(doubleClicked(KviTalTableWidgetItem *,const QPoint &,int)),this,SLOT(doubleClicked(KviTalTableWidgetItem *,const QPoint &,int)));
	fillTransferView();

	connect(KviFileTransferManager::instance(),SIGNAL(transferRegistered(KviFileTransfer *)),this,SLOT(transferRegistered(KviFileTransfer *)));
	connect(KviFileTransferManager::instance(),SIGNAL(transferUnregistering(KviFileTransfer *)),this,SLOT(transferUnregistering(KviFileTransfer *)));

	KviFileTransferManager::instance()->setTransferWindow(this);

	//setFocusHandler(m_pInput,this);

	m_pTimer->start(2000);
}

KviFileTransferWindow::~KviFileTransferWindow()
{
	KviFileTransferManager::instance()->setTransferWindow(0);
	g_pFileTransferWindow = 0;
	delete m_pMemPixmap;
}

bool KviFileTransferWindow::eventFilter( QObject *obj, QEvent *ev )
{
	if( (obj==m_pTableWidget) && ( ev->type() == QEvent::KeyPress ) )
	{
		QKeyEvent *keyEvent = (QKeyEvent*)ev;
		switch(keyEvent->key())
		{
			case Qt::Key_Delete:
				if(m_pTableWidget->currentItem())
				{
					delete m_pTableWidget->currentItem();
					return TRUE;
				}
				break;
		}
	}
	return KviWindow::eventFilter( obj, ev );
}

void KviFileTransferWindow::fontChange(const QFont &oldFont)
{
	QFontMetrics fm(font());
	m_iLineSpacing = fm.lineSpacing();
	KviWindow::fontChange(oldFont);
}

void KviFileTransferWindow::tipRequest(KviDynamicToolTip * tip,const QPoint &pnt)
{
	KviFileTransferItem * it = (KviFileTransferItem *)m_pTableWidget->itemAt(pnt);
	if(!it)return;
	QString txt = it->transfer()->tipText();
	tip->tip(m_pTableWidget->visualItemRect(it),txt);
}

void KviFileTransferWindow::fillTransferView()
{
	KviPointerList<KviFileTransfer> * l = KviFileTransferManager::instance()->transferList();
	if(!l)return;
	KviFileTransferItem * it;
	for(KviFileTransfer * t = l->first();t;t = l->next())
	{
		it = new KviFileTransferItem(m_pTableWidget,t);
		t->setDisplayItem(it);
	}
}

KviFileTransferItem * KviFileTransferWindow::findItem(KviFileTransfer * t)
{
	int i;
	KviFileTransferItem * it;

	for(i=0;i<m_pTableWidget->rowCount();i++)
	{
		it = (KviFileTransferItem *)m_pTableWidget->item(i,0);
		if(it->transfer() == t)return it;
	}
	return 0;
}

void KviFileTransferWindow::transferRegistered(KviFileTransfer * t)
{
	KviFileTransferItem * it = new KviFileTransferItem(m_pTableWidget,t);
	//t->setDisplayItem(it);
}

void KviFileTransferWindow::transferUnregistering(KviFileTransfer * t)
{
	KviFileTransferItem * it = findItem(t);
	//t->setDisplayItem(0);
	if(it)delete it;
    it = 0;
}

void KviFileTransferWindow::doubleClicked(KviFileTransferItem *it,const QPoint &pnt,int col)
{
	if(it) openLocalFile();
}

void KviFileTransferWindow::rightButtonPressed(KviFileTransferItem *it,const QPoint &pnt,int col)
{
	if(!m_pContextPopup)m_pContextPopup = new KviTalPopupMenu(this);
	if(!m_pLocalFilePopup)m_pLocalFilePopup = new KviTalPopupMenu(this);
	if(!m_pOpenFilePopup)
	{
		m_pOpenFilePopup= new KviTalPopupMenu(this);
		connect(m_pOpenFilePopup,SIGNAL(activated(int)),this,SLOT(openFilePopupActivated(int)));
	}

	m_pContextPopup->clear();

	int id;

	if(it)
	{
		KviFileTransferItem * i = (KviFileTransferItem *)it;
		if(i->transfer())
		{

			QString szFile = i->transfer()->localFileName();
			if(!szFile.isEmpty())
			{
				m_pLocalFilePopup->clear();

				QString tmp = "<b>file:/";
				tmp += szFile;
				tmp += "</b><br>";

				QFileInfo fi(szFile);
				if(fi.exists())
				{
					tmp += "<nobr>";
					tmp += __tr2qs_ctx("Size: %1","filetransferwindow").arg(KviQString::makeSizeReadable(fi.size()));
					tmp += "</nobr><br>";
				}

#ifdef COMPILE_KDE_SUPPORT
				tmp += "<nobr>Mime: ";
				tmp += KMimeType::findByPath(szFile)->name();
				tmp += "</nobr>";
#endif //COMPILE_KDE_SUPPORT

				QLabel * l = new QLabel(tmp,m_pLocalFilePopup);
				QPalette p;
				l->setStyleSheet("background-color: " + p.color(QPalette::Normal, QPalette::Mid).name());
				m_pLocalFilePopup->insertItem(l);

#ifdef COMPILE_KDE_SUPPORT
				QString mimetype = KMimeType::findByPath(szFile)->name();
				KService::List offers = KMimeTypeTrader::self()->query(mimetype,"Application");

				id = m_pLocalFilePopup->insertItem(__tr2qs_ctx("&Open","filetransferwindow"),this,SLOT(openLocalFile()));
				m_pLocalFilePopup->setItemParameter(id,-1);

				m_pOpenFilePopup->clear();

				int id;
				int idx = 0;

				for(KService::List::Iterator itOffers = offers.begin();
	   				itOffers != offers.end(); ++itOffers)
				{
					id = m_pOpenFilePopup->insertItem(
							SmallIcon((*itOffers).data()->icon()),
							(*itOffers).data()->name()
						);
					m_pOpenFilePopup->setItemParameter(id,idx);
					idx++;
				}

				m_pOpenFilePopup->insertSeparator();

				id = m_pOpenFilePopup->insertItem(__tr2qs_ctx("&Other...","filetransferwindow"),this,SLOT(openLocalFileWith()));
				m_pOpenFilePopup->setItemParameter(id,-1);

				m_pLocalFilePopup->insertItem(__tr2qs_ctx("Open &With","filetransferwindow"),m_pOpenFilePopup);
				m_pLocalFilePopup->insertSeparator();
				m_pLocalFilePopup->insertItem(__tr2qs_ctx("Open &Location","filetransferwindow"),this,SLOT(openLocalFileFolder()));
				m_pLocalFilePopup->insertItem(__tr2qs_ctx("Terminal at Location","filetransferwindow"),this,SLOT(openLocalFileTerminal()));
				m_pLocalFilePopup->insertSeparator();
#endif //COMPILE_KDE_SUPPORT

//-| Grifisx & Noldor |-
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
				id = m_pLocalFilePopup->insertItem(__tr2qs_ctx("&Open","filetransferwindow"),this,SLOT(openLocalFile()));
				m_pLocalFilePopup->setItemParameter(id,-1);
				m_pOpenFilePopup->insertSeparator();
				m_pLocalFilePopup->insertItem(__tr2qs_ctx("Open &With","filetransferwindow"),this,SLOT(openLocalFileWith()));
				m_pLocalFilePopup->insertSeparator();
				m_pLocalFilePopup->insertItem(__tr2qs_ctx("Open &Location","filetransferwindow"),this,SLOT(openLocalFileFolder()));
				m_pLocalFilePopup->insertItem(__tr2qs_ctx("MS-DOS Prompt at Location","filetransferwindow"),this,SLOT(openLocalFileTerminal()));
				m_pLocalFilePopup->insertSeparator();
#endif
// G&N end

				m_pLocalFilePopup->insertItem(__tr2qs_ctx("&Copy Path to Clipboard","filetransferwindow"),this,SLOT(copyLocalFileToClipboard()));

				id = m_pLocalFilePopup->insertItem(__tr2qs_ctx("&Delete file","filetransferwindow"),this,SLOT(deleteLocalFile()));
				m_pLocalFilePopup->setItemEnabled(id,i->transfer()->terminated());
				m_pContextPopup->insertItem(__tr2qs_ctx("Local &File","filetransferwindow"),m_pLocalFilePopup);


			}

			i->transfer()->fillContextPopup(m_pContextPopup,col);
			m_pContextPopup->insertSeparator();
		}
	}


	bool bHaveTerminated = false;
	int i;
	KviFileTransferItem * item;

	for(i=0;i<m_pTableWidget->rowCount();i++)
	{
		item = (KviFileTransferItem *)m_pTableWidget->item(i,0);
		if(item->transfer()->terminated())
		{
			bHaveTerminated = true;
			break;
		}
	}

	id = m_pContextPopup->insertItem(__tr2qs_ctx("&Clear Terminated","filetransferwindow"),this,SLOT(clearTerminated()));
	m_pContextPopup->setItemEnabled(id,bHaveTerminated);

	bool bAreTransfersActive = false;
	if(m_pTableWidget->rowCount() >= 1)
		bAreTransfersActive = true;

	id = m_pContextPopup->insertItem(__tr2qs_ctx("Clear &All","filetransferwindow"),this,SLOT(clearAll()));
	m_pContextPopup->setItemEnabled(id,bAreTransfersActive);

	m_pContextPopup->popup(pnt);
}


KviFileTransfer * KviFileTransferWindow::selectedTransfer()
{
	if(m_pTableWidget->selectedItems().count() == 0) return 0;
	KviFileTransferItem * it = (KviFileTransferItem *)m_pTableWidget->selectedItems().first();
	if(!it)return 0;
	KviFileTransferItem * i = (KviFileTransferItem *)it;
	return i->transfer();
}

void KviFileTransferWindow::openFilePopupActivated(int id)
{
#ifdef COMPILE_KDE_SUPPORT
	int ip = m_pOpenFilePopup->itemParameter(id);
	if(ip < 0)return;
	QString txt = m_pOpenFilePopup->text(id);

	KviFileTransfer * t = selectedTransfer();
	if(!t)return;
	QString tmp = t->localFileName();
	if(tmp.isEmpty())return;


	QString mimetype = KMimeType::findByPath(tmp)->name();
	KService::List offers = KMimeTypeTrader::self()->query(mimetype,"Application");

	for(KService::List::Iterator itOffers = offers.begin();
	   				itOffers != offers.end(); ++itOffers)
	{
		if(txt == (*itOffers).data()->name())
		{
			KUrl::List lst;
			KUrl url;
			url.setPath(tmp);
			lst.append(url);
			KRun::run(*((*itOffers).data()),lst,g_pFrame);
			break;
		}
	}
#endif //COMPILE_KDE_SUPPORT
}

void KviFileTransferWindow::openLocalFileTerminal()
{
//-| Grifisx & Noldor |-
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
	KviFileTransfer * t = selectedTransfer();
	if(!t)return;
	QString tmp = t->localFileName();
	if(tmp.isEmpty())return;
	int idx = tmp.lastIndexOf("/");
	if(idx == -1)return;
	tmp = tmp.left(idx);
	tmp.append("\"");
	// FIXME: this is not a solution ...because if the drive isn't system's drive the command 'cd' naturaly doesn't work
	tmp.prepend("cmd.exe /k cd \"");
	system(tmp.toLocal8Bit().data());
#else
// G&N end
	#ifdef COMPILE_KDE_SUPPORT
		KviFileTransfer * t = selectedTransfer();
		if(!t)return;
		QString tmp = t->localFileName();
		if(tmp.isEmpty())return;

		int idx = tmp.lastIndexOf("/");
		if(idx == -1)return;
		tmp = tmp.left(idx);

		tmp.prepend("konsole --workdir=\"");
		tmp.append("\"");

		KRun::runCommand(tmp,g_pFrame);
	#endif //COMPILE_KDE_SUPPORT
#endif
}

void KviFileTransferWindow::deleteLocalFile()
{
	KviFileTransfer * t = selectedTransfer();
	if(!t)return;

	QString fName = t->localFileName();
	QString tmp;

	KviQString::sprintf(tmp,__tr2qs_ctx("Do you really want to delete the file %Q?","filetransferwindow"),&fName);

	if(QMessageBox::warning(this,__tr2qs_ctx("Confirm delete","filetransferwindow"),
			tmp,__tr2qs_ctx("Yes","filetransferwindow"),__tr2qs_ctx("No","filetransferwindow")) != 0)
		return;

	if(!QFile::remove(fName))
		QMessageBox::warning(this,__tr2qs_ctx("Delete failed","filetransferwindow"),
			__tr2qs_ctx("Failed to remove the file","filetransferwindow"),
			__tr2qs_ctx("OK","filetransferwindow"));
}

void KviFileTransferWindow::openLocalFile()
{
//-| Grifisx & Noldor |-
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)

	KviFileTransfer * t = selectedTransfer();
	if(!t)return;
	QString tmp = t->localFileName();
	if(tmp.isEmpty())return;
	tmp.replace("/","\\");
	ShellExecute(0,"open",tmp.toLocal8Bit().data(),NULL,NULL,SW_SHOWNORMAL);  //You have to link the shell32.lib
#else
// G&N end
	#ifdef COMPILE_KDE_SUPPORT
		KviFileTransfer * t = selectedTransfer();
		if(!t)return;
		QString tmp = t->localFileName();
		if(tmp.isEmpty())return;

		QString mimetype = KMimeType::findByPath(tmp)->name();  //KMimeType
		KService::Ptr offer = KMimeTypeTrader::self()->preferredService(mimetype,"Application");
		if(!offer)
		{
			openLocalFileWith();
			return;
		}

		KUrl::List lst;
		KUrl url;
		url.setPath(tmp);
		lst.append(url);

		KRun::run(*offer, lst, g_pFrame);
	#endif //COMPILE_KDE_SUPPORT
#endif
}

void KviFileTransferWindow::openLocalFileWith()
{
//-| Grifisx & Noldor |-
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
	KviFileTransfer * t = selectedTransfer();
	if(!t)return;
	QString tmp = t->localFileName();
	if(tmp.isEmpty())return;
	tmp.replace("/","\\");
	tmp.prepend("rundll32.exe shell32.dll,OpenAs_RunDLL "); // this if to show the 'open with...' window
	WinExec(tmp.toLocal8Bit().data(),SW_SHOWNORMAL);
#else
// G&N end
	#ifdef COMPILE_KDE_SUPPORT
		KviFileTransfer * t = selectedTransfer();
		if(!t)return;
		QString tmp = t->localFileName();
		if(tmp.isEmpty())return;

		KUrl::List lst;
		KUrl url;
		url.setPath(tmp);
		lst.append(url);
		KRun::displayOpenWithDialog(lst,g_pFrame);
	#endif //COMPILE_KDE_SUPPORT
#endif
}

void KviFileTransferWindow::copyLocalFileToClipboard()
{
	KviFileTransfer * t = selectedTransfer();
	if(!t)return;
	QString tmp = t->localFileName();
	if(tmp.isEmpty())return;
	QApplication::clipboard()->setText(tmp);
}

void KviFileTransferWindow::openLocalFileFolder()
{
//-| Grifisx & Noldor|-
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
	KviFileTransfer * t = selectedTransfer();
	if(!t)return;
	QString tmp = t->localFileName();
	if(tmp.isEmpty())return;
	tmp=QFileInfo(tmp).absolutePath();
	tmp.replace('/','\\');
	tmp.prepend("explorer.exe ");
	WinExec(tmp.toLocal8Bit().data(), SW_MAXIMIZE);
#else
// G&N end
	#ifdef COMPILE_KDE_SUPPORT
		KviFileTransfer * t = selectedTransfer();
		if(!t)return;
		QString tmp = t->localFileName();
		if(tmp.isEmpty())return;

		int idx = tmp.lastIndexOf("/");
		if(idx == -1)return;
		tmp = tmp.left(idx);

		QString mimetype = KMimeType::findByPath(tmp)->name(); // inode/directory
		KService::Ptr offer = KMimeTypeTrader::self()->preferredService(mimetype,"Application");
		if(!offer)return;

		KUrl::List lst;
		KUrl url;
		url.setPath(tmp);
		lst.append(url);
		KRun::run(*offer,lst,g_pFrame);
	#endif //COMPILE_KDE_SUPPORT
#endif
}

void KviFileTransferWindow::heartbeat()
{
	int i;
	QModelIndex index;
	KviFileTransferItem* it;
	int dummy = (int) time(NULL);

	for(i=0;i<=m_pTableWidget->rowCount();i++)
	{
		it = (KviFileTransferItem *)m_pTableWidget->item(i,0);

		if(it)
		{
			if(it->transfer()->active())
			{
				m_pTableWidget->model()->setData(m_pTableWidget->model()->index(i,0), dummy, Qt::DisplayRole);
				m_pTableWidget->model()->setData(m_pTableWidget->model()->index(i,1), dummy, Qt::DisplayRole);
				m_pTableWidget->model()->setData(m_pTableWidget->model()->index(i,2), dummy, Qt::DisplayRole);
			}
		}
	}
}

void KviFileTransferWindow::clearAll()
{
	QString tmp;

	bool bHaveAllTerminated = true;
	int i;
	KviFileTransferItem * item;

	for(i=0;i<m_pTableWidget->rowCount();i++)
	{
		item = (KviFileTransferItem *)m_pTableWidget->item(i,0);
		if(!item->transfer()->terminated())
		{
			bHaveAllTerminated = false;
			break;
		}
	}

	KviQString::sprintf(tmp,__tr2qs_ctx("Clear all transfers, including any in progress?","filetransferwindow"));

	// If any transfer is active asks for confirm
	if(!bHaveAllTerminated)
		if(QMessageBox::warning(this,__tr2qs_ctx("Clear All Transfers? - KVIrc","filetransferwindow"), tmp,__tr2qs_ctx("Yes","filetransferwindow"),__tr2qs_ctx("No","filetransferwindow")) != 0)
			return;

	KviFileTransferManager::instance()->killAllTransfers();
}

void KviFileTransferWindow::clearTerminated()
{
	KviFileTransferManager::instance()->killTerminatedTransfers();
}

void KviFileTransferWindow::getBaseLogFileName(KviStr &buffer)
{
	buffer.sprintf("FILETRANSFER");
}

QPixmap * KviFileTransferWindow::myIconPtr()
{
	return g_pIconManager->getSmallIcon(KVI_SMALLICON_FILETRANSFER);
}

void KviFileTransferWindow::resizeEvent(QResizeEvent *e)
{
	int h = m_pInput->heightHint();
	m_pSplitter->setGeometry(0,0,width(),height() - h);
	m_pInput->setGeometry(0,height() - h,width(),h);
}

QSize KviFileTransferWindow::sizeHint() const
{
	return m_pSplitter->sizeHint();
}

void KviFileTransferWindow::fillCaptionBuffers()
{
	m_szPlainTextCaption = __tr2qs_ctx("File Transfers","filetransferwindow");

	m_szHtmlActiveCaption = "<nobr><font color=\"";
	m_szHtmlActiveCaption += KVI_OPTION_COLOR(KviOption_colorCaptionTextActive).name();
	m_szHtmlActiveCaption += "\"><b>";
	m_szHtmlActiveCaption += m_szPlainTextCaption;
	m_szHtmlActiveCaption += "</b></font></nobr>";

	m_szHtmlInactiveCaption = "<nobr><font color=\"";
	m_szHtmlInactiveCaption += KVI_OPTION_COLOR(KviOption_colorCaptionTextInactive).name();
	m_szHtmlInactiveCaption += "\"><b>";
	m_szHtmlInactiveCaption += m_szPlainTextCaption;
	m_szHtmlInactiveCaption += "</b></font></nobr>";
}

void KviFileTransferWindow::die()
{
	close();
}



//#warning "Load & save properties of this kind of window"

//void KviFileTransferWindow::saveProperties()
//{
//	KviWindowProperty p;
//	p.rect = externalGeometry();
//	p.isDocked = isAttacched();
//	QValueList<int> l(m_pSplitter->sizes());
//	if(l.count() >= 1)p.splitWidth1 = *(l.at(0));
//	if(l.count() >= 2)p.splitWidth2 = *(l.at(1));
//	p.timestamp = m_pView->timestamp();
//	p.imagesVisible = m_pView->imagesVisible();
//	p.isMaximized = isAttacched() && isMaximized();
//	p.topSplitWidth1 = 0;
//	p.topSplitWidth2 = 0;
//	p.topSplitWidth3 = 0;
//	g_pOptions->m_pWinPropertiesList->setProperty(caption(),&p);
//}
//
//void KviFileTransferWindow::setProperties(KviWindowProperty *p)
//{
//	QValueList<int> l;
//	l.append(p->splitWidth1);
//	l.append(p->splitWidth2);
//	m_pVertSplitter->setSizes(l);
//	m_pIrcView->setTimestamp(p->timestamp);
//	m_pIrcView->setShowImages(p->imagesVisible);
//}

void KviFileTransferWindow::applyOptions()
{
	m_pIrcView->applyOptions();
	KviWindow::applyOptions();
}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "m_filetransferwindow.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
