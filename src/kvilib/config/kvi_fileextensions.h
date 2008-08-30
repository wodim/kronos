#ifndef _KVI_FILEEXTENSIONS_H_
#define _KVI_FILEEXTENSIONS_H_
//=============================================================================
//
//   File : kvi_fileextensions.h
//   Creation date : Tue Dec 26 2006 05:09:22 CEST by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2006 Szymon Stefanek (pragma at kvirc dot net)
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

/**
* \file kvi_fileextensions.h
* \author Szymon Stefanek
* \brief This file contains the file extensions and filters used by KVIrc
*/

/**
* \brief File extensions
* \def KVI_FILEEXTENSION_CONFIG Configuration file .kvc
* \def KVI_FILEEXTENSION_SCRIPT Script file .kvs
* \def KVI_FILEEXTENSION_THEMEPACKAGE Theme package .kvt
* \def KVI_FILEEXTENSION_ADDONPACKAGE Addon package .kva
*/
#define KVI_FILEEXTENSION_CONFIG "kvc"
#define KVI_FILEEXTENSION_SCRIPT "kvs"
#define KVI_FILEEXTENSION_THEMEPACKAGE "kvt"
#define KVI_FILEEXTENSION_ADDONPACKAGE "kva"

/**
* \brief Filters
* \def KVI_FILTER_CONFIG Configuration file *.kvs
* \def KVI_FILTER_SCRIPT Script file *.kvs
* \def KVI_FILTER_THEME Theme package *.kvt
* \def KVI_FILTER_ADDON Addon package *.kva
* \def KVI_FILTER_SCRIPTS Script files *.kvs *.kva
* \def KVI_FILTER_INI Ini file *.ini
* \def KVI_FILTER_IMAGE Image file *.png *.jpg *.jpeg *.bmp *.gif *.xpm
*/
#define KVI_FILTER_CONFIG "KVIrc Config (*.kvc)"
#define KVI_FILTER_SCRIPT "KVIrc Script (*.kvs)"
#define KVI_FILTER_THEME "KVIrc Theme (*.kvt)"
#define KVI_FILTER_ADDON "KVIrc Addon (*.kva)"
// Provided for convenience
#define KVI_FILTER_SCRIPTS "KVIrc Script and Addon (*.kvs *.kva)"
#define KVI_FILTER_INI "INI File (*.ini)"
#define KVI_FILTER_IMAGE "Image File (*.png *.jpg *.jpeg *.bmp *.gif *.xpm)"

#endif //_KVI_FILEEXTENSIONS_H_
