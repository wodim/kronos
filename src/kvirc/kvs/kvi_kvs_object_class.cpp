//=============================================================================
//
//   File : kvi_kvs_objectclass.cpp
//   Created on Sat 23 Apr 2005 20:31:32 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2005 Szymon Stefanek <pragma at kvirc dot net>
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

#include "kvi_kvs_object_class.h"
#include "kvi_kvs_object_controller.h"
#include "kvi_kvs_object.h"
#include "kvi_kvs_kernel.h"
#include "kvi_kvs_object_functionhandlerimpl.h"
#include "kvi_kvs_object_functioncall.h"
#include "kvi_kvs_runtimecontext.h"
#include "kvi_kvs_variantlist.h"
#include "kvi_kvs_script.h"
#include "kvi_fileutils.h"
#include "kvi_window.h"
#include "kvi_cmdformatter.h"

KviKvsObjectClass::KviKvsObjectClass(
		KviKvsObjectClass * pParent,
		const QString &szName,
		KviKvsObjectAllocateInstanceProc pProc,
		bool bBuiltin)
{
	m_pParentClass  = pParent;
	if(m_pParentClass)
		m_pParentClass->registerChildClass(this);
	m_szName        = szName;
	m_bBuiltin      = bBuiltin;
	m_bDirty        = !bBuiltin;
	m_pFunctionHandlers = new KviPointerHashTable<QString,KviKvsObjectFunctionHandler>(17,false);
	m_pFunctionHandlers->setAutoDelete(true);
	m_pChildClasses = new KviPointerList<KviKvsObjectClass>;
	m_pChildClasses->setAutoDelete(false);
	m_allocProc     = pProc ? pProc : pParent->m_allocProc;

	// inherit everything from the class above
	if(pParent)
	{
		KviPointerHashTableIterator<QString,KviKvsObjectFunctionHandler> it(*(pParent->functionHandlers()));
		while(KviKvsObjectFunctionHandler * fh = it.current())
		{
			m_pFunctionHandlers->insert(it.currentKey(),fh->clone());
			++it;
		}
	}

	// "object" class is automatically registered in the controller constructor
	KviKvsKernel::instance()->objectController()->registerClass(this);
}

KviKvsObjectClass::~KviKvsObjectClass()
{
	KviKvsKernel::instance()->objectController()->killAllObjectsWithClass(this);
	if(m_pParentClass)m_pParentClass->unregisterChildClass(this);
	KviKvsKernel::instance()->objectController()->unregisterClass(this);
	delete m_pFunctionHandlers;
	while(m_pChildClasses->first())delete m_pChildClasses->first();
	delete m_pChildClasses;
}

void KviKvsObjectClass::registerFunctionHandler(const QString & szFunctionName,KviKvsObjectFunctionHandlerProc pProc,unsigned int uFlags)
{
	m_pFunctionHandlers->replace(szFunctionName,new KviKvsObjectCoreCallFunctionHandler(pProc,uFlags));
}

void KviKvsObjectClass::registerFunctionHandler(const QString & szFunctionName,const QString &szBuffer,unsigned int uFlags)
{
	QString szContext = m_szName;
	szContext += "::";
	szContext += szFunctionName;
	m_pFunctionHandlers->replace(szFunctionName,new KviKvsObjectScriptFunctionHandler(szContext,szBuffer,uFlags));
}

void KviKvsObjectClass::registerStandardNothingReturnFunctionHandler(const QString &szFunctionName)
{
	m_pFunctionHandlers->replace(szFunctionName,new KviKvsObjectStandardNothingReturnFunctionHandler());
}

void KviKvsObjectClass::registerStandardTrueReturnFunctionHandler(const QString &szFunctionName)
{
	m_pFunctionHandlers->replace(szFunctionName,new KviKvsObjectStandardTrueReturnFunctionHandler());
}

void KviKvsObjectClass::registerStandardFalseReturnFunctionHandler(const QString &szFunctionName)
{
	m_pFunctionHandlers->replace(szFunctionName,new KviKvsObjectStandardFalseReturnFunctionHandler());
}


KviKvsObject * KviKvsObjectClass::allocateInstance(KviKvsObject * pParent,const QString &szName,KviKvsRunTimeContext * pContext,KviKvsVariantList * pParams)
{
	if(!m_allocProc)return 0;
	KviKvsObject * pObject = m_allocProc(this,pParent,szName);
	if(!pObject)return 0;

	KviKvsVariant * v=pParams->first(); // FIXME: what the hell is this ?
	if(!pObject->init(pContext,pParams))
	{
		// internal init failure : abort
		delete pObject;
		return 0;
	}

	KviKvsVariant ret;
	KviKvsVariantList copy;
	copy.setAutoDelete(false);
	while(v)
	{
		copy.append(v);
		v = pParams->next();
	}

	if(!pObject->callFunction(pObject,"constructor",QString::null,pContext,&ret,&copy))
	{
		// ops...constructor failed (script error!)
		delete pObject;
		return 0;
	} else {
		if(ret.isInteger())
		{
			if(ret.integer() == 0)
			{
				// implementation failure...
				delete pObject;
				return 0;
			}
		} else if(ret.isHObject())
		{
			if(ret.hobject() == (kvs_hobject_t)0)
			{
				// implementation failure...
				delete pObject;
				return 0;
			}
		}
	}

	return pObject;
}

void KviKvsObjectClass::registerChildClass(KviKvsObjectClass *pClass)
{
	m_pChildClasses->append(pClass);
}

void KviKvsObjectClass::unregisterChildClass(KviKvsObjectClass *pClass)
{
	m_pChildClasses->removeRef(pClass);
}

bool KviKvsObjectClass::save(const QString &szFileName)
{
	if(!m_pParentClass)return false;
	if(isBuiltin())return false;


	QString szBuffer;
	QString szParentName = m_pParentClass->name();

	KviQString::sprintf(szBuffer,
					"# Automatically generated KVS class dump\n\n" \
					"class(\"%Q\",\"%Q\")\n" \
					"{\n",
					&m_szName,&szParentName);
		
	KviPointerHashTableIterator<QString,KviKvsObjectFunctionHandler> it(*m_pFunctionHandlers);
	
	while(KviKvsObjectFunctionHandler * h = it.current())
	{
		if(h->isScriptHandler())
		{
			szBuffer += "	";
			if(h->flags() & KviKvsObjectFunctionHandler::Internal)
			szBuffer += "internal ";
			szBuffer += "function ";
			szBuffer += it.currentKey();
			szBuffer += "()\n";
			QString szCode = h->scriptHandlerCode();
			KviCommandFormatter::blockFromBuffer(szCode);
			KviCommandFormatter::indent(szCode);
			szBuffer += szCode;
			szBuffer += "\n";
		}
		++it;
	}
	
	szBuffer += "}\n";
	
	return KviFileUtils::writeFile(szFileName,szBuffer);
}
void KviKvsObjectClass::getFunctionCode(QString &szCode,KviKvsObjectFunctionHandler &h)
{
	szCode=h.scriptHandlerCode();
}
	


bool KviKvsObjectClass::load(const QString &szFileName)
{
	QString szBuffer;
	if(!KviFileUtils::readFile(szFileName,szBuffer,0xffffff))return false;
	return KviKvsScript::run(szBuffer,g_pActiveWindow);
}
