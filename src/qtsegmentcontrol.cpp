#include <QtGui/QIcon>
#include <QtGui/QMenu>
#include <QtGui/QPainter>
#include <QtGui/QStyleOption>
#include <QtGui/QMouseEvent>
#include <QtCore/QDebug>
#include <QtGui/QPixmapCache>

#include "qtsegmentcontrol.h"

#ifdef Q_WS_MAC
#include <Carbon/Carbon.h>

static ThemeDrawState getDrawState(QStyle::State flags)
{
    ThemeDrawState tds = kThemeStateActive;
    if (flags & QStyle::State_Sunken) {
        tds = kThemeStatePressed;
    } else if (flags & QStyle::State_Active) {
        if (!(flags & QStyle::State_Enabled))
            tds = kThemeStateUnavailable;
    } else {
        if (flags & QStyle::State_Enabled)
            tds = kThemeStateInactive;
        else
            tds = kThemeStateUnavailableInactive;
    }
    return tds;
}

#else

static void drawBorderPixmap(const QPixmap &pixmap, QPainter *painter, const QRect &rect,
                     int left, int top, int right,
                     int bottom)
{
    QSize size = pixmap.size();
    //painter->setRenderHint(QPainter::SmoothPixmapTransform);

    //top
    if (top > 0) {
        painter->drawPixmap(QRect(rect.left() + left, rect.top(), rect.width() -right - left, top), pixmap,
                            QRect(left, 0, size.width() -right - left, top));

        //top-left
        if(left > 0)
            painter->drawPixmap(QRect(rect.left(), rect.top(), left, top), pixmap,
                                QRect(0, 0, left, top));

        //top-right
        if (right > 0)
            painter->drawPixmap(QRect(rect.left() + rect.width() - right, rect.top(), right, top), pixmap,
                                QRect(size.width() - right, 0, right, top));
    }

    //left
    if (left > 0)
        painter->drawPixmap(QRect(rect.left(), rect.top()+top, left, rect.height() - top - bottom), pixmap,
                            QRect(0, top, left, size.height() - bottom - top));

    //center
    painter->drawPixmap(QRect(rect.left() + left, rect.top()+top, rect.width() -right - left,
                             rect.height() - bottom - top), pixmap,
                       QRect(left, top, size.width() -right -left,
                             size.height() - bottom - top));
    //right
    if (right > 0)
        painter->drawPixmap(QRect(rect.left() +rect.width() - right, rect.top()+top, right, rect.height() - top - bottom), pixmap,
                            QRect(size.width() - right, top, right, size.height() - bottom - top));

    //bottom
    if (bottom > 0) {
        painter->drawPixmap(QRect(rect.left() +left, rect.top() + rect.height() - bottom,
                                 rect.width() - right - left, bottom), pixmap,
                            QRect(left, size.height() - bottom,
                                 size.width() - right - left, bottom));
        //bottom-left
        if (left > 0)
            painter->drawPixmap(QRect(rect.left(), rect.top() + rect.height() - bottom, left, bottom), pixmap,
                                QRect(0, size.height() - bottom, left, bottom));

        //bottom-right
        if (right > 0)
            painter->drawPixmap(QRect(rect.left() + rect.width() - right, rect.top() + rect.height() - bottom, right, bottom), pixmap,
                                QRect(size.width() - right, size.height() - bottom, right, bottom));

    }
}

#endif


class QtStyleOptionSegmentControlSegment : public QStyleOption
{
public:
    enum StyleOptionType { Type = 100000  };
    enum StyleOptionVersion { Version = 1 };

    enum SegmentPosition { Beginning, Middle, End, OnlyOneSegment };
    enum SelectedPosition { NotAdjacent, NextIsSelected, PreviousIsSelected };

    QString text;
    QIcon icon;
    QSize iconSize;
    SegmentPosition position;
    SelectedPosition selectedPosition;

    QtStyleOptionSegmentControlSegment()
       : position(OnlyOneSegment), selectedPosition(NotAdjacent) { }
    QtStyleOptionSegmentControlSegment(const QtStyleOptionSegmentControlSegment &other)
        : QStyleOption(Version, Type) { *this = other; }

protected:
    QtStyleOptionSegmentControlSegment(int version);
};



static void drawSegmentControlSegmentSegment(const QStyleOption *option, QPainter *painter, QWidget *widget)
{
    // ### Change to qstyleoption_cast!
    if (const QtStyleOptionSegmentControlSegment *segment
            = static_cast<const QtStyleOptionSegmentControlSegment *>(option)) {
#ifdef Q_WS_MAC
        if (qobject_cast<QMacStyle *>(widget->style())) {
            CGContextRef cg = qt_mac_cg_context(painter->device());
            HIThemeSegmentDrawInfo sgi;
            bool selected = (segment->state & QStyle::State_Selected);
            sgi.version = 0;
            // Things look the same regardless of enabled.
            sgi.state = getDrawState(segment->state | QStyle::State_Enabled);
            sgi.value = selected ? kThemeButtonOn : kThemeButtonOff;
            sgi.size = kHIThemeSegmentSizeNormal;
            sgi.kind = kHIThemeSegmentKindNormal;
            sgi.adornment = kHIThemeSegmentAdornmentNone;
            switch (segment->position) {
            case QtStyleOptionSegmentControlSegment::Beginning:
                sgi.position = kHIThemeSegmentPositionFirst;
                if (segment->selectedPositions == QtStyleOptionSegmentControlSegment::NotAdjacent
                    || selected)
                    sgi.adornment |= kHIThemeSegmentAdornmentTrailingSeparator;
                break;
            case QtStyleOptionSegmentControlSegment::Middle:
                sgi.position = kHIThemeSegmentPositionMiddle;
                if (selected && !(segment->selectedPositions & QtStyleOptionSegmentControlSegment::PreviousIsSelected))
                    sgi.adornment |= kHIThemeSegmentAdornmentLeadingSeparator;
                if (selected || !(segment->selectedPositions & QtStyleOptionSegmentControlSegment::NextIsSelected)) // Also when we're selected.
                    sgi.adornment |= kHIThemeSegmentAdornmentTrailingSeparator;
                break;
            case QStyleOptionTab::End:
                sgi.position = kHIThemeSegmentPositionLast;
                if (selected && !(segment->selectedPositions & QtStyleOptionSegmentControlSegment::PreviousIsSelected))
                    sgi.adornment |= kHIThemeSegmentAdornmentLeadingSeparator;
                break;
            case QStyleOptionTab::OnlyOneTab:
                sgi.position = kHIThemeSegmentPositionOnly;
                break;
            }

            HIRect hirect = CGRectMake(segment->rect.x(), segment->rect.y(),
                                       segment->rect.width(), segment->rect.height());
            HIThemeDrawSegment(&hirect, &sgi, cg, kHIThemeOrientationNormal);
            CFRelease(cg);
        } else
#endif
        {
            Q_UNUSED(widget);
            painter->save();

            bool selected = (segment->state & QStyle::State_Selected);

            QPixmap pm;

            QSize segmentSize = widget->rect().size();
            QString key = QString("qt_segment %0 %1 %2").arg(option->state).arg(segmentSize.width()).arg(segmentSize.height());
            if (!QPixmapCache::find(key, pm)) {
                pm = QPixmap(segmentSize);
                pm.fill(Qt::transparent);
                QPainter pmPainter(&pm);
                QStyleOptionButton btnOpt;
                btnOpt.QStyleOption::operator =(*option);
                btnOpt.rect = QRect(QPoint(0, 0), segmentSize);;
                if (selected)
                    btnOpt.state |= QStyle::State_Sunken;
                else
                    btnOpt.state |= QStyle::State_Raised;

                widget->style()->drawPrimitive(QStyle::PE_PanelButtonCommand, &btnOpt, &pmPainter, widget);
                pmPainter.end();
                QPixmapCache::insert(key, pm);
            }
            int margin = widget->style()->pixelMetric(QStyle::PM_DefaultFrameWidth, option, widget);
            switch (segment->position) {
            case QtStyleOptionSegmentControlSegment::Beginning:
                painter->setClipRect(option->rect);
                painter->drawPixmap(0, 0, pm);
                painter->setOpacity(0.6);
                painter->setPen(option->palette.dark().color());
                painter->drawLine(option->rect.topRight() + QPoint(-1, margin), option->rect.bottomRight() + QPoint(-1, -margin));
                break;
            case QtStyleOptionSegmentControlSegment::Middle:
                painter->setClipRect(option->rect);
                painter->drawPixmap(0, 0, pm);
                painter->setPen(option->palette.dark().color());
                painter->drawLine(option->rect.topRight() + QPoint(-1, margin), option->rect.bottomRight() + QPoint(-1, -margin));
                break;
            case QStyleOptionTab::End:
                painter->setClipRect(option->rect);
                painter->drawPixmap(0, 0, pm);
                break;
            case QStyleOptionTab::OnlyOneTab:
                painter->setClipRect(option->rect);
                painter->drawPixmap(0, 0, pm);
                break;
            }
            painter->restore();
        }
    }
}

static QSize segmentSizeFromContents(const QStyleOption *option, const QSize &contentSize)
{
    QSize ret = contentSize;
    if (const QtStyleOptionSegmentControlSegment *segment
            = static_cast<const QtStyleOptionSegmentControlSegment *>(option)) {
        ret.rwidth() += 20;
        ret.rheight() += 10;
        if (!segment->icon.isNull())
            ret.rwidth() += 5;
    }
    return ret;
}

static void drawSegmentControlSegmentLabel(const QStyleOption *option, QPainter *painter, QWidget *)
{
    if (const QtStyleOptionSegmentControlSegment *segment
            = static_cast<const QtStyleOptionSegmentControlSegment *>(option)) {
#ifdef Q_WS_MAC
        if (qobject_cast<QMacStyle *>(widget->style())) {
            retRect.adjust(+11, +4, -11, -6);
            switch (segment->position) {
            default:
            case QtStyleOptionSegmentControlSegment::Middle:
                break;
            case QtStyleOptionSegmentControlSegment::Beginning:
            case QtStyleOptionSegmentControlSegment::End:
                retRect.adjust(+1, 0, -1, 0);
                break;
            case QtStyleOptionSegmentControlSegment::OnlyOneSegment:
                retRect.adjust(+2, 0, -2, 0);
                break;
            }
        } else
#endif
        {
            // retRect.adjust(-10, 0, 0, +10);
        }
        painter->drawText(segment->rect, Qt::AlignCenter, segment->text);
    }

}

static void drawSegmentControlFocusRect(const QStyleOption *option, QPainter *painter, QWidget *widget)
{
    widget->style()->drawPrimitive(QStyle::PE_FrameFocusRect, option, painter, widget);
}

static void drawSegmentControlSegment(const QStyleOption *option,
                                      QPainter *painter, QWidget *widget)
{
    drawSegmentControlSegmentSegment(option, painter, widget);
    drawSegmentControlSegmentLabel(option, painter, widget);
}


struct SegmentInfo {
    SegmentInfo() : menu(0), selected(false), enabled(true) {}
    ~SegmentInfo() { delete menu; }
    QString text;
    QString toolTip;
    QString whatsThis;
    QIcon icon;
    QMenu *menu;
    bool selected;
    bool enabled;
    QRect rect;
};

class QtSegmentControlPrivate {
public:
    QtSegmentControlPrivate(QtSegmentControl *myQ)
        : q(myQ), lastSelected(-1), layoutDirty(true), pressedIndex(-1), wasPressed(-1), focusIndex(-1) {};
    ~QtSegmentControlPrivate() {};

    void layoutSegments();
    void postUpdate(int index = -1, bool geoToo = false);

    QtSegmentControl *q;
    QtSegmentControl::SelectionBehavior selectionBehavior;
    QSize iconSize;
    QVector<SegmentInfo> segments;
    int lastSelected;
    bool layoutDirty;
    int pressedIndex;
    int wasPressed;
    int focusIndex;
    inline bool indexOK(int index) { return index >= 0 && index < segments.count(); }
};

void QtSegmentControlPrivate::layoutSegments()
{
    if (!layoutDirty)
        return;
    const int segmentCount = segments.count();
    QRect rect;
    for (int i = 0; i < segmentCount; ++i) {
        QSize ssh = q->segmentSizeHint(i);
        rect.setSize(ssh);
        segments[i].rect = rect;
        rect.setLeft(rect.left() + ssh.width());
    }
    layoutDirty = false;

}

void QtSegmentControlPrivate::postUpdate(int /*index*/, bool geoToo)
{
    if (geoToo) {
        layoutDirty = true;
        q->updateGeometry();
    }
    q->update();
}

QtSegmentControl::QtSegmentControl(QWidget *parent)
    : QWidget(parent), d(new QtSegmentControlPrivate(this))
{
    setFocusPolicy(Qt::TabFocus);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    setAttribute(Qt::WA_WState_OwnSizePolicy, false);
}

QtSegmentControl::~QtSegmentControl()
{
    delete d;
}

int QtSegmentControl::count() const
{
    return d->segments.count();
}

void QtSegmentControl::setCount(int newCount)
{
    d->segments.resize(newCount);
}

bool QtSegmentControl::isSegmentSelected(int index) const
{
    if (!d->indexOK(index))
        return false;

    return d->segments.at(index).selected;
}

int QtSegmentControl::selectedSegment() const
{
    return d->lastSelected;
}

void QtSegmentControl::setSegmentSelected(int index, bool selected)
{
    if (!d->indexOK(index))
        return;

    if (d->segments[index].selected != selected) {
        d->segments[index].selected = selected;
        d->lastSelected = index;
        if (d->selectionBehavior == SelectOne) {
            const int segmentCount = d->segments.count();
            for (int i = 0; i < segmentCount; ++i) {
                SegmentInfo &info = d->segments[i];
                if (i != index && info.selected) {
                    info.selected = false;
                    d->postUpdate(i);
                }
            }
        }
        d->postUpdate(index);
        emit segmentSelected(index);
    }
}

void QtSegmentControl::setSegmentEnabled(int index, bool enabled)
{
    if (!d->indexOK(index))
        return;

    if (d->segments[index].enabled != enabled) {
        d->segments[index].enabled = enabled;
        d->postUpdate(index);
    }
}

void QtSegmentControl::setSelectionBehavior(SelectionBehavior behavior)
{
    if (d->selectionBehavior == behavior)
        return;
    d->selectionBehavior = behavior;
    if (behavior == SelectOne) {
        // This call will do the right thing.
        setSegmentSelected(d->lastSelected, true);
    } else if (behavior == SelectNone) {
        d->lastSelected = -1;
        const int segmentCount = d->segments.count();
        for (int i = 0; i < segmentCount; ++i) {
            d->segments[i].selected = false;
        }
        d->postUpdate(-1);
    }
}

QtSegmentControl::SelectionBehavior QtSegmentControl::selectionBehavior() const
{
    return d->selectionBehavior;
}

void QtSegmentControl::setSegmentText(int index, const QString &text)
{
    if (!d->indexOK(index))
        return;

    if (d->segments[index].text != text) {
        d->segments[index].text = text;
        d->postUpdate(index, true);
    }
}

QString QtSegmentControl::segmentText(int index) const
{
    return d->indexOK(index) ? d->segments.at(index).text : QString();
}

void QtSegmentControl::setSegmentIcon(int index, const QIcon &icon)
{
    if (!d->indexOK(index))
        return;

    d->segments[index].icon = icon;
    d->postUpdate(index, true);
}

QIcon QtSegmentControl::segmentIcon(int index) const
{
    return d->indexOK(index) ? d->segments.at(index).icon : QIcon();
}

void QtSegmentControl::setIconSize(const QSize &size)
{
    if (d->iconSize == size)
        return;

    d->iconSize = size;
    d->postUpdate(-1, true);
}

QSize QtSegmentControl::iconSize() const
{
    return d->iconSize;
}

void QtSegmentControl::setSegmentMenu(int index, QMenu *menu)
{
    if (!d->indexOK(index))
        return;

    if (menu != d->segments[index].menu) {
        QMenu *oldMenu = d->segments[index].menu;
        d->segments[index].menu = menu;
        delete oldMenu;
        d->postUpdate(index, true);
    }
}

QMenu *QtSegmentControl::segmentMenu(int index) const
{
    return d->indexOK(index) ? d->segments.at(index).menu : 0;
}

void QtSegmentControl::setSegmentToolTip(int segment, const QString &tipText)
{
    if (!d->indexOK(segment))
        return;

    d->segments[segment].toolTip = tipText;
}

QString QtSegmentControl::segmentToolTip(int segment) const
{
    return d->indexOK(segment) ? d->segments.at(segment).toolTip : QString();
}

void QtSegmentControl::setSegmentWhatsThis(int segment, const QString &whatsThisText)
{
    if (!d->indexOK(segment))
        return;

    d->segments[segment].whatsThis = whatsThisText;
}

QString QtSegmentControl::segmentWhatsThis(int segment) const
{
    return d->indexOK(segment) ? d->segments.at(segment).whatsThis : QString();
}

QSize QtSegmentControl::segmentSizeHint(int segment) const
{
    QSize size;
    const SegmentInfo &segmentInfo = d->segments[segment];
    QFontMetrics fm(font());
    size = fm.size(0, segmentInfo.text);
    if (!segmentInfo.icon.isNull()) {
        QSize size2 = segmentInfo.icon.actualSize(iconSize());
        size.rwidth() += size2.width();
        size.rheight() = qMax(size.height(), size2.height());
    }
    QtStyleOptionSegmentControlSegment opt;
    opt.initFrom(this);
    opt.text = segmentInfo.text;
    opt.icon = segmentInfo.icon;
    opt.iconSize = d->iconSize;
    size = segmentSizeFromContents(&opt, size);
    return size;
}

QSize QtSegmentControl::sizeHint() const
{
    d->layoutSegments();
    QRect rect;
    const int segmentCount = d->segments.count();
    for (int i = 0; i < segmentCount; ++i) {
        rect = rect.united(segmentRect(i));
    }
    return rect.size();
}

QRect QtSegmentControl::segmentRect(int index) const
{
    return d->indexOK(index) ? d->segments[index].rect : QRect();
}

int QtSegmentControl::segmentAt(const QPoint &pos) const
{
    const int segmentCount = d->segments.count();
    for (int i = 0; i < segmentCount; ++i) {
        QRect rect = segmentRect(i);
        if (rect.contains(pos))
            return i;
    }
    return -1;
}


void QtSegmentControl::keyPressEvent(QKeyEvent *event)
{
    if (event->key() != Qt::Key_Left && event->key() != Qt::Key_Right
            && event->key() != Qt::Key_Space) {
        event->ignore();
        return;
    }
    qDebug() << "I got a key event";
}

void QtSegmentControl::paintEvent(QPaintEvent *)
{
    d->layoutSegments();
    QPainter p(this);
    p.fillRect(rect(), Qt::blue);
    QtStyleOptionSegmentControlSegment segmentInfo;
    const int segmentCount = d->segments.count();
    for (int i = 0; i < segmentCount; ++i) {
        initStyleOption(i, &segmentInfo);
        drawSegmentControlSegment(&segmentInfo, &p, this);
    }
}

void QtSegmentControl::mousePressEvent(QMouseEvent *event)
{
    d->wasPressed = d->pressedIndex = segmentAt(event->pos());
    d->postUpdate(d->pressedIndex);
}

void QtSegmentControl::mouseMoveEvent(QMouseEvent *event)
{
    int index = segmentAt(event->pos());
    if (index != d->wasPressed) {
        d->pressedIndex = -1;
        d->postUpdate(d->wasPressed);
    } else if (index == d->wasPressed && d->pressedIndex == -1) {
        d->pressedIndex = d->wasPressed;
        d->postUpdate(d->wasPressed);
    }
}

void QtSegmentControl::mouseReleaseEvent(QMouseEvent *event)
{
    int index = segmentAt(event->pos());
    // This order of reset is important.
    d->pressedIndex = -1;
    if (index == d->wasPressed && d->selectionBehavior != SelectNone) {
        if (d->selectionBehavior == SelectAll) {
            setSegmentSelected(index, !d->segments[index].selected);
        } else {
            setSegmentSelected(index, true);
        }
    }
    d->postUpdate(index);
    d->wasPressed = -1;
}

void QtSegmentControl::keyReleaseEvent(QKeyEvent *event)
{
    QWidget::keyReleaseEvent(event);
}

bool QtSegmentControl::event(QEvent *event)
{
    return QWidget::event(event);
}

void QtSegmentControl::initStyleOption(int segment, QStyleOption *option) const
{
    if (!option || !d->indexOK(segment))
        return;
    option->initFrom(this);
    if (segment == d->pressedIndex)
        option->state |= QStyle::State_Sunken;
    // ## Change to qstyleoption_cast
    if (QtStyleOptionSegmentControlSegment *sgi = static_cast<QtStyleOptionSegmentControlSegment *>(option)) {
        sgi->iconSize = d->iconSize;
        const SegmentInfo &segmentInfo = d->segments[segment];
        if (d->segments.count() == 1) {
            sgi->position = QtStyleOptionSegmentControlSegment::OnlyOneSegment;
        } else if (segment == 0) {
            sgi->position = QtStyleOptionSegmentControlSegment::Beginning;
        } else if (segment == d->segments.count() - 1) {
            sgi->position = QtStyleOptionSegmentControlSegment::End;
        } else {
            sgi->position = QtStyleOptionSegmentControlSegment::Middle;
        }

        if (segment == d->focusIndex)
            sgi->state |= QStyle::State_HasFocus;

        if (segmentInfo.selected) {
            sgi->state |= QStyle::State_Selected;
        } else {
            if (d->indexOK(segment - 1) && d->segments[segment - 1].selected) {
                sgi->selectedPosition = QtStyleOptionSegmentControlSegment::PreviousIsSelected;
            } else if (d->indexOK(segment + 1) && d->segments[segment + 1].selected) {
                sgi->selectedPosition = QtStyleOptionSegmentControlSegment::NextIsSelected;
            } else {
                sgi->selectedPosition = QtStyleOptionSegmentControlSegment::NotAdjacent;
            }
        }
        sgi->rect = segmentInfo.rect;
        sgi->text = segmentInfo.text;
        sgi->icon = segmentInfo.icon;
    }
}
