//=============================================================================
//
//   File : kvi_kvs_treenode_specialcommanddo.cpp
//   Creation date : Thu 06 Nov 2003 14:09:53 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2003-2008 Szymon Stefanek <pragma at kvirc dot net>
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


#include "kvi_kvs_treenode_specialcommanddo.h"
#include "kvi_kvs_treenode_expression.h"
#include "kvi_kvs_treenode_instruction.h"
#include "kvi_kvs_runtimecontext.h"
#include "kvi_locale.h"

KviKvsTreeNodeSpecialCommandDo::KviKvsTreeNodeSpecialCommandDo(const QChar * pLocation,KviKvsTreeNodeExpression * e,KviKvsTreeNodeInstruction * i)
: KviKvsTreeNodeSpecialCommand(pLocation,"do")
{
	m_pExpression = e;
	m_pExpression->setParent(this);
	m_pInstruction = i;
	if(i)m_pInstruction->setParent(this);
}

KviKvsTreeNodeSpecialCommandDo::~KviKvsTreeNodeSpecialCommandDo()
{
	delete m_pExpression;
	if(m_pInstruction)delete m_pInstruction;
}

void KviKvsTreeNodeSpecialCommandDo::contextDescription(QString &szBuffer)
{
	szBuffer = "Special Command \"do\"";
}

void KviKvsTreeNodeSpecialCommandDo::dump(const char *)
{
}

bool KviKvsTreeNodeSpecialCommandDo::execute(KviKvsRunTimeContext * c)
{
	for(;;)
	{
		if(m_pInstruction)
		{
			if(!m_pInstruction->execute(c))
			{
				if(c->error())return false;

				if(c->breakPending())
				{
					c->handleBreak();
					return true;
				}

				if(c->continuePending())
				{
					c->handleContinue();
					continue;
				}
				return false;
			}
		}

		KviKvsVariant v;
		if(!m_pExpression->evaluateReadOnly(c,&v))return false;
		if(!v.asBoolean())break;
	}
	return true;
}
