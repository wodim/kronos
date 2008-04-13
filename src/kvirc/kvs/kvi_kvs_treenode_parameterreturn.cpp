//=============================================================================
//
//   File : kvi_kvs_treenode_parameterreturn.cpp
//   Created on Fri 30 Jan 2004 01:31:01 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2004 Szymon Stefanek <pragma at kvirc dot net>
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
#include "kvi_kvs_treenode_parameterreturn.h"
#include "kvi_kvs_treenode_datalist.h"
#include "kvi_kvs_runtimecontext.h"
#include "kvi_kvs_variantlist.h"
#include "kvi_locale.h"

KviKvsTreeNodeParameterReturn::KviKvsTreeNodeParameterReturn(const QChar * pLocation,KviKvsTreeNodeDataList * pDataList)
: KviKvsTreeNodeInstruction(pLocation)
{
	m_pDataList = pDataList;
	m_pDataList->setParent(this);
}

KviKvsTreeNodeParameterReturn::~KviKvsTreeNodeParameterReturn()
{
	delete m_pDataList;
}

void KviKvsTreeNodeParameterReturn::contextDescription(QString &szBuffer)
{
	szBuffer = "Parameter Return Evaluation";
}


void KviKvsTreeNodeParameterReturn::dump(const char * prefix)
{
	qDebug("%s ParameterReturn",prefix);
	QString tmp = prefix;
	tmp += "  ";
	m_pDataList->dump(tmp.toUtf8().data());
}

bool KviKvsTreeNodeParameterReturn::execute(KviKvsRunTimeContext * c)
{
	KviKvsVariantList lBuffer;
	if(!m_pDataList->evaluate(c,&lBuffer))return false;
	if(lBuffer.count() == 0)
	{
		c->returnValue()->setNothing();
		return true;
	}
	if(lBuffer.count() == 1)
	{
		c->returnValue()->copyFrom(*(lBuffer.first()));
		return true;
	}

	QString all;
	lBuffer.allAsString(all);
	c->returnValue()->setString(all);
	return true;
}
