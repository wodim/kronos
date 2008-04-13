//=============================================================================
//
//   File : kvi_kvs_treenode_command.cpp
//   Created on Thu 09 Oct 2003 01:49:40 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2003 Szymon Stefanek <pragma at kvirc dot net>
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

#include "kvi_kvs_treenode_command.h"
#include "kvi_kvs_treenode_switchlist.h"

KviKvsTreeNodeCommand::KviKvsTreeNodeCommand(const QChar * pLocation,const QString &szCmdName)
: KviKvsTreeNodeInstruction(pLocation)
{
	m_szCmdName = szCmdName;
	m_pSwitches = 0;
}

KviKvsTreeNodeCommand::~KviKvsTreeNodeCommand()
{
	if(m_pSwitches)delete m_pSwitches;
}

//#warning "All the dump() functions could be killed (or moved to print on the kvirc windows)"

void KviKvsTreeNodeCommand::contextDescription(QString &szBuffer)
{
	KviQString::sprintf(szBuffer,"Command \"%s\"",&m_szCmdName);
}

void KviKvsTreeNodeCommand::dump(const char * prefix)
{
	qDebug("%s Command(%s)",prefix,m_szCmdName.toUtf8().data());
	dumpSwitchList(prefix);
}

void KviKvsTreeNodeCommand::dumpSwitchList(const char * prefix)
{
	if(!m_pSwitches)return;
	QString tmp = prefix;
	tmp.append("  ");
	m_pSwitches->dump(tmp.toUtf8().data());
}

void KviKvsTreeNodeCommand::setSwitchList(KviKvsTreeNodeSwitchList * sw)
{
	m_pSwitches = sw;
	m_pSwitches->setParent(this);
}
