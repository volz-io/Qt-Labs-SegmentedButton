#include <QtGui/QApplication>
#include <QtGui/QWidget>
#include <QtGui/QVBoxLayout>
#include "qtsegmentcontrol.h"

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    QWidget widget;
    QtSegmentControl *segmentControl = new QtSegmentControl(&widget);
    segmentControl->setSelectionBehavior(QtSegmentControl::SelectOne);
    segmentControl->setCount(5);
    segmentControl->setSegmentText(0, "Homer");
    segmentControl->setSegmentText(1, "Marge");
    segmentControl->setSegmentText(2, "Bart");
    segmentControl->setSegmentText(3, "Lisa");
    segmentControl->setSegmentText(4, "Maggie");

    QVBoxLayout *vboxlayout = new QVBoxLayout();
    vboxlayout->addWidget(segmentControl);
    widget.setLayout(vboxlayout);
    widget.show();

    return app.exec();
}
