//=============================================================================
//
//   File : kvi_menubar.cpp
//   Creation date : Wen Jun 21 2000 13:12:11 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2000-2008 Szymon Stefanek (pragma at kvirc dot net)
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
//   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//=============================================================================


#include "kvi_menubar.h"
#include "kvi_app.h"
#include "kvi_locale.h"
#include "kvi_frame.h"
#include "kvi_mdimanager.h"
#include "kvi_iconmanager.h"
#include "kvi_internalcmd.h"
#include "kvi_settings.h"
#include "kvi_ircurl.h"
#include "kvi_console.h"
#include "kvi_kvs_popupmenu.h"
#include "kvi_malloc.h"
#include "kvi_moduleextension.h"
#include "kvi_actionmanager.h"
#include "kvi_coreactionnames.h"
#include "kvi_kvs_script.h"
#include "kvi_tal_popupmenu.h"

KviMenuBar::KviMenuBar(KviFrame * par,const char * name)
      : KviTalMenuBar(par,name)
{
	m_pFrm = par;

	m_iNumDefaultItems = 0;
	m_pDefaultItemId = 0;

	KviTalPopupMenu * pop = new KviTalPopupMenu(this,"KVIrc");
	connect(pop,SIGNAL(aboutToShow()),this,SLOT(setupMainPopup()));
#ifndef COMPILE_ON_MAC
	addDefaultItem("&KVIrc",pop);
#else
	// Qt/Mac creates already a "KVirc" menu item on its own, and we don't like double entries ;-)
	addDefaultItem("&IRC",pop);
#endif //COMPILE_ON_MAC
	m_pRecentServersPopup = new KviTalPopupMenu(this,"recentservers");
	connect(m_pRecentServersPopup,SIGNAL(aboutToShow()),this,SLOT(setupRecentServersPopup()));
	connect(m_pRecentServersPopup,SIGNAL(activated(int)),this,SLOT(newConnectionToServer(int)));

	m_pScriptItemList = 0;

	pop = new KviTalPopupMenu(this,"scripting");
	connect(pop,SIGNAL(aboutToShow()),this,SLOT(setupScriptingPopup()));
	addDefaultItem(__tr2qs("Scri&pting"),pop);

	pop = new KviTalPopupMenu(this,"tools");
	connect(pop,SIGNAL(aboutToShow()),this,SLOT(setupToolsPopup()));
	connect(pop,SIGNAL(activated(int)),this,SLOT(toolsPopupSelected(int)));
	addDefaultItem(__tr2qs("&Tools"),pop);

	m_pToolbarsPopup = new KviTalPopupMenu(this,"toolbars");
	connect(m_pToolbarsPopup,SIGNAL(aboutToShow()),this,SLOT(setupToolbarsPopup()));

	pop = new KviTalPopupMenu(this,"settings");
	connect(pop,SIGNAL(aboutToShow()),this,SLOT(setupSettingsPopup()));
	addDefaultItem(__tr2qs("&Settings"),pop);

	addDefaultItem(__tr2qs("&Window"),par->mdiManager()->windowPopup());

	pop = new KviTalPopupMenu(this,"help");
	connect(pop,SIGNAL(aboutToShow()),this,SLOT(setupHelpPopup()));
	connect(pop,SIGNAL(activated(int)),this,SLOT(actionTriggered(int)));
	addDefaultItem(__tr2qs("&Help"),pop);
}

KviMenuBar::~KviMenuBar()
{
	if(m_pScriptItemList)delete m_pScriptItemList;
	if(m_pDefaultItemId)kvi_free(m_pDefaultItemId);
}

void KviMenuBar::showEvent(QShowEvent *)
{
	// force a re-layout of the menubar in Qt4 (see the note in enterSDIMode())
	// by resetting the corner widget
	m_pFrm->mdiManager()->relayoutMenuButtons();
}

void KviMenuBar::addDefaultItem(const QString &text,KviTalPopupMenu * pop)
{
	m_iNumDefaultItems++;
	m_pDefaultItemId = (int *)kvi_realloc((void *)m_pDefaultItemId,sizeof(int) * m_iNumDefaultItems);
	pop->menuAction()->setText(text);
	addAction(pop->menuAction());
	m_pDefaultItemId[m_iNumDefaultItems - 1] = actions().indexOf(pop->menuAction());
}

void KviMenuBar::setupHelpPopup()
{
	KviTalPopupMenu * help = (KviTalPopupMenu *)sender();
	help->clear();

	int id = help->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_HELP)),__tr2qs("&Help Browser (Panel)"));
	help->setItemParameter(id,KVI_INTERNALCOMMAND_HELP_NEWSTATICWINDOW);

	id = help->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_MDIHELP)),__tr2qs("Help Browser (&Window)"));
	help->setItemParameter(id,KVI_INTERNALCOMMAND_HELP_NEWMDIWINDOW);
	help->insertSeparator();

	id = help->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_IDEA)),__tr2qs("&Tip of the Day"));
	help->setItemParameter(id,KVI_INTERNALCOMMAND_TIP_OPEN);
	help->insertSeparator();

	id = help->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_KVIRC)),__tr2qs("About &KVIrc"));

	help->setItemParameter(id,KVI_INTERNALCOMMAND_ABOUT_ABOUTKVIRC);
	help->insertSeparator();
	id = help->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_HOMEPAGE)),__tr2qs("KVIrc Home&page"));
	help->setItemParameter(id,KVI_INTERNALCOMMAND_KVIRC_HOMEPAGE);
	if(kvi_strEqualCIN(KviLocale::localeName(),"ru",2))
	{
		id = help->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_HOMEPAGE)),__tr2qs("KVIrc Russian Home&page"));
		help->setItemParameter(id,KVI_INTERNALCOMMAND_KVIRC_HOMEPAGE_RU);
	}
	if(kvi_strEqualCIN(KviLocale::localeName(),"fr",2))
	{
		id = help->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_HOMEPAGE)),__tr2qs("KVIrc French Home&page"));
		help->setItemParameter(id,KVI_INTERNALCOMMAND_KVIRC_HOMEPAGE_FR);
	}
	help->insertSeparator();
	id = help->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_MESSAGE)),__tr2qs("Subscribe to the Mailing List"));
	help->setItemParameter(id,KVI_INTERNALCOMMAND_OPENURL_KVIRC_MAILINGLIST);
	id = help->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_BOMB)),__tr2qs("Report a Bug / Propose Improvements"));
	help->setItemParameter(id,KVI_INTERNALCOMMAND_OPENURL_KVIRC_BUGTRACK);
	help->insertSeparator();
	id = help->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_CHANNEL)),__tr2qs("Join KVIrc International Channel on Freenode"));
	help->setItemParameter(id,KVI_INTERNALCOMMAND_OPENURL_KVIRC_ON_FREENODE);
	id = help->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_CHANNEL)),__tr2qs("Join KVIrc International Channel on IRCNet"));
	help->setItemParameter(id,KVI_INTERNALCOMMAND_OPENURL_KVIRC_ON_IRCNET);
	if(kvi_strEqualCIN(KviLocale::localeName(),"it",2))
	{
		// join #kvirc.net on azzurra
		id = help->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_CHANNEL)),__tr2qs("Join KVIrc Italian Channel on AzzurraNet"));
		help->setItemParameter(id,KVI_INTERNALCOMMAND_OPENURL_KVIRC_IT_ON_AZZURRA);
	}
	if(kvi_strEqualCIN(KviLocale::localeName(),"fr",2))
	{
		// join #kvirc-fr on freenode
		id = help->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_CHANNEL)),__tr2qs("Join KVIrc French Channel on Freenode"));
		help->setItemParameter(id,KVI_INTERNALCOMMAND_OPENURL_KVIRC_FR_ON_FREENODE);
		// join #kvirc on europnet
		id = help->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_CHANNEL)),__tr2qs("Join KVIrc French Channel on EuropNet"));
		help->setItemParameter(id,KVI_INTERNALCOMMAND_OPENURL_KVIRC_FR_ON_EUROPNET);
	}
}

void KviMenuBar::actionTriggered(int id)
{
	KviTalPopupMenu * help = (KviTalPopupMenu *)sender();
	m_pFrm->executeInternalCommand(help->itemParameter(id));
}

void KviMenuBar::setupSettingsPopup()
{

	KviTalPopupMenu * opt = (KviTalPopupMenu *)sender();
	opt->clear();

	opt->insertItem(__tr2qs("Toolbars"),m_pToolbarsPopup);

	int id = opt->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_STATUSBAR)),__tr2qs("Show StatusBar"),m_pFrm,SLOT(toggleStatusBar()));
	opt->setItemChecked(id,m_pFrm->mainStatusBar());


	opt->insertSeparator();
	// FIXME: #warning "Toggle these items on the fly ?"
	ACTION_POPUP_ITEM(KVI_COREACTION_GENERALOPTIONS,opt)
	ACTION_POPUP_ITEM(KVI_COREACTION_THEMEOPTIONS,opt)
	ACTION_POPUP_ITEM(KVI_COREACTION_MANAGETHEMES,opt)
	ACTION_POPUP_ITEM(KVI_COREACTION_MANAGEADDONS,opt)
	ACTION_POPUP_ITEM(KVI_COREACTION_SERVEROPTIONS,opt)
	ACTION_POPUP_ITEM(KVI_COREACTION_TOOLBAREDITOR,opt)
	ACTION_POPUP_ITEM(KVI_COREACTION_EDITREGUSERS,opt)

	opt->insertSeparator();
	opt->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_FLOPPY)),__tr2qs("&Save Configuration"),g_pApp,SLOT(saveConfiguration()));
}

void KviMenuBar::setupScriptingPopup()
{
	KviTalPopupMenu * script = (KviTalPopupMenu *)sender();
	script->clear();

	ACTION_POPUP_ITEM(KVI_COREACTION_ACTIONEDITOR,script)
	ACTION_POPUP_ITEM(KVI_COREACTION_ALIASEDITOR,script)
	ACTION_POPUP_ITEM(KVI_COREACTION_EVENTEDITOR,script)
	ACTION_POPUP_ITEM(KVI_COREACTION_POPUPEDITOR,script)
	ACTION_POPUP_ITEM(KVI_COREACTION_RAWEDITOR,script)
	script->insertSeparator();
	ACTION_POPUP_ITEM(KVI_COREACTION_CODETESTER,script)
	script->insertSeparator();
	ACTION_POPUP_ITEM(KVI_COREACTION_EXECUTEKVS,script)

	script->insertSeparator();
	script->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_TEXTEXCLAMATIVE)),__tr2qs("Restore &Default Script"),g_pApp,SLOT(restoreDefaultScript()));
}

void KviMenuBar::setupMainPopup()
{
	KviTalPopupMenu * main = (KviTalPopupMenu *)sender();
	main->clear();

	ACTION_POPUP_ITEM(KVI_COREACTION_NEWIRCCONTEXT,main)
	if(m_pFrm->activeContext())
		if(m_pFrm->activeContext()->state()==KviIrcContext::Connected)
		{
			int id = main->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_QUIT)),__tr2qs("Disconnect"),m_pFrm,SLOT(executeInternalCommand(int)));
			main->setItemParameter(id,KVI_INTERNALCOMMAND_QUIT);
		}
	main->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_WORLD)),__tr2qs("New &Connection To"),m_pRecentServersPopup);

	main->insertSeparator();

	if(m_pFrm->dockExtension())
	{
		int id = main->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_TRAYICON)),__tr2qs("Hide &Dock Icon"),m_pFrm,SLOT(executeInternalCommand(int)));
		main->setItemParameter(id,KVI_INTERNALCOMMAND_TRAYICON_HIDE);
	} else {
		int id = main->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_TRAYICON)),__tr2qs("Show &Dock Icon"),m_pFrm,SLOT(executeInternalCommand(int)));
		main->setItemParameter(id,KVI_INTERNALCOMMAND_TRAYICON_SHOW);
	}

// Qt/Mac creates a Quit item on its own <= this is bad
#ifndef COMPILE_ON_MAC
	main->insertSeparator();

	main->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_QUITAPP)),__tr2qs("&Quit"),g_pFrame,SLOT(close()));
#endif //COMPILE_ON_MAC
}

void KviMenuBar::setupRecentServersPopup()
{
	KviTalPopupMenu * m = (KviTalPopupMenu *)sender();
	g_pApp->fillRecentServersPopup(m);
	m->insertSeparator();
	m->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_SERVER)),__tr2qs("&Other..."));
}

void KviMenuBar::newConnectionToServer(int id)
{
	QString text = m_pRecentServersPopup->text(id);
	if(!text.isEmpty())
	{
		if(text == __tr2qs("&Other..."))
		{
			KviKvsScript::run("options.edit KviServerOptionsWidget",m_pFrm->firstConsole());
		} else {
			text.replace(QString("&"),QString(""));
			KviStr szCommand;
			if(KviIrcUrl::parse(text.toUtf8().data(),szCommand,KVI_IRCURL_CONTEXT_FIRSTFREE))
			{
				KviKvsScript::run(szCommand.ptr(),g_pApp->activeConsole());
			}
		}
	}
}

void KviMenuBar::setupToolsPopup()
{
	KviTalPopupMenu * m = (KviTalPopupMenu *)sender();
	if(!m)return;

	m->clear();

	KviModuleExtensionDescriptorList * l = g_pModuleExtensionManager->getExtensionList("tool");
	if(l)
	{
		for(KviModuleExtensionDescriptor * d = l->first();d;d = l->next())
		{
			int id;
			if(d->icon())
			{
				id = m->insertItem(*(d->icon()),d->visibleName());
			} else {
				id = m->insertItem(d->visibleName());
			}
			m->setItemParameter(id,d->id());
		}
	}
	m->insertSeparator();
	ACTION_POPUP_ITEM(KVI_COREACTION_SOCKETSPY,m)
	ACTION_POPUP_ITEM(KVI_COREACTION_NETWORKLINKS,m)
	ACTION_POPUP_ITEM(KVI_COREACTION_CHANNELLIST,m)
	m->insertSeparator();

	ACTION_POPUP_ITEM(KVI_COREACTION_SCREENSHOT,m)

	// moved the old tools here
	m->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_ICONMANAGER)),__tr2qs("Show &Icon Table"),g_pIconManager,SLOT(showIconWidget()));
#ifdef COMPILE_KDE_SUPPORT
	int id;
	id = m->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_TERMINAL)),__tr2qs("Open &Terminal"),m_pFrm,SLOT(executeInternalCommand(int)));
	m->setItemParameter(id,KVI_INTERNALCOMMAND_TERM_OPEN);
#endif

	KviPointerList<KviAction> list;
	KviActionManager::instance()->listActionsByCategory("tools",&list);
	if(!list.isEmpty())
	{
		m->insertSeparator();
		for(KviAction * ac = list.first();ac;ac = list.next())
			ac->addToPopupMenu(m);
	}
}

void KviMenuBar::toolsPopupSelected(int id)
{
	KviTalPopupMenu * m = (KviTalPopupMenu *)sender();
	if(!m) return;
	int idext = m->itemParameter(id);
	g_pModuleExtensionManager->allocateExtension("tool",idext,m_pFrm->firstConsole());
}

void KviMenuBar::setupToolbarsPopup()
{
	m_pFrm->fillToolBarsPopup(m_pToolbarsPopup);
}

//
// Script items
//

int KviMenuBar::getDefaultItemRealIndex(int iDefaultIndex)
{
	if(iDefaultIndex < 0)
		iDefaultIndex = 0;
	if(iDefaultIndex >= m_iNumDefaultItems)
		return m_pDefaultItemId[m_iNumDefaultItems - 1] + 1;

	return m_pDefaultItemId[iDefaultIndex];
}

KviScriptMenuBarItem * KviMenuBar::findMenu(const QString &text)
{
	if(!m_pScriptItemList)
		return 0;
	for(KviScriptMenuBarItem * i = m_pScriptItemList->first();i;i = m_pScriptItemList->next())
	{
		if(KviQString::equalCI(i->szText,text))return i;
	}
	return 0;
}

KviScriptMenuBarItem * KviMenuBar::findMenu(KviKvsPopupMenu * p)
{
	if(!m_pScriptItemList)return 0;
	for(KviScriptMenuBarItem * i = m_pScriptItemList->first();i;i = m_pScriptItemList->next())
	{
		if(i->pPopup == p) return i;
	}
	return 0;
}

bool KviMenuBar::removeMenu(const QString &text)
{
	KviScriptMenuBarItem * i = findMenu(text);
	if(i)
	{
		disconnect(i->pPopup,SIGNAL(destroyed()),this,SLOT(menuDestroyed()));
		removeAction(actions().at(i->id));
		m_pScriptItemList->removeRef(i);
		return true;
	}
	return false;
}

void KviMenuBar::menuDestroyed()
{
	KviScriptMenuBarItem * i = findMenu(((KviKvsPopupMenu *)sender()));
	if(i)
	{
		removeAction(actions().at(i->id));
		m_pScriptItemList->removeRef(i);
	}
}

void KviMenuBar::addMenu(const QString &text,KviKvsPopupMenu * p,int index)
{
	if(!m_pScriptItemList)
	{
		m_pScriptItemList = new KviPointerList<KviScriptMenuBarItem>;
		m_pScriptItemList->setAutoDelete(true);
	}

	KviScriptMenuBarItem * it = new KviScriptMenuBarItem;
	it->szText = text;
	it->szPopupName = p->objectName();
	it->pPopup = p;
	it->pPopup->menuAction()->setText(text);
	if(index == -1 || index >= actions().count())
	{
		addAction(it->pPopup->menuAction());
		it->id = actions().indexOf(it->pPopup->menuAction());
	} else {
		it->id = index;
		insertAction(actions().value(index), it->pPopup->menuAction());
	}

	connect(p,SIGNAL(destroyed()),this,SLOT(menuDestroyed()));
	m_pScriptItemList->append(it);
}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "kvi_menubar.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
