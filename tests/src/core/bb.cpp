/***************************************************************************
  bb.cpp - bb

 ---------------------
 begin                : 7.4.2017
 copyright            : (C) 2017 by
 email                : [your-email-here]
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "qgstest.h"
#include <QObject>
#include <qgsapplication.h>

class bb: public QObject
{

    Q_OBJECT

private slots:
    void initTestCase(); // will be called before the first testfunction is executed.
    void cleanupTestCase() {} // will be called after the last testfunction was executed.
    void init() {} // will be called before each testfunction is executed.
    void cleanup() {} // will be called after every testfunction.

    // Add your test methods here
};

void bb::initTestCase()
{

}

void bb::cleanupTestCase()
{

}

void bb::init()
{

}

void bb::cleanup()
{

}

QGSTEST_MAIN( bb )
#include "bb.moc"
