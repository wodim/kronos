#ifndef _OPTW_LAG_H_
#define _OPTW_LAG_H_
//=============================================================================
//
//   File : optw_lag.h
//   Creation date : Wed Cct 16 10:45:54 CEST 2002 by Juanjo �lvarez (juanjux@yahoo.es)
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2002 Juanjo �lvarez
//   Copyright (C) 2002-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "KviOptionsWidget.h"

#define KVI_OPTIONS_WIDGET_ICON_KviLagOptionsWidget KVI_SMALLICON_STATS
#define KVI_OPTIONS_WIDGET_NAME_KviLagOptionsWidget __tr2qs_no_lookup("Lag")
#define KVI_OPTIONS_WIDGET_PARENT_KviLagOptionsWidget KviToolsOptionsWidget
#define KVI_OPTIONS_WIDGET_KEYWORDS_KviLagOptionsWidget __tr2qs_no_lookup("time,ping")

class KviLagOptionsWidget : public KviOptionsWidget
{
	Q_OBJECT
public:
	KviLagOptionsWidget(QWidget * parent);
	~KviLagOptionsWidget();
};

#endif //_OPTW_LAG_H_
