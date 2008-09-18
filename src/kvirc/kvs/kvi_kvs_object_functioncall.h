#ifndef _KVI_KVS_OBJECT_FUNCTIONCALL_H_
#define _KVI_KVS_OBJECT_FUNCTIONCALL_H_
//=============================================================================
//
//   File : kvi_kvs_object_functioncall.h
//   Creation date : Sun 24 Apr 2005 04:05:54 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2005-2008 Szymon Stefanek <pragma at kvirc dot net>
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

#include "kvi_settings.h"

#include "kvi_kvs_runtimecall.h"
#include "kvi_kvs_variantlist.h"
#include "kvi_kvs_variant.h"


class KVIRC_API KviKvsObjectFunctionCall : public KviKvsRunTimeCall
{
protected:
	KviKvsVariant * m_pResult;
public:
	KviKvsObjectFunctionCall(KviKvsRunTimeContext * pContext,
					KviKvsVariantList * pParams,
					KviKvsVariant * pResult)
		: KviKvsRunTimeCall(pContext,pParams), m_pResult(pResult)
		{};
	~KviKvsObjectFunctionCall(){};
public:
	KviKvsVariant * returnValue(){ return m_pResult; };
};

#endif //!_KVI_KVS_OBJECT_FUNCTIONCALL_H_
