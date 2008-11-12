//=============================================================================
//
//   File : class_treewidget.cpp
//   Creation date : Fri Jan 28 14:21:48 CEST 2005
//   by Tonino Imbesi(Grifisx) and Alessandro Carbone(Noldor)
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2005-2008 Tonino Imbesi(Grifisx) and Alessandro Carbone(Noldor)
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

#include "class_treewidget.h"

#include "kvi_error.h"
#include "kvi_debug.h"
#include "kvi_locale.h"
#include "kvi_iconmanager.h"
#include <QTreeWidget>


// FIXME: fix the doc (function QT4 renamed)
// OLD:
// itemRenamed signal
// itemRenamed event
// NEW:
// itemActivated signal
// itemActavatedEvent


#include <QEvent>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QHeaderView>
#include <QUrl>

/*
	@doc: treewidget
	@keyterms:
		treewidget widget class
	@title:
		treewidget class
	@type:
		class
	@short:
		A treewidget class
	@inherits:
		[class]widget[/class]
	@description:
		It can display and control a hierarchy of multi-column items, and provides the ability to add new items at any time.
		The items are added by creating children [class]treewidgetitem[/class] objects: simply allocating them with $new
		will add the items to the treewidget and simply deleting them will remove them.
		Allocating a [class]treewidgetitem[/class] item2 as a child of item1 will insert it to the same treewidget creating
		a subtree of items spannig from item1. The subtree can be opened or closed by a simple click either
		on the parent item or on the little plus sign on the side of it (when [classfnc:treewidget]$setRootIsDecorated[/classfnc]
		is set to $true. The treewidget can be in Single, Multi, Extended or NoSelection selection mode.
		In single selection mode there can be only one selected item at a time and the selected item is also
		the current item (this mode is the default). In Multi and Extended selection mode there can be multiple selected items
		and the current item is one of them. The difference between Multi and Extended is in the way
		that items can be selected by the mouse actions: experiment with the two modes :).
		The NoSelection mode has obviously no selection at all.
	@functions:
		!fn: $addColumn(<text_label:string> <width:unsigned integer>)
		Adds a width pixels wide column with the column header label to the list view.

		!fn: $setSorting(<column:integer>,<sort_order:string>)
		Sets the list view to be sorted by column in ascending order if sort_order is "ascending" or descending order if it is "descending".
		!fn: $setSortingEnabled(<bEnabled:boolean>)
		If <bEnabled> is true, user sorting is enabled for the tree. The default value is false.
		In order to avoid performance issues, it is recommended that sorting is enabled after inserting the items into the tree.
		!fn: $hideListViewHeader()
		Hide the treewidget column header.

		!fn: $showListViewHeader()
		Show the treewidget column header.

		!fn: <boolean> $isListViewHeaderVisible()
		Returns '1' if the treewidget header is currently visible. Otherwise this function returns '0'.

		!fn: $setAllColumnsShowFocus(<bAllColumnsShowFocus:boolean>)
		When the argument is $true, causes the treewidget to display the focus and selection
		by highlighting all the columns of the item. When the argument is $false then
		only the first column is selected/highlighted.

		!fn: $setSelectionMode(<mode:string>)
		Sets the selection mode for this treewidget. <mode> can be one of "Single","NoSelection","Multi" or "Extended".

		!fn: <treewidgetitem> $selectedItem()
		Returns the currently selected [class]treewidgetitem[/class] or $null if no items are selected.
		This function works only if the list view is in single selection mode.

		!fn: <treewidgetitem> $currentItem()
		Returns the current [class]treewidgetitem[/class] or $null if no item is current at the moment.

		!fn: <treewidgetitem> $firstChild()
		Returns the first child [class]treewidgetitem[/class] of this treewidget or $null if there are no items at all.

		!fn: $clickEvent(<item:object>)
		This function is called when the user clicks in the list view.
		In its argument the [class]treewidgetitem[/class] object clicked or 0 if the user didn't click on an item.[br]
		The default implementation emits the [classfnc]$clicked[/classfnc]() signal.

		!fn: $selectionChangedEvent(<item:object>)
		This event handle whenever the set of selected items has changed.
		The argument is the newly selected item if the treewidget is in single selection mode.[br]
		When the treewidget is in Multi or Extended selection mode then item is always $null.[br]
		The default implementation emits the [classfnc]$selectionChanged[/classfnc]() signal.

		!fn: $currentChangedEvent(<item:object>)
		This event are called whenever the current item has changed.
		In its argument is the newly selected item or 0 if the change made no item current.[br]
		The default implementation emits the [classfnc]$currentChanged[/classfnc]() signal.

		!fn: $returnPressedEvent(<item:object>)
		This function is called by the framework when the enter key is pressed.[br]
		In its arument the currrent item.[br]
		The default implementation emits the [classfnc]$returnPressed[/classfnc]() signal.

		!fn: $spacePressedEvent(<item:object>)
		This function is called by the framework when the space key is pressed.[br]
		In its arument the currrent item.[br]
		The default implementation emits the [classfnc]$spacePressed[/classfnc]() signal.

		!fn: $onItemEvent(<item:object)
		This event is called by framwork when the user moves the mouse cursor onto item.
		The default implementation emits the [classfnc]$onItem[/classfnc]() signal.

		!fn: $itemExpandedEvent(<item:object>)
		This event is called when an item has been expanded, i.e. when the children of item are shown.
		The default implementation emits the [classfnc]$expanded[/classfnc]() signal.

		!fn: $itemCollapsedEvent(<item:object>)
		This event is called when an item has been collapsed, i.e. when the children of item are hidden.
		The default implementation emits the [classfnc]$collapsed[/classfnc]() signal.

		!fn: $itemRenamedEvent(<item:object>,<col:integer>,<text:string>)
		This event is called when the item has been renamed in text, e.g. by in in-place renaming, in column col.[br]
		The default implementation emits the [classfnc]$itemRenamed[/classfnc]() signal.

		!fn: $rightButtonClickEvent(<item:object>,<x:integer>,>y:integer>)
		This signal is emitted when the right button is clicked.[br]
		The arguments are the relevant item (may be 0), the point in global coordinates and the relevant column (or -1 if the click was outside the list).
		The default implementation emits the [classfnc]$rightButtonClicked[/classfnc]() signal.
	@signals:
		!sg: $clicked()
		This signal is emitted by the default implementation of [classfnc]$clickEvent[/classfnc]().
		!sg: $selectionChanged()
		This signal is emitted by the default implementation of [classfnc]$selectionChangedEvent[/classfnc]().
		!sg: $currentChanged()
		This signal is emitted by the default implementation of [classfnc]$currentChangedEvent[/classfnc]().
		!sg: $returnPressed()
		This signal is emitted by the default implementation of [classfnc]$returnPressedEvent[/classfnc]().
		!sg: $spacePressed()
		This signal is emitted by the default implementation of [classfnc]$spacePredssedEvent[/classfnc]().
		!sg: $onItem()
		This signal is emitted by the default implementation of [classfnc]$onItemEvent[/classfnc]().
		!sg: $itemExpanded()
		This signal is emitted by the default implementation of [classfnc]$itemExpandedEvent[/classfnc]().
		!sg: $itemCollapsed()
		This signal is emitted by the default implementation of [classfnc]$itemCollapsedEvent[/classfnc]().
		!sg: $itemRenamed()
		This signal is emitted by the default implementation of [classfnc]$itemRenamedEvent[/classfnc]().
		!sg: $rightButtonClicked()
		This signal is emitted by the default implementation of [classfnc]$rightButtonClickedEvent[/classfnc]().


*/

//---------------------------------------------------------------------------------

KVSO_BEGIN_REGISTERCLASS(KviKvsObject_treewidget,"listview","widget")

	KVSO_REGISTER_HANDLER_NEW(KviKvsObject_treewidget,setHeaderLabels)
	KVSO_REGISTER_HANDLER_NEW(KviKvsObject_treewidget,addColumn)

	KVSO_REGISTER_HANDLER_NEW(KviKvsObject_treewidget,setColumnText)
	KVSO_REGISTER_HANDLER_NEW(KviKvsObject_treewidget,setColumnCount)

	KVSO_REGISTER_HANDLER_NEW(KviKvsObject_treewidget,setSorting)
	KVSO_REGISTER_HANDLER_NEW(KviKvsObject_treewidget,setSortingEnabled)
	KVSO_REGISTER_HANDLER_NEW(KviKvsObject_treewidget,setRootIsDecorated)
	KVSO_REGISTER_HANDLER_NEW(KviKvsObject_treewidget,setAllColumnsShowFocus)
	KVSO_REGISTER_HANDLER_NEW(KviKvsObject_treewidget,clear)
	//FIXME
	KVSO_REGISTER_HANDLER(KviKvsObject_treewidget,"selectedItem",selectedItems)//<--- remove this
	KVSO_REGISTER_HANDLER_NEW(KviKvsObject_treewidget,selectedItems)

	KVSO_REGISTER_HANDLER_NEW(KviKvsObject_treewidget,currentItem)
	KVSO_REGISTER_HANDLER_NEW(KviKvsObject_treewidget,setSelectionMode)
	KVSO_REGISTER_HANDLER_NEW(KviKvsObject_treewidget,listViewHeaderIsVisible)


	KVSO_REGISTER_HANDLER_NEW(KviKvsObject_treewidget,hideListViewHeader)
	KVSO_REGISTER_HANDLER_NEW(KviKvsObject_treewidget,showListViewHeader)
	KVSO_REGISTER_HANDLER_NEW(KviKvsObject_treewidget,firstChild)
	KVSO_REGISTER_HANDLER_NEW(KviKvsObject_treewidget,selectionChangedEvent);
	KVSO_REGISTER_HANDLER_NEW(KviKvsObject_treewidget,currentChangedEvent);

	// FIX-ME
	KVSO_REGISTER_HANDLER(KviKvsObject_treewidget,"returnPressedEvent",itemActivatedEvent);//<- remove me
	KVSO_REGISTER_HANDLER_NEW(KviKvsObject_treewidget,itemActivatedEvent);
	//

//	KVSO_REGISTER_HANDLER_NEW(KviKvsObject_treewidget,"spacePressedEvent",function_spacePressedEvent);
	KVSO_REGISTER_HANDLER_NEW(KviKvsObject_treewidget,onItemEvent);
	KVSO_REGISTER_HANDLER_NEW(KviKvsObject_treewidget,itemClickedEvent);
	KVSO_REGISTER_HANDLER_NEW(KviKvsObject_treewidget,itemExpandedEvent);
	KVSO_REGISTER_HANDLER_NEW(KviKvsObject_treewidget,itemCollapsedEvent);

	//FIXME
	KVSO_REGISTER_HANDLER(KviKvsObject_treewidget,"itemRenamedEvent",itemChangedEvent);//<- remove me
	KVSO_REGISTER_HANDLER_NEW(KviKvsObject_treewidget,itemChangedEvent);
	//

	KVSO_REGISTER_HANDLER(KviKvsObject_treewidget,"rightButtonClickedEvent",customContextMenuRequestedEvent);
	KVSO_REGISTER_HANDLER_NEW(KviKvsObject_treewidget,customContextMenuRequestedEvent);

	KVSO_REGISTER_STANDARD_NOTHINGRETURN_HANDLER(KviKvsObject_treewidget,"fileDroppedEvent")

KVSO_END_REGISTERCLASS(KviKvsObject_treewidget)

KVSO_BEGIN_CONSTRUCTOR(KviKvsObject_treewidget,KviKvsObject_widget)

KVSO_END_CONSTRUCTOR(KviKvsObject_treewidget)


KVSO_BEGIN_DESTRUCTOR(KviKvsObject_treewidget)

KVSO_END_CONSTRUCTOR(KviKvsObject_treewidget)

bool KviKvsObject_treewidget::init(KviKvsRunTimeContext *,KviKvsVariantList *)
{
	setObject(new KviKvsTreeWidget(parentScriptWidget(),getName().toUtf8().data(),this),true);

	// hack for compatibility with "old" addColumn method;
	((QTreeWidget*) widget())->setColumnCount(0);

	connect(widget(),SIGNAL(itemClicked(QTreeWidgetItem *,int)),this,SLOT(slotClicked(QTreeWidgetItem *,int)));
	connect(widget(),SIGNAL(itemSelectionChanged()),this,SLOT(slotSelectionChanged()));
	connect(widget(),SIGNAL(currentItemChanged(QTreeWidgetItem *,QTreeWidgetItem *)),this,SLOT(slotCurrentChanged(QTreeWidgetItem *,QTreeWidgetItem *)));
	connect(widget(),SIGNAL(itemActivated(QTreeWidgetItem *,int)),this,SLOT(slotItemActivated(QTreeWidgetItem *,int)));
	connect(widget(),SIGNAL(itemEntered(QTreeWidgetItem *,int)),this,SLOT(slotOnItemEntered(QTreeWidgetItem *,int)));
	connect(widget(),SIGNAL(itemExpanded(QTreeWidgetItem *)),this,SLOT(slotItemExpanded(QTreeWidgetItem *)));
	connect(widget(),SIGNAL(itemCollapsed(QTreeWidgetItem *)),this,SLOT(slotItemCollapsed(QTreeWidgetItem *)));
	//connect(widget(),SIGNAL(rightButtonClicked(QTreeWidgetItem *,const QPoint &,int)),this,SLOT(slotRightButtonClicked(QTreeWidgetItem *,const QPoint &,int)));
	connect(widget(),SIGNAL(itemChanged(QTreeWidgetItem *,int)),this,SLOT(slotItemChanged(QTreeWidgetItem *,int)));

	widget()->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(widget(),SIGNAL(customContextMenuRequested(const QPoint &)),this,SLOT(slotCustomContextMenuRequested(const QPoint &)));

	return true;
}

KVSO_CLASS_FUNCTION(treewidget,setHeaderLabels)
{
	QStringList columns;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("labels",KVS_PT_STRINGLIST,KVS_PF_OPTIONAL,columns)
	KVSO_PARAMETERS_END(c)
	if (widget()){
		((QTreeWidget *)object())->setHeaderLabels(columns);
	}
    return true;
}

KVSO_CLASS_FUNCTION(treewidget,setColumnText)
{
	if (!widget()) return true;
	QString szLabel;
	kvs_int_t iCol;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("column",KVS_PT_INT,0,iCol)
		KVSO_PARAMETER("label",KVS_PT_STRING,0,szLabel)
	KVSO_PARAMETERS_END(c)
	QTreeWidgetItem *header=((QTreeWidget *)widget())->headerItem();
	header->setText(iCol,szLabel);
	return true;
}

KVSO_CLASS_FUNCTION(treewidget,addColumn)
{
	if (!widget()) return true;
	QString szLabel;
	kvs_int_t iW;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("label",KVS_PT_STRING,0,szLabel)
		KVSO_PARAMETER("width",KVS_PT_INT,0,iW)
	KVSO_PARAMETERS_END(c)
	int col=((QTreeWidget *)widget())->columnCount();
	QTreeWidgetItem *header=((QTreeWidget *)widget())->headerItem();
	header->setText(col,szLabel);
	((QTreeWidget *)widget())->setColumnWidth(col,iW);
	col++;
	((QTreeWidget *)widget())->setColumnCount(col);
	//col++;
	return true;
}
/*
KVSO_CLASS_FUNCTION(treewidget,setAcceptDrops)
{
	bool bEnable;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("bEnable",KVS_PT_BOOLEAN,0,bEnable)
	KVSO_PARAMETERS_END(c)
	if (widget())
		((QTreeWidget *)object())->setAcceptDrops(bEnable);
	return true;
}
*/
KVSO_CLASS_FUNCTION(treewidget,clear)
{
	if (widget())
		((QTreeWidget *)object())->clear();
	return true;
}

KVSO_CLASS_FUNCTION(treewidget,selectedItems)
{
	if(widget())
	{
		QList<QTreeWidgetItem *> list=((QTreeWidget *)widget())->selectedItems();
		KviKvsArray * pArray = new KviKvsArray();
		c->returnValue()->setArray(pArray);
		for (int i=0;i<list.count();i++)
			pArray->set(i,new KviKvsVariant(KviKvsObject_treewidgetitem::itemToHandle((QTreeWidgetItem*)list.at(i))));
	}
	else
		c->returnValue()->setHObject((kvs_hobject_t)0);
	return true;
}

KVSO_CLASS_FUNCTION(treewidget,firstChild)
{
	if(widget())
		c->returnValue()->setHObject(KviKvsObject_treewidgetitem::itemToHandle((QTreeWidgetItem*)((QTreeWidget *)widget())->topLevelItem(0)));
	else
		c->returnValue()->setHObject((kvs_hobject_t)0);
	return true;
}

KVSO_CLASS_FUNCTION(treewidget,currentItem)
{
	if(widget())
		c->returnValue()->setHObject(KviKvsObject_treewidgetitem::itemToHandle((QTreeWidgetItem*)((QTreeWidget *)widget())->currentItem()));
	else
		c->returnValue()->setHObject((kvs_hobject_t)0);
	return true;
}
/*
KVSO_CLASS_FUNCTION(treewidget,setColumnText)
{
	kvs_int_t uCol;
	QString szText;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("column",KVS_PT_UNSIGNEDINTEGER,0,uCol)
		KVSO_PARAMETER("text",KVS_PT_STRING,0,szText)
		KVSO_PARAMETERS_END(c)
	if (widget())((QTreeWidget *)widget())->setColumnText(uCol,szText);
    return true;
}
*/
KVSO_CLASS_FUNCTION(treewidget,setColumnCount)
{
	kvs_int_t uCol;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("column",KVS_PT_UNSIGNEDINTEGER,0,uCol)
	KVSO_PARAMETERS_END(c)
	if (widget())((QTreeWidget *)widget())->setColumnCount(uCol);
	return true;
}

KVSO_CLASS_FUNCTION(treewidget,setSelectionMode)
{
	QString szMode;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("mode",KVS_PT_NONEMPTYSTRING,0,szMode)
		KVSO_PARAMETERS_END(c)
	if(!widget())return true;
	if(KviQString::equalCI(szMode,"NoSelection"))
		((QTreeWidget *)widget())->setSelectionMode(QAbstractItemView::NoSelection);
	else if(KviQString::equalCI(szMode,"Multi"))
		((QTreeWidget *)widget())->setSelectionMode(QTreeWidget::MultiSelection);
	else if(KviQString::equalCI(szMode,"Extended"))
		((QTreeWidget *)widget())->setSelectionMode(QTreeWidget::ExtendedSelection);
	else if(KviQString::equalCI(szMode,"Single"))
		((QTreeWidget *)widget())->setSelectionMode(QTreeWidget::SingleSelection);
	else c->warning(__tr2qs_ctx("Invalid selection mode '%Q'","objects"),&szMode);
	return true;
}

KVSO_CLASS_FUNCTION(treewidget,setSorting)
{
	kvs_int_t iCol;
	QString szOrder;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("column",KVS_PT_INT,0,iCol)
		KVSO_PARAMETER("sort_order",KVS_PT_STRING,0,szOrder)
	KVSO_PARAMETERS_END(c)
	if (!widget())return true;
	if (KviQString::equalCI(szOrder,"ascending")) ((QTreeWidget *)widget())->sortItems(iCol,Qt::AscendingOrder);
	else if (KviQString::equalCI(szOrder,"descending")) ((QTreeWidget *)widget())->sortItems(iCol,Qt::DescendingOrder);
	else c->warning(__tr2qs_ctx("Unknown 'Q' sort order: switching to ascending order","objects"),&szOrder);
	return true;
}
KVSO_CLASS_FUNCTION(treewidget,setSortingEnabled)
{
	bool bEnables;
	KVSO_PARAMETERS_BEGIN(c)
			KVSO_PARAMETER("bEnables",KVS_PT_BOOLEAN,0,bEnables)
	KVSO_PARAMETERS_END(c)
	if (!widget())return true;
	((QTreeWidget *)widget())->setSortingEnabled(bEnables);
	return true;
}

KVSO_CLASS_FUNCTION(treewidget,setRootIsDecorated)
{
	bool bEnabled;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("bEnabled",KVS_PT_BOOL,0,bEnabled)
	KVSO_PARAMETERS_END(c)
	if (widget()) 	((QTreeWidget *)widget())->setRootIsDecorated(bEnabled);
	return true;
}

KVSO_CLASS_FUNCTION(treewidget,setAllColumnsShowFocus)
{
	bool bEnabled;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("bAllColumnsShowFocus",KVS_PT_BOOL,0,bEnabled)
	KVSO_PARAMETERS_END(c)
	if (widget())((QTreeWidget *)widget())->setAllColumnsShowFocus(bEnabled);
	return true;
}

KVSO_CLASS_FUNCTION(treewidget,hideListViewHeader)
{
	((QTreeWidget *)widget())->header()->hide();
	return true;
}

KVSO_CLASS_FUNCTION(treewidget,showListViewHeader)
{
	((QTreeWidget *)widget())->header()->show();
	return true;
}

KVSO_CLASS_FUNCTION(treewidget,listViewHeaderIsVisible)
{
	c->returnValue()->setBoolean(((QTreeWidget *)widget())->header()->isVisible());
	return true;
}

KVSO_CLASS_FUNCTION(treewidget,itemClickedEvent)
{
	emitSignal("itemClicked",c,c->params());
	return true;
}

void KviKvsObject_treewidget::slotClicked(QTreeWidgetItem * i,int col)
{
	KviKvsVariant *column=new KviKvsVariant((kvs_int_t)col);

	KviKvsVariantList params(new KviKvsVariant(KviKvsObject_treewidgetitem::itemToHandle(i)),column);
	callFunction(this,"itemClickedEvent",0,&params);
}

KVSO_CLASS_FUNCTION(treewidget,selectionChangedEvent)
{
	emitSignal("selectionChanged",c,c->params());
	return true;
}

void KviKvsObject_treewidget::slotSelectionChanged()
{
	if (((QTreeWidget *)widget())->selectionMode()==QTreeWidget::SingleSelection)
	{
		QTreeWidgetItem *it=(QTreeWidgetItem *) ((QTreeWidget *)widget())->currentItem();
		KviKvsVariantList params(new KviKvsVariant(KviKvsObject_treewidgetitem::itemToHandle(it)));
		callFunction(this,"selectionChangedEvent",0,&params);
	}
	else{
		KviKvsVariantList params(new KviKvsVariant((kvs_hobject_t)0));
		callFunction(this,"selectionChangedEvent",0,&params);
	}
}

KVSO_CLASS_FUNCTION(treewidget,currentChangedEvent)
{
	emitSignal("currentChanged",c,c->params());
	return true;
}

void KviKvsObject_treewidget::slotCurrentChanged(QTreeWidgetItem * i,QTreeWidgetItem *prev)
{
	KviKvsVariantList params(new KviKvsVariant(KviKvsObject_treewidgetitem::itemToHandle(i)),new KviKvsVariant(KviKvsObject_treewidgetitem::itemToHandle(prev)));
	callFunction(this,"currentChangedEvent",0,&params);
}

KVSO_CLASS_FUNCTION(treewidget,itemActivatedEvent)
{
	//FIXME: compatibility
	emitSignal("returnPressed",c,c->params());//<----remove me
	//

	emitSignal("itemActivated",c,c->params());
	return true;
}

void KviKvsObject_treewidget::slotItemActivated(QTreeWidgetItem * i,int col)
{
	KviKvsVariant *column=new KviKvsVariant((kvs_int_t)col);
	KviKvsVariantList params(new KviKvsVariant(KviKvsObject_treewidgetitem::itemToHandle(i)),column);
	callFunction(this,"itemActivatedEvent",0,&params);
}

KVSO_CLASS_FUNCTION(treewidget,onItemEvent)
{

	emitSignal("onItem",c,c->params());
	return true;
}

void KviKvsObject_treewidget::slotOnItemEntered(QTreeWidgetItem * i,int col)
{
	KviKvsVariant *column=new KviKvsVariant((kvs_int_t)col);

	KviKvsVariantList params(new KviKvsVariant(KviKvsObject_treewidgetitem::itemToHandle(i)),column);
	callFunction(this,"onItemEvent",0,&params);
}

KVSO_CLASS_FUNCTION(treewidget,itemExpandedEvent)
{
	emitSignal("itemExpanded",c,c->params());
	return true;
}

void KviKvsObject_treewidget::slotItemExpanded(QTreeWidgetItem * i)
{
	KviKvsVariantList params(new KviKvsVariant(KviKvsObject_treewidgetitem::itemToHandle(i)));
	callFunction(this,"itemExpandedEvent",0,&params);
}

KVSO_CLASS_FUNCTION(treewidget,itemCollapsedEvent)
{
	emitSignal("itemCollapsed",c,c->params());
	return true;
}

void KviKvsObject_treewidget::slotItemCollapsed(QTreeWidgetItem * i)
{
	KviKvsVariantList params(new KviKvsVariant(KviKvsObject_treewidgetitem::itemToHandle(i)));
	callFunction(this,"itemCollapsedEvent",0,&params);
}

KVSO_CLASS_FUNCTION(treewidget,customContextMenuRequestedEvent)
{
	emitSignal("rightButtonClicked",c,c->params());
	emitSignal("customContextMenuRequested",c,c->params());
	return true;
}

void KviKvsObject_treewidget::slotCustomContextMenuRequested(const QPoint &pnt)
{
	KviKvsVariant *xpos=new KviKvsVariant((kvs_int_t)pnt.x());
	KviKvsVariant *ypos=new KviKvsVariant((kvs_int_t)pnt.y());
	QTreeWidgetItem *it=(QTreeWidgetItem *) ((QTreeWidget *)widget())->itemAt(pnt);
	KviKvsVariantList params(new KviKvsVariant(KviKvsObject_treewidgetitem::itemToHandle(it)),xpos,ypos);
	callFunction(this,"customContextMenuRequestedEvent",0,&params);
}

KVSO_CLASS_FUNCTION(treewidget,itemChangedEvent)
{
	//FIXME
	emitSignal("itemRenamed",c,c->params());//<--remove me

	emitSignal("itemChanged",c,c->params());
	return true;
}

void KviKvsObject_treewidget::slotItemChanged(QTreeWidgetItem *item,int col)
{
	KviKvsVariant *column=new KviKvsVariant((kvs_int_t)col);
	KviKvsVariantList params(new KviKvsVariant(KviKvsObject_treewidgetitem::itemToHandle(item)),column);
	callFunction(this,"itemChangedEvent",0,&params);
}

void KviKvsObject_treewidget::fileDropped(QString &szFile,QTreeWidgetItem *item)
{
	KviKvsVariant *file=new KviKvsVariant(szFile);
	KviKvsVariantList params(new KviKvsVariant(KviKvsObject_treewidgetitem::itemToHandle(item)),file);
	callFunction(this,"fileDroppedEvent",0,&params);
}

KviKvsTreeWidget::KviKvsTreeWidget(QWidget * par,const char *,KviKvsObject_treewidget *parent)
:QTreeWidget(par)
{
	m_pParentScript=parent;
	setAcceptDrops(true);
	setDragEnabled(true);
	setDropIndicatorShown(true);
	setDragDropMode(QAbstractItemView::DragDrop);
	viewport()->setAcceptDrops(true);
}

KviKvsTreeWidget::~KviKvsTreeWidget()
{
}

void KviKvsTreeWidget::dragEnterEvent( QDragEnterEvent * e )
{
	debug("Drag enter event");
	if(!e->mimeData()->hasUrls())
	{
		debug("Ignore drag");
		e->ignore();
		return;
	}
}

void KviKvsTreeWidget::dropEvent(QDropEvent * e)
{
	debug("Drop event");
	QList<QUrl> list;
	if(e->mimeData()->hasUrls())
	{
		list = e->mimeData()->urls();

		if(!list.isEmpty())
		{
			QList<QUrl>::Iterator it = list.begin();
			for( ; it != list.end(); ++it )
			{
				QUrl url = *it;
				QString path = url.path();
				debug("path %s",path.toUtf8().data());
				#if !defined(COMPILE_ON_WINDOWS) && !defined(COMPILE_ON_MINGW)
					if(path[0] != '/')path.prepend("/"); //HACK HACK HACK for Qt bug (?!?)
				#endif
			//	QTreeWidgetItem *i = itemAt( contentsToViewport(e->pos()) );
			//	m_pParentScript->fileDropped(path,i);
			}
		}
	}
}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "m_class_treewidget.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
