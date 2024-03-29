//=============================================================================
//
//   File : OptionsWidget_irc.cpp
//   Creation date : Sun Dec  2 18:59:48 2001 GMT by Szymon Stefanek
//
//   This file is part of the KVIrc irc client distribution
//   Copyright (C) 2001-2010 Szymon Stefanek (pragma at kvirc dot net)
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
//   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//=============================================================================

#include "OptionsWidget_irc.h"

#include "kvi_settings.h"
#include "KviLocale.h"
#include "KviOptions.h"
#include "KviIrcMask.h"


OptionsWidget_irc::OptionsWidget_irc(QWidget * parent)
: KviOptionsWidget(parent)
{
	setObjectName("irc_options_widget");
}

OptionsWidget_irc::~OptionsWidget_irc()
{
}


OptionsWidget_ircGeneral::OptionsWidget_ircGeneral(QWidget * parent)
: KviOptionsWidget(parent)
{
	setObjectName("irc_options_widget");
	createLayout();
	KviBoolSelector *b = addBoolSelector(0,0,0,0,__tr2qs_ctx("Minimize console after successful login","options"),KviOption_boolMinimizeConsoleAfterConnect);

	mergeTip(b,__tr2qs_ctx("<center>This option will cause KVIrc to minimize the console window after successfully logging into a server.</center>","options"));

	b = addBoolSelector(0,1,0,1,__tr2qs_ctx("Show network name in console window list entry","options"),KviOption_boolShowNetworkNameForConsoleWindowListEntry);
	mergeTip(b,__tr2qs_ctx("<center>This option will cause KVIrc to show the network name as the console window list entry instead of the server name. This is nice to keep on unless your servers are not organized in networks or you often connect to two servers of the same network.</center>","options"));

	addRowSpacer(0,2,0,2);
}

OptionsWidget_ircGeneral::~OptionsWidget_ircGeneral()
{
}


OptionsWidget_ircAdvanced::OptionsWidget_ircAdvanced(QWidget * parent)
: KviOptionsWidget(parent)
{
	setObjectName("irc_advanced_options_widget");
	createLayout();
	KviBoolSelector *b = addBoolSelector(0,0,0,0,__tr2qs_ctx("Force immediate quit","options"),KviOption_boolForceBrutalQuit);
	mergeTip(b,
		__tr2qs_ctx("<center>This option causes KVIrc to close " \
		"the connection immediately after sending the QUIT message.<br>" \
		"When this option is disabled, KVIrc will wait for the server " \
		"to close the connection.<br>" \
		"Note that if you use this, your QUIT message may be not displayed.</center>","options"));
	addBoolSelector(0,1,0,1,__tr2qs_ctx("Prepend gender info to realname","options"),KviOption_boolPrependGenderInfoToRealname);
	addBoolSelector(0,3,0,3,__tr2qs_ctx("Prepend smart nick color info to realname","options"),KviOption_boolPrependNickColorInfoToRealname);
	addRowSpacer(0,4,0,4);
}

OptionsWidget_ircAdvanced::~OptionsWidget_ircAdvanced()
{
}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "m_OptionsWidget_irc.moc"
#endif //COMPILE_USE_STANDALONE_MOC_SOURCES
