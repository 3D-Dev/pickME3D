//----------------------------------------------//
//	File: common.h								//
//	Desc: 										//
//	Update: 2012.6  (by 001	)				//
//----------------------------------------------//

#ifndef COMMON_H
#define COMMON_H
#include <QObject>
#include "Define.h"

class CRoom3DApplication;

extern CRoom3DApplication* g_pApp;


#define APP_ICON_PATH				tr(":/AppIcon.png")
#if USE_FILE_MENU /*002->Button to File Menu*/
#define BUTTON_IMG_FILE				tr(":/File.png")
#else
#define BUTTON_IMG_NEW				tr(":/New.png")
#define BUTTON_IMG_OPEN				tr(":/Open.png")
#define BUTTON_IMG_SAVE				tr(":/Save.png")
#endif
#define BUTTON_IMG_UNDO				tr(":/Undo.png")
#define BUTTON_IMG_REDO				tr(":/Redo.png")
#define BUTTON_IMG_BRIGHT			tr(":/Contrast.png")
#define BUTTON_IMG_SETSCALE			tr(":/SetScale.png")
#define BUTTON_IMG_SETFLOOR			tr(":/SetFloor.png")
#define BUTTON_IMG_SETCORNER		tr(":/SetCorner.png")
#define BUTTON_IMG_SETHEIGHT		tr(":/SetHeight.png")
#define BUTTON_IMG_CALIBRATE		tr(":/Calibrate.png")
#define BUTTON_IMG_RESET			tr(":/Reset.png")
#define CHECKBOX_IMG_BACK			tr(":/bgimg.png")
#define CHECKBOX_IMG_FOCUSED		tr(":/focused.png")
#define CHECKBOX_IMG_CLICKED		tr(":/clicked.png")
#define CHECKBOX_IMG_CHECKED		tr(":/checked.png")

#define BUTTON_IMG_SHOW_FLOOR		tr(":/ShowFloor.png")
#define BUTTON_IMG_HIDE_FLOOR		tr(":/HideFloor.png")
#define BUTTON_IMG_EXPORT			tr(":/SavePhoto.png")
#define BUTTON_IMG_SHARE_PHOTO		tr(":/SharePhoto.png")
#define BUTTON_IMG_IMPORT_MODEL		tr(":/ImportModel.png")
#define BUTTON_IMG_REPORT			tr(":/Report.png")
#define BUTTON_IMG_PURCHASE			tr(":/Purchase.png")
#define BUTTON_IMG_FIND				tr(":/Find.png")
#define BUTTON_IMG_STEP_COMMENT		tr(":/StepComment.png")
#define BUTTON_IMG_STEP1			tr(":/Step1.png")
#define BUTTON_IMG_STEP2			tr(":/Step2.png")
#define BUTTON_IMG_STEP3			tr(":/Step3.png")
#define BUTTON_IMG_BROWSE			tr(":/Browse.png")
#define BUTTON_IMG_TUTORIAL			tr(":/Help.png")
#define BUTTON_IMG_MORE_ITEM		tr(":/MoreItems.png")
#define DEFAULT_ITEM_IMG			tr(":/DefaultItemImg.png")
#define BUTTON_IMG_MINIMIZE			tr(":/Minimize.png")
#define BUTTON_IMG_CLOSE			tr(":/Close.png")
#define IMG_SPLASH					tr(":/Room3DSplash.png")
#define BUTTON_IMG_SKIP				tr(":/Skip.png")
#define BUTTON_MASK_SKIP				tr(":/SkipMask.png")
#define BACK_IMG_STEPBAR			tr(":/StepBarBG.png")
#define BACK_IMG_TOOLBOXBAR			tr(":/ToolBoxBarBG.png")
#define BACK_IMG_MENUBAR			tr(":/MenuBarBG.png")
#define BACK_IMG_SCENEVIEW			tr(":/SceneViewBG.png")
#define BORDER_IMG_STEPBAR			tr(":/StepBarBorder.png")
#define BORDER_IMG_TOOLBOXBAR		tr(":/ToolBoxBarBorder.png")
#define BORDER_IMG_MENUBAR			tr(":/MenuBarBorder.png")
#define BORDER_IMG_SCENEVIEW		tr(":/SceneViewBorder.png")
#define INNER_BORDER_IMG_SCENEVIEW	tr(":/SceneViewInnerBorder.png")
#define BACK_IMG_WOOD				tr(":/woodbackground.png")
#define BUTTON_IMG_WOOD				tr(":/woodbutton.png")
#define BACK_IMG_TITLEBAR				tr(":/TitleBarBG.png")
#define BACK_IMG_TITLEBAR_LEFT			tr(":/TitlebarLeft.png")
#define BACK_IMG_TITLEBAR_MID			tr(":/TitlebarMid.png")
#define BACK_IMG_TITLEBAR_RIGHT			tr(":/TitlebarRight.png")
#define IMG_PICKME_3D					tr(":/PickMe3D.png")
#define BACK_IMG_BOTTEOM_EDGE				tr(":/BottomEdgeBG.png")

#define INTORO_VIDEO_PATH					tr("Video/Introduction.avi")
#define STEP1_TUTORIAL_VIDEO_PATH			tr("Video/Step1Tutorial.avi")
#define STEP2_TUTORIAL_VIDEO_PATH			tr("Video/Step2Tutorial.avi")
#define STEP3_TUTORIAL_VIDEO_PATH			tr("Video/Step3Tutorial.avi")
#define BUTTON_SOUND				("Video/one_click.mp3")
#define GUIDE_ARROW_IMG			tr(":/GuideArrow.png")
#define GUIDE_ARROW_MASK			tr(":/GuideArrowMask.png")


QString Trans(const char*);
#define TR	Trans 

#define STR_DESCRIPTION				TR("Description")
#define STR_SEARCH					TR("Find")
#define STR_PRICE					TR("Price")
#define STR_APPROX_PRICE			TR("Approx Price")
#define STR__PRICE_NOTE				TR("Price Note")
#define STR_PRICE_UPDATE_ON			TR("Price update on")
#define STR_PRICE_RANGE				TR("Price range")
#define STR_CENTIMETER				TR("cm")
#define STR_DOLAR					TR("$")
#define STR_EQUAL					TR("Equal")
#define STR_AT_LEAST				TR("At least")
#define STR_AT_MOST					TR("At most")
#define	STR_SIZE					TR("Size")
#define	STR_DIMENTION				TR("Dimention")
#define	STR_BRIGHTNESS				TR("Brightness")
#define	STR_CONTRAST				TR("Contrast")
#define STR_SET_DEFAULT				TR("Set default")
#define STR_BRIGHT_DLG_TITLE		TR("Set brightness and contrast")

#define	FILTER_IMAGES				TR("Images (*.bmp *.png *.jpg)")
#define	SELECT_IMAGE				TR("Select Image File")
#define SET_COMMIT					TR("Commit")
#define STR_VISIBLE_FLOOR			TR("Visible Floor")

#define TOOLBOX3_DEFAULT_DESC			TR("No item description.")
#define TOOLBOX3_NO_SELECTED_ITEM		TR("No item was selected.")

// camera info
#define STR_CAMERA_INPUT_SCALE		TR("Input Scale")
#define STR_CAMERA_LENGTH			TR("Length: (m)")
#define STR_CAMERA_X_AXIS			TR("X Axis (Red Line)")
#define STR_CAMERA_Y_AXIS			TR("Y Axis (Green Line)")
#define STR_CAMERA_Z_AXIS			TR("Z Axis (Blue Line)")
#define STR_CAMERA_SELECT_HELPER_AIXS	TR("Select Helper Axis")
#define STR_CAMERA_AXIS_			TR("Axis:")
#define STR_SAVE_FILE_AS			TR("Save File As")
#define STR_SAFE_FILE_FILTER		TR("Room3D Document (*.r3d)")
#define STR_OPEN_FILE				TR("Open File")
#define STR_OPEN_FILE_FILTER		TR("Room3D Document (*.r3d)")
#define STR_NO_PRICE_NOTE			TR("No price note.")
#define STR_NO_UPDATE_ON			TR("No update on.")
#define STR_NO_SIZE					TR("No size.")

#define STR_HTML_HEADER				"<HTML><BODY> <P>"
#define STR_HTML_BIG_FONT_BEGIN		"<font color=\"#CDCDCC\" size=\"5\" face=\"Courier New\">"
#define STR_HTML_MID_FONT_BEGIN		"<font color=\"#CDCDCC\" size=\"3\" face=\"Courier New\">"
#define STR_HTML_FONT_END			"</font>"
#define STR_HTML_TAILER				"</P></BODY></HTML>"
#define STR_HTEML_SPACE				"&nbsp;"

#define COMMENT_FONT_FAMILY "Microsoft JhengHei"

#define STR_SAMPLE_PHOTO_DIR_NAME	"Sample Room photo"

// This class is temperary class for creating ts file with lupdate command.
// lupdate command is a command provided by QtLinguist.
class CRoom3DTempClassForTSFile : QObject
{
public:
	CRoom3DTempClassForTSFile()
	{
		setObjectName(tr("Description"));
		setObjectName(tr("Find"));
		setObjectName(tr("Price"));
		setObjectName(tr("cm"));
		setObjectName(tr("$"));
		setObjectName(tr("Equal"));
		setObjectName(tr("At least"));
		setObjectName(tr("At most"));
		setObjectName(tr("Size"));
		setObjectName(tr("Brightness"));
		setObjectName(tr("Contrast"));
		setObjectName(tr("Set default"));
		setObjectName(tr("Set brightness and contrast"));
//
//
		setObjectName(tr("Images (*.bmp *.png *.jpg)"));
		setObjectName(tr("Select Image File"));
		setObjectName(tr("Commit"));
		setObjectName(tr("Visible Floor"));
		setObjectName(tr("Visible Floor"));
		setObjectName(tr("No item description."));
		setObjectName(tr("No item was selected."));
		setObjectName(tr("Input Scale"));
		setObjectName(tr("Length: (m)"));
		setObjectName(tr("X Axis (Red Line)"));
		setObjectName(tr("Y Axis (Green Line)"));
		setObjectName(tr("Z Axis (Blue Line)"));
		setObjectName(tr("Select Helper Axis"));
		setObjectName(tr("Axis:"));

		setObjectName(tr("Save File As"));
		setObjectName(tr("Images (*.png)"));
		setObjectName(tr("Dimention"));
		setObjectName(tr("Approx Price"));
		setObjectName(tr("Price update on"));
		setObjectName(tr("Price range"));
		setObjectName(tr("Price Note"));
		setObjectName(tr("No price note."));
		setObjectName(tr("No update on."));
		setObjectName(tr("No size."));
	}
};

#define GIZMO_IMG_ROTATION1			QString(":/Rotation1.png")
#define GIZMO_IMG_ROTATION2			QString(":/Rotation2.png")
#define GIZMO_IMG_UPDOWN1			QString(":/Updown1.png")
#define GIZMO_IMG_UPDOWN2			QString(":/Updown2.png")
#define GIZMO_IMG_DELETE1			QString(":/Delete1.png")
#define GIZMO_IMG_DELETE2			QString(":/Delete2.png")
#define GIZMO_IMG_LOCK1				QString(":/Lock1.png")
#define GIZMO_IMG_LOCK2				QString(":/Lock2.png")
#define GIZMO_IMG_UNLOCK1			QString(":/Unlock1.png")
#define GIZMO_IMG_UNLOCK2			QString(":/Unlock2.png")
#define GIZMO_IMG_SHOW1				QString(":/Unlock1.png")
#define GIZMO_IMG_SHOW2				QString(":/Unlock2.png")
#define GIZMO_IMG_HIDE1				QString(":/Lock1.png")
#define GIZMO_IMG_HIDE2				QString(":/Lock2.png")

#define ITEM_ROOT_PATH				"Items"
#define ITEM_INFO_XML_FILE_NAME		"product_information.XML"
#define ITEM_DRAG_MIME_TYPE			"application/room3d-item"
#define MSG_NOT_IMPLEMENTED			"This function was not implemented yet."
#define RESOURCE_FOLDER_NAME			"Resources"
#define RESOURCE_FOLDER_NAME			"Resources"
#define STR_CONFIG_FILE_NAME			"Config.conf"

#define DATABASE_TYPE					"QODBC"
#define DATABSE_CONNECT_NAME			"3DItemDevice"
#define DATABSE_CONNECT_OPTION			"SQL_ATTR_ACCESS_MODE=SQL_MODE_READ_WRITE;SQL_ATTR_TRACE=SQL_OPT_TRACE_OFF"
#define DATABASE_NAME_PREFIX			"DRIVER={Microsoft Access Driver (*.mdb)};FIL={MS Access};DBQ="

#define ITEM_DATABASE_FILE_NAME			"Items.mdb"
#define MASTER_TABLE_NAME				"MasterTable"
#define ITEM_TABLE_NAME					"ItemTable"

#define FIELD_CATEGORY_NAME_ENG			"CategoryName_Eng"
#define FIELD_CATEGORY_NAME_THAI		"CategoryName_Thai"
#define FIELD_CATEGORY_CODE				"CategoryCode"
#define FIELD_CATEGORY_CODE1			"CategoryCode1"
#define FIELD_CATEGORY_CODE2			"CategoryCode2"
#define FIELD_CATEGORY_CODE3			"CategoryCode3"

#define FIELD_PRODUCT_NAME				"ProductName"
#define FIELD_PRODUCT_ID				"ProductID"
#define FIELD_DESCRIPTION				"Description"
#define FIELD_PRICE						"Price"
#define FIELD_PRICE_NOTE				"PriceNote"
#define FIELD_WIDTH						"Width"
#define FIELD_LENGTH					"Length"
#define FIELD_HEIGHT					"Height"
#define FIELD_IMAGE_PATH				"ImagePath"
#define FIELD_MODEL_PATH				"ModelPath"
#define FIELD_UPDATED_DATE				"UpdatedDate"
#define FIELD_DELETE_FLAG				"DeleteFlag"

#define XML_TAG_PRODUCT_NAME			"Product_Name"
#define XML_TAG_PRODUCT_CATEGORY1		"Product_Category1"
#define XML_TAG_PRODUCT_CATEGORY2		"Product_Category2"
#define XML_TAG_PRODUCT_CATEGORY3		"Product_Category3"
#define XML_TAG_PRODUCT_ID				"Product_ID"
#define XML_TAG_PRODUCT_DESC			"Product_Description"
#define XML_TAG_PRODUCT_PRICE			"Product_Price"
#define XML_TAG_PRODUCT_PRICE_NOTE		"Product_Price_Note"
#define XML_TAG_PRODUCT_WIDTH			"Product_Width"
#define XML_TAG_PRODUCT_LENGTH			"Product_Length"
#define XML_TAG_PRODUCT_HEIGHT			"Product_Height"
#define XML_TAG_PRODUCT_UPDATE_YEAR		"Update_Year"
#define XML_TAG_PRODUCT_UPDATE_MONTH	"Update_Month"
#define XML_TAG_PRODUCT_UPDATE_DAY		"Update_Day"
#define XML_TAG_PRODUCT_IMG_PATH		"Product_Image_Path"
#define XML_TAG_PRODUCT_Path			"Product_Data_Path"

#define MODEL_FILE_NAME					"model.3ds"
#define IMAGE_FILE_NAME					"2d image.png"
#define RELATIVE_PATH_MASK				"Items"

#define ITEM_MORE_DLG_COLUMN_NAME			tr("Name")
#define ITEM_MORE_DLG_COLUMN_CATEGORY		tr("Category")
#define ITEM_MORE_DLG_COLUMN_PRICE			tr("Price")
#define ITEM_MORE_DLG_COLUMN_WIDTH			tr("Width")
#define ITEM_MORE_DLG_COLUMN_LENGTH			tr("Length")
#define ITEM_MORE_DLG_COLUMN_HEIGHT			tr("Height")
#define ITEM_MORE_DLG_COLUMN_IMAGE			tr("Image")

#endif // COMMON_H
