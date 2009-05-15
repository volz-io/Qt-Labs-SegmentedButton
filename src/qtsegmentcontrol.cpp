#include <QtGui/QIcon>
#include <QtGui/QMenu>
#include <QtGui/QPainter>
#include <QtGui/QStyleOption>
#include <QtCore/QDebug>

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

#endif

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
    QtSegmentControlPrivate(QtSegmentControl *myQ) : q(myQ), lastSelected(-1), layoutDirty(true) {};
    ~QtSegmentControlPrivate() {};

    void layoutSegments();
    void postUpdate(int index = -1, bool geoToo = false);

    QtSegmentControl *q;
    QtSegmentControl::SelectionBehavior selectionBehavior;
    QSize iconSize;
    QVector<SegmentInfo> segments;
    int lastSelected;
    bool layoutDirty;
    inline bool indexOK(int index) { return index >= 0 && index < segments.count(); }
};

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

static void drawSegmentControlSegmentSegment(const QStyleOption *option, QPainter *painter, QWidget *)
{
#ifdef Q_WS_MAC
    // ### Change to qstyleoption_cast!
    if (const QtStyleOptionSegmentControlSegment *segment
            = static_cast<const QtStyleOptionSegmentControlSegment *>(option)) {
        CGContextRef cg = qt_mac_cg_context(painter->device());
        HIThemeSegmentDrawInfo sgi;
        sgi.version = 0;
        sgi.state = getDrawState(segment->state);
        sgi.value = (segment->state & QStyle::State_Selected) ? kThemeButtonOn : kThemeButtonOff;
        sgi.size = kHIThemeSegmentSizeNormal;
        sgi.kind = kHIThemeSegmentKindNormal;
        sgi.position = segment->position;
        sgi.adornment = kHIThemeSegmentAdornmentNone;
        HIRect hirect = CGRectMake(segment->rect.x(), segment->rect.y(),
                                   segment->rect.width(), segment->rect.height());
        HIThemeDrawSegment(&hirect, &sgi, cg, kHIThemeOrientationNormal);
        CFRelease(cg);
    }
#else
    painter->drawRect(option->rect, QColor(0, 255, 0, 135));
#endif
}

static QSize segmentSizeFromContents(const QStyleOption *option, const QSize &contentSize)
{
    QSize ret = contentSize;
    if (const QtStyleOptionSegmentControlSegment *segment
            = static_cast<const QtStyleOptionSegmentControlSegment *>(option)) {
        ret.rwidth() += 20;
        if (!segment->icon.isNull())
            ret.rwidth() += 5;
    }
    return ret;
}

static void drawSegmentControlSegmentLabel(const QStyleOption *option, QPainter *painter, QWidget *)
{
    if (const QtStyleOptionSegmentControlSegment *segment
            = static_cast<const QtStyleOptionSegmentControlSegment *>(option)) {
        painter->drawText(segment->rect, Qt::AlignCenter, segment->text);
    }

}

static void drawSegmentControlSegment(const QStyleOption *option,
                                      QPainter *painter, QWidget *widget)
{
    drawSegmentControlSegmentSegment(option, painter, widget);
    drawSegmentControlSegmentLabel(option, painter, widget);
}

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
    // ### change up selection and update()
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
        rect.unite(segmentRect(i));
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
    QWidget::mousePressEvent(event);
}

void QtSegmentControl::mouseMoveEvent(QMouseEvent *event)
{
    QWidget::mouseMoveEvent(event);
}

void QtSegmentControl::mouseReleaseEvent(QMouseEvent *event)
{
    QWidget::mouseReleaseEvent(event);
}

void QtSegmentControl::keyPressEvent(QKeyEvent *event)
{
    QWidget::keyPressEvent(event);
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
