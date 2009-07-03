#include <QtGui/QApplication>
#include <QtGui/QWidget>
#include <QtGui/QCheckBox>
#include <QtGui/QVBoxLayout>
#include "qtsegmentcontrol.h"

class MyWindow : public QWidget
{
    Q_OBJECT
public:
    MyWindow() {
        controller = new QtSegmentControl();
        controller->setCount(4);
        controller->setSegmentText(0, tr("Select One"));
        controller->setSegmentText(1, tr("Select All"));
        controller->setSegmentText(2, tr("Select None"));
        controller->setSegmentText(3, tr("Permanently Disabled"));
        controller->setSegmentEnabled(3, false);
        controller->setSelectionBehavior(QtSegmentControl::SelectOne);
        connect(controller, SIGNAL(segmentSelected(int)), this, SLOT(updateBehavior(int)));
        segmentControl = new QtSegmentControl();
        segmentControl->setCount(5);
        segmentControl->setSegmentText(0, "Homer");
        segmentControl->setSegmentText(1, "Marge");
        segmentControl->setSegmentText(2, "Bart");
        segmentControl->setSegmentText(3, "Lisa");
        segmentControl->setSegmentText(4, "Maggie");
        controller->setSegmentSelected(0, true);

        QCheckBox *button = new QCheckBox(tr("Disable Bart"));
        button->setChecked(false);
        connect(button, SIGNAL(toggled(bool)), SLOT(updateBart(bool)));


        QVBoxLayout *vboxlayout = new QVBoxLayout();
        vboxlayout->addWidget(controller);
        vboxlayout->addWidget(segmentControl);
        vboxlayout->addWidget(button);

        button = new QCheckBox(tr("Disable the whole thing"));
        button->setChecked(false);
        connect(button, SIGNAL(toggled(bool)), SLOT(updateDisabledController(bool)));

        vboxlayout->addWidget(button);
        setLayout(vboxlayout);
    }

private slots:
    void updateBehavior(int newOne) {
        segmentControl->setSelectionBehavior(QtSegmentControl::SelectionBehavior(newOne));
    }

    void updateBart(bool enabled) {
        segmentControl->setSegmentEnabled(2, !enabled);
    }

    void updateDisabledController(bool disabled)
    {
        segmentControl->setDisabled(disabled);
    }

private:
    QtSegmentControl *controller;
    QtSegmentControl *segmentControl;
};


int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    MyWindow window;
    window.show();
    return app.exec();
}

#include "main.moc"
