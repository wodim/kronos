//
//   File : codetester.cpp
//   Creation date : Mon Dec 23 2002 20:28:18 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2002 Szymon Stefanek (pragma at kvirc dot net)
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
#include "codetester.h"

#include "kvi_iconmanager.h"
#include "kvi_options.h"
#include "kvi_locale.h"
#include "kvi_config.h"
#include "kvi_filedialog.h"
#include "kvi_fileutils.h"
#include "kvi_scripteditor.h"
#include "kvi_app.h"
#include "kvi_console.h"
#include "kvi_parameterlist.h"
#include "kvi_kvs_script.h"
#include "kvi_kvs_variantlist.h"
#include "kvi_tal_popupmenu.h"

#include <QPushButton>
#include <QLayout>
#include <QLabel>
#include <QToolButton>


extern KviPointerList<KviCodeTesterWindow> * g_pCodeTesterWindowList;


KviCodeTester::KviCodeTester(QWidget * par)
: QWidget(par)
{
	setObjectName("code_tester");
	QGridLayout * g = new QGridLayout(this);
	m_pEditor = KviScriptEditor::createInstance(this);
	g->addWidget(m_pEditor,0,0,0,4);
	//g->addMultiCellWidget(m_pEditor,0,0,0,3);
	m_pExecuteButton = new QPushButton(__tr2qs("&Execute"),this);
	g->addWidget(m_pExecuteButton,1,3);
	connect(m_pExecuteButton,SIGNAL(clicked()),this,SLOT(execute()));

	m_pModeLabel = new QLabel(__tr("Params:"),this);
	g->addWidget(m_pModeLabel,1,1);
	m_pParams = new QLineEdit(this);
	g->addWidget(m_pParams,1,2);
}

KviCodeTester::~KviCodeTester()
{
	KviScriptEditor::destroyInstance(m_pEditor);
}

//#warning "Allow to bind the command to a specified window"

void KviCodeTester::execute()
{
	QString parms = m_pParams->text();
	QString buffer;
	m_pEditor->getText(buffer);
	KviConsole * con = g_pApp->activeConsole();
	//KviParameterList * l = new KviParameterList(parms.ptr());
	KviKvsScript::run(buffer,con,new KviKvsVariantList(new QString(parms)));
}

KviCodeTesterWindow::KviCodeTesterWindow(KviFrame * lpFrm)
: KviWindow(KVI_WINDOW_TYPE_SCRIPTEDITOR,lpFrm,"codetester",0)
{
	g_pCodeTesterWindowList->append(this);

	m_pTester = new KviCodeTester(this);
}

KviCodeTesterWindow::~KviCodeTesterWindow()
{
	g_pCodeTesterWindowList->removeRef(this);
}

QPixmap * KviCodeTesterWindow::myIconPtr()
{
	return g_pIconManager->getSmallIcon(KVI_SMALLICON_BOMB);
}

void KviCodeTesterWindow::resizeEvent(QResizeEvent *e)
{
	m_pTester->setGeometry(0,0,width(),height());
}

void KviCodeTesterWindow::fillCaptionBuffers()
{
	m_szPlainTextCaption = __tr2qs("Script Tester");

	static QString p1("<nobr><font color=\"");
	static QString p2("\"><b>");
	static QString p3("</b></font></nobr>");

	m_szHtmlActiveCaption = p1;
	m_szHtmlActiveCaption += KVI_OPTION_COLOR(KviOption_colorCaptionTextActive).name();
	m_szHtmlActiveCaption += p2;
	m_szHtmlActiveCaption += m_szPlainTextCaption;
	m_szHtmlActiveCaption += p3;

	m_szHtmlInactiveCaption = p1;
	m_szHtmlInactiveCaption += KVI_OPTION_COLOR(KviOption_colorCaptionTextInactive).name();
	m_szHtmlInactiveCaption += p2;
	m_szHtmlInactiveCaption += m_szPlainTextCaption;
	m_szHtmlInactiveCaption += p3;
}

void KviCodeTesterWindow::getConfigGroupName(KviStr &szName)
{
	szName = "codetester";
}

void KviCodeTesterWindow::saveProperties(KviConfig *cfg)
{
/*
#ifdef COMPILE_SCRIPTTOOLBAR
	cfg->writeEntry("Sizes",m_pEditor->sizes());
	cfg->writeEntry("LastRaw",m_pEditor->lastEditedRaw().ptr());
	//debug("LAST EDITED=%s",m_pEditor->lastEditedRaw().ptr());
#endif // COMPILE_SCRIPTTOOLBAR
*/
}

void KviCodeTesterWindow::loadProperties(KviConfig *cfg)
{
/*
#ifdef COMPILE_SCRIPTTOOLBAR
	QValueList<int> def;
	def.append(20);
	def.append(80);
	m_pEditor->setSizes(cfg->readIntListEntry("Sizes",def));
	KviStr tmp = cfg->readEntry("LastRaw","");
	m_pEditor->editRaw(tmp);
	//debug("LAST EDITED WAS %s",tmp.ptr());
#endif // COMPILE_SCRIPTTOOLBAR
*/
}
