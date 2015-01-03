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

    /**Alter the brightness or contrast of a QImage.
     * @param image QImage to alter
     * @param brightness brightness value, in the range -255 to 255. A brightness value of 0 indicates
     * no change to brightness, a negative value will darken the image, and a positive value will brighten
     * the image.
     * @param contrast contrast value. Must be a positive or zero value. A value of 1.0 indicates no change
     * to the contrast, a value of 0 represents an image with 0 contrast, and a value > 1.0 will increase the
     * contrast of the image.
     */
    static void adjustBrightnessContrast( QImage &image, const int brightness, const double contrast );

  private:

    class OperationProperties
    {
        //empty base class for storing properties for an operation
    };

    typedef QRgb( *pixelOp )( const QRgb& rgb, const OperationProperties* properties );

    struct imageBlock
    {
      unsigned int beginLine;
      unsigned int endLine;
      unsigned int lineLength;
      QImage* image;
      pixelOp opFunction;
      const OperationProperties* properties;
    };

    static void runPixelOperation( QImage &image, pixelOp operation, const OperationProperties* properties );

    static void processWholeImage( QImage &image, pixelOp operation, const OperationProperties* properties );

    static void processImageInBlocks( QImage &image, pixelOp operation, const OperationProperties* properties );
    static void processBlock( const QgsImageOperation::imageBlock &block );

    //operations
    static QRgb grayscaleLightnessOp( const QRgb& rgb, const OperationProperties* properties );
    static QRgb grayscaleLuminosityOp( const QRgb& rgb, const OperationProperties* properties );
    static QRgb grayscaleAverageOp( const QRgb& rgb, const OperationProperties* properties );

    class BrightnessContrastProperties : public OperationProperties
    {
      public:
        int brightness;
        double contrast;
    };
    static QRgb brightnessContrastOp( const QRgb& rgb, const OperationProperties* properties );

    //helper functions for operations
    static int adjustColorComponent( int colorComponent, int brightness, double contrastFactor );

};

#endif // QGSIMAGEOPERATION_H

