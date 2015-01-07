#include <QApplication>
#include <QTime>

#include <gperftools/profiler.h>

#include <qgsapplication.h>
#include <qgsgeometry.h>
#include <qgsmaplayerregistry.h>
#include <qgsmaprenderersequentialjob.h>
#include <qgsmapsettings.h>
#include <qgsvectorlayer.h>
#include <qgsvectordataprovider.h>
#include <qgsvectorfilewriter.h>
#include <qgsimageoperation.h>
#include <QImage>

int main( int argc, char *argv[] )
{
  QApplication a( argc, argv );

#if 0
  //QgsApplication::init();
  //QgsApplication::initQgis();

  //QgsVectorLayer* vl = new QgsVectorLayer("Linestring", "x", "memory");

  //QgsVectorLayer* vl = new QgsVectorLayer("Polygon", "x", "memory");

  QgsVectorLayer* vl = new QgsVectorLayer( "Point", "x", "memory" );

  Q_ASSERT( vl->isValid() );

  QgsFeatureList fl;
  for ( int i = 0; i < 1000000; ++i )
  {
    QgsFeature f;
    QgsPolyline pl;
    QgsPoint pBase( i % 1000, i / 1000 );
    pl << pBase << pBase + QgsVector( 10, 0 ) << pBase + QgsVector( 10, 10 ) << pBase + QgsVector( 0, 10 );
    QgsPolygon pg;
    pg << pl;
    //f.setGeometry(QgsGeometry::fromPolygon(pg));
    //f.setGeometry(QgsGeometry::fromPolyline(pl));
    f.setGeometry( QgsGeometry::fromPoint( QgsPoint( i % 1000, i / 1000 ) ) );
    fl << f;
  }
  vl->dataProvider()->addFeatures( fl );

  /*
  const QgsCoordinateReferenceSystem *destCRS = 0;
  QString shpName = "/tmp/vvv.shp";
  QgsVectorFileWriter::deleteShapeFile(shpName);
  QgsVectorFileWriter::writeAsVectorFormat(vl, shpName, "UTF-8", destCRS);

  QgsVectorLayer* vl2 = new QgsVectorLayer(shpName, "aaa", "ogr");
  Q_ASSERT(vl2->isValid());*/

  QgsMapLayerRegistry::instance()->addMapLayer( vl );
  //QgsMapLayerRegistry::instance()->addMapLayer(vl2);

  QgsMapSettings ms;
  ms.setLayers( QStringList() << vl->id() );
  ms.setExtent( vl->extent() );
  ms.setOutputSize( QSize( 512, 512 ) );

  QgsMapRendererSequentialJob job( ms );
#endif






  QTime t;

  QImage image( QString( TEST_DATA_DIR ) + QDir::separator() +  "sample_image.png" );

  ProfilerStart( "/tmp/speedtest.prof" );

  int total = 0;
  int cnt = 2000;
  for ( int i = 0; i < cnt; i++ )
  {
    QImage newIm( image );

    t.start();


    QgsImageOperation::adjustBrightnessContrast( newIm, 200, 1.0 );

    int tt = t.elapsed();
    total += tt;

    qDebug( "ttttt %d ms", tt );
    //newIm.save( "/tmp/profile.png");
  }

  ProfilerStop();

  qDebug( "AVG %d", total / cnt );

  //job.renderedImage().save("/tmp/profile.png");

// QgsApplication::exitQgis();

  return 0;
}
