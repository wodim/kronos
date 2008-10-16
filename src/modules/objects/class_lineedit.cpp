//=============================================================================
//
//   File : class_lineedit.cpp
//   Creation date : Wed 20 Sep 2000 04:34:40 CEST by Krzysztof Godlewski
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2000 Krzysztof Godlewski
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
//   Inc. ,59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
//=============================================================================

#include "kvi_error.h"
#include "kvi_debug.h"
#include "kvi_locale.h"
#include "class_lineedit.h"
#include <QRegExpValidator>
#include <QLineEdit>
static const char * mode_tbl[] = {
			"Normal",
			"NoEcho",
			"Password"
			  };

static const int mode_cod[] =  {
		QLineEdit::Normal,
		QLineEdit::NoEcho,
		QLineEdit::Password
			};

#define mode_num	(sizeof(mode_tbl) / sizeof(mode_tbl[0]))


/*
	@doc:	lineedit
	@keyterms:
		lineedit object class, line editor, input
	@title:
		lineedit class
	@type:
		class
	@short:
		Simple line editor for inputting short text
	@inherits:
		[class]object[/class]
		[class]widget[/class]
	@description:
		This is a simple, one-line editor usually used for inputting
		short text like names, phone numbers etc. It can display text
		in three echo modes (see [classfnc]$setEchoMode[/classfnc]()
		for details).
	@functions:
		!fn: <string> $text()
		Returns text contained by the widget.[br]
		See also [classfnc]$setText[/classfnc]().
		!fn: $setText(<text:string>)
		Sets editor text to <text>.[br]
		See also [classfnc]$text[/classfnc]().
		!fn: $setEchoMode(<echo_mode:string>)
		Sets the line edit's echo mode. Possible value are:[br]
		-Normal: display chars as they entered[br]
		-Noecho : do not display anything[br]
		-Password : display asterisks instead of the characters actually entered[br]
		See also [classfnc]$echoMode[/classfnc]().
		!fn: <string> $echoMode()
		Return the line edit's echo mode.
		See also [classfnc]$setEchoMode[/classfnc]().
		!fn: <integer> $maxLength()
		Returns the current maximum length of the text that can be typed
		 in the editor.[br]
		See also [classfnc]$setMaxLength[/classfnc]().
		!fn: $setMaxLength(<max_lenght:integer>)
		Sets maximum length of the text that can be typed in the
		editor.[br]
		See also [classfnc]$maxLength[/classfnc]().
		!fn: <boolean> $frame()
		Returns '1' if the editor is drawn inside a frame, which is 2
		pixels wide, '0' otherwise.[br]
		See also [classfnc]$setFrame[/classfnc]().
		!fn: $setFrame(<bframe:boolean>)
		Sets editor frame drawing to <bool>.[br]
		See also [classfnc]$frame[/classfnc]().
		!fn: <integer> $cursorPosition()
		Returns current cursor position in the editor.[br]
		See also [classfnc]$setCursorPosition[/classfnc]().
		!fn: $setCursorPosition(<index:integer>)
		Moves the cursor to place pointed by <index>.[br]
		See also [classfnc]$cursorPosition[/classfnc]().
		!fn: $selectAll()
		Selects whole text inside the editor.[br]
		See also [classfnc]$setSelection[/classfnc](),
		[classfnc]$cut[/classfnc](), [classfnc]$copy[/classfnc](),
		[classfnc]$paste[/classfnc]().
		!fn: $setSelection(<start:integer>, <length:integer>)
		Selects <length> characters in the editor starting at position
		<start>.[br]
		See also [classfnc]$selectAll[/classfnc](),
		[classfnc]$cut[/classfnc](), [classfnc]$copy[/classfnc](),
		[classfnc]$paste[/classfnc]().
		!fn: $copy()
		Copies selected text in the editor to the clipboard.[br]
		See also [classfnc]$setSelection[/classfnc](),
		[classfnc]$cut[/classfnc](), [classfnc]$paste[/classfnc]().
		!fn: $cut()
		Cuts selected text from the editor to the clipboard.[br]
		See also [classfnc]$setSelection[/classfnc](),
		[classfnc]$copy[/classfnc](), [classfnc]$paste[/classfnc]().
		!fn: $paste()
		Pastes text from the clipboard into the editor. The pasted text
		will start at cursor position.[br]
		See also [classfnc]$setSelection[/classfnc](),
		[classfnc]$copy[/classfnc](), [classfnc]$cut[/classfnc]().
		!fn: $clear()
		Deletes all text from the editor. Equal to calling [classfnc]$setText[/classfnc]("").
		!fn: $setDragEnabled(<bEnabled:bool>)
		With this property user can drag text in the lineedit.
		!fn: $setReadOnly(<bReadonly:boolean>)
		Sets the lineedit to read only mode.
		!fn: $setInputMask(<mask:string>)
		Sets the validation input mask to inputMask.[br]
		Example:[br]
		[br]
		%esempio->$setInputMask( "+99 99 99 99 99;_" );[br]
		%esempio->$setInputMask( "000.000.000.000;_" );[br]
		%esempio->Ip Number Mask.[br]
		%esempio->setInputMask( ">AAAAA-AAAAA-AAAAA-AAAAA-AAAAA;#" );[br]
		[br]
		The mask format understands these mask characters:[br]
		Character		Meaning[br]
		A			ASCII alphabetic character required. A-Z, a-z.[br]
		a			ASCII alphabetic character permitted but not required.[br]
		N			ASCII alphanumeric character required. A-Z, a-z, 0-9.[br]
		n			ASCII alphanumeric character permitted but not required.[br]
		X			Any character required.[br]
		x			Any character permitted but not required.[br]
		9			ASCII digit required. 0-9.[br]
		0			ASCII digit permitted but not required.[br]
		D			ASCII digit required. 1-9.[br]
		d			ASCII digit permitted but not required.[br]
		#			ASCII digit or plus/minus sign permitted but not required.[br]
		>			All following alphabetic characters are uppercased.[br]
		<			All following alphabetic characters are lowercased.[br]
		!			Switch off case conversion.[br]
		\			Use \ to escape the special characters listed above to use them as separators.[br]
		[br]
		The mask consists of a string of mask characters and separators, optionally[br]
		followed by a semi-colon and the character used for blanks: the blank characters [br]
		are always removed from the text after editing. The default blank character is space. [br]
		!fn: $returnPressedEvent()
		This function is called by the framework when the enter key is pressed.
		!fn: $lostFocusEvent()
		Called when the lineedit lost focus.
		!fn: $textChangedEvent(<new text:string>)
		This event is called when the text changed, In $0 there is the new text.
	@signals:
		!sg: $returnPressed()
		This signal is emitted by the default implementation of [classfnc]$returnPressedEvent[/classfnc]().
		!sg: $lostFocus()
		This signal is emitted by the default implementation of [classfnc]$lostFocusEvent[/classfnc]().
		!sg: $textChanged()
		This signal is emitted by the default implementation of [classfnc]$textChangedEvent[/classfnc]().

*/



KVSO_BEGIN_REGISTERCLASS(KviKvsObject_lineedit,"lineedit","widget")

	KVSO_REGISTER_HANDLER_NEW(KviKvsObject_lineedit,text)
	KVSO_REGISTER_HANDLER_NEW(KviKvsObject_lineedit,setText)
	KVSO_REGISTER_HANDLER_NEW(KviKvsObject_lineedit,maxLength)
	KVSO_REGISTER_HANDLER_NEW(KviKvsObject_lineedit,setMaxLength)
	KVSO_REGISTER_HANDLER_NEW(KviKvsObject_lineedit,frame)
	KVSO_REGISTER_HANDLER_NEW(KviKvsObject_lineedit,setCursorPosition)
	KVSO_REGISTER_HANDLER_NEW(KviKvsObject_lineedit,cursorPosition)
	KVSO_REGISTER_HANDLER_NEW(KviKvsObject_lineedit,setFrame)
	KVSO_REGISTER_HANDLER_NEW(KviKvsObject_lineedit,selectAll)
	KVSO_REGISTER_HANDLER_NEW(KviKvsObject_lineedit,setSelection)
	KVSO_REGISTER_HANDLER_NEW(KviKvsObject_lineedit,copy)
	KVSO_REGISTER_HANDLER_NEW(KviKvsObject_lineedit,cut)
	KVSO_REGISTER_HANDLER_NEW(KviKvsObject_lineedit,paste)
	KVSO_REGISTER_HANDLER_NEW(KviKvsObject_lineedit,echoMode)
	KVSO_REGISTER_HANDLER_NEW(KviKvsObject_lineedit,setEchoMode)
	KVSO_REGISTER_HANDLER_NEW(KviKvsObject_lineedit,clear)
	KVSO_REGISTER_HANDLER_NEW(KviKvsObject_lineedit,dragAndDrop)
	KVSO_REGISTER_HANDLER_NEW(KviKvsObject_lineedit,setInputMask)
	KVSO_REGISTER_HANDLER_NEW(KviKvsObject_lineedit,setReadOnly)
	KVSO_REGISTER_HANDLER_NEW(KviKvsObject_lineedit,returnPressedEvent)
	KVSO_REGISTER_HANDLER_NEW(KviKvsObject_lineedit,lostFocusEvent)
	KVSO_REGISTER_HANDLER_NEW(KviKvsObject_lineedit,textChangedEvent)
	KVSO_REGISTER_HANDLER_NEW(KviKvsObject_lineedit,setInputValidator)


KVSO_END_REGISTERCLASS(KviKvsObject_lineedit)


KVSO_BEGIN_CONSTRUCTOR(KviKvsObject_lineedit,KviKvsObject_widget)

KVSO_END_CONSTRUCTOR(KviKvsObject_lineedit)


KVSO_BEGIN_DESTRUCTOR(KviKvsObject_lineedit)

KVSO_END_CONSTRUCTOR(KviKvsObject_lineedit)


bool KviKvsObject_lineedit::init(KviKvsRunTimeContext * pContext,KviKvsVariantList * pParams)
{
	SET_OBJECT(QLineEdit)
	connect(widget(),SIGNAL(returnPressed()),this,SLOT(slotreturnPressed()));
	connect(widget(),SIGNAL(lostFocus()),this,SLOT(slotlostFocus()));
	connect(widget(),SIGNAL(textChanged(const QString & )),this,SLOT(slottextChanged(const QString & )));
	return true;
}

KVSO_CLASS_FUNCTION(lineedit,text)
{
	CHECK_INTERNAL_POINTER(widget())	
	c->returnValue()->setString(((QLineEdit *)widget())->text());
	return true;
}

KVSO_CLASS_FUNCTION(lineedit,setText)
{
	CHECK_INTERNAL_POINTER(widget())	
	QString szText;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("text",KVS_PT_STRING,0,szText)
	KVSO_PARAMETERS_END(c)
	((QLineEdit *)widget())->setText(szText);
	return true;
}
KVSO_CLASS_FUNCTION(lineedit,setInputValidator)
{
	CHECK_INTERNAL_POINTER(widget())	
	QString szReg;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("reg_expression",KVS_PT_STRING,0,szReg)
	KVSO_PARAMETERS_END(c)
	((QLineEdit *)widget())->setValidator(new QRegExpValidator(QRegExp(szReg),((QLineEdit *)widget())));
	return true;
}
KVSO_CLASS_FUNCTION(lineedit,maxLength)
{
	CHECK_INTERNAL_POINTER(widget())	
	c->returnValue()->setInteger(((QLineEdit *)widget())->maxLength());
	return true;
}

KVSO_CLASS_FUNCTION(lineedit,setMaxLength)
{
	CHECK_INTERNAL_POINTER(widget())	
	kvs_uint_t iMaxlen;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("maxlen",KVS_PT_UNSIGNEDINTEGER,0,iMaxlen)
	KVSO_PARAMETERS_END(c)
	((QLineEdit *)widget())->setMaxLength(iMaxlen);
	return true;
}

KVSO_CLASS_FUNCTION(lineedit,frame)
{
	CHECK_INTERNAL_POINTER(widget())	
	c->returnValue()->setBoolean(((QLineEdit *)widget())->hasFrame());
	return true;
}

KVSO_CLASS_FUNCTION(lineedit,setFrame)
{
	CHECK_INTERNAL_POINTER(widget())	
	bool bFrame;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("bframe",KVS_PT_BOOL,0,bFrame)
	KVSO_PARAMETERS_END(c)
	((QLineEdit *)widget())->setFrame(bFrame);
	return true;
}

KVSO_CLASS_FUNCTION(lineedit,cursorPosition)
{
	CHECK_INTERNAL_POINTER(widget())	
	c->returnValue()->setInteger(((QLineEdit *)widget())->cursorPosition());
	return true;
}

KVSO_CLASS_FUNCTION(lineedit,setCursorPosition)
{
	CHECK_INTERNAL_POINTER(widget())	
	kvs_uint_t iPos;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("position",KVS_PT_UNSIGNEDINTEGER,0,iPos)
	KVSO_PARAMETERS_END(c)
	((QLineEdit *)widget())->setCursorPosition(iPos);
	return true;
}

KVSO_CLASS_FUNCTION(lineedit,selectAll)
{
	CHECK_INTERNAL_POINTER(widget())	
	((QLineEdit *)widget())->selectAll();
	return true;
}

KVSO_CLASS_FUNCTION(lineedit,setSelection)
{
	CHECK_INTERNAL_POINTER(widget())	
	kvs_uint_t uStart,uLen;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("start",KVS_PT_UNSIGNEDINTEGER,0,uStart)
		KVSO_PARAMETER("len",KVS_PT_UNSIGNEDINTEGER,0,uLen)
	KVSO_PARAMETERS_END(c)
	((QLineEdit *)widget())->setSelection(uStart, uLen);
	return true;
}

KVSO_CLASS_FUNCTION(lineedit,copy)
{
	CHECK_INTERNAL_POINTER(widget())	
	((QLineEdit *)widget())->copy();
	return true;
}

KVSO_CLASS_FUNCTION(lineedit,cut)
{
	CHECK_INTERNAL_POINTER(widget())	
	((QLineEdit *)widget())->cut();
	return true;
}

KVSO_CLASS_FUNCTION(lineedit,paste)
{
	CHECK_INTERNAL_POINTER(widget())	
	((QLineEdit *)widget())->paste();
	return true;
}

KVSO_CLASS_FUNCTION(lineedit,echoMode)
{
	CHECK_INTERNAL_POINTER(widget())	
	int mode = ((QLineEdit *)widget())->echoMode();
	QString szEchomode="";
	for(unsigned int i = 0; i < mode_num; i++)
	{
		if(mode == mode_cod[i])
		{
			szEchomode=mode_tbl[i];
			break;
		}
	}
	c->returnValue()->setString(szEchomode);
	return true;
}

KVSO_CLASS_FUNCTION(lineedit,setEchoMode)
{
	CHECK_INTERNAL_POINTER(widget())	
	QString szMode;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("mode",KVS_PT_STRING,0,szMode)
	KVSO_PARAMETERS_END(c)
	for(unsigned int i = 0; i < mode_num; i++)
	{
		if(KviQString::equalCI(szMode, mode_tbl[i]))
		{
			((QLineEdit *)widget())->setEchoMode( \
				((QLineEdit::EchoMode)mode_cod[i]));
			return true;
		}
	}
	c->warning(__tr2qs_ctx("Unknown echo mode '%Q'","objects"),&szMode);
	return true;
}

KVSO_CLASS_FUNCTION(lineedit,clear)
{
	CHECK_INTERNAL_POINTER(widget())	
	((QLineEdit *)widget())->clear();
	return true;
}

//-| Grifisx & Noldor |-Start:
KVSO_CLASS_FUNCTION(lineedit,dragAndDrop)
{
	CHECK_INTERNAL_POINTER(widget())	
	bool bEnabled;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("bEnabled",KVS_PT_BOOL,0,bEnabled)
	KVSO_PARAMETERS_END(c)
	((QLineEdit *)widget())->setDragEnabled(bEnabled);
	return true;
}

KVSO_CLASS_FUNCTION(lineedit,setReadOnly)
{
	CHECK_INTERNAL_POINTER(widget())	
	bool bEnabled;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("bReadonly",KVS_PT_BOOL,0,bEnabled)
	KVSO_PARAMETERS_END(c)
	((QLineEdit *)widget())->setReadOnly(bEnabled);
	return true;
}

KVSO_CLASS_FUNCTION(lineedit,setInputMask)
{
	CHECK_INTERNAL_POINTER(widget())	
	QString szMask;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("mask",KVS_PT_STRING,0,szMask)
	KVSO_PARAMETERS_END(c)
	((QLineEdit *)widget())->setInputMask(szMask);
	return true;
}

KVSO_CLASS_FUNCTION(lineedit,returnPressedEvent)
{
	emitSignal("returnPressed",c);
	return true;
}

void KviKvsObject_lineedit::slotreturnPressed()
{
	KviKvsVariantList * params = 0;
	callFunction(this,"returnPressedEvent",params);

}
// FIND ME
KVSO_CLASS_FUNCTION(lineedit,lostFocusEvent)
{
	emitSignal("lostFocus",c);
	return true;
}

void KviKvsObject_lineedit::slotlostFocus()
{
	KviKvsVariantList * params = 0;
	callFunction(this,"lostFocusEvent",params);
}
/////
KVSO_CLASS_FUNCTION(lineedit,textChangedEvent)
{
	emitSignal("textChanged",c,c->params());
	return true;

}

void KviKvsObject_lineedit::slottextChanged(const QString &text)
{
	KviKvsVariantList params(new KviKvsVariant(text));
	callFunction(this,"textChangedEvent",&params);
}


#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "m_class_lineedit.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES

