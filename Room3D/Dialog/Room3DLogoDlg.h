//----------------------------------------------//
//	File: Room3DLogoDlg.h						//
//	Desc: 										//
//	Update: 2012.7  (by 001	)					//
//----------------------------------------------//
#pragma once
#include <qsplashscreen>

/*!
	\class CRoom3DLogoDlg
	\brief This is class to display log widow.(by 001, 2012/7/7).
	\sa 
*/
class CRoom3DLogoDlg : public QSplashScreen
{
	Q_OBJECT
public:
	CRoom3DLogoDlg(QWidget* a_pParent, QString a_sImgPath);
	~CRoom3DLogoDlg(void);
protected:
	virtual void drawContents ( QPainter * painter );
	void AddLogoText(QString a_sMessage);

protected slots:
	void OnMessageChanged(QString a_sMessage);
protected:
	virtual void resizeEvent(QResizeEvent * event);

protected:
	QStringList m_sMessages;
};
