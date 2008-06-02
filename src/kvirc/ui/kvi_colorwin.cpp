//=============================================================================
//
//   File : kvi_colorwin.cpp
//   Creation date : Wed Jan 6 1999 04:30:20 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 1999-2007 Szymon Stefanek (pragma at kvirc dot net)
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

#define __KVIRC__

#include "kvi_colorwin.h"
#include "kvi_app.h"
#include "kvi_options.h"

#include <QPainter>
#include <QString>
#include <QEvent>
#include <QMouseEvent>

KviColorWindow::KviColorWindow()
:QWidget(0)
{
	setObjectName("toplevel_color_window");
	setWindowFlags(Qt::Popup);
	setFocusPolicy(Qt::NoFocus);
	setFixedSize(146,38);
	m_pOwner = 0;
	QFont fnt = QFont();
	fnt.setStyleHint(QFont::TypeWriter);
	fnt.setPointSize(10);
	//QFont fnt("fixed",10);
	setFont(fnt);
	m_iTimerId = -1;
}

KviColorWindow::~KviColorWindow()
{
	if(m_iTimerId != -1)
		killTimer(m_iTimerId);
//	if(m_pOwner)m_pOwner->setFocus();
}

void KviColorWindow::popup(QWidget *owner)
{
	m_pOwner = owner;
	show();
}


void KviColorWindow::paintEvent(QPaintEvent *)
{
	static int clrIdx[16]={ 1,0,0,0,0,0,0,0,1,1,1,1,0,0,0,1 };
	QPainter p(this);

	for(int i=0;i<16;i++)
	{
		p.fillRect((i % 8) * 18,(i / 8) * 18,18,18,KVI_OPTION_MIRCCOLOR(i));
		p.setPen(KVI_OPTION_MIRCCOLOR(clrIdx[i]));
		KviStr szI(KviStr::Format,"%d",i);
		p.drawText((i % 8) * 18,(i / 8) * 18,18,18,Qt::AlignVCenter | Qt::AlignHCenter,szI.ptr());
	}
}

void KviColorWindow::keyPressEvent(QKeyEvent *e)
{
	if(m_iTimerId != -1)
		killTimer(m_iTimerId);
	hide();
	if(m_pOwner)g_pApp->sendEvent(m_pOwner,e);
}

void KviColorWindow::mousePressEvent(QMouseEvent *e)
{
	if(e->pos().x() < 0)goto hideme;
    if(e->pos().x() > width())goto hideme;
    if(e->pos().y() < 0)goto hideme;
    if(e->pos().y() > height())goto hideme;
	QString str;
	int key=e->x()/18;
	if (e->x()<36 && e->y()>18) key +=8;
	if (e->x()>36 && e->y()>18) key -=2;
	int ascii=key+48;
	str.setNum(key);
	if (e->x()>36 && e->y()>18)
		if(m_pOwner) g_pApp->sendEvent(m_pOwner,new QKeyEvent(QEvent::KeyPress,Qt::Key_1,Qt::NoModifier,"1"));
	if(m_pOwner) g_pApp->sendEvent(m_pOwner,new QKeyEvent(QEvent::KeyPress,key,(Qt::KeyboardModifiers)Qt::NoModifier,str));
	hideme:
	if(m_iTimerId != -1)
		killTimer(m_iTimerId);
	hide();
}
void KviColorWindow::show()
{
	m_iTimerId = startTimer(10000); //10 sec ...seems enough
	QWidget::show();
}

void KviColorWindow::timerEvent(QTimerEvent *)
{
	if(m_iTimerId != -1)
		killTimer(m_iTimerId);
	hide();
}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "kvi_colorwin.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
