//=============================================================================
//		
//   File : scripteditor.cpp
//   Created on Sun Mar 28 1999 16:11:48 CEST by Szymon Stefanek
//	 Code improvements by Carbone Alessandro & Tonino Imbesi
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 1999-2004 Szymon Stefanek <pragma at kvirc dot net>
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

#include "scripteditor.h"

#include "kvi_fileutils.h"
#include "kvi_locale.h"
#include "kvi_filedialog.h"
#include "kvi_qstring.h"
#include "kvi_config.h"
#include "kvi_module.h"
#include "kvi_pointerlist.h"
#include "kvi_app.h"
#include "kvi_console.h"
#include "kvi_window.h"
#include "kvi_iconmanager.h"
#include "kvi_kvs_kernel.h"
#include <kvi_tal_groupbox.h>
#include "kvi_tal_scrollview.h"

//FIXME
#include <Q3TextEdit>
#include <Q3PopupMenu>

#include <QLayout>
#include <QToolButton>
#include <QMessageBox>
#include <QTimer>
#include <QCursor>
#include <QFont>
#include <QRect>
#include <QEvent>
#include <QKeyEvent>
#include <QMenu>
#include <QListWidget>

extern KviPointerList<KviScriptEditorImplementation> * g_pScriptEditorWindowList;
extern KviModule * g_pEditorModulePointer;

static QColor g_clrBackground(0,0,0);
static QColor g_clrNormalText(255,255,255);
static QColor g_clrBracket(255,0,0);
static QColor g_clrComment(0,120,0);
static QColor g_clrFunction(0,17,255);
static QColor g_clrKeyword(85,85,255);
static QColor g_clrVariable(255,0,0);
static QColor g_clrPunctuation(180,180,0);
static QColor g_clrFind(0,0,0);
static QFont g_fntNormal("Courier New",8);

KviCompletionBox::KviCompletionBox(QWidget * parent = 0)
: QListWidget(parent)
{
	QPalette p = palette();
	p.setColor(foregroundRole(),QColor(0,0,0));
	p.setColor(backgroundRole(),QColor(255,255,255));
//	setPaletteBackgroundColor(QColor(255,255,255));
	setPalette(p);
//	setHScrollBarMode(KviTalListBox::AlwaysOff);

	QFont listfont=font();
	listfont.setPointSize(8);
	setFont(listfont);
//	setVariableWidth(false);
	setFixedWidth(200);
	//completelistbox->setColumnMode(KviTalListBox::Variable);
	hide();
}

void KviCompletionBox::updateContents(QString buffer)
{
	buffer=buffer.stripWhiteSpace();
	KviPointerList<QString> list;
	clear();
	
	QString szModule;
	QChar* pCur = (QChar *)buffer.ucs2();
	
	int pos=buffer.find('.');
	
	if(pos>0)
	{
		szModule=buffer.left(pos);
		if(szModule[0].unicode()=='$')
			szModule.remove(0,1);
	}
	
	if(pCur->unicode() == '$')
	{
		buffer.remove(0,1);
		if(!buffer.isEmpty())
		{
			if(szModule.isEmpty())
				KviKvsKernel::instance()->completeFunction(buffer,&list);
			else
				debug("we need a module completion!");
			for ( QString* szCurrent = list.first(); szCurrent; szCurrent = list.next() )
			{
				szCurrent->prepend('$');
				//szCurrent->append('(');
				addItem(*szCurrent);
			}
		}
	}
	else
	{
		if(szModule.isEmpty())
			KviKvsKernel::instance()->completeCommand(buffer,&list);
		else
			debug("we need a module completion!");
		for ( QString* szCurrent = list.first(); szCurrent; szCurrent = list.next() )
		{
			szCurrent->append(' ');
			addItem(*szCurrent);
		}
	}
//	debug("%s %s %i %i",__FILE__,__FUNCTION__,__LINE__,count());
}

void KviCompletionBox::keyPressEvent(QKeyEvent * e)
{
//	debug("%s %s %i %x",__FILE__,__FUNCTION__,__LINE__,e->key());
	switch(e->key())
	{
		case Qt::Key_Escape:
			hide();
			setFocus();
			break;
		case Qt::Key_Return:
			break;
		default:
			if(!e->text().isEmpty())
			{
				e->ignore();
			}
		
	}
	QListWidget::keyPressEvent(e);
}

KviScriptEditorWidgetColorOptions::KviScriptEditorWidgetColorOptions(QWidget * pParent)
: QDialog(pParent)
{
	m_pSelectorInterfaceList = new KviPointerList<KviSelectorInterface>;
        m_pSelectorInterfaceList->setAutoDelete(false);
	setWindowTitle(__tr2qs_ctx("Preferences","editor"));
	QGridLayout * g = new QGridLayout(this,3,3,4,4);

	KviFontSelector * f = new KviFontSelector(this,__tr2qs_ctx("Font:","editor"),&g_fntNormal,true);
	g->addMultiCellWidget(f,0,0,0,2);
	m_pSelectorInterfaceList->append(f);
	KviTalGroupBox * gbox = new KviTalGroupBox(1,Qt::Horizontal,__tr2qs("Colors" ),this);
	g->addMultiCellWidget(gbox,1,1,0,2);
	KviColorSelector * s = addColorSelector(gbox,__tr2qs_ctx("Background:","editor"),&g_clrBackground,true);
	s = addColorSelector(gbox,__tr2qs_ctx("Normal text:","editor"),&g_clrNormalText,true);
	s = addColorSelector(gbox,__tr2qs_ctx("Brackets:","editor"),&g_clrBracket,true);
	s = addColorSelector(gbox,__tr2qs_ctx("Comments:","editor"),&g_clrComment,true);
	s = addColorSelector(gbox,__tr2qs_ctx("Functions:","editor"),&g_clrFunction,true);
	s = addColorSelector(gbox,__tr2qs_ctx("Keywords:","editor"),&g_clrKeyword,true);
	s = addColorSelector(gbox,__tr2qs_ctx("Variables:","editor"),&g_clrVariable,true);
	s = addColorSelector(gbox,__tr2qs_ctx("Punctuation:","editor"),&g_clrPunctuation,true);
	s = addColorSelector(gbox,__tr2qs_ctx("Find:","editor"),&g_clrFind,true);
	
	QPushButton * b = new QPushButton(__tr2qs_ctx("&OK","editor"),this);
	b->setDefault(true);
	connect(b,SIGNAL(clicked()),this,SLOT(okClicked()));
	g->addWidget(b,2,1);

	b = new QPushButton(__tr2qs_ctx("Cancel","editor"),this);
	connect(b,SIGNAL(clicked()),this,SLOT(reject()));
	g->addWidget(b,2,2);


	g->setRowStretch(0,1);
	g->setColStretch(0,1);
}

KviScriptEditorWidgetColorOptions::~KviScriptEditorWidgetColorOptions()
{
	delete m_pSelectorInterfaceList;
}

KviColorSelector * KviScriptEditorWidgetColorOptions::addColorSelector(QWidget * pParent,const QString & txt,QColor * pOption,bool bEnabled)
{
	KviColorSelector * s = new KviColorSelector(pParent,txt,pOption,bEnabled);
	m_pSelectorInterfaceList->append(s);
		return s;
}

void KviScriptEditorWidgetColorOptions::okClicked()
{
	for(KviSelectorInterface * i = m_pSelectorInterfaceList->first();i;i = m_pSelectorInterfaceList->next())
	{
		i->commit();
	}

	accept();
}


KviScriptEditorWidget::KviScriptEditorWidget(QWidget * pParent)
: QTextEdit(pParent)
{
	
	/*QPalette p=palette();
	p.setColor(QPalette::Base, QColor(0,0,0));
	setPalette(p);
	*/
	setWordWrapMode(QTextOption::NoWrap);
	m_pParent=pParent;
	m_szHelp="Nothing";
	updateOptions();
	m_szFind="";
	completelistbox=new KviCompletionBox(this);
	connect (completelistbox,SIGNAL(itemActivated(QListWidgetItem *)),this,SLOT(slotComplete(QListWidgetItem *)));
}

KviScriptEditorWidget::~KviScriptEditorWidget()
{

}

void KviScriptEditorWidget::contextMenuEvent(QContextMenuEvent *event)
{
	QMenu *pop=createStandardContextMenu();
	pop->insertItem(__tr2qs("Context sensitive help"),this,SLOT(slotHelp()),Qt::CTRL+Qt::Key_H);
	pop->insertItem(__tr2qs("&Replace"),this,SLOT(slotReplace()),Qt::CTRL+Qt::Key_R);
	pop->exec(event->globalPos());
     delete pop;
}

void KviScriptEditorWidget::slotFind()
{
	m_szFind=((KviScriptEditorImplementation*)m_pParent)->getFindlineedit()->text();
	setText(text());
}

void KviScriptEditorWidget::slotReplace()
{
	KviScriptEditorReplaceDialog *dialog=new KviScriptEditorReplaceDialog(this,tr("Find & Repalce"));
	connect (dialog,SIGNAL(replaceAll(const QString &,const QString &)),m_pParent,SLOT(slotReplaceAll(const QString &,const QString &)));
	connect (dialog,SIGNAL(initFind()),m_pParent,SLOT(slotInitFind()));
	connect (dialog,SIGNAL(nextFind(const QString &)),m_pParent,SLOT(slotNextFind(const QString &)));
	if(dialog->exec()){};

}

void KviScriptEditorWidget::slotHelp()
{
	contextSensitiveHelp();
}

void KviScriptEditorWidget::updateOptions()
{
	QPalette p=palette();
	p.setColor(QPalette::Base, g_clrBackground);
	p.setColor(QPalette::Text,g_clrNormalText);
	setPalette(p);
	setFont(g_fntNormal);
	setColor(g_clrNormalText);
	setTextFormat(Qt::PlainText);
	
	// this will rehighlight everything
	setText(text()); // an "hack" to ensure Update all in the editor
	KviScriptSyntaxHighlighter *h = new KviScriptSyntaxHighlighter(this);
	(void)h;
	p = ((KviScriptEditorImplementation*)m_pParent)->getFindlineedit()->palette(); 
	p.setColor(foregroundRole(),g_clrFind); 
	((KviScriptEditorImplementation*)m_pParent)->getFindlineedit()->setPalette(p); 
}

void KviScriptEditorWidget::keyPressEvent(QKeyEvent * e)
{

	if(e->state() == Qt::ControlButton)
	{
		switch(e->key())
		{
			case Qt::Key_B:
				insert("$b");
				return;
			case Qt::Key_K:
				insert("$k");
				return;
			case Qt::Key_O:
				insert("$o");
				return;
			case Qt::Key_U:
				insert("$u");
				return;
			case Qt::Key_Enter:
			case Qt::Key_Return:
			case Qt::Key_Backspace:
			case Qt::Key_PageUp:
				 e->ignore(); // allow the parent to process it
				 return;
			break;
		}
	}

	if(e->state() == Qt::ShiftButton)
	{
		if (e->key() == Qt::Key_Insert) 
		{
			completition();
			return;
		}
	}
	switch(e->key())
	{
		case Qt::Key_Period:
		case Qt::Key_Left:
		case Qt::Key_Right:
			if(!completelistbox->isVisible()) completition(0);
			break;
		case Qt::Key_Up:
		case Qt::Key_Escape:
		case Qt::Key_PageUp:
		case Qt::Key_PageDown:
		case Qt::Key_End:
		case Qt::Key_Home:
			if(completelistbox->isVisible()) completelistbox->hide();
			break;
		case Qt::Key_Down:
			if(completelistbox->isVisible())
			{
				completelistbox->setFocus();
				completelistbox->setCurrentItem(0);
				return;
			}
			break;
		case Qt::Key_Return:
			QTextEdit::keyPressEvent(e);
		/*	int para,pos;
			getCursorPosition(&para,&pos);
			if(para > 0)
			{
				QString szPrev=text(para-1);
				if(!szPrev.isEmpty())
				{
					if(szPrev.at(szPrev.length() - 1).unicode() == ' ')
						szPrev.remove(szPrev.length() - 1,1);
					QString szCur;
					const QChar * pCur = (const QChar *)szPrev.ucs2();
					if(pCur)
					{
						while(pCur->unicode() && pCur->isSpace()) 
						{
							szCur.append(*pCur);
							pCur++;
						}
					}
					insertAt(szCur,para,0);
					setCursorPosition(para,szCur.length()+pos);
				}
//				debug("|%i|",pos);
			}
			*/
			return;
		default:
			setFocus();
			break;
	}
	QTextEdit::keyPressEvent(e);
	emit keyPressed();
	if(completelistbox->isVisible()) 
		completition(0);
}

void KviScriptEditorWidget::mouseReleaseEvent (QMouseEvent *e)
{

	completelistbox->hide();
	if (e->button() == Qt::RightButton)
	{
	bool bIsFirstWordInLine;
	QString buffer;
	/*int para = paragraphAt(e->pos());
	int index=charAt(e->pos(),&para);
	buffer=this->text(para);
	getWordOnCursor(buffer,index);
	*/
	QRect r=cursorRect();
	QTextCursor cur=cursorForPosition(e->pos());
	cur.select(QTextCursor::WordUnderCursor);
	buffer=cur.selectedText();
	debug ("TEXTaaaaaaaa %s",buffer.toUtf8().data());

	QString tmp=buffer;
	KviPointerList<QString> *l;
	if (tmp.left(1) == "$")
	{
		tmp.remove(0,1);
		l=KviKvsKernel::instance()->completeCommandAllocateResult(tmp);
		if (l->count() != 1) buffer="";
		else buffer=*(l->at(0));
	}
	else
	{
		l=KviKvsKernel::instance()->completeCommandAllocateResult(tmp);
		if (l->count() != 1) buffer="";
		else buffer=*(l->at(0));
	}
	for (int i=0;i<l->count();i++){
		QString str=*(l->at(i));
		debug ("TEXT %s",str.toUtf8().data());
	}
//	KviKvsKernel::instance()->freeCompletionResult(l);
	debug (buffer);
	m_szHelp=buffer;
	}
	QTextEdit::mouseReleaseEvent(e);


}

bool KviScriptEditorWidget::contextSensitiveHelp() const
{
	
/*	QString buffer;
	int para,index;
	getCursorPosition(&para,&index);
	buffer=text(para);

	getWordOnCursor(buffer,index);
*/
	
	QRect r=cursorRect();
	QTextCursor cur=cursorForPosition(QPoint(r.x(),r.y()));
	cur.select(QTextCursor::WordUnderCursor);
	QString text=cur.selectedText();
	debug ("TEXT %s",text.toUtf8().data());
	QString tmp=text;
	/*KviPointerList<QString> * l;
	if(tmp.left(1) == "$")
	{
		tmp.remove(0,1);
		l = g_pUserParser->completeFunctionAllocateResult(tmp);
	} else {
		l = g_pUserParser->completeCommandAllocateResult(tmp);
	}
	debug ("command or func %s",tmp.toUtf8().data());
	bool bOk = false;
	if(l)
	{
		for(QString * s = l->first();s;s = l->next())
		{
			if(KviQString::equalCI(*s,buffer))
			{
				l->last();
				bOk = true;
			}
		}
	}
	g_pUserParser->freeCompletionResult(l);
	if(!bOk)return false;
	*/
/*
	QString parse;
	KviQString::sprintf(parse,"timer -s (help,0){ help -s %Q; }",&buffer);
	debug ("parsing %s",parse.latin1());
	KviKvsScript::run(parse,(KviWindow*)g_pApp->activeConsole());
	
	
	*/
	return true;
}

void KviScriptEditorWidget::getWordOnCursor(QString &buffer,int index) const
{
	QRegExp re("[ \t=,\\(\\)\"}{\\[\\]\r\n+-*><;@!]");
	//debug("BUFFER IS %s",buffer.toUtf8().data());
	int start = buffer.findRev(re,index);
	int end = buffer.find(re,index);

	QString tmp;
	if(start!=end)
	{
		if(start<0)start=0;
		else start++;
		if(end<0)end=index;
		tmp = buffer.mid(start,end-start);
	}
	buffer = tmp;
	//debug("BUFFER NOW IS %s",buffer.toUtf8().data());
}

void KviScriptEditorWidget::completition(bool bCanComplete)
{
	/*
	
	int line,index;
	QString buffer;
	QString word;
	getCursorPosition(&line,&index);
	buffer=this->text(line);
	bool bIsFirstWordInLine;
	getWordBeforeCursor(buffer,index,&bIsFirstWordInLine);
	if(!buffer.isEmpty())
		completelistbox->updateContents(buffer);
	if (completelistbox->count() == 1) word=completelistbox->text(0);
	if (!word.isEmpty() && bCanComplete)
	{
		insert(word);
		completelistbox->hide();
	}
	if( completelistbox->count() == 0 )
		completelistbox->hide();
	else if(!completelistbox->isVisible())
	{
		if (completelistbox->count() <6) completelistbox->resize(completelistbox->width(),(completelistbox->count()*completelistbox->fontMetrics().height()+20));
		else completelistbox->resize(completelistbox->width(),6*completelistbox->fontMetrics().height()+20);
		int posy=paragraphRect(line).bottom();
		int posx=fontMetrics().width(this->text(line).left(index));
		completelistbox->move(posx,posy);
		completelistbox->show();
	}
	*/
}

void KviScriptEditorWidget::getWordBeforeCursor(QString &buffer,int index,bool *bIsFirstWordInLine)
{
	QString tmp = buffer.left(index);
	buffer=tmp;
	int idx = buffer.findRev(' ');
	int idx1 = buffer.findRev("=");
	int idx2 = buffer.findRev(','); 
	int idx3 = buffer.findRev('(');
	int idx4 = buffer.findRev('"');
	if(idx1 > idx) idx= idx1;	
	if(idx2 > idx)idx = idx2;
	if(idx3 > idx)idx = idx3;
	if(idx4 > idx)idx = idx4;
	*bIsFirstWordInLine = false;
	if(idx > -1)buffer.remove(0,idx);
	else
	{
		*bIsFirstWordInLine = true;
		buffer.insert(0," ");
	}

}

void KviScriptEditorWidget::slotComplete(QListWidgetItem *item)
{/*
	QString complete=str;
	int line,index;
	getCursorPosition(&line,&index);
	QString buffer;
	buffer=this->text(line);
	bool bIsFirstWordInLine;
	getWordBeforeCursor(buffer,index,&bIsFirstWordInLine);
	int len=buffer.length();
//	if (buffer[1].unicode() == '$') len --;
	complete.remove(0,len-1);
	if (buffer[1].unicode() == '$') complete.append("(");
	else complete.append(" ");
	insert (complete);
	completelistbox->hide();
	setFocus();
	*/

}

KviScriptSyntaxHighlighter::KviScriptSyntaxHighlighter(KviScriptEditorWidget * pWidget)
: QSyntaxHighlighter(pWidget)
{
}

KviScriptSyntaxHighlighter::~KviScriptSyntaxHighlighter()
{
}

#define IN_COMMENT 1
#define IN_LINE 2
#define IN_STRING 4

void KviScriptSyntaxHighlighter::highlightBlock(const QString &text)
{
	const QChar * pBuf = (const QChar *)text.ucs2();
	const QChar * c = pBuf;
	QTextCharFormat commentFormat;
	commentFormat.setForeground(g_clrComment);
	commentFormat.setToolTip("Prova del tooltip");
	commentFormat.setFont(g_fntNormal);

	QTextCharFormat bracketFormat;
	 bracketFormat.setForeground(g_clrBracket);
	 bracketFormat.setFont(g_fntNormal);

	 QTextCharFormat punctuationFormat;
	 punctuationFormat.setForeground(g_clrPunctuation);
	 punctuationFormat.setFont(g_fntNormal);

	 QTextCharFormat keywordFormat;
	keywordFormat.setForeground(g_clrKeyword);
	keywordFormat.setFont(g_fntNormal);

	QTextCharFormat variableFormat;
	variableFormat.setForeground(g_clrVariable);
	variableFormat.setFont(g_fntNormal);

	QTextCharFormat normaltextFormat;
	normaltextFormat.setForeground(g_clrNormalText);
	normaltextFormat.setFont(g_fntNormal);
	
	QTextCharFormat functionFormat;
	functionFormat.setForeground(g_clrFunction);
	functionFormat.setFont(g_fntNormal);
	
	
	//if(!c)return setCurrentBlockState(endStateOfLastPara);
	int endStateOfLastPara=currentBlockState();
	if(endStateOfLastPara < 0)endStateOfLastPara = 0;
	
	bool bNewCommand = !(endStateOfLastPara & IN_LINE);
	bool bInComment = endStateOfLastPara & IN_COMMENT;
	bool bInString = endStateOfLastPara & IN_STRING;
	
	const QChar * pBegin;

	while(c->unicode())
	{
		if(bInComment)
		{
			pBegin = c;
			while(c->unicode() && (c->unicode() != '*'))c++;
			if(!c->unicode())
			{
				setFormat(pBegin - pBuf,c - pBegin,commentFormat);
				setCurrentBlockState(IN_COMMENT);
				return;
			}
			c++;
			if(c->unicode() == '/')
			{
				// end of the comment!
				c++;
				setFormat(pBegin - pBuf,c - pBegin,commentFormat);
				bInComment = false;
				bNewCommand = true;
			}
			continue;
		}

		if(c->isSpace())
		{
			while(c->unicode() && c->isSpace())c++;
			if(!c->unicode())continue;
		}

		pBegin = c;

		// this does not break the bNewCommand flag
		if((c->unicode() == '{') || (c->unicode() == '}'))
		{
			c++;
			setFormat(pBegin - pBuf,1,bracketFormat);
			continue;
		}

		if(bNewCommand)
		{
			bNewCommand = false;

			if(c->unicode() == '#')
			{
				if(c > pBuf)
				{
					const QChar * prev = c - 1;
					if((prev->unicode() == ']') || (prev->unicode() == '}'))
					{
						// array or hash count
						c++;
						setFormat(pBegin - pBuf,c - pBegin,punctuationFormat);
						continue;
					}
				}
				// comment until the end of the line
				while(c->unicode())c++;
				setFormat(pBegin - pBuf,c - pBegin,commentFormat);
				continue;
			}
			if(c->unicode() == '/')
			{
				c++;
				if(c->unicode() == '/')
				{
					while(c->unicode())c++;
					setFormat(pBegin - pBuf,c - pBegin,commentFormat);
					continue;
				} else if(c->unicode() == '*')
				{
					c++;
					setFormat(pBegin - pBuf,c - pBegin,commentFormat);
					bInComment = true;
					continue;
				}
				c--;
			}
			if(c->unicode() && (c->isLetterOrNumber() || (c->unicode() == '_')))
			{
				c++;
				while(c->unicode() && (c->isLetterOrNumber() || (c->unicode() == '.') || (c->unicode() == '_') || (c->unicode() == ':')))c++;
				setFormat(pBegin - pBuf,c - pBegin,keywordFormat);
				// special processing for callbacks and magic commands
				if(pBegin->unicode() == 'e')
				{
					if(c - pBegin == 4)
					{
						// might be "else"
						QString tmp(pBegin,4);
						if(tmp.lower() == "else")bNewCommand = true;
						continue;
					}
				}
				else
				if(pBegin->unicode() == 'f')
				{
					if(c - pBegin == 8)
					{
						// might be "function"
						QString tmp(pBegin,8);
						if(tmp.lower() == "function")bNewCommand = true;
						continue;
					}
				}

				if(pBegin->unicode() == 'i')
				{
					if(c - pBegin == 8)
					{
						// might be "internal"
						QString tmp(pBegin,8);
						if(tmp.lower() == "internal")bNewCommand = true;
						continue;
					}
				}

				// not an else or special command function... FIXME: should check for callbacks.. but that's prolly too difficult :)
				continue;
			}
		}
		if(c->unicode() == '$')
		{
			c++;
			if(c->unicode() == '$')
			{
				c++;
				setFormat(pBegin - pBuf,c - pBegin,keywordFormat);
			} else {
				while(c->unicode() && (c->isLetterOrNumber() || (c->unicode() == '.') || (c->unicode() == '_') || (c->unicode() == ':')))c++;
				setFormat(pBegin - pBuf,c - pBegin,functionFormat);
			}
			continue;
		}
		
		if(c->unicode() == '-')
		{
			QChar * pTmp =(QChar *) c;
			c++;
			if(c->unicode() == '-')	c++;
			if(c->isLetter())
			{
				while(c->unicode() && (c->isLetterOrNumber() || (c->unicode() == '_')))c++;
				setFormat(pBegin - pBuf,c - pBegin,keywordFormat);
				continue;
			} else {
				while(c!=pTmp) c--;
			}
		}

		if(c->unicode() == '%')
		{
			c++;
			if(c->unicode() && (c->isLetterOrNumber() || (c->unicode() == ':') || (c->unicode() == '_')))
			{
				while(c->unicode() && (c->isLetterOrNumber() || (c->unicode() == ':') || (c->unicode() == '_')))c++;
				setFormat(pBegin - pBuf,c - pBegin,variableFormat);
				continue;
			}
			c--;
		}

		if(!c->unicode())continue;

		if(c->isLetterOrNumber() || c->unicode() == '_')
		{
			c++;
			while(c->unicode() && c->isLetterOrNumber() || (c->unicode() == '_'))c++;
			setFormat(pBegin - pBuf,c - pBegin,normaltextFormat);
			continue;
		}

		if(c->unicode() == '\\')
		{
			c++;
			setFormat(pBegin - pBuf,c - pBegin,punctuationFormat);
			// the next char is to be interpreted as normal text
			pBegin = c;
			if(c->unicode() && (c->unicode() != '\n'))
			{
				c++;
				setFormat(pBegin - pBuf,c - pBegin,normaltextFormat);
				continue;
			}
			// this is never returned since Qt sux in string processing
			// it sets the newlines to spaces and we have no secure way to undestand that this was the end of a line
			setCurrentBlockState(IN_LINE);
			return;
		}

		if(c->unicode() == '"')
		{
			bInString = !bInString;
			c++;
			setFormat(pBegin - pBuf,c - pBegin,punctuationFormat);
			continue;
		} else if(c->unicode() == ';')
		{
			if(!bInString)	bNewCommand = true; // the next will be a new command
		}

		c++;
		if(bInString)
		{
			setFormat(pBegin - pBuf,c - pBegin,normaltextFormat);
		} else {
			setFormat(pBegin - pBuf,c - pBegin,punctuationFormat);
		}
	}
/*
	bool i=TRUE;
	QString szFind=((KviScriptEditorWidget *)textEdit())->m_szFind;
	if (!szFind.isEmpty())
	{
		int index=0;
		while (i)
		{
			index=text.find(szFind,index,false);
			if (index != -1)
			{
				setFormat(index,szFind.length(),g_clrFind);
				index += szFind.length();
			}
		else i=false;
		}
	}
	*/
	if(bInString)
		setCurrentBlockState(IN_LINE | IN_STRING);
	
}

// 22.02.2005 :: 00:01
// valgrind --leak-check=yes --num-callers=10 -v kvirc -f
//
//==30299== Warning: SIGSEGV not in user code; either from syscall kill()
//==30299==    or possible Valgrind bug.  This message is only shown 3 times.
//==30299== Warning: SIGSEGV not in user code; either from syscall kill()
//==30299==    or possible Valgrind bug.  This message is only shown 3 times.
//==30299== Warning: SIGSEGV not in user code; either from syscall kill()
//==30299==    or possible Valgrind bug.  This message is only shown 3 times.


KviScriptEditorImplementation::KviScriptEditorImplementation(QWidget * par)
:KviScriptEditor(par)
{
	if(g_pScriptEditorWindowList->isEmpty())loadOptions();
	g_pScriptEditorWindowList->append(this);
	m_lastCursorPos=QPoint(0,0);
	QGridLayout * g = new QGridLayout(this,2,3,0,0);

	m_pFindLineedit = new QLineEdit(" ",this);
	m_pFindLineedit->setText("");

	QPalette p = m_pFindLineedit->palette(); 
	p.setColor(foregroundRole(), g_clrFind); 
	m_pFindLineedit->setPalette(p);

	
	m_pEditor = new KviScriptEditorWidget(this);
	g->addMultiCellWidget(m_pEditor,0,0,0,3);
	g->setRowStretch(0,1);

	QToolButton * b = new QToolButton(Qt::DownArrow,this,"dsa2");
	b->setMinimumWidth(24);
	g->addWidget(b,1,0);

	KviTalPopupMenu * pop = new KviTalPopupMenu(b);
	pop->insertItem(__tr2qs_ctx("&Open...","editor"),this,SLOT(loadFromFile()));
	pop->insertItem(__tr2qs_ctx("&Save As...","editor"),this,SLOT(saveToFile()));
	pop->insertSeparator();
	pop->insertItem(__tr2qs_ctx("&Configure Editor...","editor"),this,SLOT(configureColors()));
	b->setPopup(pop);
	b->setPopupDelay(1);

	g->setColStretch(1,1);
	g->setColStretch(2,10);
	g->addWidget(m_pFindLineedit,1,2);
	QLabel *lab= new QLabel("find",this);
	lab->setText(tr("Find"));

	g->addWidget(lab,1,1);
	m_pRowColLabel = new QLabel("0",this);
	m_pRowColLabel->setFrameStyle(QFrame::Sunken | QFrame::Panel);
	m_pRowColLabel->setMinimumWidth(80);
	g->addWidget(m_pRowColLabel,1,3);

	connect(m_pFindLineedit,SIGNAL(returnPressed()),m_pEditor,SLOT(slotFind()));
	connect(m_pFindLineedit,SIGNAL(returnPressed()),this,SLOT(slotFind()));
	connect(m_pEditor,SIGNAL(keyPressed()),this,SLOT(updateRowColLabel()));
	connect(m_pEditor,SIGNAL(textChanged()),this,SLOT(updateRowColLabel()));
	connect(m_pEditor,SIGNAL(selectionChanged()),this,SLOT(updateRowColLabel()));
	m_lastCursorPos = QPoint(-1,-1);
}

KviScriptEditorImplementation::~KviScriptEditorImplementation()
{
	g_pScriptEditorWindowList->removeRef(this);
	if(g_pScriptEditorWindowList->isEmpty())saveOptions();
}

void KviScriptEditorImplementation::loadOptions()
{
	QString tmp;
	g_pEditorModulePointer->getDefaultConfigFileName(tmp);

	KviConfig cfg(tmp,KviConfig::Read);
	g_clrBackground = cfg.readColorEntry("Background",QColor(0,0,0));;
	g_clrNormalText = cfg.readColorEntry("NormalText",QColor(100,255,0));
	g_clrBracket = cfg.readColorEntry("Bracket",QColor(255,0,0));
	g_clrComment = cfg.readColorEntry("Comment",QColor(0,120,0));
	g_clrFunction = cfg.readColorEntry("Function",QColor(255,255,0));
	g_clrKeyword = cfg.readColorEntry("Keyword",QColor(120,120,150));
	g_clrVariable = cfg.readColorEntry("Variable",QColor(200,200,200));
	g_clrPunctuation = cfg.readColorEntry("Punctuation",QColor(180,180,0));
	g_clrFind = cfg.readColorEntry("Find",QColor(255,0,0));
	g_fntNormal = cfg.readFontEntry("Font",QFont("Fixed",12));
}

bool KviScriptEditorImplementation::isModified()
{
	return m_pEditor->isModified();
}

void KviScriptEditorImplementation::slotFind()
{
	emit find(m_pFindLineedit->text());

}

void KviScriptEditorImplementation::slotNextFind(const QString &text)
{
//	emit nextFind(const QString &text);
}

void KviScriptEditorImplementation::slotInitFind()
{
	emit initFind();
}

void KviScriptEditorImplementation::slotReplaceAll(const QString &txt,const QString &txt1)
{
	emit replaceAll(txt,txt1);
}

void KviScriptEditorImplementation::saveOptions()
{
	QString tmp;
	g_pEditorModulePointer->getDefaultConfigFileName(tmp);

	KviConfig cfg(tmp,KviConfig::Write);
	cfg.writeEntry("Background",g_clrBackground);;
	cfg.writeEntry("NormalText",g_clrNormalText);
	cfg.writeEntry("Bracket",g_clrBracket);
	cfg.writeEntry("Comment",g_clrComment);
	cfg.writeEntry("Function",g_clrFunction);
	cfg.writeEntry("Keyword",g_clrKeyword);
	cfg.writeEntry("Variable",g_clrVariable);
	cfg.writeEntry("Punctuation",g_clrPunctuation);
	cfg.writeEntry("Find",g_clrFind);
	cfg.writeEntry("Font",g_fntNormal);
}

void KviScriptEditorImplementation::setFocus()
{
	m_pEditor->setFocus();
}

void KviScriptEditorImplementation::focusInEvent(QFocusEvent *)
{
	m_pEditor->setFocus();
}

void KviScriptEditorImplementation::setEnabled(bool bEnabled)
{
	QWidget::setEnabled(bEnabled);
	m_pEditor->setEnabled(bEnabled);
	m_pRowColLabel->setEnabled(bEnabled);
}

void KviScriptEditorImplementation::saveToFile()
{
	QString fName;
	if(KviFileDialog::askForSaveFileName(fName,
		__tr2qs_ctx("Choose a Filename - KVIrc","editor"),
		QString::null,
		QString::null,false,true,true))
	{
		QString buffer = m_pEditor->text();

		//if(tmp.isEmpty())tmp = "";
		//KviStr buffer = tmp.toUtf8().data();
		if(!KviFileUtils::writeFile(fName,buffer))
		{
			QString tmp;
			QMessageBox::warning(this,
				__tr2qs_ctx("Save Failed - KVIrc","editor"),
				KviQString::sprintf(tmp,__tr2qs_ctx("Can't open the file %s for writing.","editor"),&fName));
		}
	}
}

void KviScriptEditorImplementation::setText(const KviQCString &txt)
{
	m_pEditor->setText(txt.data());
	m_pEditor->setTextFormat(Qt::PlainText);
	//m_pEditor->moveCursor(KviTalTextEdit::MoveEnd,false);
	m_pEditor->setModified(false);
	updateRowColLabel();
}

void KviScriptEditorImplementation::getText(KviQCString &txt)
{
	txt = m_pEditor->text();
}

QLineEdit * KviScriptEditorImplementation::getFindlineedit()
{
	return m_pFindLineedit;
}

void KviScriptEditorImplementation::setText(const QString &txt)
{
	m_pEditor->setText(txt);
	m_pEditor->setTextFormat(Qt::PlainText);
//	m_pEditor->moveCursor(KviTalTextEdit::MoveEnd,false);
	m_pEditor->setModified(false);
	updateRowColLabel();
}

void KviScriptEditorImplementation::getText(QString &txt)
{
	txt = m_pEditor->text();
}

void KviScriptEditorImplementation::setFindText(const QString &txt)
{
	m_pFindLineedit->setText(txt);
	m_pEditor->slotFind();

}

void KviScriptEditorImplementation::setFindLineeditReadOnly(bool b)
{
	m_pFindLineedit->setReadOnly(b);
	
}

void KviScriptEditorImplementation::updateRowColLabel()
{
/*
	int iRow,iCol;
	m_pEditor->getCursorPosition(&iRow,&iCol);
	if(iRow != m_lastCursorPos.x() || iCol != m_lastCursorPos.y())
	{
		m_lastCursorPos = QPoint(iRow,iCol);
		QString tmp;
		KviQString::sprintf(tmp,__tr2qs_ctx("Row: %d Col: %d","editor"),iRow,iCol);
		m_pRowColLabel->setText(tmp);
	}
	*/
}

QPoint KviScriptEditorImplementation::getCursor()
{
	return m_lastCursorPos;
}

void KviScriptEditorImplementation::setCursorPosition(QPoint pos)
{
	/*
	m_pEditor->setCursorPosition(pos.x(),pos.y());
	m_pEditor->setFocus();
	m_pEditor->ensureCursorVisible();
	QString tmp;
	KviQString::sprintf(tmp,__tr2qs_ctx("Row: %d Col: %d","editor"),pos.x(),pos.y());
	m_pRowColLabel->setText(tmp);
	
	m_lastCursorPos=pos;
	*/
}

void KviScriptEditorImplementation::loadFromFile()
{
	/*
	QString fName;
	if(KviFileDialog::askForOpenFileName(fName,
		__tr2qs_ctx("Load Script File - KVIrc","editor"),
		QString::null,
		QString::null,false,true))
	{
		QString buffer;
		if(KviFileUtils::loadFile(fName,buffer))
		{
			m_pEditor->setText(buffer);
			m_pEditor->moveCursor(KviTalTextEdit::MoveEnd,false);
			updateRowColLabel();
		} else {
			QString tmp;
			QMessageBox::warning(this,
				__tr2qs_ctx("Open Failed - KVIrc","editor"),
				KviQString::sprintf(tmp,__tr2qs_ctx("Can't open the file %s for reading.","editor"),&fName));
		}
	}
	*/
}

void KviScriptEditorImplementation::configureColors()
{
	KviScriptEditorWidgetColorOptions dlg(this);
	if(dlg.exec() == QDialog::Accepted)

	{
		m_pEditor->updateOptions();
		saveOptions();
	}
}

KviScriptEditorReplaceDialog::KviScriptEditorReplaceDialog( QWidget* parent, const char* name)
: QDialog( parent)
{
	m_pParent=parent;
	emit initFind();
	QPalette p = palette(); 
	p.setColor(foregroundRole(),QColor( 0, 0, 0 )); 
	p.setColor(backgroundRole(),QColor( 236, 233, 216 )); 
	setPalette(p);

QGridLayout *layout = new QGridLayout( this, 1, 1, 11, 6, "replace layout"); 
 
	m_pFindlineedit = new QLineEdit( this, "findlineedit" );
	layout->addMultiCellWidget( m_pFindlineedit, 2, 2, 1, 2 );

	m_pReplacelineedit = new QLineEdit( this, "replacelineedit" );
	layout->addMultiCellWidget( m_pReplacelineedit, 3, 3, 1, 2 );

	m_pFindlineedit->setFocus();

	QLabel *findlabel = new QLabel( this, "findlabel" );
	findlabel->setText(tr("Word to Find"));
	layout->addWidget( findlabel, 2, 0 );

	QLabel *replacelabel = new QLabel( this, "replacelabel" );
	replacelabel->setText(tr("Replace with"));
	layout->addWidget( replacelabel, 3, 0 );

	QPushButton *cancelbutton = new QPushButton( this, "cancelButton" );
	cancelbutton->setText(tr("&Cancel"));
	layout->addWidget( cancelbutton, 5, 2 );

	replacebutton = new QPushButton( this, "replacebutton" );
	replacebutton->setText(tr("&Replace"));
	replacebutton->setEnabled( FALSE );
	layout->addWidget( replacebutton, 5, 0 );

	checkReplaceAll = new KviStyledCheckBox( this, "replaceAll" );
	checkReplaceAll->setText(tr("&Replace in all Aliases"));
	layout->addWidget( checkReplaceAll, 4, 0 );
	
	findNext = new QPushButton(this, "findNext(WIP)" );	
	findNext->setText(tr("&Findnext"));
	layout->addWidget( findNext, 2, 3 );
	findNext->setEnabled(false);

	replace = new QPushButton(this, "replace" );	
	replace->setText(tr("&Replace(WIP)"));
	layout->addWidget( replace, 3, 3 );
	replace->setEnabled(false);

	// signals and slots connections
	connect( replacebutton, SIGNAL( clicked() ), this, SLOT( slotReplace() ) );
	connect( findNext, SIGNAL( clicked() ),this,SLOT( slotNextFind()));
	connect( cancelbutton, SIGNAL( clicked() ), this, SLOT( reject() ) );
	connect( m_pFindlineedit, SIGNAL( textChanged(const QString &)), this, SLOT( textChanged(const QString &)));
}


KviScriptEditorReplaceDialog::~KviScriptEditorReplaceDialog()
{
}

void KviScriptEditorReplaceDialog::textChanged(const QString &txt)
{
	if (!txt.isEmpty()) replacebutton->setEnabled(TRUE);
	else replacebutton->setEnabled(FALSE);

}

void KviScriptEditorReplaceDialog::slotReplace()
{
	QString txt=((KviScriptEditorWidget *)m_pParent)->text();
	if (checkReplaceAll->isChecked()) emit replaceAll(m_pFindlineedit->text(),m_pReplacelineedit->text());
	txt.replace(m_pFindlineedit->text(),m_pReplacelineedit->text(),false);
	((KviScriptEditorWidget *)m_pParent)->setText(txt);
	((KviScriptEditorWidget *)m_pParent)->setModified(true);
	m_pFindlineedit->setText("");
	m_pReplacelineedit->setText("");
	setTabOrder(m_pFindlineedit,m_pReplacelineedit);
}

void KviScriptEditorReplaceDialog::slotNextFind()
{
	emit nextFind(m_pFindlineedit->text());
}
