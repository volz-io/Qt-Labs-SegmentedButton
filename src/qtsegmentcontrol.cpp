#include "qtsegmentcontrol.h"

struct SegmentInfo {
    SegmentInfo() : menu(0), selected(false) {}
    ~SegmentInfo() { delete menu; }
    QString text;
    QString toolTip;
    QString whatsThis;
    QIcon icon;
    QMenu *menu;
    bool selected;
};

class QtSegmentControlPrivate {
public:
    QtSegmentControlPrivate(QtSegmentControl *myQ) : q(myQ), lastSelected(-1) {};
    ~QtSegmentControlPrivate() {};

    QtSegmentControl *q;
    QtSegmentControl::SelectionBehavior selectionBehavior;
    QSize iconSize;
    QVector<SegmentInfo> segments;
    int lastSelected;
    inline bool indexOK(int index) { return index >= 0 && index < segments.count(); }
};

QtSegmentControl::QtSegmentControl(QWidget *parent)
    : QWidget(parent), d(new QtSegmentControl(this))
{
}

QtSegmentControl::~QtSegmentControl()
{
    delete d;
}

int QtSegmentControl::count() const
{
    return segments.count();
}

void QtSegmentControl::setCount(int newCount)
{
    segments.resize(newCount);
}

bool QtSegmentControl::isSegmentSelected(int index) const
{
    if (!indexOK(index))
        return false;

    return segments.at(index).selected;
}

int QtSegmentControl::selectedSegment() const
{
    return d->lastSelected;
}

void QtSegmentControl::setSegmentSelected(int index, bool selected)
{
    if (!indexOK(index))
        return;

    if (segmentInfo[index].selected != selected) {
        segmentInfo[index].selected = selected;
        update(); // ### segment rect;
        emit segmentSelected(index);
    }
}

void QtSegmentControl::setSegmentEnabled(int index, bool enabled)
{
    if (!indexOK(index))
        return;

    if (segmentInfo[index].enabled != enabled) {
        segmentInfo[index].enabled = enabled;
        update(); // ### segment rect
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
    if (!indexOK(index))
        return;

    if (d->segmentInfo[index].text != text) {
        d->segmentInfo[index].text = text;
        update(); // ### segment rect;
    }
}

QString QtSegmentControl::segmentText(int index) const
{
    indexOK(index) ? d->segmentInfo.at(index).text : QString();
}

void QtSegmentControl::setSegmentIcon(int index, const QIcon &icon)
{
    if (!indexOK(index))
        return;

    if (d->segmentInfo[index].icon != icon) {
        d->segmentInfo[index].icon = icon;
        update(); // ## segment rect;
    }
}

QIcon QtSegmentControl::segmentIcon(int index)
{
    indexOK(index) ? d->segmentInfo.at(index).icon : QIcon();
}

void QtSegmentControl::setIconSize(const QSize &size)
{
    if (d->iconSize == size)
        return;

    d->iconSize = size;
    update();
}

QSize QtSegmentControl::iconSize() const
{
    return d->iconSize;
}

void QtSegmentControl::setSegmentMenu(int index, QMenu *menu)
{
    if (!indexOK(index))
        return;

    if (menu != d->segmentInfo[index].menu) {
        QMenu *oldMenu = d->segmentInfo[index].menu;
        d->segmentInfo[index].menu = menu;
        delete oldMenu;
        update(); // ### segment rect
    }
}

QMenu *QtSegmentControl::segmentMenu(int index) const
{
    indexOK(index) ? d->segmentInfo.at(index).menu : 0;
}

void QtSegmentControl::setSegmentToolTip(int segment, const QString &tipText)
{
    if (!indexOK(index))
        return;

    d->segmentInfo[segment].toolTipText = tipText;
}

QString QtSegmentControl::toolTipText(int segment) const
{
    return d->indexOK(segment) ? d->segmentInfo.at(segment).toolTipText : QString();
}

void QtSegmentControl::setSegmentWhatsThis(int segment, const QString &whatsThisText)
{
    if (!d->indexOK(segment))
        return;

    d->segmentInfo[segment].whatsThisText = whatsThisText;
}

QString QtSegmentControl::segmentWhatsThis(int segment) const
{
    return d->indexOK(segment) ? d->segmentInfo.at(segment).whatsThisText : QString();
}

QSize QtSegmentControl::segmentSizeHint(int segment, const QSize &size) const
{
    return QSize(20, 20);
}


void QtSegmentControl::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.fillRect(rect(), Qt::blue);
}

void QtSegmentControl::mousePressEvent(QMouseEvent *event)
{
    return QWidget::mousePressEvent(event);
}

void QtSegmentControl::mouseMoveEvent(QMouseEvent *event)
{
    return QWidget::mouseMoveEvent(event);
}

void QtSegmentControl::mouseReleaseEvent(QMouseEvent *event)
{
    return QWidget::mouseReleaseEvent(event);
}

void QtSegmentControl::keyPressEvent(QKeyEvent *event)
{
    return QWidget::keyPressEvent(event);
}

void QtSegmentControl::keyReleaseEvent(QKeyEvent *event)
{
    return QWidget::keyReleaseEvent(event);
}

bool QtSegmentControl::event(QEvent *event)
{
    return QWidget::event(event);
}

