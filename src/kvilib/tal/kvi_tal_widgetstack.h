#ifndef _KVI_TAL_WIDGETSTACK_H_
#define _KVI_TAL_WIDGETSTACK_H_

//=============================================================================
//
//   File : kvi_tal_widgetstack.h
//   Creation date : Mon May 05 2008 11:25:08 by Alessandro Carbone
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2008 Alessandro Carbone (elfonol at gmail dot com)
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

#include <QStackedWidget>

class KVILIB_API KviTalWidgetStack : public QStackedWidget
{
	Q_OBJECT
public:
	KviTalWidgetStack(QWidget * pParent)
	: QStackedWidget(pParent) {};
	virtual ~KviTalWidgetStack() {};
};

#endif // _KVI_TAL_WIDGETSTACK_H_
