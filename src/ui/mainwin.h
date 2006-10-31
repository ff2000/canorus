/*****************************************************************************/
/*									     */
/* This program is free software; you can redistribute it and/or modify it   */
/* under the terms of the GNU General Public License as published by the     */ 
/* Free Software Foundation; version 2 of the License.	                     */
/*									     */
/* This program is distributed in the hope that it will be useful, but       */
/* WITHOUT ANY WARRANTY; without even the implied warranty of                */ 
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General  */
/* Public License for more details.                                          */
/*									     */
/* You should have received a copy of the GNU General Public License along   */
/* with this program; (See "LICENSE.GPL"). If not, write to the Free         */
/* Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA        */
/* 02111-1307, USA.				                             */
/*									     */
/*---------------------------------------------------------------------------*/
/*									     */
/*		Reinhard Katzmann, GERMANY			             */
/*		reinhard@suamor.de					     */
/*									     */
/*		Matevž Jekovec, SLOVENIA                                     */
/*		matevz.jekovec@guest.arnes.si				     */
/*									     */
/*****************************************************************************/

#include <QObject>

#include "ui_mainwin.h"

#include "core/document.h"
#include "core/muselement.h"
#include "core/note.h"
#include "core/clef.h"

class QKeyEvent;
class QSlider;
class CARtMidiDevice;
class CAPlayback;
class CAToolBar;
class CAButtonMenu;
class CALCDNumber;
class CASheet;
class CAKeySigPSP;
class CAPluginManager;

enum CAMode {
	InsertMode,
	SelectMode,
	EditMode,
	ReadOnlyMode
};

enum CAFixedTimeSig {
	TS_44,
	TS_22,
	TS_34,
	TS_24,
	TS_38,
	TS_68
};

class CAScrollWidget;
class CAViewPort;
class CAScoreViewPort;

#define _currentScrollWidget ((CAScrollWidget*)(moMainWin.tabWidget->currentWidget()))

class CAMainWin : public QMainWindow
{
	Q_OBJECT

public:
	CAMainWin(QMainWindow *oParent = 0);
	~CAMainWin();

	void initToolBar();
	void newDocument();
	
	////////////////////////////////////////////////////
	//Score operations
	////////////////////////////////////////////////////
	void addSheet(CASheet *s);
	void insertMusElementAt(const QPoint coords, CAScoreViewPort *v);
	
	/**
	 * Delete all viewports and its contents.
	 * Delete all signals.
	 * Release all buttons and modes.
	 * 
	 * WARNING! This function delets the UI only (drawable elements). All the data classes should stay intact. Use _document.clear() in order to clear the data part as well.
	 */
	void clearUI();
	
	CAScrollWidget* currentScrollWidget() { return _currentScrollWidget; }
	
	/**
	 * Rebuild the UI from the data part.
	 * 
	 * This method is called eg. when multiple viewports share the same logical source and a change has been made in the logical musElement list.
	 * This way, sheet argument is a pointer to the data sheet where the change occured and viewports showing the given sheet are updated (CAEngraver create CADrawable elements for every viewport).
	 * If no sheet argument is passed, the whole UI is rebuilt from the data part. This is called for eg. on Open file after the data part has been read.
	 * 
	 * @param sheet Pointer to the common CASheet viewports use.
	 * @param repaint Should the viewports be repainted as well (sometimes we want just engraver to generate the drawable notes and wait for other events and then repaint once).
	 */
	void rebuildUI(CASheet *sheet=0, bool repaint=true);

	void initMidi();

private slots:
	////////////////////////////////////////////////////
	//Menu bar actions
	////////////////////////////////////////////////////
	void closeEvent(QCloseEvent *event);
	//File menu
	void on_actionNew_activated();
	void on_actionNew_sheet_activated();
	void on_actionOpen_activated();
	void on_actionSave_activated();
	void on_actionSave_as_activated();
	
	//Insert menu
	void on_actionNew_staff_activated();
	void on_action_Clef_activated();
	void on_action_Key_signature_activated();
	
	//View menu
	void on_action_Fullscreen_toggled(bool);
	void on_actionAnimated_scroll_toggled(bool);
	void on_actionLock_scroll_playback_toggled(bool);
	void on_actionZoom_to_selection_activated();
	void on_actionZoom_to_fit_activated();
	void on_actionZoom_to_width_activated();
	void on_actionZoom_to_height_activated();
	
	//Playback menu
	void on_actionPlay_toggled(bool);
	
	//Perspective menu
	void on_actionSource_view_perspective_activated();
	
	//Window menu
	void on_actionSplit_horizontally_activated();
	void on_actionSplit_vertically_activated();
	void on_actionUnsplit_activated();
	void on_actionNew_viewport_activated();
	
	//Help menu
	void on_actionAbout_Qt_activated();
	void on_actionAbout_Canorus_activated();

	// Toolbar 
	void sl_mpoVoiceNum_valChanged(int iVoice);
	void on_actionNoteSelect_toggled(bool);
	void on_actionClefSelect_toggled(bool);
	void on_actionTimeSigSelect_toggled(bool);
	void harmonyAnalysisActivated();	//TODO: This should do plugin externally automatically

	////////////////////////////////////////////////////
	//Process ViewPort signals
	////////////////////////////////////////////////////
	/**
	 * Process the mouse press events of the children viewports.
	 * 
	 * @param e Mouse event which gets processed.
	 * @param coords Absolute world coordinates where the mouse cursor was at time of the event.
	 * @param v Pointer to viewport where the event happened.
	 */
	void viewPortMousePressEvent(QMouseEvent *e, const QPoint coords, CAViewPort *v);

	/**
	 * Process the mouse move events of the children viewports.
	 * 
	 * @param e Mouse event which gets processed.
	 * @param coords Absolute world coordinates where the mouse cursor was at time of the event.
	 * @param v Pointer to viewport where the event happened.
	 */
	void viewPortMouseMoveEvent(QMouseEvent *e, const QPoint coords, CAViewPort *v);
	
	/**
	 * Process the wheel events of the children viewports.
	 * 
	 * @param e Wheel event which gets processed.
	 * @param coords Absolute world coordinates where the mouse cursor was at time of the event.
	 * @param v Pointer to viewport where the event happened.
	 */
	void viewPortWheelEvent(QWheelEvent *e, const QPoint coords, CAViewPort *v);
	
	/**
	 * Process the key events of the children viewports.
	 * 
	 * @param e Key event which gets processed.
	 * @param v Pointer to the viewport where the event happened.
	 */
	void viewPortKeyPressEvent(QKeyEvent *e, CAViewPort *v);
	
	void sourceViewPortCommit(QString documentString);
	
	/**
	 * Called when the tab is switched.
	 */
	void on_tabWidget_currentChanged(int);

	void keyPressEvent(QKeyEvent *);

private slots:
	void playbackFinished();	///Temporarily as we don't find better solution.
	//void on_repaintTimer_timeout();	///Used for repaint events

private:	
	////////////////////////////////////////////////////
	//General properties
	////////////////////////////////////////////////////
	CADocument _document;	///Every main window has its own unique CADocument.
	CAMode _currentMode;	///Every main window has its own current mode (view, insert, edit etc.). See enum CAMode.
	
	void setMode(CAMode mode);
	inline CAMode currentMode() { return _currentMode; }
	CARtMidiDevice *_midiOut;
	CAPluginManager *_pluginManager;
	CAPlayback *_playback;
	QString _fileName;

	////////////////////////////////////////////////////
	//User interface, toolbar
	////////////////////////////////////////////////////
	CAToolBar    *mpoMEToolBar;
	CAButtonMenu *mpoClefMenu;          /// Menu for selection of a clef
	CAButtonMenu *mpoNoteMenu;          /// Menu for the selection of a note length
	CAButtonMenu *mpoTimeSigMenu;       /// Menu for selection of a key signature
	QAction      *actionNoteSelect;     /// Action for having a note length selected
	QAction      *actionClefSelect;     /// Action for having a clef selected
	QAction      *actionTimeSigSelect;  /// Action for having a clef selected
	CAKeySigPSP  *mpoKeySigPSP;	    /// Key signature perspective
	
	////////////////////////////////////////////////////
	//User interface, widgets
	////////////////////////////////////////////////////
	Ui::MainWindow moMainWin;	///Main window widget representative

	QList<CAViewPort *> _viewPortList;	/// List of all available viewports for any
	                                    /// sheet for this document
	CAViewPort *_activeViewPort;	    /// Current active viewport
	bool _animatedScroll;		        /// animate scroll/zoom
	bool _lockScrollPlayback;	        /// Lock the scroll UI while playback
	CAViewPort *_playbackViewPort;	    /// Viewport needed to be updated when playback
	                                    /// is active	
	QTimer *_repaintTimer;	            /// Used when playback is active
	CALCDNumber *mpoVoiceNum;           /// LCD placed in Toolbar for showing current voice

	////////////////////////////////////////////////////
	//User interface, action objects from toolbars
	////////////////////////////////////////////////////
	QAction *mpoVoiceNumAction;  ///Voice number action

	////////////////////////////////////////////////////
	//Insert element
	////////////////////////////////////////////////////
	CAMusElement::CAMusElementType _insertMusElement;	/// Current element to be added. 0, if in view mode, CAMusElementType, if in insert mode
	CAPlayable::CAPlayableLength _insertPlayableLength;	/// Length of note/rest to be added
	int _insertPlayableDotted;	/// Number of dots to be inserted for the note/rest
	int _insertNoteExtraAccs;	/// Extra note accidentals for new notes which user adds/removes with +/- keys
	int _insertNoteAccs;	/// Note accidentals at specific coordinates updated regularily when in insert mode
	int _insertTimeSigBeats; /// Time signature number of beats to be inserted
	int _insertTimeSigBeat; /// Time signature beat to be inserted
	CAClef::CAClefType _insertClef;	/// Type of the clef to be inserted
};
