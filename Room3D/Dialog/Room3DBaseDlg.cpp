#include "Room3DBaseDlg.h"

CRoom3DBaseDlg::CRoom3DBaseDlg(QWidget* a_pParent) : QDialog(a_pParent)
{
	setWindowModality(Qt::WindowModal);
}

CRoom3DBaseDlg::~CRoom3DBaseDlg(void)
{
}

void CRoom3DBaseDlg::paintEvent(QPaintEvent *e)
{
}