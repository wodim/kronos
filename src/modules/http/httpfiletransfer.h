#ifndef _HTTPFILETRANSFER_H_
#define _HTTPFILETRANSFER_H_
//==================================================================================
//
//   File : httpfiletransfer.h
//   Creation date : Tue Apr 22 2003 02:00:12 GMT by Szymon Stefanek
//
//   This config is part of the KVirc irc client distribution
//   Copyright (C) 2003 Szymon Stefanek (pragma at kvirc dot net)
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
//==================================================================================

#include "kvi_filetransfer.h"
#include "kvi_http.h"
#include "kvi_time.h"

#include "kvi_kvs_variant.h"

#include <qstringlist.h>
#include <qtimer.h>

class QPainter;
class KviTalPopupMenu;

class KviHttpFileTransfer : public KviFileTransfer
{
	enum GeneralStatus { Initializing, Connecting, Downloading, Success, Failure };
	Q_OBJECT
public:
	KviHttpFileTransfer();
	~KviHttpFileTransfer();
private:
	KviHttpRequest * m_pHttpRequest;
	GeneralStatus    m_eGeneralStatus;
	QString          m_szStatusString;

	QStringList      m_lHeaders;
	QStringList      m_lRequest;

	kvi_time_t       m_tStartTime;
	kvi_time_t       m_tTransferStartTime;
	kvi_time_t       m_tTransferEndTime;
	
	KviKvsVariant    m_vMagicIdentifier;
	bool             m_bNotifyCompletion;
	bool             m_bAutoClean;
	bool             m_bNoOutput;
	
	QTimer         * m_pAutoCleanTimer;
	
	QString          m_szCompletionCallback;
public:
	KviHttpRequest * request(){ return m_pHttpRequest; };
	static unsigned int runningTransfers();
	static void init();
	static void done();

	virtual bool startDownload();
	
	void setCompletionCallback(const QString &szCallback){ m_szCompletionCallback = szCallback; };
	void setMagicIdentifier(const KviKvsVariant &vIdentifier){ m_vMagicIdentifier = vIdentifier; };
	void setNotifyCompletion(bool bNotify){ m_bNotifyCompletion = bNotify; };
	void setAutoClean(bool bAutoClean){ m_bAutoClean = bAutoClean; };
	void setNoOutput(bool bNoOutput){ m_bNoOutput = bNoOutput; };

	virtual void displayPaint(QPainter * p,int column,int width,int height);
	virtual int displayHeight(int iLineSpacing);
	virtual void fillContextPopup(KviTalPopupMenu * m,int column);
	virtual bool active();
	virtual void die();
	virtual QString tipText();
	virtual QString localFileName();
protected slots:
	void statusMessage(const QString &txt);
	void transferTerminated(bool bSuccess);
	void headersReceived(KviAsciiDict<KviStr> *h);
	void requestSent(const QStringList &sl);

	void resolvingHost(const QString &hostname);
	void contactingHost(const QString &ipandport);
	void receivedResponse(const QString &response);
	void connectionEstabilished();

	void abort();
	
	void autoClean();
};


#endif //!_HTTPFILETRANSFER_H_
