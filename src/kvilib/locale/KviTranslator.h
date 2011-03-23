#ifndef _KviTranslator_h_
#define _KviTranslator_h_
//=============================================================================
//
//   File : KviTranslator.h
//   Creation date : Wed 23 Mar 2011 03:32:12
//   Originally in KviLocale.h
//      Created on: Sat Jan 16 1999 18:15:01 by Szymon Stefanek
//
//   This file is part of the KVIrc irc client distribution
//   Copyright (C) 1999-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "kvi_settings.h"

#include <QTranslator>
#include <QString>


///
/// \class KviTranslator
/// \brief The KviTranslator class
///
/// This class...
///
class KVILIB_API KviTranslator : public QTranslator
{
	Q_OBJECT
public:

	///
	/// Creates an instance of KviTranslator
	///
	KviTranslator(QObject * parent,const char * name);

	///
	/// Destroys the instance of KviTranslator
	/// and frees all the relevant resources
	///
	virtual ~KviTranslator();

public:
	virtual QString translate(const char * context,const char * message,const char * comment) const;


}; // class KviTranslator


#endif //!_KviTranslator_h_
