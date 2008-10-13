//=============================================================================
//
//   File : kvi_tal_vbox.cpp
//   Creation date : Mon Jan 22 2007 11:25:08 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2007 Szymon Stefanek (pragma at kvirc dot net)
//   Copyright (C) 2008 Elvio Basello (hellvis69 at netsons dot org)
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

#include "kvi_tal_vbox.h"

#include <QChildEvent>

KviTalVBox::KviTalVBox(QWidget * pParent, char * pcName)
: QWidget(pParent)
{
	setObjectName(pcName);

	m_pLayout = new QVBoxLayout(this);
	m_pLayout->setMargin(3);
	m_pLayout->setSpacing(2);

	setLayout(m_pLayout);
}

KviTalVBox::~KviTalVBox()
{
}

void KviTalVBox::childEvent(QChildEvent * e)
{
	if(!e->child()->isWidgetType())
		return;
	if(e->child()->parent() != this)
		return;

	switch(e->type())
	{
		case QEvent::ChildAdded:
			m_pLayout->addWidget((QWidget *)(e->child()));
			break;
		case QEvent::ChildRemoved:
			m_pLayout->removeWidget((QWidget *)(e->child()));
			break;
		default:
			break;
	}
}

void KviTalVBox::setStretchFactor(QWidget * pChild, int iStretch)
{
	m_pLayout->setStretchFactor(pChild,iStretch);
}

void KviTalVBox::setSpacing(int iSpacing)
{
	m_pLayout->setSpacing(iSpacing);
}

void KviTalVBox::addStretch(int iStretch)
{
	m_pLayout->addStretch(iStretch);
}

void KviTalVBox::setMargin(int iMargin)
{
	m_pLayout->setContentsMargins(iMargin,iMargin,iMargin,iMargin);
}

void KviTalVBox::setAlignment(Qt::Alignment alignment)
{
	m_pLayout->setAlignment(alignment);
}

void KviTalVBox::setAlignment(QWidget * pChild, Qt::Alignment alignment)
{
	m_pLayout->setAlignment(pChild,alignment);
}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
	#include "kvi_tal_vbox.moc"
#endif //COMPILE_USE_STANDALONE_MOC_SOURCES
