#include "Room3DItemDelegate.h"
#include <QStyleOptionViewItemV4>
#include <QPainter>

void CRoom3DItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
								const QModelIndex &index) const
{
	QStyleOptionViewItemV4 opt = option;
	initStyleOption(&opt, index);
	// draw the icon
	QIcon::Mode mode = QIcon::Normal;
	if (!(option.state & QStyle::State_Enabled))
		mode = QIcon::Disabled;
	else if (option.state & QStyle::State_Selected)
		mode = QIcon::Selected;
	QIcon::State state = option.state & QStyle::State_Open ? QIcon::On : QIcon::Off;
	QRect itemRect = option.rect;

	// In fact below code doesn't be needed , but it is nesessary to fix item draw error.
	opt.icon.pixmap(itemRect.width(), itemRect.height() , mode , state);
	
	opt.icon.paint(painter, itemRect, option.decorationAlignment, mode, state);

	itemRect.setSize(QSize(itemRect.width() - 1 , itemRect.height() - 1));
	QPen pen;
	if (option.state & QStyle::State_Selected)
	{
		pen.setStyle(Qt::SolidLine);
		pen.setColor(QColor(0,0,0));
	}
	else
	{
		pen.setStyle(Qt::DashLine);
		pen.setColor(QColor(128,128,128));
	}
	painter->setPen(pen);
	painter->drawRect(itemRect);

}

void CRoom3DMoreItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
								const QModelIndex &index) const
{
	if(index.column() == 6)
	{
		QStyleOptionViewItemV4 opt = option;
		initStyleOption(&opt, index);
		// draw the icon
		QIcon::Mode mode = QIcon::Normal;
		if (!(option.state & QStyle::State_Enabled))
			mode = QIcon::Disabled;
		else if (option.state & QStyle::State_Selected)
			mode = QIcon::Selected;
		QIcon::State state = option.state & QStyle::State_Open ? QIcon::On : QIcon::Off;
		QRect itemRect = option.rect;

		// In fact below code doesn't be needed , but it is nesessary to fix item draw error.
		opt.icon.pixmap(itemRect.width(), itemRect.height() , mode , state);

		opt.icon.paint(painter, itemRect, option.decorationAlignment, mode, state);

		itemRect.setSize(QSize(itemRect.width() - 1 , itemRect.height() - 1));
		QPen pen;
		if (option.state & QStyle::State_Selected)
		{
			pen.setStyle(Qt::SolidLine);
			pen.setColor(QColor(0,0,0));
		}
		else
		{
			pen.setStyle(Qt::DashLine);
			pen.setColor(QColor(128,128,128));
		}
		painter->setPen(pen);
		painter->drawRect(itemRect);

	}
	else
	{
		QStyledItemDelegate::paint(painter, option, index);
	}

}

