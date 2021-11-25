#ifndef ROOM3D_H
#define ROOM3D_H

#include <QtGui/QMainWindow>
#include <QBitmap>
#include "ui_room3d.h"
#include "Room3DToolBoxBar.h"
#include "Room3DStepBar.h"
#include "Room3DMenuBar.h"
#include "Room3DSceneView.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "Room3DLogoDlg.h"
#include "Room3DIntroVideoDlg.h"
#include "Room3DGuideDlg.h"
#if USE_THREAD
#include "Room3DWaitingDlg.h"
#endif
#include "norwegianwoodstyle.h"
#include "Room3DTutorialVideoDlg.h"

class Room3D : public QMainWindow
{
	Q_OBJECT

	enum BUTTON_ID
	{
		BTN_MINIMIZE,
		BTN_CLOSE
	};
public:
	Room3D(QWidget *parent = 0, Qt::WFlags flags = 0);
	~Room3D();
	void UpdateControlState(void* a_pUserData = NULL);
	void UpdateView();
	CRoom3DSceneView* GetSceneView(){return m_pSceneView;}
	CRoom3DSceneViewWrapper* GetSceneViewWrapper(){return m_pScenViewWrapper;}
	CRoom3DLogoDlg*		GetLogoDlg(){return m_pLogoDlg;}
	CRoom3DIntroVideoDlg*	GetIntroVideoDlg(){return m_pIntoroVideoDlg;}
	void ShowLogoDlg(bool a_blShow);
	void ShowGuideDlg(int a_nId, QPoint a_offsetPt = QPoint(15,0));
	void ShowTutorialVideo();
	void HideTutorialVideo();
	void resizeEvent(QResizeEvent *event);
#if USE_THREAD
	void ShowWaitingDlg();
	void OnModelLoadFinish();
#endif

protected slots:
		void pressedButton(int a_nBtnID);

protected:
	void InitControls();
	void UpdateGuideWidget();

private:
	Ui::Room3DClass ui;

	CRoom3DToolBoxBar*	m_pToolBoxBar;

	CRoom3DImageWidget *m_pCentralWidget;
	QGridLayout *gridLayout;
	// title bar
	QHBoxLayout *m_pTitleBarHorzLayout;
	CRoom3DImageWidget* m_pTitleBarLeft;
	CRoom3DImageWidget* m_pTitleBarMid;
	CRoom3DImageWidget* m_pTitlePickMe3D;
	CRoom3DImageWidget* m_pTitleBarRight;
	CRoom3DImageButton* m_pMinimizeBtn;
	CRoom3DImageButton* m_pCloseBtn;
	QHBoxLayout *m_pTitleBarMidHorzLayout;
	QHBoxLayout *m_pTitleBarRightHorzLayout;


	QHBoxLayout *horizontalLayout;
	CRoom3DImageWidget *frame_2;
	QVBoxLayout *verticalLayout;
	QVBoxLayout *verticalLayout1;
	CRoom3DSceneViewWrapper*			m_pScenViewWrapper;
	CRoom3DMenuBar*		m_pMenuBar;
	CRoom3DSceneView*	m_pSceneView;
	CRoom3DStepBar*		m_pStepBar;

	CRoom3DImageWidget*	m_pBottomEdge;

	CRoom3DLogoDlg *m_pLogoDlg;
	CRoom3DIntroVideoDlg* m_pIntoroVideoDlg;
	CRoom3DGuideDlg *m_pGuideDlg;
#if USE_THREAD
	CRoom3DWaitingDlg* m_pWaitingDlg;
#endif
	CRoom3DTutorialVideoDlg* m_pStep1TutorialDlg;
	CRoom3DTutorialVideoDlg* m_pStep2TutorialDlg;
	CRoom3DTutorialVideoDlg* m_pStep3TutorialDlg;
public:
	int m_nGuideId;
	QWidget* m_pGuidedWidget;
	QPoint m_offsetPt;
};

#endif // ROOM3D_H
