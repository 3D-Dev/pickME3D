//----------------------------------------------//
//	File: Room3DMenuBar.h								//
//	Desc: 										//
//	Update: 2012.6  (by 001	)				//
//----------------------------------------------//

#pragma once
#include <QFrame>
#include "Room3DImageButton.h"
#include "Room3DMenu.h"
#include <QHBoxLayout>
#include <QSpacerItem>

/*!
	\class CRoom3DMenuBar
	\brief This is menubar class to contain common command buttons.(by 001, 2012/6/21).
	\sa 
*/
class CRoom3DMenuBar : public QFrame
{
	Q_OBJECT
	enum BUTTON_ID
	{
#if USE_FILE_MENU /*002->Button to File Menu*/
		BTN_FILE = 0,
#else
		BTN_NEW = 0,
		BTN_OPEN,
		BTN_SAVE,
#endif
		BTN_UNDO,
		BTN_REDO,
		BTN_RESET,
		BTN_BRIGHT,
		BTN_VISIBLE_FLOOR,
		BTN_SAVE_PHOTO,
		BTN_SHARE_PHOTO,
		BTN_IMPORT_MODEL,
		BTN_REPORT,
		BTN_TUTORIAL
	};
public:
	CRoom3DMenuBar(QWidget* parent = 0);
	~CRoom3DMenuBar(void);
	void UpdateControlState();
protected:
	void InitControls();
	void UpdateLayout();

	// function to response button click
	void ChangeFloorVisible();
	void ExportToImage();
	void ExportToHTML();
	bool SharePhoto();

public slots:
	void pressedButton(int a_nButtonID);
#if USE_FILE_MENU /*002->Button to File Menu*/
	void OnNewProject();
	void OnSaveProject();
	void OnOpenProject();
#endif
protected:
	CRoom3DImageWidget* m_pBkWidget;
	QVBoxLayout*		m_pBaseLayout;

#if USE_FILE_MENU /*002->Button to File Menu*/
	CRoom3DImageButton*		m_pFileBtn;
	CRoom3DMenu*			m_pFileMenu;
	CRoom3DAction*			m_pFileAction;
	CRoom3DAction*			m_pNewAction;
	CRoom3DAction*			m_pSaveAction;
	CRoom3DAction*			m_pOpenAction;
#else
	CRoom3DImageButton*		m_pNewBtn;
	CRoom3DImageButton*		m_pOpenBtn;
	CRoom3DImageButton*		m_pSaveBtn;
#endif
	CRoom3DImageButton*		m_pUndoBtn;
	CRoom3DImageButton*		m_pRedoBtn;
	CRoom3DImageButton*		m_pResetBtn;
	CRoom3DImageButton*		m_pBrightBtn;
#if USE_SHOW_HIDE_FLOOR
	CRoom3DImageButton*		m_pVisibleFloorBtn;
#endif
	CRoom3DImageButton*		m_pSavePhotoBtn;
	CRoom3DImageButton*		m_pSharePhotoBtn;
	CRoom3DImageButton*		m_pImportModelBtn;
	CRoom3DImageButton*		m_pReportBtn;

	CRoom3DImageButton*		m_pTutorialBtn;

	QHBoxLayout*			m_pHorizontalLayout;
	QSpacerItem*			m_pHorizontalEndSpacer;
	QSpacerItem*			m_pHorizontalStartSpacer;
	QSpacerItem*			m_pHorizontalMidSpacer;
};
