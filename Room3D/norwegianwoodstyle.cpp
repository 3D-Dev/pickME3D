#include <QtGui>

#include "norwegianwoodstyle.h"

void NorwegianWoodStyle::polish(QPalette &palette)
{
}

void NorwegianWoodStyle::polish(QWidget *widget)
{
    if (qobject_cast<QPushButton *>(widget)
            || qobject_cast<QComboBox *>(widget))
        widget->setAttribute(Qt::WA_Hover, true);
}

void NorwegianWoodStyle::unpolish(QWidget *widget)
{
    if (qobject_cast<QPushButton *>(widget)
            || qobject_cast<QComboBox *>(widget))
        widget->setAttribute(Qt::WA_Hover, false);
}

int NorwegianWoodStyle::pixelMetric(PixelMetric metric,
                                    const QStyleOption *option,
                                    const QWidget *widget) const
{
    switch (metric) {
		case PM_IndicatorHeight:
			return QCleanlooksStyle::pixelMetric(metric, option, widget) + 24;
		case PM_IndicatorWidth:
			return QCleanlooksStyle::pixelMetric(metric, option, widget) + 24;
		case PM_ComboBoxFrameWidth:
    default:
        return QCleanlooksStyle::pixelMetric(metric, option, widget);
    }
}

int NorwegianWoodStyle::styleHint(StyleHint hint, const QStyleOption *option,
                                  const QWidget *widget,
                                  QStyleHintReturn *returnData) const
{
    switch (hint) {
    default:
        return QCleanlooksStyle::styleHint(hint, option, widget, returnData);
    }
}

void NorwegianWoodStyle::drawPrimitive(PrimitiveElement element,
                                       const QStyleOption *option,
                                       QPainter *painter,
                                       const QWidget *widget) const
{
    switch (element) {
	case PE_IndicatorCheckBox:
		{
		QPixmap focused(CHECKBOX_IMG_FOCUSED);
		QPixmap clicked(CHECKBOX_IMG_CLICKED);
		QPixmap bgimg(CHECKBOX_IMG_BACK);
		QPixmap checked(CHECKBOX_IMG_CHECKED);
		bool darker;
		QBrush brush;
		QRect roundRect = option->rect;
		const QStyleOptionButton *checkOption =
			qstyleoption_cast<const QStyleOptionButton *>(option);
		painter->save();
		painter->setRenderHint(QPainter::Antialiasing, true);
		painter->drawPixmap(roundRect, bgimg);
		if ( option->state & (State_MouseOver) )
			painter->drawPixmap(roundRect, focused);
		if (option->state & (State_On)) 
			painter->drawPixmap(roundRect, checked);
		if (option->state & (State_Sunken)) 
			painter->drawPixmap(roundRect, clicked);
		painter->restore();

	}
	break;

    default:
        QCleanlooksStyle::drawPrimitive(element, option, painter, widget);
    }
}

void NorwegianWoodStyle::drawControl(ControlElement element,
                                     const QStyleOption *option,
                                     QPainter *painter,
                                     const QWidget *widget) const
{
    switch (element) {
    default:
        QCleanlooksStyle::drawControl(element, option, painter, widget);
    }
}

void NorwegianWoodStyle::setTexture(QPalette &palette, QPalette::ColorRole role,
                                    const QPixmap &pixmap)
{
}

QPainterPath NorwegianWoodStyle::roundRectPath(const QRect &rect)
{
    int radius = qMin(rect.width(), rect.height()) / 2;
    int diam = 2 * radius;

    int x1, y1, x2, y2;
    rect.getCoords(&x1, &y1, &x2, &y2);

    QPainterPath path;
    path.moveTo(x2, y1 + radius);
    path.arcTo(QRect(x2 - diam, y1, diam, diam), 0.0, +90.0);
    path.lineTo(x1 + radius, y1);
    path.arcTo(QRect(x1, y1, diam, diam), 90.0, +90.0);
    path.lineTo(x1, y2 - radius);
    path.arcTo(QRect(x1, y2 - diam, diam, diam), 180.0, +90.0);
    path.lineTo(x1 + radius, y2);
    path.arcTo(QRect(x2 - diam, y2 - diam, diam, diam), 270.0, +90.0);
    path.closeSubpath();
    return path;
}