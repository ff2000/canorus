/*!
	Copyright (c) 2007-2019, Matevž Jekovec, Canorus development team
	All Rights Reserved. See AUTHORS for a complete list of authors.

	Licensed under the GNU GENERAL PUBLIC LICENSE. See LICENSE.GPL for details.
*/

#ifndef MIDIEXPORT_H_
#define MIDIEXPORT_H_

#include <QByteArray>
#include <QList>
#include <QString>
#include <QTextStream>
#include <QVector>

#include "score/barline.h"
#include "score/clef.h"
#include "score/document.h"
#include "score/keysignature.h"
#include "score/lyricscontext.h"
#include "score/note.h"
#include "score/rest.h"
#include "score/syllable.h"
#include "score/timesignature.h"

#include "export/export.h"
#include "interface/mididevice.h"
#include "interface/playback.h"

class CAMidiExport : public CAExport, public CAMidiDevice {
public:
    CAMidiExport(QTextStream* out = 0);

    /*
    //////////////////////////////
    // Interface to file export //
    //////////////////////////////
*/
    QMap<int, QString> getOutputPorts() { return m_InputPorts; }
    QMap<int, QString> getInputPorts() { return m_OutputPorts; }

    bool openOutputPort(int) { return true; } // return true on success, false otherwise
    bool openInputPort(int) { return true; } // return true on success, false otherwise
    void closeOutputPort() {}
    void closeInputPort() {}
    void send(QVector<unsigned char> message, int time);
    void sendMetaEvent(int timeLength, char event, char a, char b, int c);
    void writeFile(); // direct access to the writing

    /*
	///////////////////////////
	// Polling export status //
	///////////////////////////
	// Setter methods are private!
*/

private:
    QByteArray writeTime(int time);
    void exportDocumentImpl(CADocument* doc);
    void exportSheetImpl(CASheet* sheet);
    int midiTrackCount;
    QByteArray trackChunk; // for the time beeing we build one big track
    int timeIncrement(int time);
    int _trackTime; // which this is the time line for
    QVector<QByteArray> trackChunks; // for the future
    QVector<int> trackTimes;
    void streamQByteArray(QByteArray x); // streaming binary data to midi file, possibly with print for debugging
    QByteArray variableLengthValue(int value);
    QByteArray word16(char x);
    QByteArray textEvent(int time, QString s);
    QByteArray trackEnd(void);
    QByteArray timeSignature(void);
    QByteArray keySignature(void);
    void setChunkLength(QByteArray* x);

    /*

	////////////////////
	// Helper methods //
	////////////////////

	///////////////////////////
	// Getter/Setter methods //
	///////////////////////////
*/
    inline void setCurVoice(CAVoice* voice) { _curVoice = voice; }
    inline void setCurSheet(CASheet* sheet) { _curSheet = sheet; }
    /*
	/////////////
	// Members //
	/////////////
	QTextStream *_out;
*/
    CAVoice* _curVoice;
    CASheet* _curSheet;

    // Dummy members for getOutputPorts/getInputPorts
    QMap<int, QString> m_InputPorts;
    QMap<int, QString> m_OutputPorts;

    /*
	CAContext *_curContext;
	int _curContextIndex;
	int _curIndentLevel;
*/
};

#endif /* MIDIEXPORT_H_*/
