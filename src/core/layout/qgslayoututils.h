/***************************************************************************
                         qgslayoututils.h
                             -------------------
    begin                : November 2014
    copyright            : (C) 2014 by Nyall Dawson
    email                : nyall dot dawson at gmail dot com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef QGSLAYOUTUTILS_H
#define QGSLAYOUTUTILS_H

#include "qgslayoutpage.h" //for page size and orientation enums
#include "qgslayoutobject.h"
#include <QPointF>
#include <QRectF>

class QPainter;

/** \ingroup Layout
 * Utilities for layouts.
 */
class CORE_EXPORT QgsLayoutUtils
{
  public:

    /**Draws an arrow head on to a QPainter.
     * @param p destination painter
     * @param x x-coordinate of arrow center
     * @param y y-coordinate of arrow center
     * @param angle angle in degrees which arrow should point toward, measured
     * clockwise from pointing vertical upward
     * @param arrowHeadWidth size of arrow head
    */
    //TODO
    static void drawArrowHead( QPainter* p, const double x, const double y, const double angle, const double arrowHeadWidth );

    /**Calculates the angle of the line from p1 to p2 (counter clockwise,
     * starting from a line from north to south)
     * @param p1 start point of line
     * @param p2 end point of line
     * @returns angle in degrees, clockwise from south
    */
    //TODO
    static double angle( const QPointF& p1, const QPointF& p2 );

    /**Rotates a point / vector around the origin.
     * @param angle rotation angle in degrees, counterclockwise
     * @param x in/out: x coordinate before / after the rotation
     * @param y in/out: y cooreinate before / after the rotation
    */
    //TODO
    static void rotate( const double angle, double& x, double& y );

    /**Ensures that an angle is in the range -360 < angle < 360
     * @param angle angle in degrees
     * @param allowNegative set to true to allow angles between (-360,0). If false,
     * angles are converted to positive angles in the range [0, 360).
     * @returns equivalent angle within the range
     * @see snappedAngle
    */
    static double normalizedAngle( const double angle, const bool allowNegative = false );

    /**Snaps an angle to its closest 45 degree angle
     * @param angle angle in degrees
     * @returns angle snapped to 0, 45/90/135/180/225/270 or 315 degrees
    */
    //TODO
    static double snappedAngle( const double angle );

    /**Calculates the largest scaled version of originalRect which fits within boundsRect, when it is rotated by
     * a specified amount.
     * @param originalRect QRectF to be rotated and scaled
     * @param boundsRect QRectF specifying the bounds which the rotated and scaled rectangle must fit within
     * @param rotation the rotation in degrees to be applied to the rectangle
     * @returns largest scaled version of the rectangle possible
    */
    //TODO
    static QRectF largestRotatedRectWithinBounds( const QRectF originalRect, const QRectF boundsRect, const double rotation );

    /**Returns the size in mm corresponding to a font point size
     * @param pointSize font size in points
     * @see mmToPoints
    */
    //TODO
    static double pointsToMM( const double pointSize );

    /**Returns the size in mm corresponding to a font point size
     * @param mmSize font size in mm
     * @see pointsToMM
    */
    //TODO
    static double mmToPoints( const double mmSize );

    /**Resizes a QRectF relative to a resized bounding rectangle.
     * @param rectToResize QRectF to resize, contained within boundsBefore. The
     * rectangle is linearly scaled to retain its relative position and size within
     * boundsAfter.
     * @param boundsBefore QRectF of bounds before resize
     * @param boundsAfter QRectF of bounds after resize
    */
    //TODO
    static void relativeResizeRect( QRectF &rectToResize, const QRectF &boundsBefore, const QRectF &boundsAfter );

    /**Returns a scaled position given a before and after range
     * @param position initial position within before range to scale
     * @param beforeMin minimum value in before range
     * @param beforeMax maximum value in before range
     * @param afterMin minimum value in after range
     * @param afterMax maximum value in after range
     * @returns position scaled to range specified by afterMin and afterMax
    */
    //TODO
    static double relativePosition( const double position, const double beforeMin, const double beforeMax, const double afterMin, const double afterMax );

    /**Returns a font where size is set in pixels and the size has been upscaled with FONT_WORKAROUND_SCALE
     * to workaround QT font rendering bugs
     * @param font source font with size set in points
     * @returns font with size set in pixels
     * @note added in version 2.5
    */
    //TODO
    static QFont scaledFontPixelSize( const QFont& font );

    /**Calculate font ascent in millimeters, including workarounds for QT font rendering issues
     * @param font input font
     * @returns font ascent in millimeters
     * @note added in version 2.5
     * @see fontDescentMM
     * @see fontHeightMM
     * @see fontHeightCharacterMM
     * @see textWidthMM
     */
    //TODO
    static double fontAscentMM( const QFont& font );

    /**Calculate font descent in millimeters, including workarounds for QT font rendering issues
     * @param font input font
     * @returns font descent in millimeters
     * @note added in version 2.5
     * @see fontAscentMM
     * @see fontHeightMM
     * @see fontHeightCharacterMM
     * @see textWidthMM
     */
    //TODO
    static double fontDescentMM( const QFont& font );

    /**Calculate font height in millimeters, including workarounds for QT font rendering issues
     * The font height is the font ascent + descent + 1 (for the baseline).
     * @param font input font
     * @returns font height in millimeters
     * @note added in version 2.5
     * @see fontAscentMM
     * @see fontDescentMM
     * @see fontHeightCharacterMM
     * @see textWidthMM
     */
    //TODO
    static double fontHeightMM( const QFont& font );

    /**Calculate font height in millimeters of a single character, including workarounds for QT font
     * rendering issues
     * @param font input font
     * @param character character to calculate height for
     * @returns character height in millimeters
     * @note added in version 2.5
     * @see fontAscentMM
     * @see fontDescentMM
     * @see fontHeightMM
     * @see textWidthMM
     */

    //TODO
    static double fontHeightCharacterMM( const QFont& font, const QChar& character );

    /**Calculate font width in millimeters for a string, including workarounds for QT font
     * rendering issues
     * @param font input font
     * @param text string to calculate width of
     * @returns string width in millimeters
     * @note added in version 2.5
     * @see fontAscentMM
     * @see fontDescentMM
     * @see fontHeightMM
     * @see fontHeightCharacterMM
     */

    //TODO
    static double textWidthMM( const QFont& font, const QString& text );

    /**Draws text on a painter at a specific position, taking care of layout specific issues (calculation to pixel,
     * scaling of font and painter to work around Qt font bugs)
     * @param painter destination QPainter
     * @param pos position to draw text
     * @param text string to draw
     * @param font font to use for drawing text
     * @param color color to draw text
     * @note added in version 2.5
     */
    //TODO
    static void drawText( QPainter* painter, const QPointF& pos, const QString& text, const QFont& font, const QColor& color = QColor() );

    /**Draws text on a painter within a rectangle, taking care of layout specific issues (calculation to pixel,
     * scaling of font and painter to work around Qt font bugs)
     * @param painter destination QPainter
     * @param rect rectangle to draw into
     * @param text string to draw
     * @param font font to use for drawing text
     * @param color color to draw text
     * @param halignment optional horizontal alignment
     * @param valignment optional vertical alignment
     * @param flags allows for passing Qt::TextFlags to control appearance of rendered text
     * @note added in version 2.5
     */
    //TODO
    static void drawText( QPainter* painter, const QRectF& rect, const QString& text, const QFont& font, const QColor& color = QColor(), const Qt::AlignmentFlag halignment = Qt::AlignLeft, const Qt::AlignmentFlag valignment = Qt::AlignTop, const int flags = Qt::TextWordWrap );

};

#endif //QGSLAYOUTUTILS_H
