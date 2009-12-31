//=============================================================================
//
//   File : optw_windowlist.cpp
//   Creation date : Wed Jan 16 04:30:39 2002 GMT by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2002-2008 Szymon Stefanek (pragma at kvirc dot net)
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

#include "optw_windowlist.h"

#include "kvi_settings.h"
#include "kvi_locale.h"
#include "kvi_options.h"


KviWindowListOptionsWidget::KviWindowListOptionsWidget(QWidget * parent)
: KviOptionsWidget(parent)
{
	setObjectName("windowlist_options_widget");

	createLayout();

	addBoolSelector(0,0,0,0,__tr2qs_ctx("Use tree window list","options"),KviOption_boolUseTreeWindowListWindowList);
	addBoolSelector(0,1,0,1,__tr2qs_ctx("Sort windows by name","options"),KviOption_boolSortWindowListItemsByName);
	addBoolSelector(0,2,0,2,__tr2qs_ctx("Show window icons in window list","options"),KviOption_boolUseWindowListIcons);
	KviBoolSelector * b = addBoolSelector(0,3,0,3,__tr2qs_ctx("Show activity meter in window list","options"),KviOption_boolUseWindowListActivityMeter);
	mergeTip(b,__tr2qs_ctx("<center>This option enables the usage of the window list activity meter. " \
		"Each window entry will have a small indicator of the activity that is going on in that window. " \
		"The indicator is a small square that changes colors, dark colors mean low activity, " \
		"while bright colors signal high activity. KVIrc also uses some heuristics to determine " \
		"whether the activity is somewhat \"human\" or it is generated by automated entities (such as bots " \
		"or IRC servers). \"Human\" activity causes the indicator to be shaded red while automated " \
		"activity causes the indicator to be shaded blue.</center>","options"));
	addBoolSelector(0,4,0,4,__tr2qs_ctx("Show IRC context indicator in window list","options"),KviOption_boolUseWindowListIrcContextIndicator);
	addBoolSelector(0,5,0,5,__tr2qs_ctx("Enable window tooltips","options"),KviOption_boolShowWindowListToolTips);
	addRowSpacer(0,6,0,6);
}


KviWindowListOptionsWidget::~KviWindowListOptionsWidget()
{
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


KviTreeWindowListOptionsWidget::KviTreeWindowListOptionsWidget(QWidget * parent)
: KviOptionsWidget(parent)
{
	setObjectName("treewindowlist_options_widget");
}

KviTreeWindowListOptionsWidget::~KviTreeWindowListOptionsWidget()
{
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


KviTreeWindowListForegroundOptionsWidget::KviTreeWindowListForegroundOptionsWidget(QWidget * parent)
: KviOptionsWidget(parent)
{
	setObjectName("treewindowlist_options_widget");

	createLayout();

	addFontSelector(0,0,0,0,__tr2qs_ctx("Font:","options"),KviOption_fontTreeWindowList);

	KviTalGroupBox *g = addGroupBox(0,1,0,1,Qt::Horizontal,__tr2qs_ctx("Text/Alert Colors","options"));

	addColorSelector(g,__tr2qs_ctx("Normal:","options"),KviOption_colorTreeWindowListForeground);
	addColorSelector(g,__tr2qs_ctx("Selected:","options"),KviOption_colorTreeWindowListActiveForeground);
	addColorSelector(g,__tr2qs_ctx("Alert Level 1:","options"),KviOption_colorTreeWindowListHighlight1Foreground);
	addColorSelector(g,__tr2qs_ctx("Alert Level 2:","options"),KviOption_colorTreeWindowListHighlight2Foreground);
	addColorSelector(g,__tr2qs_ctx("Alert Level 3:","options"),KviOption_colorTreeWindowListHighlight3Foreground);
	addColorSelector(g,__tr2qs_ctx("Alert Level 4:","options"),KviOption_colorTreeWindowListHighlight4Foreground);
	addColorSelector(g,__tr2qs_ctx("Alert Level 5:","options"),KviOption_colorTreeWindowListHighlight5Foreground);

	addColorSelector(0,2,0,2,__tr2qs_ctx("Progress bar color:","options"),KviOption_colorTreeWindowListProgress);

	addRowSpacer(0,3,0,3);
}


KviTreeWindowListForegroundOptionsWidget::~KviTreeWindowListForegroundOptionsWidget()
{
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


KviTreeWindowListBackgroundOptionsWidget::KviTreeWindowListBackgroundOptionsWidget(QWidget * parent)
: KviOptionsWidget(parent)
{
	setObjectName("treewindowlist_options_widget");

	createLayout();

	KviTalGroupBox * g = addGroupBox(0,0,1,0,Qt::Horizontal,__tr2qs_ctx("Background Colors","options"));
	addColorSelector(g,__tr2qs_ctx("Normal:","options"),KviOption_colorTreeWindowListBackground);
	addColorSelector(g,__tr2qs_ctx("Selected:","options"),KviOption_colorTreeWindowListActiveBackground);

	addPixmapSelector(0,1,1,1,__tr2qs_ctx("Background image:","options"),KviOption_pixmapTreeWindowListBackground);

	addLabel(0,2,0,2,__tr2qs_ctx("Horizontal align:","options"));
	m_pHorizontalAlign=new QComboBox(this);
	addWidgetToLayout(m_pHorizontalAlign,1,2,1,2);

	addLabel(0,3,0,3,__tr2qs_ctx("Vertical align:","options"));
	m_pVerticalAlign=new QComboBox(this);
	addWidgetToLayout(m_pVerticalAlign,1,3,1,3);

	m_pHorizontalAlign->addItem(__tr2qs_ctx("Tile","options"));
	m_pHorizontalAlign->addItem(__tr2qs_ctx("Left","options"));
	m_pHorizontalAlign->addItem(__tr2qs_ctx("Right","options"));
	m_pHorizontalAlign->addItem(__tr2qs_ctx("Center","options"));

	m_pVerticalAlign->addItem(__tr2qs_ctx("Tile","options"));
	m_pVerticalAlign->addItem(__tr2qs_ctx("Top","options"));
	m_pVerticalAlign->addItem(__tr2qs_ctx("Bottom","options"));
	m_pVerticalAlign->addItem(__tr2qs_ctx("Center","options"));

	switch( KVI_OPTION_UINT(KviOption_uintTreeWindowListPixmapAlign) & Qt::AlignHorizontal_Mask)
	{
		case Qt::AlignLeft:
			m_pHorizontalAlign->setCurrentIndex(1);
			break;
		case Qt::AlignRight:
			m_pHorizontalAlign->setCurrentIndex(2);
			break;
		case Qt::AlignHCenter:
			m_pHorizontalAlign->setCurrentIndex(3);
			break;
		default:
			m_pHorizontalAlign->setCurrentIndex(0);
	}

	switch( KVI_OPTION_UINT(KviOption_uintTreeWindowListPixmapAlign) & Qt::AlignVertical_Mask)
	{
		case Qt::AlignTop:
			m_pVerticalAlign->setCurrentIndex(1);
			break;
		case Qt::AlignBottom:
			m_pVerticalAlign->setCurrentIndex(2);
			break;
		case Qt::AlignVCenter:
			m_pVerticalAlign->setCurrentIndex(3);
			break;
		default:
			m_pVerticalAlign->setCurrentIndex(0);
	}

	layout()->setRowStretch(1,1);
}


KviTreeWindowListBackgroundOptionsWidget::~KviTreeWindowListBackgroundOptionsWidget()
{
}

void KviTreeWindowListBackgroundOptionsWidget::commit()
{
	KviOptionsWidget::commit();

	int iFlags=0;
	switch(m_pHorizontalAlign->currentIndex())
	{
		case 1:
			iFlags|=Qt::AlignLeft;
			break;
		case 2:
			iFlags|=Qt::AlignRight;
			break;
		case 3:
			iFlags|=Qt::AlignHCenter;
			break;
	}
	switch(m_pVerticalAlign->currentIndex())
	{
		case 1:
			iFlags|=Qt::AlignTop;
			break;
		case 2:
			iFlags|=Qt::AlignBottom;
			break;
		case 3:
			iFlags|=Qt::AlignVCenter;
			break;
	}

	KVI_OPTION_UINT(KviOption_uintTreeWindowListPixmapAlign)=iFlags;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

KviTreeWindowListFeaturesOptionsWidget::KviTreeWindowListFeaturesOptionsWidget(QWidget * parent)
: KviOptionsWidget(parent)
{
	setObjectName("treewindowlist_options_widget");

	createLayout();

	addBoolSelector(0,1,0,1,__tr2qs_ctx("Show header","options"),KviOption_boolShowTreeWindowListHeader);
	
	addUIntSelector(0,2,0,2,__tr2qs_ctx("Minimum width:","options"),KviOption_uintTreeWindowListMinimumWidth,24,1024,125);
	addUIntSelector(0,3,0,3,__tr2qs_ctx("Maximum width:","options"),KviOption_uintTreeWindowListMaximumWidth,24,9999,125);

	addRowSpacer(0,4,0,4);
}


KviTreeWindowListFeaturesOptionsWidget::~KviTreeWindowListFeaturesOptionsWidget()
{
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


KviClassicWindowListOptionsWidget::KviClassicWindowListOptionsWidget(QWidget * parent)
: KviOptionsWidget(parent)
{
	setObjectName("classicwindowlist_options_widget");

	createLayout();

	addFontSelector(0,0,0,0,__tr2qs_ctx("Font:","options"),KviOption_fontWindowList);
	KviTalGroupBox *g = addGroupBox(0,1,0,1,Qt::Horizontal,__tr2qs_ctx("Text/Alert Colors","options"));
	addColorSelector(g,__tr2qs_ctx("Normal:","options"),KviOption_colorWindowListNormalText);
	addColorSelector(g,__tr2qs_ctx("Minimized:","options"), KviOption_colorWindowListMinimizedText);
	addColorSelector(g,__tr2qs_ctx("Alert Level 1:","options"),KviOption_colorWindowListHighlight1Text);
	addColorSelector(g,__tr2qs_ctx("Alert Level 2:","options"),KviOption_colorWindowListHighlight2Text);
	addColorSelector(g,__tr2qs_ctx("Alert Level 3:","options"),KviOption_colorWindowListHighlight3Text);
	addColorSelector(g,__tr2qs_ctx("Alert Level 4:","options"),KviOption_colorWindowListHighlight4Text);
	addColorSelector(g,__tr2qs_ctx("Alert Level 5:","options"),KviOption_colorWindowListHighlight5Text);
	addColorSelector(0,2,0,2,__tr2qs_ctx("Progress bar color:","options"),KviOption_colorWindowListProgressBar);
	addUIntSelector(0,3,0,3,__tr2qs_ctx("Minimum width of buttons:","options"),KviOption_uintWindowListButtonMinWidth,24,9999,100);
	//addUIntSelector(0,4,0,4,__tr2qs_ctx("Maximum number of rows:","options"),KviOption_uintWindowListMaximumRows,1,16,2);
	addBoolSelector(0,4,0,4,__tr2qs_ctx("Set maximum button width","options"),KviOption_boolClassicWindowListSetMaximumButtonWidth);
	addUIntSelector(0,5,0,5,__tr2qs_ctx("Maximum width of buttons:","options"),KviOption_uintClassicWindowListMaximumButtonWidth,24,9999,100);
	addBoolSelector(0,6,0,6,__tr2qs_ctx("Use flat buttons","options"),KviOption_boolUseFlatClassicWindowListButtons);
	addRowSpacer(0,7,0,7);
}


KviClassicWindowListOptionsWidget::~KviClassicWindowListOptionsWidget()
{
}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "m_optw_windowlist.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
