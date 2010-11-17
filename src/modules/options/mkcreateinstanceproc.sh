#!/bin/sh

#################################################################################
#
# Generator for instances.h and instances.cpp
#
# This file is part of the KVirc irc client distribution
# Copyright (C) 2001-2008 Szymon Stefanek (pragma at kvirc dot net)
#
#################################################################################


echo "" > instances.h

#################################################################################
cat >> instances.h <<EOF

#ifndef __OPTIONS_INSTANCES_H__
#define __OPTIONS_INSTANCES_H__

//=============================================================================
//
//   File : instances.h
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2001-2008 Szymon Stefanek (pragma at kvirc dot net)
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

//
// Instance creation routines for the KVIrc options module
// DO NOT EDIT THIS FILE!! ALL CHANGES WILL BE LOST!!
// This file is automatically generated by mkcreateinstanceproc.sh
// so any change should go in that script
//

#include "kvi_optionswidget.h"
#include "kvi_module.h"
#include "kvi_pointerlist.h"
#include "kvi_qstring.h"

typedef struct _KviOptionsWidgetInstanceEntry KviOptionsWidgetInstanceEntry;


typedef struct _KviOptionsWidgetInstanceEntry
{
	KviOptionsWidget                          * (*createProc)(QWidget *);
	KviOptionsWidget                          * pWidget;   // singleton
	int                                         iIcon;
	QString                                     szName;
	QString                                     szNameNoLocale;
	const char                                * szClassName;
	int                                         iPriority;
	QString                                     szKeywords;
	QString                                     szKeywordsNoLocale;
	QString                                     szGroup;
	bool                                        bIsContainer;
	bool                                        bIsNotContained;
	KviPointerList<KviOptionsWidgetInstanceEntry> * pChildList;
	bool                                        bDoInsert; // a helper for KviOptionsDialog::fillListView()
} KviOptionsWidgetInstanceEntry;


class KviOptionsInstanceManager : public QObject
{
	Q_OBJECT
public:
	KviOptionsInstanceManager();
	virtual ~KviOptionsInstanceManager();
protected:
	KviPointerList<KviOptionsWidgetInstanceEntry> *  m_pInstanceTree;
public:
	KviPointerList<KviOptionsWidgetInstanceEntry> * instanceEntryTree(){ return m_pInstanceTree; };
	KviOptionsWidget * getInstance(KviOptionsWidgetInstanceEntry * e,QWidget * par);
	KviOptionsWidgetInstanceEntry * findInstanceEntry(const char * clName);
	void cleanup(KviModule * m);
protected:
	KviOptionsWidgetInstanceEntry * findInstanceEntry(const char * clName,KviPointerList<KviOptionsWidgetInstanceEntry> * l);
	KviOptionsWidgetInstanceEntry * findInstanceEntry(const QObject * ptr,KviPointerList<KviOptionsWidgetInstanceEntry> * l);
	void deleteInstanceTree(KviPointerList<KviOptionsWidgetInstanceEntry> * l);
protected slots:
	void widgetDestroyed();
};

#endif //__OPTIONS_INSTANCES_H__

EOF
######################################################################################################

CLASS_LIST=`grep -h "[ ]*\:[ ]*public[ ]*KviOptionsWidget" optw_*.h | sed -e 's/[ ]*\:[ ]*public[ ]*KviOptionsWidget'//g | sed -e 's/[ 	]*class[ ]*//g'`


TARGET="instances.cpp"

echo "" > $TARGET

#################################################################################
cat >> instances.cpp <<EOF

//=============================================================================
//
//   File : instances.cpp
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2001-2008 Szymon Stefanek (stefanek@tin.it)
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

//
// Instance creation routines for the KVIrc options module
// DO NOT EDIT THIS FILE!! ALL CHANGES WILL BE LOST!!
// This file is automatically generated by mkcreateinstanceproc.sh
// so any change should go in that script
//

EOF
#################################################################################


for afile in optw_*.h; do
	echo "#include \"$afile\"" >> $TARGET
done

#################################################################################
cat >> instances.cpp <<EOF

#include "kvi_locale.h"
#include "kvi_iconmanager.h"
#include "instances.h"

int g_iOptionWidgetInstances = 0;

EOF
#################################################################################


for aclass in $CLASS_LIST; do
	echo -n "KviOptionsWidget * class$aclass" >> $TARGET
		echo "_createInstanceProc(QWidget * parent)" >> $TARGET
	echo "{" >> $TARGET
	echo "	return new $aclass(parent);" >> $TARGET
	echo "}" >> $TARGET
	echo "" >> $TARGET
done

#################################################################################
cat >> instances.cpp <<EOF


EOF
#################################################################################

for aclass in $CLASS_LIST; do
	echo "static const char * g_szName_$aclass = KVI_OPTIONS_WIDGET_NAME_$aclass;" >> $TARGET
	echo "static const char * g_szClassName_$aclass = \"$aclass\";" >> $TARGET
done

#################################################################################
cat >> instances.cpp <<EOF

KviOptionsInstanceManager::KviOptionsInstanceManager()
: QObject(0)
{

	//qDebug("Instantiating");
	// Create the global widget dict : case sensitive, do not copy keys
	m_pInstanceTree = new KviPointerList<KviOptionsWidgetInstanceEntry>;
	m_pInstanceTree->setAutoDelete(true);

EOF
#################################################################################

CLASSDIR=".classtmp"

mkdir -p $CLASSDIR

rm -f $CLASSDIR/*

for aclass in $CLASS_LIST; do
	PARENT=`grep -h "KVI_OPTIONS_WIDGET_PARENT_$aclass" optw_*.h | sed -e 's/#define//g' | sed -e s/KVI_OPTIONS_WIDGET_PARENT_$aclass//g | sed -e 's/[ ]*//g'`
	if [ -z "$PARENT" ]; then
		PARENT="NOPARENT"
	fi
	echo $aclass >> $CLASSDIR/$PARENT
done

SPACES=""

printclass()
{
	echo "" >> $TARGET
	found=
	for anum in $DECLARED; do
		if [ "$anum" = "$1" ]; then
			found="yes"
			break
		fi
	done
	if [ -z "$found" ]; then
		echo "$3	KviOptionsWidgetInstanceEntry * e$1;" >> $TARGET
		DECLARED="$DECLARED $1"
	fi
	echo "$3	e$1 = new KviOptionsWidgetInstanceEntry;"  >> $TARGET
	echo -n "$3	e$1->createProc = &class$2"  >> $TARGET
		echo "_createInstanceProc;"  >> $TARGET
	echo "$3	e$1->pWidget = 0;" >> $TARGET
	echo "$3	e$1->szClassName = g_szClassName_$2;" >> $TARGET
	echo "$3	e$1->iIcon = KVI_OPTIONS_WIDGET_ICON_$2;" >> $TARGET

	echo "$3	#ifdef KVI_OPTIONS_WIDGET_PRIORITY_$2" >> $TARGET
	echo "$3	e$1->iPriority = KVI_OPTIONS_WIDGET_PRIORITY_$2;" >> $TARGET
	echo "$3	#else" >> $TARGET
	echo "$3	e$1->iPriority = 0;" >> $TARGET
	echo "$3	#endif" >> $TARGET

	echo "$3	#ifdef KVI_OPTIONS_WIDGET_KEYWORDS_$2" >> $TARGET
	echo "$3	e$1->szKeywordsNoLocale = KVI_OPTIONS_WIDGET_KEYWORDS_$2;" >> $TARGET
	echo "$3	e$1->szKeywords = __tr2qs_ctx_no_xgettext(e$1->szKeywordsNoLocale.toUtf8().data(),\"options\");;" >> $TARGET
	echo "$3	#endif" >> $TARGET

	echo "$3	#ifdef KVI_OPTIONS_WIDGET_GROUP_$2" >> $TARGET
	echo "$3	e$1->szGroup = KVI_OPTIONS_WIDGET_GROUP_$2;" >> $TARGET
	echo "$3	#else" >> $TARGET
	echo "$3	e$1->szGroup = \"general\";" >> $TARGET
	echo "$3	#endif" >> $TARGET

	echo "$3	#ifdef KVI_OPTIONS_WIDGET_CONTAINER_$2" >> $TARGET
	echo "$3	e$1->bIsContainer = KVI_OPTIONS_WIDGET_CONTAINER_$2;" >> $TARGET
	echo "$3	#else" >> $TARGET
	echo "$3	e$1->bIsContainer = false;" >> $TARGET
	echo "$3	#endif" >> $TARGET

	echo "$3	#ifdef KVI_OPTIONS_WIDGET_NOTCONTAINED_$2" >> $TARGET
	echo "$3	e$1->bIsNotContained = KVI_OPTIONS_WIDGET_NOTCONTAINED_$2;" >> $TARGET
	echo "$3	#else" >> $TARGET
	echo "$3	e$1->bIsNotContained = false;" >> $TARGET
	echo "$3	#endif" >> $TARGET

	echo "$3	e$1->szNameNoLocale = g_szName_$2;" >> $TARGET
	echo "$3	e$1->szName = __tr2qs_ctx_no_xgettext(g_szName_$2,\"options\");" >> $TARGET

	if [ "$1" = "0" ]; then
		echo "$3	m_pInstanceTree->append(e$1);" >> $TARGET
	else
		VAL=`expr $1 - 1`
		echo "$3	e$VAL->pChildList->append(e$1);" >> $TARGET
	fi
}

addchildren()
{
	if [ -f "$CLASSDIR/$2" ]; then
		LEVEL=`cat $CLASSDIR/$2`
		for achild in $LEVEL; do
			printclass $1 $achild "$3"
			if [ -f "$CLASSDIR/$achild" ]; then
				echo "" >> $TARGET
				echo "$3	e$1->pChildList = new KviPointerList<KviOptionsWidgetInstanceEntry>;" >> $TARGET
				echo "$3	e$1->pChildList->setAutoDelete(true);" >> $TARGET
				NEXTLEVEL=`expr $1 + 1`
				addchildren $NEXTLEVEL $achild "$3	"
			else
				echo "$3	e$1->pChildList = 0;" >> $TARGET
			fi
		done
	fi
}

addchildren 0 NOPARENT ""


rm -fr $CLASSDIR

#################################################################################
cat >> $TARGET <<EOF

}

void KviOptionsInstanceManager::deleteInstanceTree(KviPointerList<KviOptionsWidgetInstanceEntry> * l)
{
	if(l)
	{
		for(KviOptionsWidgetInstanceEntry * e = l->first();e;e = l->next())
		{
			if(e->pWidget)
			{
				if(e->pWidget->parent()->inherits("KviOptionsWidgetContainer"))
				{
					disconnect(e->pWidget,SIGNAL(destroyed()),this,SLOT(widgetDestroyed()));
					delete e->pWidget->parent();
					e->pWidget =  0;
				} else {
					qDebug("Ops...i have deleted the options dialog ?");
				}
			} //else qDebug("Clas %s has no widget",e->szName);
			if(e->pChildList)deleteInstanceTree(e->pChildList);
		}
		delete l;
	}
}


KviOptionsInstanceManager::~KviOptionsInstanceManager()
{
	if(m_pInstanceTree)
		qDebug("Ops...KviOptionsInstanceManager::cleanup() not called ?");
}

void KviOptionsInstanceManager::cleanup(KviModule *)
{
	deleteInstanceTree(m_pInstanceTree);
	m_pInstanceTree = 0;
}

void KviOptionsInstanceManager::widgetDestroyed()
{
	KviOptionsWidgetInstanceEntry * e = findInstanceEntry(sender(),m_pInstanceTree);
	if(e)
		e->pWidget = 0;
	if(g_iOptionWidgetInstances > 0)
		g_iOptionWidgetInstances--;

}

KviOptionsWidget * KviOptionsInstanceManager::getInstance(KviOptionsWidgetInstanceEntry * e,QWidget * par)
{
	if(!e)
		return NULL;

#if 0
	if(e->pWidget)
	{
		if(e->pWidget->parent() != par)
		{
			QWidget * oldPar = (QWidget *)e->pWidget->parent();
			e->pWidget->setParent(par);
			oldPar->deleteLater();
			e->pWidget=0;
		}
	}
#endif

	if(!(e->pWidget))
	{
		e->pWidget = e->createProc(par);
		g_iOptionWidgetInstances++;
		connect(e->pWidget,SIGNAL(destroyed()),this,SLOT(widgetDestroyed()));
	}

	if(e->pWidget->parent() != par)
	{
		QWidget * oldPar = (QWidget *)e->pWidget->parent();
		e->pWidget->setParent(par); //reparent(par,QPoint(0,0));
		if(oldPar->inherits("KviOptionsWidgetContainer"))
			delete oldPar;
		// else it's very likely a QStackedWidget, child of a KviOptionsWidget: don't delete
	}

	if(e->bIsContainer)
	{
		// need to create the container structure!
		e->pWidget->createTabbedPage();
		if(e->pChildList)
		{
			KviPointerList<KviOptionsWidgetInstanceEntry> tmp;
			tmp.setAutoDelete(false);

			for(KviOptionsWidgetInstanceEntry * e2 = e->pChildList->first();e2;e2 = e->pChildList->next())
			{
				// add only non containers and widgets not explicitly marked as noncontained
				if((!e2->bIsContainer) && (!e2->bIsNotContained))
				{
					KviOptionsWidgetInstanceEntry * ee = tmp.first();
					int idx = 0;
					while(ee)
					{
						if(ee->iPriority >= e2->iPriority)break;
						idx++;
						ee = tmp.next();
					}
					tmp.insert(idx,e2);
				}
			}

			for(KviOptionsWidgetInstanceEntry * e3 = tmp.last();e3;e3 = tmp.prev())
			{
				KviOptionsWidget * ow = getInstance(e3,e->pWidget->tabWidget());
				e->pWidget->addOptionsWidget(e3->szName,*(g_pIconManager->getSmallIcon(e3->iIcon)),ow);
			}
		}
	}
	return e->pWidget;
}

KviOptionsWidgetInstanceEntry * KviOptionsInstanceManager::findInstanceEntry(const QObject * ptr,KviPointerList<KviOptionsWidgetInstanceEntry> * l)
{
	if(l)
	{
		for(KviOptionsWidgetInstanceEntry * e = l->first();e;e = l->next())
		{
			if(ptr == e->pWidget)return e;
			if(e->pChildList)
			{
				KviOptionsWidgetInstanceEntry * e2 = findInstanceEntry(ptr,e->pChildList);
				if(e2)return e2;
			}
		}
	}
	return 0;
}

KviOptionsWidgetInstanceEntry * KviOptionsInstanceManager::findInstanceEntry(const char * clName,KviPointerList<KviOptionsWidgetInstanceEntry> * l)
{
	if(l)
	{
		for(KviOptionsWidgetInstanceEntry * e = l->first();e;e = l->next())
		{
			if(kvi_strEqualCI(e->szClassName,clName))return e;
			if(e->pChildList)
			{
				KviOptionsWidgetInstanceEntry * e2 = findInstanceEntry(clName,e->pChildList);
				if(e2)return e2;
			}
		}
	}
	return 0;
}

KviOptionsWidgetInstanceEntry * KviOptionsInstanceManager::findInstanceEntry(const char * clName)
{
	return findInstanceEntry(clName,m_pInstanceTree);
}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
	#include "m_instances.moc"
#endif

EOF
#################################################################################

