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
    QtSegmentControlPrivate(QtSegmentControl *myQ) : q(myQ), count(0), lastSelected(-1) {};
    ~QtSegmentControlPrivate() {};

    QtSegmentControl *q;
    QtSegmentControl::SelectionBehavior selectionBehavior;
    QSize iconSize;
    QVector<SegmentInfo> segments;
    int lastSelected;
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
    // Stuff Good.
    const SegmentInfo &info = segments.at(index);
}
