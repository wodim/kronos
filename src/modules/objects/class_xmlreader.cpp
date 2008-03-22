//=============================================================================
//
//   File : class_xmlreader.cpp
//   Created on Tue 27 Dec 2005 00:14:09 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC Client distribution
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

#include "class_xmlreader.h"

#include "kvi_locale.h"

#include "kvi_kvs_variantlist.h"
#include "kvi_kvs_hash.h"


/*
	@doc: xmlreader
	@keyterms:
		xml
	@title:
		xmlreader class
	@type:
		class
	@short:
		A simple xml document parser
	@inherits:
		[class]object[/class]
	@description:
		This class implements a really simple xml document parser.
		You will usually derive your own class from this one and reimplement
		some of the events that it triggers.
		You will typically reimplement [classfnc:xmlparser]onElementStart[/classfnc]()
		and [classfnc:xmlparser]onElementEnd[/classfnc]() that will be called
		during the execution of [classfnc:xmlparser]$parse[/classfnc]() in an order
		reflecting the order of elements in the parsed document.
	@functions:
		!fn: <boolean> $parse(<xml_data:string>)
		Call this function to parse a string that contains an XML document.
		A typical call for this method will look like:
		[example]
			%x = [fnc]$new[/fnc](xmlparser)
			%x->$parse([fnc]$file.read[/fnc]("/home/somefile.xml"))
		[/example]
		During the call the <xml_data> string will be parsed and the
		relevant on* events (see below) will be triggered.
		$parse will return $true when the parsing terminates succesfully
		or $false if it aborts for some reason (unrecoverable error
		in the document, user abort etc...).
		If this function return $false then you can call $lastError() to
		obtain a descriptive error message.
		
		!fn: <string> $lastError()
		Returns the last error occured inside the parser.
		You will typically call this function when $parse() above returns $false.
	
		!fn: <boolean> $onDocumentStart()
		This function is called when the document parsing starts.
		You can reimplement it in order to handle this notification.
		You should return $true if you want document parsing to continue
		and $false if you want it to be aborted.
		The default implementation does nothing besides returning $true.

		!fn: <boolean> $onDocumentEnd()
		This function is called when the document parsing terminates succesfully.
		You can reimplement it in order to handle this notification.
		You should return $true if you want document parsing to continue
		and $false if you want it to be aborted.
		The default implementation does nothing besides returning $true.
		
		!fn: <boolean> $onElementStart(<qualified_name:string>,<attributes:hash>,<namespace:string>,<local_name:string>)
		This function is called when an element opening tag is encountered.
		The <qualified_name> of the tag is passed as the first parameter.
		The <attributes> are passed in the form of a hash with attribute
		values indexed by their names. 
		When the <qualified_name> contains a namespace then it is also reported
		in the splitted <namespace> <local_name> pair.
		You should return $true if you want document parsing to continue
		and $false if you want it to be aborted.
		The default implementation does nothing besides returning $true.
		
		!fn: <boolean> $onElementEnd(<qualified_name:string>,<namespace:string>,<local_name:string>)
		This function is called when an element closing tag is encountered.
		The <qualified_name> of the tag is passed as the first parameter.
		When the <qualified_name> contains a namespace then it is also reported
		in the splitted <namespace> <local_name> pair.
		You should return $true if you want document parsing to continue
		and $false if you want it to be aborted.
		The default implementation does nothing besides returning $true.

		!fn: <boolean> $onText($0 = <text:string>)
		This function is called when a chunk of text is encountered inside the document.
		The parsed <text> chunk is passed as the first parameter.
		You should return $true if you want document parsing to continue
		and $false if you want it to be aborted.
		The default implementation does nothing besides returning $true.
		
		!fn: <boolean> $onWarning(<message:string>)
		This function is called when the parser generates a recoverable error.
		The error <message> is passed as the first parameter.
		You should return $true if you want document parsing to continue
		and $false if you want it to be aborted.
		The default implementation does nothing besides returning $true.
		
		!fn: <boolean> $onError(<message:string>)
		This function is called when the parser generates an unrecoverable error.
		The error <message> is passed as the first parameter.
		The document parsing can't continue.
		The default implementation does nothing besides returning $true.
*/


#ifndef QT_NO_XML

#include <qxml.h>

class KviXmlHandler : public QXmlDefaultHandler
{
protected:
	KviKvsObject_xmlreader * m_pReader;
	QString m_szErrorString;
public:
	KviXmlHandler(KviKvsObject_xmlreader * pReader)
	{
		m_pReader = pReader;
	}
	~KviXmlHandler()
	{
	}
private:
	bool kvsCodeFailure()
	{
		m_szErrorString = __tr2qs("Error in KVS class implementation: processing aborted");
		return false;
	}
	bool kvsCodeAbort()
	{
		m_szErrorString = __tr2qs("Processing aborted");
		return false;
	}
	void decodeException(QString &szMsg,bool bError,const QXmlParseException &exception)
	{
		if(bError)
			KviQString::sprintf(szMsg,__tr2qs("Error near line %d, column %d"),exception.lineNumber(),exception.columnNumber());
		else
			KviQString::sprintf(szMsg,__tr2qs("Warning near line %d, column %d"),exception.lineNumber(),exception.columnNumber());
		szMsg += ": ";
		szMsg += exception.message();
	}
	bool handleKvsCallReturnValue(KviKvsVariant * pRetVal)
	{
		if(!pRetVal->asBoolean())return kvsCodeAbort();
		return true;
	}
public:
	virtual bool startDocument()
	{
		KviKvsVariant ret;
		if(!m_pReader->callFunction(m_pReader,"onDocumentStart",&ret))
			return kvsCodeFailure();
		return handleKvsCallReturnValue(&ret);
	}
	
	virtual bool endDocument()
	{
		KviKvsVariant ret;
		if(!m_pReader->callFunction(m_pReader,"onDocumentEnd",&ret))
			return kvsCodeFailure();
		return handleKvsCallReturnValue(&ret);
	}
	
	virtual bool startElement(const QString &szNamespaceUri,const QString &szLocalName,const QString &szQualifiedName,const QXmlAttributes &attrs)
	{
		KviKvsVariant ret;
		KviKvsVariantList par;
		par.setAutoDelete(true);
		par.append(new KviKvsVariant(szQualifiedName));
		KviKvsHash * pHash = new KviKvsHash();
		par.append(new KviKvsVariant(pHash));
		par.append(new KviKvsVariant(szNamespaceUri));
		par.append(new KviKvsVariant(szLocalName));
		int c = attrs.count();
		for(int i=0;i<c;i++)
			pHash->set(attrs.qName(i),new KviKvsVariant(attrs.value(i)));
		if(!m_pReader->callFunction(m_pReader,"onElementStart",&ret,&par))
			return kvsCodeFailure();
		return handleKvsCallReturnValue(&ret);
	}
	
	virtual bool endElement(const QString &szNamespaceUri,const QString &szLocalName,const QString &szQualifiedName)
	{
		KviKvsVariant ret;
		KviKvsVariantList par;
		par.setAutoDelete(true);
		par.append(new KviKvsVariant(szQualifiedName));
		par.append(new KviKvsVariant(szNamespaceUri));
		par.append(new KviKvsVariant(szLocalName));
		if(!m_pReader->callFunction(m_pReader,"onElementEnd",&ret,&par))
			return kvsCodeFailure();
		return handleKvsCallReturnValue(&ret);
	}
	
	virtual bool characters(const QString &szChars)
	{
		KviKvsVariant ret;
		KviKvsVariantList par;
		par.setAutoDelete(true);
		par.append(new KviKvsVariant(szChars));
		if(!m_pReader->callFunction(m_pReader,"onText",&ret,&par))
			return kvsCodeFailure();
		return handleKvsCallReturnValue(&ret);
	}

	virtual bool warning(const QXmlParseException &exception)
	{
		// recoverable
		QString szMsg;
		decodeException(szMsg,false,exception);

		KviKvsVariant ret;
		KviKvsVariantList par;
		par.setAutoDelete(true);
		par.append(new KviKvsVariant(szMsg));
		if(!m_pReader->callFunction(m_pReader,"onWarning",&ret,&par))
			return kvsCodeFailure();
		return handleKvsCallReturnValue(&ret);
	}

	virtual bool error(const QXmlParseException &exception)
	{
		// recoverable
		QString szMsg;
		decodeException(szMsg,false,exception);

		KviKvsVariant ret;
		KviKvsVariantList par;
		par.setAutoDelete(true);
		par.append(new KviKvsVariant(szMsg));
		if(!m_pReader->callFunction(m_pReader,"onWarning",&ret,&par))
			return kvsCodeFailure();
		return handleKvsCallReturnValue(&ret);
	}

	virtual bool fatalError(const QXmlParseException &exception)
	{
		QString szMsg;
		decodeException(szMsg,true,exception);
		m_pReader->fatalError(szMsg);
		return true;
	}
	
	virtual QString errorString()
	{
		return m_szErrorString;
	}
};

#endif // !QT_NO_XML


KVSO_BEGIN_REGISTERCLASS(KviKvsObject_xmlreader,"xmlreader","object")
	KVSO_REGISTER_HANDLER(KviKvsObject_xmlreader,"lastError",function_lastError)
	KVSO_REGISTER_HANDLER(KviKvsObject_xmlreader,"parse",function_parse)

	KVSO_REGISTER_STANDARD_TRUERETURN_HANDLER(KviKvsObject_xmlreader,"onDocumentStart")
	KVSO_REGISTER_STANDARD_TRUERETURN_HANDLER(KviKvsObject_xmlreader,"onDocumentEnd")
	KVSO_REGISTER_STANDARD_TRUERETURN_HANDLER(KviKvsObject_xmlreader,"onElementStart")
	KVSO_REGISTER_STANDARD_TRUERETURN_HANDLER(KviKvsObject_xmlreader,"onElementEnd")
	KVSO_REGISTER_STANDARD_TRUERETURN_HANDLER(KviKvsObject_xmlreader,"onText")
	KVSO_REGISTER_STANDARD_TRUERETURN_HANDLER(KviKvsObject_xmlreader,"onWarning")
	KVSO_REGISTER_STANDARD_TRUERETURN_HANDLER(KviKvsObject_xmlreader,"onError")
KVSO_END_REGISTERCLASS(KviKvsObject_xmlreader)

KVSO_BEGIN_CONSTRUCTOR(KviKvsObject_xmlreader,KviKvsObject)
KVSO_END_CONSTRUCTOR(KviKvsObject_xmlreader)

KVSO_BEGIN_DESTRUCTOR(KviKvsObject_xmlreader)
KVSO_END_DESTRUCTOR(KviKvsObject_xmlreader)

void KviKvsObject_xmlreader::fatalError(const QString &szError)
{
	m_szLastError = szError;

	KviKvsVariantList vArgs;
	vArgs.append(new KviKvsVariant(m_szLastError));
	callFunction(this,"onError",&vArgs);
}

bool KviKvsObject_xmlreader::function_parse(KviKvsObjectFunctionCall *c)
{
	QString szString;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("string",KVS_PT_STRING,0,szString)
	KVSO_PARAMETERS_END(c)

#ifdef QT_NO_XML
	fatalError(__tr2qs("XML support not available in the Qt library"));
	c->returnValue()->setBoolean(false);
#else
	m_szLastError = "";
	KviXmlHandler handler(this);
	QXmlInputSource source;
	source.setData(szString);
	//debug("PARSING(%s) LEN(%d)",szString.utf8().data(),szString.utf8().length());
	QXmlSimpleReader reader;
	reader.setContentHandler(&handler);
	reader.setErrorHandler(&handler);
	c->returnValue()->setBoolean(reader.parse(source));
#endif
	return true;
}

bool KviKvsObject_xmlreader::function_lastError(KviKvsObjectFunctionCall *c)
{
	c->returnValue()->setString(m_szLastError);
	return true;
}

#include "m_class_xmlreader.moc"
