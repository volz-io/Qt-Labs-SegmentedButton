/*
   A whole bunch of stuff here.
*/

#include <QtGui/QWidget>
class QMenu;

class QtSegmentControlPrivate;

class QtSegmentedControl
{
    Q_OBJECT
    Q_ENUMS(SelectionBehavior)
    Q_PROPERTY(SelectionBehavior READ selectionBehavior WRITE setSelectionBehavior)
    Q_PREPERTY(selectedSection READ selectedSection NOTIFY sectionSelected)
    Q_PREPERTY(int count READ count WRITE setCount)
    Q_PREPERTY(QSize iconSize READ iconSize WRITE setIconSize)
public:
    enum SelectionBehavior { SelectOne, SelectAll, SelectNone }

    QtSegmentedControl(QWidget *parent = 0);
    ~QtSegmentedControl();

    int count() const;
    void setCount();

    bool isSegmentSelected(int index) const;
    int selectedSegment() const;
    void setSegmentSelected(int index, bool selected);

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

    void setSegmentMenu(int segment, QMenu *menu);
    QMenu *segmentMenu(int segment) const;

    void setSegmentToolTip(int segment, const QString &tipText);
    QString segmentToolTip(int segment) const;

    void setSegmentWhatsThis(int segment, const QString &whatsThisText);
    QString segmentWhatsThis(int segment) const;

    virtual QSize segmentSizeHint(int segment, const QSize &size) const;

Q_SIGNALS:
    void segmentSelected(int selected);

private:
    QtSegmentControlPrivate *d;
};
