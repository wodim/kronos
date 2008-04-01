//=============================================================================
//
//   File : kvi_kvs_treenode_data.cpp
//   Creation date : Sun 05 Oct 2003 21.52 CEST by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2003 Szymon Stefanek (pragma at kvirc dot net)
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

#include "kvi_kvs_treenode_data.h"
#include "kvi_locale.h"


KviKvsTreeNodeData::KviKvsTreeNodeData(const QChar * pLocation)
: KviKvsTreeNode(pLocation), m_pEndingLocation(0)
{
}


KviKvsTreeNodeData::~KviKvsTreeNodeData()
{
}

void KviKvsTreeNodeData::contextDescription(QString &szBuffer)
{
	szBuffer = "Data Evaluation";
}

void KviKvsTreeNodeData::dump(const char * prefix)
{
	qDebug("%s Data",prefix);
}

bool KviKvsTreeNodeData::isReadOnly()
{
	return true;
}

bool KviKvsTreeNodeData::canEvaluateToObjectReference()
{
	return false;
}

bool KviKvsTreeNodeData::canEvaluateInObjectScope()
{
	return false;
}

bool KviKvsTreeNodeData::isFunctionCall()
{
	return false;
}

bool KviKvsTreeNodeData::convertStringConstantToNumeric()
{
	return false;
}

/*
bool KviKvsTreeNodeData::canReleaseResult()
{
	return false;
}
*/

bool KviKvsTreeNodeData::evaluateReadOnly(KviKvsRunTimeContext * c,KviKvsVariant * pBuffer)
{
	c->error(this,__tr2qs("Internal error: pure virtual evaluateReadOnly called"));
	return false;
}

KviKvsRWEvaluationResult * KviKvsTreeNodeData::evaluateReadWrite(KviKvsRunTimeContext * c)
{
	c->error(this,__tr2qs("Internal error: trying to evaluate as read-write a read-only data location"));
	return 0;
}


bool KviKvsTreeNodeData::evaluateReadOnlyInObjectScope(KviKvsObject * o,KviKvsRunTimeContext * c,KviKvsVariant * pBuffer)
{
	c->error(this,__tr2qs("Internal error: pure virtual evaluateReadOnlyInObjectScope called"));
	return false;
}

KviKvsRWEvaluationResult * KviKvsTreeNodeData::evaluateReadWriteInObjectScope(KviKvsObject * o,KviKvsRunTimeContext * c)
{
	c->error(this,__tr2qs("Internal error: trying to evaluate as read-write a read-only data location"));
	return 0;
}
