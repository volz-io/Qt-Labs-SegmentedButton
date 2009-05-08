#include <QtGui/QApplication>
#include <QtGui/QWidget>
#include <QtGui/QVBoxLayout>
#include "qtsegmentcontrol.h"

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    QWidget widget;
    widget.resize(200, 200);
    QtSegmentControl *segmentControl = new QtSegmentControl(&widget);
    segmentControl->setCount(5);
    segmentControl->setSegmentText(0, "Homer");
    segmentControl->setSegmentText(1, "Marge");
    segmentControl->setSegmentText(2, "Bart");
    segmentControl->setSegmentText(3, "Lisa");
    segmentControl->setSegmentText(4, "Maggie");

    QVBoxLayout *vboxlayout = new QVBoxLayout(&widget);
    vboxlayout->addWidget(segmentControl);
    widget.show();

    return app.exec();
}
