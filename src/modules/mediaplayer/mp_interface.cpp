//=============================================================================
//
//   File : mp_interface.cpp
//   Creation date : Fri Mar 25 20:01:25 2005 GMT by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2005 Szymon Stefanek (pragma at kvirc dot net)
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

#include "mp_interface.h"
#include "mp_mp3.h"
#include "kvi_options.h"
#include <qtextcodec.h>

static QTextCodec * mediaplayer_get_codec()
{
	QTextCodec * c= QTextCodec::codecForName(KVI_OPTION_STRING(KviOption_stringWinampTextEncoding)); 
	if(!c)c = QTextCodec::codecForLocale(); 
	return c;

}
int KviMediaPlayerInterface::position()
{
	return -1;
}

int KviMediaPlayerInterface::length()
{
	return -1;
}

bool KviMediaPlayerInterface::quit()
{
	return false;
}

bool KviMediaPlayerInterface::hide()
{
	return false;
}

bool KviMediaPlayerInterface::show()
{
	return false;
}

bool KviMediaPlayerInterface::minimize()
{
	return false;
}

QString KviMediaPlayerInterface::getLocalFile()
{
	QString ret = mrl();
	if(ret.isEmpty())return ret;
	if(ret.startsWith("file://",false))
	{
		ret.remove(0,7);
		return ret;
	}
	return QString::null;
}

#define SCAN_MP3_FILE \
	QString f = getLocalFile(); \
	if(f.isEmpty())return QString::null; \
	mp3info mp3; \
	if(!scan_mp3_file(f,&mp3))return QString::null; \
	QTextCodec *pCodec; \
	pCodec=mediaplayer_get_codec();

#define SCAN_MP3_FILE_RET_INT \
	QString f = getLocalFile(); \
	if(f.isEmpty())return -1; \
	mp3info mp3; \
	if(!scan_mp3_file(f,&mp3))return -1;


QString KviMediaPlayerInterface::artist()
{
	SCAN_MP3_FILE
	return pCodec->toUnicode(KviQCString(mp3.id3.artist));
}

QString KviMediaPlayerInterface::title()
{
	SCAN_MP3_FILE
	return pCodec->toUnicode(KviQCString(mp3.id3.title));
}


QString KviMediaPlayerInterface::genre()
{
	SCAN_MP3_FILE
	return pCodec->toUnicode(KviQCString(get_typegenre(mp3.id3.genre[0])));
}

QString KviMediaPlayerInterface::comment()
{
	SCAN_MP3_FILE
	return pCodec->toUnicode(KviQCString(mp3.id3.comment));
}

QString KviMediaPlayerInterface::year()
{
	SCAN_MP3_FILE
	return QString(mp3.id3.year);
}

QString KviMediaPlayerInterface::album()
{
	SCAN_MP3_FILE
	return pCodec->toUnicode(KviQCString(mp3.id3.album));
}

bool KviMediaPlayerInterface::playMrl(const QString &mrl)
{
	return false;
}

bool KviMediaPlayerInterface::setVol(int &iVol)
{
	return false;
}

int KviMediaPlayerInterface::getVol()
{
	return -1;
}

bool KviMediaPlayerInterface::mute()
{
	return false;
}

bool KviMediaPlayerInterface::jumpTo(int &iPos)
{
	return false;
}

KviMediaPlayerInterface::PlayerStatus KviMediaPlayerInterface::status()
{
	return KviMediaPlayerInterface::Unknown;
}

int KviMediaPlayerInterface::bitRate()
{
	SCAN_MP3_FILE_RET_INT
	return header_bitrate(&(mp3.header));
}

int KviMediaPlayerInterface::sampleRate()
{
	SCAN_MP3_FILE_RET_INT
	return header_frequency(&(mp3.header));
}

int KviMediaPlayerInterface::channels()
{
	SCAN_MP3_FILE_RET_INT
	return header_channels(&(mp3.header));
}

int KviMediaPlayerInterface::getPlayListPos()
{
	return -1;
}

bool KviMediaPlayerInterface::setPlayListPos(int &iPos)
{
	return false;
}

int KviMediaPlayerInterface::getListLength()
{
	return -1;
}


int KviMediaPlayerInterface::getEqData(int &ival)
{
	return -1;
}

bool KviMediaPlayerInterface::setEqData(int &iPos, int &iVal)
{
	return false;
}

bool KviMediaPlayerInterface::getRepeat()
{
	return false;
}

bool KviMediaPlayerInterface::getShuffle()
{
	return false;
}

bool KviMediaPlayerInterface::setRepeat(bool &bVal)
{
	return false;	
}

bool KviMediaPlayerInterface::setShuffle(bool &bVal)
{
	return false;
}

QString KviMediaPlayerInterface::mediaType()
{
	QString ret = mrl();
	if(ret.endsWith(".mp3",false))ret = "MPEG Layer 3";
	else if(ret.endsWith(".ogg",false))ret = "OGG Vorbis";
	else if(ret.endsWith(".avi",false))ret = "Audio Video Interleave";
	else if(ret.endsWith(".mpeg",false))ret = "MPEG Video";
	else if(ret.endsWith(".mpg",false))ret = "MPEG Video";
	else if(ret.startsWith("http://",false))ret = "HTTP Audio Stream";
	else ret = QString::null;
	return ret;
}

