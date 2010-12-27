//=============================================================================
//
//   File : optw_generalopt.cpp
//   Creation date : Wed Nov 19 09:25:16 2005 GMT by Grifisx & Noldor
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2005-2008 Alessandro Carbone (elfonol at gmail dot com)
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

#include "optw_generalopt.h"

#include "kvi_settings.h"
#include "KviLocale.h"
#include "KviOptions.h"


KviGeneralOptOptionsWidget::KviGeneralOptOptionsWidget(QWidget * parent)
: KviOptionsWidget(parent)
{
	setObjectName("generalopt_options_widget");
	createLayout();

	addLabel(0,0,4,0,__tr2qs_ctx("This section contains the general client options<br>"\
		"<p>like<b> sound, mediafiles, URL handler </b> etc... " \
		"</p>","options"));
	addAdvancedButton(4,1,4,1);
}

KviGeneralOptOptionsWidget::~KviGeneralOptOptionsWidget()
{
}


#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "m_optw_generalopt.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
