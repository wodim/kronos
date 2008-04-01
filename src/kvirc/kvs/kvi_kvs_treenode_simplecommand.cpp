//=============================================================================
//
//   File : kvi_kvs_treenode_simplecommand.cpp
//   Created on Thu 09 Oct 2003 02:12:18 by Szymon Stefanek
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

#include "kvi_kvs_treenode_simplecommand.h"
#include "kvi_kvs_treenode_datalist.h"

KviKvsTreeNodeSimpleCommand::KviKvsTreeNodeSimpleCommand(const QChar * pLocation,const QString &szCmdName,KviKvsTreeNodeDataList * params)
: KviKvsTreeNodeCommandWithParameters(pLocation,szCmdName,params)
{
}

KviKvsTreeNodeSimpleCommand::~KviKvsTreeNodeSimpleCommand()
{
}

void KviKvsTreeNodeSimpleCommand::contextDescription(QString &szBuffer)
{
	szBuffer = "Simple Command \"";
	szBuffer += m_szCmdName;
	szBuffer += "\"";
}


void KviKvsTreeNodeSimpleCommand::dump(const char * prefix)
{
	debug("%s SimpleCommand(%s)",prefix,m_szCmdName.utf8().data());
	dumpSwitchList(prefix);
	dumpParameterList(prefix);
}
