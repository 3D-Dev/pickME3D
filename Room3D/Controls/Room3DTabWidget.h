#ifndef _TABVIEWCONTAINER_H
#define _TABVIEWCONTAINER_H

#include <QtGui/QWidget>
#include <QtGui/QPushButton>
#include <QtGui/QStackedWidget>
#include <QPainter>

#define TAB_NUM		7

class CTabButton : public QPushButton
{
	Q_OBJECT
public:
	CTabButton(QWidget *pParent, QString caption);
	~CTabButton();
private:
	QString m_Caption;
	int			m_nTabIndex;

public:
	int GetTabIndex() {return m_nTabIndex;}
	void SetTabIndex(int index) {m_nTabIndex = index;}
protected:
	void paintEvent(QPaintEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event) {}
};

class CTabViewContainer : public QWidget
{
	Q_OBJECT
public:
	CTabViewContainer(QWidget *pParent);
	~CTabViewContainer();
private:
	CTabButton* m_pTabs[7];
	QStackedWidget m_cPages;
public:
	void addTab(QWidget *page, QString& label);
	void OnUpdateTabs(int nTabIndex);

	int count() {return m_cPages.count();}
	QWidget* widget(int a_nIndex) { return m_cPages.widget(a_nIndex);}
	QWidget* currentWidget() { return m_cPages.currentWidget();}
	void setCurrentWidget(QWidget* a_pWidget);
	void clear();

	void Init();

signals:
		void currentChanged(int);
protected:
	void paintEvent(QPaintEvent *event);
	void resizeEvent(QResizeEvent *event);
};

#endif //_TABVIEWCONTAINER_H