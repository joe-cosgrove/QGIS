/***************************************************************************
                             qgscursors.h

                             -------------------
    begin                : 2007
    copyright            : (C) 2007 by Gary E. Sherman
    email                : sherman@mrcc.com
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef QGSCURSORS_H
#define QGSCURSORS_H

/** \ingroup gui
 * Bitmap cursors for map operations.
 */
extern GUI_EXPORT const char *zoom_in[];
extern GUI_EXPORT const char *zoom_out[];

extern GUI_EXPORT const unsigned char pan_bits[];      // TODO QGIS3: Removeme
extern GUI_EXPORT const unsigned char pan_mask_bits[]; // TODO QGIS3: Removeme

extern GUI_EXPORT const char *capture_point_cursor[];
extern GUI_EXPORT const char *select_cursor[];
extern GUI_EXPORT const char *select_plus_cursor[];
extern GUI_EXPORT const char *select_minus_cursor[];
extern GUI_EXPORT const char *select_intersect_cursor[];
extern GUI_EXPORT const char *identify_cursor[];
extern GUI_EXPORT const char *cross_hair_cursor[];
extern GUI_EXPORT const char *sampler_cursor[];

#endif

