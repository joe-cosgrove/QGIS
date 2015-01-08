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
#include <qgsrendererv2.h>
#include <qgsmarkersymbollayerv2.h>
#include <qgsfillsymbollayerv2.h>
#include <qgssinglesymbolrendererv2.h>
#include <QImage>

int main( int argc, char *argv[] )
{
  QApplication a( argc, argv );


  QgsApplication::init();
  QgsApplication::initQgis();

  //QgsVectorLayer* vl = new QgsVectorLayer("Linestring", "x", "memory");

  QgsVectorLayer* vl = new QgsVectorLayer( "Polygon", "x", "memory" );

  //QgsVectorLayer* vl = new QgsVectorLayer( "Point", "x", "memory" );

  Q_ASSERT( vl->isValid() );

  QgsFeatureList fl;
  for ( int i = 0; i < 1000000; ++i )
  {
    QgsFeature f;
    QgsPolyline pl;
    QgsPoint pBase( i % 512, rand() % 512 + 1 );
    pl << pBase << pBase + QgsVector( 30, 0 ) << pBase + QgsVector( 30, 30 ) << pBase + QgsVector( 0, 30 );
    QgsPolygon pg;
    pg << pl;
    f.setGeometry( QgsGeometry::fromPolygon( pg ) );
    //f.setGeometry(QgsGeometry::fromPolyline(pl));
    //f.setGeometry( QgsGeometry::fromPoint( QgsPoint( i % 512, rand() % 512 + 1 ) ) );
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
  QgsSymbolV2* symbol = QgsSymbolV2::defaultSymbol( vl->geometryType() );


  //QgsSymbolLayerV2* layer = new QgsFontMarkerSymbolLayerV2();
  QgsSymbolLayerV2* layer = new QgsLinePatternFillSymbolLayer();

  symbol->changeSymbolLayer( 0, layer );
  QgsSingleSymbolRendererV2* renderer = new QgsSingleSymbolRendererV2( symbol );
  vl->setRendererV2( renderer );
  //QgsMapLayerRegistry::instance()->addMapLayer(vl2);

  QgsMapSettings ms;
  ms.setLayers( QStringList() << vl->id() );
  ms.setExtent( vl->extent() );
  ms.setOutputSize( QSize( 512, 512 ) );

  QgsMapRendererSequentialJob job( ms );



  QTime t;

  ProfilerStart( "/tmp/speedtest.prof" );

  int total = 0;
  int cnt = 1;
  for ( int i = 0; i < cnt; i++ )
  {
    t.start();

    job.start();
    job.waitForFinished();

    int tt = t.elapsed();
    total += tt;

    qDebug( "ttttt %d ms", tt );
  }

  ProfilerStop();

  qDebug( "AVG %d", total / cnt );

  job.renderedImage().save( "/tmp/profile.png" );

  QgsApplication::exitQgis();

  return 0;
}
