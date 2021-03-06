/*!
	Copyright (c) 2008-2020, Matevž Jekovec, Canorus development team
	All Rights Reserved. See AUTHORS for a complete list of authors.

	Licensed under the GNU GENERAL PUBLIC LICENSE. See LICENSE.GPL for details.
*/

#include "widgets/midirecorderview.h"
#include "core/midirecorder.h"

#include "canorus.h"

#include <QAction>
#include <QDir>

CAMidiRecorderView::CAMidiRecorderView(CAMidiRecorder* r, QWidget* parent)
    : QDockWidget(parent)
    , _midiRecorder(r)
{

    setupUi(this); // initialize elements created by Qt Designer
    setupCustomUi();

    _status = Idle;
}

CAMidiRecorderView::~CAMidiRecorderView()
{
    if (midiRecorder()) {
        delete midiRecorder();
        setMidiRecorder(nullptr);
    }

    if (parent() && dynamic_cast<CAMainWin*>(parent())) {
        static_cast<CAMainWin*>(parent())->setMidiRecorderView(nullptr);
    }
}

void CAMidiRecorderView::setupCustomUi()
{
    setAttribute(Qt::WA_DeleteOnClose, true);

    _timer = new QTimer();
    _timer->setInterval(1000);
    _timer->start();

    connect(_timer, SIGNAL(timeout()), this, SLOT(onTimerTimeout()));

    uiTime->setText("0:00");

    uiRecord->setEnabled(true);
    uiPause->setVisible(false);
    uiStop->setEnabled(false);
}

void CAMidiRecorderView::onTimerTimeout()
{
    if (_midiRecorder) {
        if (uiTime->text().isEmpty() || _status != Pause) {
            uiTime->setText(QString("%1:%2").arg((_midiRecorder->curTime() / 1000) / 60).arg((_midiRecorder->curTime() / 1000) % 60, 2, 10, QChar('0')));
        } else {
            uiTime->setText("");
        }
    }
}

void CAMidiRecorderView::on_uiPause_clicked(bool)
{
    uiPause->setVisible(false);
    uiRecord->setVisible(true);

    _midiRecorder->pauseRecording();
    _status = Pause;
}

void CAMidiRecorderView::on_uiStop_clicked(bool)
{
    uiRecord->setVisible(true);
    uiPause->setVisible(false);
    uiStop->setEnabled(false);

    _midiRecorder->stopRecording();
    _status = Idle;
}

void CAMidiRecorderView::on_uiRecord_clicked(bool)
{
    uiRecord->setVisible(false);
    uiPause->setVisible(true);
    uiStop->setEnabled(true);

    _midiRecorder->startRecording();
    _status = Recording;
}
