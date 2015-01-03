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

#define BLOCK_THREADS 16

void QgsImageOperation::runPixelOperation( QImage &image, QgsImageOperation::pixelOp operation )
{
  if ( image.height() * image.width() < 100000 )
  {
    //small image, don't multithread
    //this threshold was determined via testing various images
    processWholeImage( image, operation );
  }
  else
  {
    //large image, multithread operation
    processImageInBlocks( image, operation );
  }
}

void QgsImageOperation::processImageInBlocks( QImage &image, pixelOp operation )
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
      ref[x] = block.opFunction( ref[x] );
    }
  }
}

void QgsImageOperation::processWholeImage( QImage &image, QgsImageOperation::pixelOp operation )
{
  int height = image.height();
  int width = image.width();
  for ( int y = 0; y < height; ++y )
  {
    QRgb* ref = ( QRgb* )image.scanLine( y );
    for ( int x = 0; x < width; ++x )
    {
      ref[x] = operation( ref[x] );
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

  runPixelOperation( image, operation );
}

QRgb QgsImageOperation::grayscaleLightnessOp( const QRgb &rgb )
{
  int red = qRed( rgb );
  int green = qGreen( rgb );
  int blue = qBlue( rgb );

  int min = qMin( qMin( red, green ), blue );
  int max = qMax( qMax( red, green ), blue );

  int lightness = qMin(( min + max ) / 2, 255 );
  return qRgba( lightness, lightness, lightness, qAlpha( rgb ) );
}

QRgb QgsImageOperation::grayscaleLuminosityOp( const QRgb &rgb )
{
  int luminosity = 0.21 * qRed( rgb ) + 0.72 * qGreen( rgb ) + 0.07 * qBlue( rgb );
  return qRgba( luminosity, luminosity, luminosity, qAlpha( rgb ) );
}

QRgb QgsImageOperation::grayscaleAverageOp( const QRgb &rgb )
{
  int average = ( qRed( rgb ) + qGreen( rgb ) + qBlue( rgb ) ) / 3;
  return qRgba( average, average, average, qAlpha( rgb ) );
}
