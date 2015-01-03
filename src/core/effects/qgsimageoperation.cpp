/***************************************************************************
                           qgsimageoperation.cpp
                           ----------------------
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

#include "qgsimageoperation.h"
#include <QtConcurrentMap>
#include <QColor>

//determined via trial-and-error. Could possibly be optimised, or varied
//depending on the image size.
#define BLOCK_THREADS 16

void QgsImageOperation::runPixelOperation( QImage &image, QgsImageOperation::pixelOp operation, const OperationProperties* properties )
{
  if ( image.height() * image.width() < 100000 )
  {
    //small image, don't multithread
    //this threshold was determined via testing various images
    processWholeImage( image, operation, properties );
  }
  else
  {
    //large image, multithread operation
    processImageInBlocks( image, operation, properties );
  }
}

void QgsImageOperation::processImageInBlocks( QImage &image, pixelOp operation, const OperationProperties* properties )
{
  QList< imageBlock > blocks;
  unsigned int height = image.height();
  unsigned int width = image.width();

  //chunk image up into vertical blocks
  blocks.reserve( BLOCK_THREADS );
  unsigned int begin = 0;
  unsigned int blockLen = height / BLOCK_THREADS;
  for ( unsigned int block = 0; block < BLOCK_THREADS; ++block )
  {
    imageBlock newBlock;
    newBlock.beginLine = begin;
    //make sure last block goes to end of image
    newBlock.endLine = block < ( BLOCK_THREADS - 1 ) ? begin + blockLen : height;
    newBlock.lineLength = width;
    newBlock.image = &image;
    newBlock.opFunction = operation;
    newBlock.properties = properties;
    blocks << newBlock;

    begin += blockLen;
  }

  //process blocks
  QtConcurrent::blockingMap( blocks, processBlock );
}

void QgsImageOperation::processBlock( const QgsImageOperation::imageBlock &block )
{
  for ( unsigned int y = block.beginLine; y < block.endLine; ++y )
  {
    QRgb* ref = ( QRgb* )block.image->scanLine( y );
    for ( unsigned int x = 0; x < block.lineLength; ++x )
    {
      ref[x] = block.opFunction( ref[x], block.properties );
    }
  }
}

void QgsImageOperation::processWholeImage( QImage &image, QgsImageOperation::pixelOp operation, const OperationProperties* properties )
{
  int height = image.height();
  int width = image.width();
  for ( int y = 0; y < height; ++y )
  {
    QRgb* ref = ( QRgb* )image.scanLine( y );
    for ( int x = 0; x < width; ++x )
    {
      ref[x] = operation( ref[x], properties );
    }
  }
}

//operations

void QgsImageOperation::convertToGrayscale( QImage &image, const GrayscaleMode mode )
{
  QgsImageOperation::pixelOp operation;

  switch ( mode )
  {
    case GrayscaleLightness:
      operation = QgsImageOperation::grayscaleLightnessOp;
      break;
    case GrayscaleLuminosity:
      operation = QgsImageOperation::grayscaleLuminosityOp;
      break;
    case GrayscaleAverage:
      operation = QgsImageOperation::grayscaleAverageOp;
      break;
  }

  runPixelOperation( image, operation, NULL );
}

QRgb QgsImageOperation::grayscaleLightnessOp( const QRgb &rgb, const OperationProperties* properties )
{
  Q_UNUSED( properties );
  int red = qRed( rgb );
  int green = qGreen( rgb );
  int blue = qBlue( rgb );

  int min = qMin( qMin( red, green ), blue );
  int max = qMax( qMax( red, green ), blue );

  int lightness = qMin(( min + max ) / 2, 255 );
  return qRgba( lightness, lightness, lightness, qAlpha( rgb ) );
}

QRgb QgsImageOperation::grayscaleLuminosityOp( const QRgb &rgb, const OperationProperties* properties )
{
  Q_UNUSED( properties );
  int luminosity = 0.21 * qRed( rgb ) + 0.72 * qGreen( rgb ) + 0.07 * qBlue( rgb );
  return qRgba( luminosity, luminosity, luminosity, qAlpha( rgb ) );
}

QRgb QgsImageOperation::grayscaleAverageOp( const QRgb &rgb, const OperationProperties* properties )
{
  Q_UNUSED( properties );
  int average = ( qRed( rgb ) + qGreen( rgb ) + qBlue( rgb ) ) / 3;
  return qRgba( average, average, average, qAlpha( rgb ) );
}

void QgsImageOperation::adjustBrightnessContrast( QImage &image, const int brightness, const double contrast )
{
  BrightnessContrastProperties properties;
  properties.brightness = brightness;
  properties.contrast = contrast;

  runPixelOperation( image, QgsImageOperation::brightnessContrastOp, &properties );
}

QRgb QgsImageOperation::brightnessContrastOp( const QRgb &rgb, const OperationProperties* properties )
{
  const BrightnessContrastProperties* bcProperties = static_cast<const BrightnessContrastProperties*>( properties );

  int red = adjustColorComponent( qRed( rgb ), bcProperties->brightness, bcProperties->contrast );
  int blue = adjustColorComponent( qBlue( rgb ), bcProperties->brightness, bcProperties->contrast );
  int green = adjustColorComponent( qGreen( rgb ), bcProperties->brightness, bcProperties->contrast );
  return qRgba( red, green, blue, qAlpha( rgb ) );
}

int QgsImageOperation::adjustColorComponent( int colorComponent, int brightness, double contrastFactor )
{
  return qBound( 0, ( int )(((((( colorComponent / 255.0 ) - 0.5 ) * contrastFactor ) + 0.5 ) * 255 ) + brightness ), 255 );
}
