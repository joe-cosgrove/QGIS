/***************************************************************************
                           qgsimageoperation.h
                           --------------------
    begin                : January 2015
    copyright            : (C) 2015 by Nyall Dawson
    email                : nyall.dawson@gmail.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef QGSIMAGEOPERATION_H
#define QGSIMAGEOPERATION_H

#include <QImage>

/** \ingroup core
 * \class QgsImageOperation
 * \brief Contains operations and filters which apply to QImages
 *
 * A set of optimised pixel manipulation operations and filters which can be applied
 * to QImages.
 * \note Added in version 2.7
 */
class CORE_EXPORT QgsImageOperation
{

  public:

    /** Modes for converting a QImage to grayscale
    */
    enum GrayscaleMode
    {
      GrayscaleLightness, /*< keep the lightness of the color, drops the saturation */
      GrayscaleLuminosity, /*< grayscale by perceptual luminosity (weighted sum of color RGB components) */
      GrayscaleAverage /*< grayscale by taking average of color RGB components */
    };

    /**Convert a QImage to a grayscale image. Alpha channel is preserved.
     * @param image QImage to convert
     * @param mode mode to use during grayscale conversion
    */
    static void convertToGrayscale( QImage &image, const GrayscaleMode mode = GrayscaleLuminosity );

  private:

    typedef QRgb( *pixelOp )( const QRgb& rgb );

    struct imageBlock
    {
      unsigned int beginLine;
      unsigned int endLine;
      unsigned int lineLength;
      QImage* image;
      pixelOp opFunction;
    };

    static void runPixelOperation( QImage &image, pixelOp operation );

    static void processWholeImage( QImage &image, pixelOp operation );

    static void processImageInBlocks( QImage &image, pixelOp operation );
    static void processBlock( const QgsImageOperation::imageBlock &block );

    //operations
    static QRgb grayscaleLightnessOp( const QRgb& rgb );
    static QRgb grayscaleLuminosityOp( const QRgb& rgb );
    static QRgb grayscaleAverageOp( const QRgb& rgb );
};

#endif // QGSIMAGEOPERATION_H

