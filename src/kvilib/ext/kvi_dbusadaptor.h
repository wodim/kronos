#ifndef KVI_DBUSADAPTOR_H
#define KVI_DBUSADAPTOR_H
//=============================================================================
//
//   File : kvi_dbusadaptor.h
//   Creation date : Thu May 08 2008 21:41:45 by Voker57
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2008 Voker57 (voker57 at gmail dot com)
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

#ifdef COMPILE_DBUS_SUPPORT
	#include <QDBusAbstractAdaptor>
	#include <QDBusInterface>
	#include <QObject>

	class KVILIB_API KviDbusAdaptor: public QDBusAbstractAdaptor
	{
		Q_OBJECT
		Q_CLASSINFO("KVIrc D-Bus Interface", "org.kvirc.KVIrc")

	public:
		KviDbusAdaptor(QObject * obj);
	};
#endif // COMPILE_DBUS_SUPPORT

#endif
