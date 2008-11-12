//=============================================================================
//
//   File : class_wizard.cpp
//   Creation date : Fri Mar 18 14:30:48 CEST 2005
//   by Tonino Imbesi(Grifisx) and Alessandro Carbone(Noldor)
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2005-2008 Alessandro Carbone (elfonol at gmail dot com)
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
//=============================================================================

#include <kvi_tal_wizard.h>

#include "class_wizard.h"
#include "kvi_error.h"
#include "kvi_debug.h"

#include "kvi_locale.h"

#include <QPushButton>

/*
	@doc:	wizard
	@keyterms:
		wizard object class,
	@title:
		wizard class
	@type:
		class
	@short:
		The wizard class provides a framework for wizard dialogs.
	@inherits:
		[class]object[/class]
		[class]widget[/class]
	@description:
		A wizard is an input dialog that consists of a sequence of dialog pages.It walks the user through a process step by step.[br]
	@functions:
	!fn: $addPage(<page_widget:object>,<title:string>)
	Add a page with title.<page_widget> must be a widget's object.
	!fn: $insertPage(<page_widget>,<title:string>,<index:integer>)
	Inserts a page at the position <index>. If index is -1, page will appended to the end of the wizard's page sequenze.
	!fn: $setTitle(<page>,<title:string>)
	Sets the title for page page to title text.
	!fn: $setBackEnabled(<page_widget>,<bEnabled:boolean>)
	If enable is set to 1, page page has a Back button; otherwise page has no Back button.[br]
	By default all pages have this button.
	!fn: $setNextEnabled(<page_widget>,<bEnabled:boolean>)
	If enable is set to 1, page page has a Next button; otherwise the Next button on page is disabled.[br]
	By default all pages have this button.
	!fn: $setFinishEnabled(<page_widget>,<bEnabled:boolean>)
	If enable is set to 1, page page has a Finish button; otherwise the Finish button on page is disabled.[br]
	By default NO pages have this button.
	!fn: $setHelpEnabled(<page_widget>,<bEnabled:boolean>)
	If enable is set to 1, page page has a Help button; otherwise the Help button on page is disabled.[br]
	By default all pages have this button.
	!fn: $setNextBtnText(<text:string>)
	Set the text for button Next.
	!fn: $setBackBtnText(<text:string>)
	Set the text for button Back.
	!fn: $setFinishBtnText(<text:string>)
	Set the text for button Finish.
	!fn: $setHelpBtnText(<text:string>)
	Set the text for button Help.
	!fn: $acceptEvent()
	This function is called when the user click on the Finish button.
	!fn: $rejectEvent()
	This function is called when thie user click on the Reject button.
	!fn: $nextClickedEvent()
	This function is called when the user click on the Next button.
	!fn: $backClickedEvent()
	This function is called when thie user click on the Back button.
	@signals:
	!sg: $nextClicked()
	This signal is emitted by the default implementation of [classfnc]$nextClickedEvent[/classfnc]().
	!sg: $backClicked()
	This signal is emitted by the default implementation of [classfnc]$backClickedEvent[/classfnc]().

*/

KVSO_BEGIN_REGISTERCLASS(KviKvsObject_wizard,"wizard","widget")

	KVSO_REGISTER_HANDLER_NEW(KviKvsObject_wizard,addPage)
	KVSO_REGISTER_HANDLER_NEW(KviKvsObject_wizard,insertPage)
	KVSO_REGISTER_HANDLER_NEW(KviKvsObject_wizard,setTitle)

	KVSO_REGISTER_HANDLER_NEW(KviKvsObject_wizard,setBackEnabled);
	KVSO_REGISTER_HANDLER_NEW(KviKvsObject_wizard,setNextEnabled);
	KVSO_REGISTER_HANDLER_NEW(KviKvsObject_wizard,setFinishEnabled);
	KVSO_REGISTER_HANDLER_NEW(KviKvsObject_wizard,setHelpEnabled);

	KVSO_REGISTER_HANDLER_NEW(KviKvsObject_wizard,setBackBtnText);
	KVSO_REGISTER_HANDLER_NEW(KviKvsObject_wizard,setNextBtnText);
	KVSO_REGISTER_HANDLER_NEW(KviKvsObject_wizard,setFinishBtnText);
	KVSO_REGISTER_HANDLER_NEW(KviKvsObject_wizard,setHelpBtnText);
	KVSO_REGISTER_HANDLER_NEW(KviKvsObject_wizard,nextClickedEvent);
	KVSO_REGISTER_HANDLER_NEW(KviKvsObject_wizard,backClickedEvent);


	KVSO_REGISTER_STANDARD_NOTHINGRETURN_HANDLER(KviKvsObject_wizard,"acceptEvent")
	KVSO_REGISTER_STANDARD_NOTHINGRETURN_HANDLER(KviKvsObject_wizard,"rejectEvent")

KVSO_END_REGISTERCLASS(KviKvsObject_wizard)

KVSO_BEGIN_CONSTRUCTOR(KviKvsObject_wizard,KviKvsObject_widget)

KVSO_END_CONSTRUCTOR(KviKvsObject_wizard)


KVSO_BEGIN_DESTRUCTOR(KviKvsObject_wizard)

KVSO_END_CONSTRUCTOR(KviKvsObject_wizard)

bool KviKvsObject_wizard::init(KviKvsRunTimeContext *,KviKvsVariantList *)
{
	setObject(new KviKvsMdmWizard(parentScriptWidget(), getName().toUtf8().data(),this), true);

	return true;
}
KVSO_CLASS_FUNCTION(wizard,addPage)
{
	CHECK_INTERNAL_POINTER(widget())
	KviKvsObject *ob;
	QString szLabel;
	kvs_hobject_t hObject;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("page_widget",KVS_PT_HOBJECT,0,hObject)
		KVSO_PARAMETER("label",KVS_PT_STRING,0,szLabel)
	KVSO_PARAMETERS_END(c)
	ob=KviKvsKernel::instance()->objectController()->lookupObject(hObject);
	if(!ob->object()->isWidgetType())
	{
		c->warning(__tr2qs_ctx("Can't add a non-widget object","objects"));
		return true;
	}
	((KviTalWizard *)widget())->addPage(((QWidget *)(ob->object())),szLabel);
	return true;
}
KVSO_CLASS_FUNCTION(wizard,insertPage)
{
	CHECK_INTERNAL_POINTER(widget())

	KviKvsObject *ob;
	QString szLabel;
	kvs_int_t uIndex;
	kvs_hobject_t hObject;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("page_widget",KVS_PT_HOBJECT,0,hObject)
		KVSO_PARAMETER("label",KVS_PT_STRING,0,szLabel)
		KVSO_PARAMETER("index",KVS_PT_UNSIGNEDINTEGER,0,uIndex)
	KVSO_PARAMETERS_END(c)
	ob=KviKvsKernel::instance()->objectController()->lookupObject(hObject);
	if(!ob->object()->isWidgetType())
	{
		c->warning(__tr2qs_ctx("Can't add a non-widget object","objects"));
		return true;
	}
	((KviTalWizard *)widget())->insertPage(((QWidget *)(ob->object())),szLabel,uIndex);
	return true;
}


KVSO_CLASS_FUNCTION(wizard,setTitle)
{
	CHECK_INTERNAL_POINTER(widget())
	KviKvsObject *ob;
	QString szTitle;
	kvs_hobject_t hObject;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("page_widget",KVS_PT_HOBJECT,0,hObject)
		KVSO_PARAMETER("title",KVS_PT_STRING,0,szTitle)
	KVSO_PARAMETERS_END(c)
	ob=KviKvsKernel::instance()->objectController()->lookupObject(hObject);
	if(!ob->object()->isWidgetType())
	{
		c->warning(__tr2qs_ctx("Widget object required","objects"));
		return true;
	}
	((KviTalWizard *)widget())->setPageTitle(((QWidget *)(ob->object())),szTitle);
	return true;
}
KVSO_CLASS_FUNCTION(wizard,setBackEnabled)
{
	CHECK_INTERNAL_POINTER(widget())
	bool bEnabled;
	KviKvsObject *ob;
	kvs_hobject_t hObject;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("page_widget",KVS_PT_HOBJECT,0,hObject)
		KVSO_PARAMETER("bEnabled",KVS_PT_BOOL,0,bEnabled)
	KVSO_PARAMETERS_END(c)
	ob=KviKvsKernel::instance()->objectController()->lookupObject(hObject);
	if(!ob->object()->isWidgetType())
	{
		c->warning(__tr2qs_ctx("Widget object required","objects"));
		return true;
	}
	((KviTalWizard *)widget())->setBackEnabled(((QWidget *)(ob->object())),bEnabled);
	return true;
}

KVSO_CLASS_FUNCTION(wizard,setNextEnabled)
{
	CHECK_INTERNAL_POINTER(widget())

	bool bEnabled;
	KviKvsObject *ob;
	kvs_hobject_t hObject;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("page_widget",KVS_PT_HOBJECT,0,hObject)
		KVSO_PARAMETER("bEnabled",KVS_PT_BOOL,0,bEnabled)
	KVSO_PARAMETERS_END(c)
	ob=KviKvsKernel::instance()->objectController()->lookupObject(hObject);
	if(!ob->object()->isWidgetType())
	{
		c->warning(__tr2qs_ctx("Widget object required","objects"));
		return true;
	}
	((KviTalWizard *)widget())->setNextEnabled(((QWidget *)(ob->object())),bEnabled);
	return true;
}
KVSO_CLASS_FUNCTION(wizard,setFinishEnabled)
{
	CHECK_INTERNAL_POINTER(widget())

	bool bEnabled;
	KviKvsObject *ob;
	kvs_hobject_t hObject;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("page_widget",KVS_PT_HOBJECT,0,hObject)
		KVSO_PARAMETER("bEnabled",KVS_PT_BOOL,0,bEnabled)
	KVSO_PARAMETERS_END(c)
	ob=KviKvsKernel::instance()->objectController()->lookupObject(hObject);
	if(!ob->object()->isWidgetType())
	{
		c->warning(__tr2qs_ctx("Widget object required","objects"));
		return true;
	}
	((KviTalWizard *)widget())->setFinishEnabled(((QWidget *)(ob->object())),bEnabled);
	return true;
}
KVSO_CLASS_FUNCTION(wizard,setHelpEnabled)
{
	CHECK_INTERNAL_POINTER(widget())

	bool bEnabled;
	KviKvsObject *ob;
	kvs_hobject_t hObject;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("page_widget",KVS_PT_HOBJECT,0,hObject)
		KVSO_PARAMETER("bEnabled",KVS_PT_BOOL,0,bEnabled)
	KVSO_PARAMETERS_END(c)
	ob=KviKvsKernel::instance()->objectController()->lookupObject(hObject);
	if(!ob->object()->isWidgetType())
	{
		c->warning(__tr2qs_ctx("Widget object required","objects"));
		return true;
	}
	((KviTalWizard *)widget())->setHelpEnabled(((QWidget *)(ob->object())),bEnabled);
	return true;
}
KVSO_CLASS_FUNCTION(wizard,setBackBtnText)
{
	CHECK_INTERNAL_POINTER(widget())

	QString szText;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("text",KVS_PT_STRING,0,szText)
	KVSO_PARAMETERS_END(c)
	((KviTalWizard *)widget())->KviTalWizard::backButton()->setText(szText);
	return true;
}
KVSO_CLASS_FUNCTION(wizard,setNextBtnText)
{
	CHECK_INTERNAL_POINTER(widget())

	QString szText;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("text",KVS_PT_STRING,0,szText)
	KVSO_PARAMETERS_END(c)
	((KviTalWizard *)widget())->KviTalWizard::nextButton()->setText(szText);
	return true;
}
KVSO_CLASS_FUNCTION(wizard,setHelpBtnText)
{
	CHECK_INTERNAL_POINTER(widget())

	QString szText;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("text",KVS_PT_STRING,0,szText)
	KVSO_PARAMETERS_END(c)
	((KviTalWizard *)widget())->KviTalWizard::helpButton()->setText(szText);
	return true;
}
KVSO_CLASS_FUNCTION(wizard,setFinishBtnText)
{
	CHECK_INTERNAL_POINTER(widget())

	QString szText;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("text",KVS_PT_STRING,0,szText)
	KVSO_PARAMETERS_END(c)
	((KviTalWizard *)widget())->KviTalWizard::finishButton()->setText(szText);
	return true;
}
bool KviKvsObject_wizard::accept()
{
	KviKvsVariantList * pParams = 0;
	callFunction(this,"acceptEvent",0,pParams);
	return true;
}
bool KviKvsObject_wizard::reject()
{
	KviKvsVariantList * pParams = 0;
	callFunction(this,"rejectEvent",0,pParams);
	return true;
}
KVSO_CLASS_FUNCTION(wizard,nextClickedEvent)
{
	emitSignal("nextClicked",c);
	return true;
}

void KviKvsObject_wizard::nextClicked()
{
	KviKvsVariantList *params=0;
	callFunction(this,"nextClickedEvent",params);
}
KVSO_CLASS_FUNCTION(wizard,backClickedEvent)
{
	emitSignal("backClicked",c);
	return true;
}

void KviKvsObject_wizard::backClicked()
{
	KviKvsVariantList *params=0;
	callFunction(this,"backClickedEvent",params);
}
KviKvsMdmWizard::KviKvsMdmWizard(QWidget * par,const char * name,KviKvsObject_wizard * parent)
:KviTalWizard(par)
{
	m_pParentScript=parent;
	setObjectName(name);
	connect (this->backButton(),SIGNAL(clicked()),this,SLOT(slotBackClicked()));
	connect (this->nextButton(),SIGNAL(clicked()),this,SLOT(slotNextClicked()));
}
void KviKvsMdmWizard::accept()
{
	if(m_pParentScript->accept())KviTalWizard::accept();

}
void KviKvsMdmWizard::reject()
{
	if(m_pParentScript->reject())KviTalWizard::reject();

}
void KviKvsMdmWizard::slotNextClicked()
{
	m_pParentScript->nextClicked();
}
void KviKvsMdmWizard::slotBackClicked()
{
	m_pParentScript->backClicked();
}
KviKvsMdmWizard::~KviKvsMdmWizard()
{
}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "m_class_wizard.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES

