/** @file drawablestaff.h
 * 
 * Copyright (c) 2006, Matevž Jekovec, Canorus development team
 * All Rights Reserved. See AUTHORS for a complete list of authors.
 * 
 * Licensed under the GNU GENERAL PUBLIC LICENSE. See COPYING for details.
 */

#ifndef DRAWABLESTAFF_H_
#define DRAWABLESTAFF_H_

#include "drawablecontext.h"

class CAStaff;
class CANote;
class CAClef;
class CADrawableClef;

class CADrawableStaff : public CADrawableContext {
	public:
		CADrawableStaff(CAStaff* staff, int x, int y);
		void draw(QPainter *, const CADrawSettings s);
		CADrawableStaff *clone();
		inline CAStaff *staff() { return (CAStaff*)_context; }
		
		/**
		 * Return the center Y coordinate of the given note in this staff.
		 * 
		 * @param note Note which the following coordinates are being calculated.
		 * @return Center of a space/line of a staff in absolute world units.
		 */
		int calculateCenterYCoord(CANote *note, CAClef *clef);
		
		/**
		 * Round the given Y coordinate to the nearest one so it fits a line or a space (ledger lines too, if needed) in a staff.
		 * 
		 * @return Center of the nearest space/line of a staff, whichever is closer in absolute world units.
		 */
		int calculateCenterYCoord(int y);
		
		/**
		 * Calculate the note pitch on the given clef and absolute world Y coordinate.
		 * 
		 * @param y Center Y coordinate of the note in absolute world units.
		 * @param clef Pointer to the currently active CAClef.
		 * @return Note pitch in logical units. 
		 */
		int calculatePitch(int y, CAClef *clef);
		
		void addClef(CADrawableClef *clef);
		CAClef *getClef(int x);
		
	private:
		QList<CADrawableClef *> _drawableClefList;	///List of all the drawable clefs. Used for fast look-up with the given key - X-coordinate usually.
};

#endif /*DRAWABLESTAFF_H_*/