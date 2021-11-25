#pragma once

#include <QStyledItemDelegate>
class CRoom3DItemDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:
	CRoom3DItemDelegate(QWidget *parent = 0) : QStyledItemDelegate(parent) {}
	~CRoom3DItemDelegate(){}

	virtual void paint(QPainter *painter, const QStyleOptionViewItem &option,const QModelIndex &index) const;
	// editing
	virtual QWidget *createEditor(QWidget *parent,const QStyleOptionViewItem &option,const QModelIndex &index) const {return NULL;}

};

class CRoom3DMoreItemDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:
	CRoom3DMoreItemDelegate(QWidget *parent = 0) : QStyledItemDelegate(parent) {}
	~CRoom3DMoreItemDelegate(){}

	virtual void paint(QPainter *painter, const QStyleOptionViewItem &option,const QModelIndex &index) const;
	// editing
	virtual QWidget *createEditor(QWidget *parent,const QStyleOptionViewItem &option,const QModelIndex &index) const {return NULL;}

};
