/*
   A whole bunch of stuff here.
*/

#include <QtGui/QWidget>

class QtSegmentedControl
{
    Q_OBJECT
public:
    enum SelectionBehavior { SelectOne, SelectAll, SelectNone }

    QtSegmentedControl(QWidget *parent = 0);
    ~QtSegmentedControl();

    int count() const;
    void setCount();

    bool isSegmentSelected(int index) const;
    void setSegmentSelected(int index);
    int selectedSegment() const;

    void setSegmentEnabled(int index, bool enable);
    bool segmentEnabled(int index) const;

    void setSelectionBehavior(SelectionBehavior behavior);
    SelectionBehavior selectionBehavior() const;

    void setSegmentText(const QString &text);
    QString segmentText() const;

    void setSegmentIcon(const QIcon &icon);
    QIcon segmentIcon() const;

    void setIconSize(const QSize &size);
    QSize iconSize() const;

    void setMenu(QMenu *menu);
    QMenu *menu() const;

    void setSegmentToolTip(int segment, const QString &tipText);
    QString segmentToolTip(int segment) const;

    void setSegmentWhatsThis(int segment, const QString &whatsThisText);
    QString segmentWhatsThis(int segment) const;

    virtual QSize segmentSizeHint(const QSize &size) const;
};
