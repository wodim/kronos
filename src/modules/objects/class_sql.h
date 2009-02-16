#ifndef _CLASS_SQLITE_H_
#define _CLASS_SQLITE_H_
//=============================================================================
//
//   File : class_sqlite.h
//   Creation date : Wed Gen 28 2009 21:07:55 by Alessandro Carbone
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2000-2008 Szymon Stefanek (pragma at kvirc dot net)
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
#include "kvi_string.h"
#include "kvi_pointerlist.h"
#include "kvi_kvs_variant.h"
#include "object_macros.h"
#include <QtSql>
#include <QHash>

class KviKvsObject_sql : public KviKvsObject
{
public:
        KVSO_DECLARE_OBJECT(KviKvsObject_sql)
protected:
        QSqlQuery *currentSQlQuery;
         QHash<QString,QSqlQuery *> connectionsDict;
    public:
        QSqlQuery & getQuery()
        {
            //if (currentSQlQuery) return *currentSQlQuery;
            return *currentSQlQuery;
        };
        bool setConnection(KviKvsObjectFunctionCall *c);
        bool queryResultsSize(KviKvsObjectFunctionCall *c);
        bool queryNext(KviKvsObjectFunctionCall *c);
        bool queryPrevious(KviKvsObjectFunctionCall *c);
        bool queryFirst(KviKvsObjectFunctionCall *c);
        bool queryLast(KviKvsObjectFunctionCall *c);
        bool queryExec(KviKvsObjectFunctionCall *c);
        bool queryRecord(KviKvsObjectFunctionCall *c);
        bool queryPrepare(KviKvsObjectFunctionCall *c);
        bool queryBindValue(KviKvsObjectFunctionCall *c);
        bool setCurrentQuery(KviKvsObjectFunctionCall *c);
        bool currentQuery(KviKvsObjectFunctionCall *c);
        bool queryFinish(KviKvsObjectFunctionCall *c);
        bool queryInit(KviKvsObjectFunctionCall *c);
        bool closeConnection(KviKvsObjectFunctionCall *c);
        bool lastError(KviKvsObjectFunctionCall *c);
};


#endif //_CLASS_SQLITE_H_