//----------------------------------------------//
//	File: Room3DBaseDlg.h						//
//	Desc: 										//
//	Update: 2012.7  (by 001	)				//
//----------------------------------------------//
#pragma once
#include "Room3DBaseDlg.h"
#include <QWebView>
#include <QWebPage>
#include <QProgressBar>
#include <QLabel>
#include <QVBoxLayout>

class CRoom3DProgressDlg : public CRoom3DBaseDlg
{
	Q_OBJECT
public:
	CRoom3DProgressDlg(QWidget* a_pParent = NULL);
	~CRoom3DProgressDlg(void);

	QProgressBar* GetProgressBar() { return m_pProgressBar; }
	QLabel* GetLabel() { return m_pLabel; }

protected:
	QVBoxLayout* m_pMainLayout;
	QProgressBar* m_pProgressBar;
	QLabel* m_pLabel;
};

/*!
	\class CRoom3DWebDlg
	\brief This is web browser class.
	\sa 
*/
class CRoom3DWebDlg : public CRoom3DBaseDlg
{
	Q_OBJECT
public:
	CRoom3DWebDlg(QWidget* a_pParent = NULL);
	~CRoom3DWebDlg(void);

public:
	void OnRequestFinish();
	void SetPercent(float fPercent);

protected:
	void resizeEvent(QResizeEvent *event);

public slots:
	void LoadFinished(bool ok);
	void handleFileDownload(QNetworkReply *reply);

private:
	QWebView *m_pWebView;
	QWebPage *m_pPage;
	CRoom3DProgressDlg *m_pProgressDlg;
};
