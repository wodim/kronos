//=============================================================================
//
//   File : libkvistr.cpp
//   Creation date : Wed Jan 02 2007 03:04:12 GMT by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2007 Szymon Stefanek (pragma at kvirc dot net)
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

//#warning: FIXME: Incomplete documentation ('seealso', 'example', etc)

#include "kvi_module.h"
#include "kvi_locale.h"
#include "kvi_qstring.h"
#include "kvi_window.h"
#include "kvi_frame.h"
#include "kvi_console.h"
#include "kvi_app.h"

#include "kvi_ircserver.h"
#include "kvi_irccontext.h"
#include "kvi_ircconnection.h"
#include "kvi_ircconnectionserverinfo.h"
#include "kvi_ircconnectionuserinfo.h"
#include "kvi_ircconnectiontarget.h"

#define GET_CONSOLE_FROM_STANDARD_PARAMS \
	kvs_int_t iContextId; \
	KVSM_PARAMETERS_BEGIN(c) \
		KVSM_PARAMETER("irc_context_id",KVS_PT_UINT,KVS_PF_OPTIONAL,iContextId) \
	KVSM_PARAMETERS_END(c) \
	KviConsole * pConsole = NULL; \
	if(c->parameterCount() > 0) \
		pConsole = g_pApp->findConsole(iContextId); \
	else \
		pConsole = c->window()->console();

#define GET_CONNECTION_FROM_STANDARD_PARAMS \
	GET_CONSOLE_FROM_STANDARD_PARAMS \
	KviIrcConnection * pConnection = NULL; \
	if(pConsole) \
		pConnection = pConsole->context()->connection();

#define STANDARD_IRC_CONNECTION_TARGET_PARAMETER(_fncName,_setCall) \
	static bool _fncName(KviKvsModuleFunctionCall * c) \
	{ \
		GET_CONNECTION_FROM_STANDARD_PARAMS \
		if(pConnection) \
		{ \
			if(pConnection->target()) \
			{ \
				_setCall; \
				return true; \
			} \
		} \
		c->returnValue()->setNothing(); \
		return true; \
	}



/*
	@doc: context.serverName
	@type:
		function
	@title:
		$context.serverName
	@short:
		Returns the IRC server name of an IRC context
	@syntax:
		<string> $contex.serverName
		<string> $contex.serverName(<irc_context_id:uint>)
	@description:
		Returns the name of the IRC server for the specified irc context.
		If no irc_context_id is specified then the current irc_context is used.
		If the irc_context_id specification is not valid then this function
		returns nothing. If the specified IRC context is not currently connected
		then this function returns nothing.
	@seealso:
		$context.serverPort, $context.serverIpAddress, $context.serverPassword
*/

STANDARD_IRC_CONNECTION_TARGET_PARAMETER(
		context_kvs_fnc_serverName,
		c->returnValue()->setString(pConnection->target()->server()->hostName())
	)

/*
	@doc: context.serverIpAddress
	@type:
		function
	@title:
		$context.serverIpAddress
	@short:
		Returns the IRC server ip address of an IRC context
	@syntax:
		<string> $contex.serverIpAddress
		<string> $contex.serverIpAddress(<irc_context_id:uint>)
	@description:
		Returns the ip address of the IRC server for the specified irc context.
		If no irc_context_id is specified then the current irc_context is used.
		If the irc_context_id specification is not valid then this function
		returns nothing. If the specified IRC context is not currently connected
		then this function returns nothing.
	@seealso:
		$context.serverPort, $context.serverName, $context.serverPassword
*/

STANDARD_IRC_CONNECTION_TARGET_PARAMETER(
		context_kvs_fnc_serverIpAddress,
		c->returnValue()->setString(pConnection->target()->server()->ipAddress())
	)

/*
	@doc: context.serverPassword
	@type:
		function
	@title:
		$context.serverPassword
	@short:
		Returns the password used to login to the server of an IRC context
	@syntax:
		<string> $contex.serverPassword
		<string> $contex.serverPassword(<irc_context_id:uint>)
	@description:
		Returns the password used to login to the IRC server for the specified irc context.
		If no irc_context_id is specified then the current irc_context is used.
		If the irc_context_id specification is not valid then this function
		returns nothing. If the specified IRC context is not currently connected
		then this function returns nothing.
	@seealso:
		$context.serverName, $context.serverIpAddress, $context.serverPort
*/

STANDARD_IRC_CONNECTION_TARGET_PARAMETER(
		context_kvs_fnc_serverPassword,
		c->returnValue()->setString(pConnection->target()->server()->password())
	)


/*
	@doc: context.serverPort
	@type:
		function
	@title:
		$context.serverPort
	@short:
		Returns the port of the IRC server of an IRC context
	@syntax:
		<uint> $contex.serverPort
		<uint> $contex.serverPort(<irc_context_id:uint>)
	@description:
		Returns the port of the IRC server for the specified irc context.
		If no irc_context_id is specified then the current irc_context is used.
		If the irc_context_id specification is not valid then this function
		returns nothing. If the specified IRC context is not currently connected
		then this function returns nothing.
	@seealso:
		$context.serverName, $context.serverIpAddress
*/

STANDARD_IRC_CONNECTION_TARGET_PARAMETER(
		context_kvs_fnc_serverPort,
		c->returnValue()->setInteger(pConnection->target()->server()->port())
	)


/*
	@doc: context.state
	@type:
		function
	@title:
		$context.state
	@short:
		Returns the state of an IRC context
	@syntax:
		<string> $context.state
		<string> $context.state(<irc_context_id:uint>)
	@description:
		Returns a string describing the state of the specified irc context.
		The string will be either "idle","connecting","loggingin" or "connected".
		If no irc_context_id is specified then the current irc_context is used.
		If the irc_context_id specification is not valid then this function
		returns nothing.
	@seealso:
		$context.serverName, $context.serverIpAddress
*/

static bool context_kvs_fnc_state(KviKvsModuleFunctionCall * c)
{
	GET_CONSOLE_FROM_STANDARD_PARAMS

	if(pConsole)
	{
		switch(pConsole->ircContext()->state())
		{
			case KviIrcContext::Idle:
				c->returnValue()->setString(QString("idle"));
			break;
			case KviIrcContext::Connecting:
				c->returnValue()->setString(QString("connecting"));
			break;
			case KviIrcContext::LoggingIn:
				c->returnValue()->setString(QString("loggingin"));
			break;
			case KviIrcContext::Connected:
				c->returnValue()->setString(QString("connected"));
			break;
			default:
				c->returnValue()->setString(QString("unknown"));
			break;
		}
		return true;
	}

	c->returnValue()->setNothing();
	return true;
}


/*
	@doc: context.list
	@type:
		function
	@title:
		$context.list
	@short:
		Returns a list of existing IRC contexts
	@syntax:
		<array> $contex.list
	@description:
		Returns the array of currently existing IRC context identifiers.
	@seealso:
	@examples:
		Print the names of the currently connected servers
		[example]
			foreach(%ic,$context.list)
				echo "IRC Context" %ic ": " $context.serverName
		[/example]
*/

static bool context_kvs_fnc_list(KviKvsModuleFunctionCall * c)
{
	KviKvsArray * pArray = new KviKvsArray();

	KviPtrList<KviWindow> * pWinList = g_pFrame->windowList();
	int idx = 0;
	for(KviWindow * pWnd = pWinList->first();pWnd;pWnd = pWinList->next())
	{
		if(pWnd->type() == KVI_WINDOW_TYPE_CONSOLE)
		{
			pArray->set(idx,new KviKvsVariant((kvs_int_t)((KviConsole *)pWnd)->ircContextId()));
			idx++;
		}
	}

	c->returnValue()->setArray(pArray);
	return true;
}

static bool context_module_init(KviModule * m)
{
	KVSM_REGISTER_FUNCTION(m,"serverName",context_kvs_fnc_serverName);
	KVSM_REGISTER_FUNCTION(m,"serverIpAddress",context_kvs_fnc_serverIpAddress);
	KVSM_REGISTER_FUNCTION(m,"serverPort",context_kvs_fnc_serverPort);
	KVSM_REGISTER_FUNCTION(m,"serverPassword",context_kvs_fnc_serverPassword);
	KVSM_REGISTER_FUNCTION(m,"state",context_kvs_fnc_state);
	KVSM_REGISTER_FUNCTION(m,"list",context_kvs_fnc_list);

	return true;
}

static bool context_module_cleanup(KviModule *m)
{
	return true;
}

KVIRC_MODULE(
	"context",
	"1.0.0",
	"Copyright (C) 2007 Szymon Stefanek (pragma at kvirc dot net)",
	"Irc Context Related Functions",
	context_module_init,
	0,
	0,
	context_module_cleanup
)
