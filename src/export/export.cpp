/*!
	Copyright (c) 2007-2019, Matevž Jekovec, Canorus development team
	All Rights Reserved. See AUTHORS for a complete list of authors.

	Licensed under the GNU GENERAL PUBLIC LICENSE. See LICENSE.GPL for details.
*/

#include "export/export.h"
#include <QTextStream>

/*!
	\class CAExport
	\brief Base class for export filters

	This class inherits CAFile and is the base class for any specific import filter (eg. LilyPond,
	CanorusML, MusicXML etc.).

	If a developer wants to write a new export filter, he should:
	1) Create a new class with the base class CAExport
	2) Implement CAExport constructors and at least exportDocumentImpl() function
	3) Register the filter (put a new fileformat to CAFileFormats and add the filter to open/save
	   dialogs in CACanorus)

	Optionally:
	Developer should change the current status and progress while operations are in progress. He should
	also rewrite the readableStatus() function.

	The following example illustrates the usage of export class:
	\code
	  CAMyExportFilter export();
	  export.setStreamToFile("jingle bells.xml");
	  export.exportDocument( curDocument );

	  export.wait();
	\endcode
*/

CAExport::CAExport(QTextStream* stream)
    : CAFile()
{
    setStream(stream);

    setExportedDocument(nullptr);
    setExportedSheet(nullptr);
    setExportedStaff(nullptr);
    setExportedVoice(nullptr);
    setExportedLyricsContext(nullptr);
    setExportedFunctionMarkContext(nullptr);
}

CAExport::~CAExport()
{
}

/*!
	Executed when a new thread is dispatched.
	It looks which part of the document should be exported and starts the procedure.
	It emits the appropriate signal when the procedure is finished.
*/
void CAExport::run()
{
    if (!stream()) {
        setStatus(-1);
    } else {
        if (exportedDocument()) {
            exportDocumentImpl(exportedDocument());
            emit documentExported(exportedDocument());
        } else if (exportedSheet()) {
            exportSheetImpl(exportedSheet());
            emit sheetExported(exportedSheet());
        } else if (exportedStaff()) {
            exportStaffImpl(exportedStaff());
            emit staffExported(exportedStaff());
        } else if (exportedVoice()) {
            exportVoiceImpl(exportedVoice());
            emit voiceExported(exportedVoice());
        } else if (exportedLyricsContext()) {
            exportLyricsContextImpl(exportedLyricsContext());
            emit lyricsContextExported(exportedLyricsContext());
        } else if (exportedFunctionMarkContext()) {
            exportFunctionMarkContextImpl(exportedFunctionMarkContext());
            emit functionMarkContextExported(exportedFunctionMarkContext());
        }

        stream()->flush();
        if (status() > 0) { // error - bad implemented filter
            // job is finished but status is still marked as working, set to Ready to prevent infinite loops
            setStatus(0);
        }
    }

    emit exportDone(status());
}

void CAExport::exportDocument(CADocument* doc, bool bStartThread)
{
    setExportedDocument(doc);
    setStatus(1); // process started
    if (bStartThread)
        start();
    else {
        if (!stream()) {
            setStatus(-1);
        } else {
            if (exportedDocument()) {
                exportDocumentImpl(exportedDocument());
                emit documentExported(exportedDocument());
            }
            stream()->flush();
            if (status() > 0) { // error - bad implemented filter
                // job is finished but status is still marked as working, set to Ready to prevent infinite loops
                setStatus(0);
            }
        }
        emit exportDone(status());
    }
}

void CAExport::exportSheet(CASheet* sheet)
{
    setExportedSheet(sheet);
    setStatus(1); // process started
    start();
}

void CAExport::exportStaff(CAStaff* staff)
{
    setExportedStaff(staff);
    setStatus(1); // process started
    start();
}

void CAExport::exportVoice(CAVoice* voice)
{
    setExportedVoice(voice);
    setStatus(1); // process started
    start();
}

void CAExport::exportLyricsContext(CALyricsContext* lc)
{
    setExportedLyricsContext(lc);
    setStatus(1); // process started
    start();
}

void CAExport::exportFunctionMarkContext(CAFunctionMarkContext* fmc)
{
    setExportedFunctionMarkContext(fmc);
    setStatus(1); // process started
    start();
}

const QString CAExport::readableStatus()
{
    switch (status()) {
    case 1:
        return tr("Exporting");
    case 0:
        return tr("Ready");
    case -1:
        return tr("Unable to open file for writing");
    }
    return tr("Ready");
}
