/***************************************************************************
                         testqgsprocessing.cpp
                         ---------------------
    begin                : January 2017
    copyright            : (C) 2017 by Nyall Dawson
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

#include "qgsprocessingregistry.h"
#include "qgsprocessingprovider.h"
#include "qgsprocessingutils.h"
#include "qgsprocessingalgorithm.h"
#include "qgsprocessingcontext.h"
#include "qgsprocessingmodelalgorithm.h"
#include <QObject>
#include <QtTest/QSignalSpy>
#include "qgis.h"
#include "qgstest.h"
#include "qgstestutils.h"
#include "qgsrasterlayer.h"
#include "qgsproject.h"
#include "qgspoint.h"
#include "qgsgeometry.h"
#include "qgsvectorfilewriter.h"
#include "qgsexpressioncontext.h"

class DummyAlgorithm : public QgsProcessingAlgorithm
{
  public:

    DummyAlgorithm( const QString &name ) : mName( name ) { mFlags = QgsProcessingAlgorithm::flags(); }

    QString name() const override { return mName; }
    QString displayName() const override { return mName; }
    virtual QVariantMap processAlgorithm( const QVariantMap &,
                                          QgsProcessingContext &, QgsProcessingFeedback * ) const override { return QVariantMap(); }

    virtual Flags flags() const override { return mFlags; }

    QString mName;

    Flags mFlags;

    void checkParameterVals()
    {
      addParameter( new QgsProcessingParameterString( "p1" ) );
      QVariantMap params;
      QgsProcessingContext context;

      QVERIFY( !checkParameterValues( params, context ) );
      params.insert( "p1", "a" );
      QVERIFY( checkParameterValues( params, context ) );
      // optional param
      addParameter( new QgsProcessingParameterString( "p2", QString(), QVariant(), false, true ) );
      QVERIFY( checkParameterValues( params, context ) );
      params.insert( "p2", "a" );
      QVERIFY( checkParameterValues( params, context ) );
    }

    void runParameterChecks()
    {
      QVERIFY( parameterDefinitions().isEmpty() );
      QVERIFY( addParameter( new QgsProcessingParameterBoolean( "p1" ) ) );
      QCOMPARE( parameterDefinitions().count(), 1 );
      QCOMPARE( parameterDefinitions().at( 0 )->name(), QString( "p1" ) );

      QVERIFY( !addParameter( nullptr ) );
      QCOMPARE( parameterDefinitions().count(), 1 );
      // duplicate name!
      QgsProcessingParameterBoolean *p2 = new QgsProcessingParameterBoolean( "p1" );
      QVERIFY( !addParameter( p2 ) );
      delete p2;
      QCOMPARE( parameterDefinitions().count(), 1 );

      QCOMPARE( parameterDefinition( "p1" ), parameterDefinitions().at( 0 ) );
      // parameterDefinition should be case insensitive
      QCOMPARE( parameterDefinition( "P1" ), parameterDefinitions().at( 0 ) );
      QVERIFY( !parameterDefinition( "invalid" ) );

      QCOMPARE( countVisibleParameters(), 1 );
      QgsProcessingParameterBoolean *p3 = new QgsProcessingParameterBoolean( "p3" );
      QVERIFY( addParameter( p3 ) );
      QCOMPARE( countVisibleParameters(), 2 );
      QgsProcessingParameterBoolean *p4 = new QgsProcessingParameterBoolean( "p4" );
      p4->setFlags( QgsProcessingParameterDefinition::FlagHidden );
      QVERIFY( addParameter( p4 ) );
      QCOMPARE( countVisibleParameters(), 2 );


      //destination styleparameters
      QVERIFY( destinationParameterDefinitions().isEmpty() );
      QgsProcessingParameterFeatureSink *p5 = new QgsProcessingParameterFeatureSink( "p5" );
      QVERIFY( addParameter( p5 ) );
      QCOMPARE( destinationParameterDefinitions(), QgsProcessingParameterDefinitions() << p5 );
      QgsProcessingParameterFeatureSink *p6 = new QgsProcessingParameterFeatureSink( "p6" );
      QVERIFY( addParameter( p6 ) );
      QCOMPARE( destinationParameterDefinitions(), QgsProcessingParameterDefinitions() << p5 << p6 );
    }

    void runOutputChecks()
    {
      QVERIFY( outputDefinitions().isEmpty() );
      QVERIFY( addOutput( new QgsProcessingOutputVectorLayer( "p1" ) ) );
      QCOMPARE( outputDefinitions().count(), 1 );
      QCOMPARE( outputDefinitions().at( 0 )->name(), QString( "p1" ) );

      QVERIFY( !addOutput( nullptr ) );
      QCOMPARE( outputDefinitions().count(), 1 );
      // duplicate name!
      QgsProcessingOutputVectorLayer *p2 = new QgsProcessingOutputVectorLayer( "p1" );
      QVERIFY( !addOutput( p2 ) );
      delete p2;
      QCOMPARE( outputDefinitions().count(), 1 );

      QCOMPARE( outputDefinition( "p1" ), outputDefinitions().at( 0 ) );
      // parameterDefinition should be case insensitive
      QCOMPARE( outputDefinition( "P1" ), outputDefinitions().at( 0 ) );
      QVERIFY( !outputDefinition( "invalid" ) );

      QVERIFY( !hasHtmlOutputs() );
      QgsProcessingOutputHtml *p3 = new QgsProcessingOutputHtml( "p3" );
      QVERIFY( addOutput( p3 ) );
      QVERIFY( hasHtmlOutputs() );
    }

    void runValidateInputCrsChecks()
    {
      addParameter( new QgsProcessingParameterMapLayer( "p1" ) );
      addParameter( new QgsProcessingParameterMapLayer( "p2" ) );
      QVariantMap parameters;

      QgsVectorLayer *layer3111 = new QgsVectorLayer( "Point?crs=epsg:3111", "v1", "memory" );
      QgsProject p;
      p.addMapLayer( layer3111 );

      QString testDataDir = QStringLiteral( TEST_DATA_DIR ) + '/'; //defined in CmakeLists.txt
      QString raster1 = testDataDir + "tenbytenraster.asc";
      QFileInfo fi1( raster1 );
      QgsRasterLayer *r1 = new QgsRasterLayer( fi1.filePath(), "R1" );
      QVERIFY( r1->isValid() );
      p.addMapLayer( r1 );

      QgsVectorLayer *layer4326 = new QgsVectorLayer( "Point?crs=epsg:4326", "v1", "memory" );
      p.addMapLayer( layer4326 );

      QgsProcessingContext context;
      context.setProject( &p );

      // flag not set
      mFlags = 0;
      parameters.insert( "p1", QVariant::fromValue( layer3111 ) );
      QVERIFY( validateInputCrs( parameters, context ) );
      mFlags = FlagRequiresMatchingCrs;
      QVERIFY( validateInputCrs( parameters, context ) );

      // two layers, different crs
      parameters.insert( "p2", QVariant::fromValue( layer4326 ) );
      // flag not set
      mFlags = 0;
      QVERIFY( validateInputCrs( parameters, context ) );
      mFlags = FlagRequiresMatchingCrs;
      QVERIFY( !validateInputCrs( parameters, context ) );

      // raster layer
      parameters.remove( "p2" );
      addParameter( new QgsProcessingParameterRasterLayer( "p3" ) );
      parameters.insert( "p3", QVariant::fromValue( r1 ) );
      QVERIFY( !validateInputCrs( parameters, context ) );

      // feature source
      parameters.remove( "p3" );
      addParameter( new QgsProcessingParameterFeatureSource( "p4" ) );
      parameters.insert( "p4", layer4326->id() );
      QVERIFY( !validateInputCrs( parameters, context ) );

      parameters.remove( "p4" );
      addParameter( new QgsProcessingParameterMultipleLayers( "p5" ) );
      parameters.insert( "p5", QVariantList() << layer4326->id() << r1->id() );
      QVERIFY( !validateInputCrs( parameters, context ) );
    }

    void runAsPythonCommandChecks()
    {
      addParameter( new QgsProcessingParameterString( "p1" ) );
      addParameter( new QgsProcessingParameterString( "p2" ) );
      QgsProcessingParameterString *hidden = new QgsProcessingParameterString( "p3" );
      hidden->setFlags( QgsProcessingParameterDefinition::FlagHidden );
      addParameter( hidden );

      QVariantMap params;
      QgsProcessingContext context;

      QCOMPARE( asPythonCommand( params, context ), QStringLiteral( "processing.run(\"test\", {})" ) );
      params.insert( "p1", "a" );
      QCOMPARE( asPythonCommand( params, context ), QStringLiteral( "processing.run(\"test\", {'p1':'a'})" ) );
      params.insert( "p2", QVariant() );
      // not set, should be no change
      QCOMPARE( asPythonCommand( params, context ), QStringLiteral( "processing.run(\"test\", {'p1':'a'})" ) );
      params.insert( "p2", "b" );
      QCOMPARE( asPythonCommand( params, context ), QStringLiteral( "processing.run(\"test\", {'p1':'a','p2':'b'})" ) );

      // hidden, shouldn't be shown
      params.insert( "p3", "b" );
      QCOMPARE( asPythonCommand( params, context ), QStringLiteral( "processing.run(\"test\", {'p1':'a','p2':'b'})" ) );
    }

};

//dummy provider for testing
class DummyProvider : public QgsProcessingProvider
{
  public:

    DummyProvider( const QString &id ) : mId( id ) {}

    virtual QString id() const override { return mId; }

    virtual QString name() const override { return "dummy"; }

    void unload() override { if ( unloaded ) { *unloaded = true; } }

    bool *unloaded = nullptr;

  protected:

    virtual void loadAlgorithms() override
    {
      QVERIFY( addAlgorithm( new DummyAlgorithm( "alg1" ) ) );
      QVERIFY( addAlgorithm( new DummyAlgorithm( "alg2" ) ) );

      //dupe name
      QgsProcessingAlgorithm *a = new DummyAlgorithm( "alg1" );
      QVERIFY( !addAlgorithm( a ) );
      delete a;

      QVERIFY( !addAlgorithm( nullptr ) );
    }

    QString mId;


};

class DummyProviderNoLoad : public DummyProvider
{
  public:

    DummyProviderNoLoad( const QString &id ) : DummyProvider( id ) {}

    bool load() override
    {
      return false;
    }

};

class TestQgsProcessing: public QObject
{
    Q_OBJECT

  private slots:
    void initTestCase();// will be called before the first testfunction is executed.
    void cleanupTestCase(); // will be called after the last testfunction was executed.
    void init() {} // will be called before each testfunction is executed.
    void cleanup() {} // will be called after every testfunction.
    void instance();
    void addProvider();
    void providerById();
    void removeProvider();
    void compatibleLayers();
    void normalizeLayerSource();
    void context();
    void mapLayers();
    void mapLayerFromStore();
    void mapLayerFromString();
    void algorithm();
    void features();
    void uniqueValues();
    void createIndex();
    void createFeatureSink();
    void parameters();
    void algorithmParameters();
    void algorithmOutputs();
    void parameterGeneral();
    void parameterBoolean();
    void parameterCrs();
    void parameterLayer();
    void parameterExtent();
    void parameterPoint();
    void parameterFile();
    void parameterMatrix();
    void parameterLayerList();
    void parameterNumber();
    void parameterRange();
    void parameterRasterLayer();
    void parameterEnum();
    void parameterString();
    void parameterExpression();
    void parameterField();
    void parameterFeatureSource();
    void parameterFeatureSink();
    void parameterRasterOut();
    void parameterFileOut();
    void parameterFolderOut();
    void checkParamValues();
    void combineLayerExtent();
    void processingFeatureSource();
    void processingFeatureSink();
    void algorithmScope();
    void validateInputCrs();
    void generateIteratingDestination();
    void asPythonCommand();
    void modelerAlgorithm();

  private:

};

void TestQgsProcessing::initTestCase()
{
  QgsApplication::init();
  QgsApplication::initQgis();
}

void TestQgsProcessing::cleanupTestCase()
{
  QFile::remove( QDir::tempPath() + "/create_feature_sink.tab" );
  QgsVectorFileWriter::deleteShapeFile( QDir::tempPath() + "/create_feature_sink2.shp" );

  QgsApplication::exitQgis();
}

void TestQgsProcessing::instance()
{
  // test that application has a registry instance
  QVERIFY( QgsApplication::processingRegistry() );
}

void TestQgsProcessing::addProvider()
{
  QgsProcessingRegistry r;
  QSignalSpy spyProviderAdded( &r, &QgsProcessingRegistry::providerAdded );

  QVERIFY( r.providers().isEmpty() );

  QVERIFY( !r.addProvider( nullptr ) );

  // add a provider
  DummyProvider *p = new DummyProvider( "p1" );
  QVERIFY( r.addProvider( p ) );
  QCOMPARE( r.providers(), QList< QgsProcessingProvider * >() << p );
  QCOMPARE( spyProviderAdded.count(), 1 );
  QCOMPARE( spyProviderAdded.last().at( 0 ).toString(), QString( "p1" ) );

  //try adding another provider
  DummyProvider *p2 = new DummyProvider( "p2" );
  QVERIFY( r.addProvider( p2 ) );
  QCOMPARE( r.providers().toSet(), QSet< QgsProcessingProvider * >() << p << p2 );
  QCOMPARE( spyProviderAdded.count(), 2 );
  QCOMPARE( spyProviderAdded.last().at( 0 ).toString(), QString( "p2" ) );

  //try adding a provider with duplicate id
  DummyProvider *p3 = new DummyProvider( "p2" );
  QVERIFY( !r.addProvider( p3 ) );
  QCOMPARE( r.providers().toSet(), QSet< QgsProcessingProvider * >() << p << p2 );
  QCOMPARE( spyProviderAdded.count(), 2 );
  delete p3;

  // test that adding a provider which does not load means it is not added to registry
  DummyProviderNoLoad *p4 = new DummyProviderNoLoad( "p4" );
  QVERIFY( !r.addProvider( p4 ) );
  QCOMPARE( r.providers().toSet(), QSet< QgsProcessingProvider * >() << p << p2 );
  QCOMPARE( spyProviderAdded.count(), 2 );
  delete p4;
}

void TestQgsProcessing::providerById()
{
  QgsProcessingRegistry r;

  // no providers
  QVERIFY( !r.providerById( "p1" ) );

  // add a provider
  DummyProvider *p = new DummyProvider( "p1" );
  QVERIFY( r.addProvider( p ) );
  QCOMPARE( r.providerById( "p1" ), p );
  QVERIFY( !r.providerById( "p2" ) );

  //try adding another provider
  DummyProvider *p2 = new DummyProvider( "p2" );
  QVERIFY( r.addProvider( p2 ) );
  QCOMPARE( r.providerById( "p1" ), p );
  QCOMPARE( r.providerById( "p2" ), p2 );
  QVERIFY( !r.providerById( "p3" ) );
}

void TestQgsProcessing::removeProvider()
{
  QgsProcessingRegistry r;
  QSignalSpy spyProviderRemoved( &r, &QgsProcessingRegistry::providerRemoved );

  QVERIFY( !r.removeProvider( nullptr ) );
  QVERIFY( !r.removeProvider( "p1" ) );
  // provider not in registry
  DummyProvider *p = new DummyProvider( "p1" );
  QVERIFY( !r.removeProvider( p ) );
  QCOMPARE( spyProviderRemoved.count(), 0 );

  // add some providers
  QVERIFY( r.addProvider( p ) );
  DummyProvider *p2 = new DummyProvider( "p2" );
  QVERIFY( r.addProvider( p2 ) );

  // remove one by pointer
  bool unloaded = false;
  p->unloaded = &unloaded;
  QVERIFY( r.removeProvider( p ) );
  QCOMPARE( spyProviderRemoved.count(), 1 );
  QCOMPARE( spyProviderRemoved.last().at( 0 ).toString(), QString( "p1" ) );
  QCOMPARE( r.providers(), QList< QgsProcessingProvider * >() << p2 );

  //test that provider was unloaded
  QVERIFY( unloaded );

  // should fail, already removed
  QVERIFY( !r.removeProvider( "p1" ) );

  // remove one by id
  QVERIFY( r.removeProvider( "p2" ) );
  QCOMPARE( spyProviderRemoved.count(), 2 );
  QCOMPARE( spyProviderRemoved.last().at( 0 ).toString(), QString( "p2" ) );
  QVERIFY( r.providers().isEmpty() );
}

void TestQgsProcessing::compatibleLayers()
{
  QgsProject p;

  // add a bunch of layers to a project
  QString testDataDir = QStringLiteral( TEST_DATA_DIR ) + '/'; //defined in CmakeLists.txt
  QString raster1 = testDataDir + "tenbytenraster.asc";
  QString raster2 = testDataDir + "landsat.tif";
  QString raster3 = testDataDir + "/raster/band1_float32_noct_epsg4326.tif";
  QFileInfo fi1( raster1 );
  QgsRasterLayer *r1 = new QgsRasterLayer( fi1.filePath(), "R1" );
  QVERIFY( r1->isValid() );
  QFileInfo fi2( raster2 );
  QgsRasterLayer *r2 = new QgsRasterLayer( fi2.filePath(), "ar2" );
  QVERIFY( r2->isValid() );
  QFileInfo fi3( raster3 );
  QgsRasterLayer *r3 = new QgsRasterLayer( fi3.filePath(), "zz" );
  QVERIFY( r3->isValid() );

  QgsVectorLayer *v1 = new QgsVectorLayer( "Polygon", "V4", "memory" );
  QgsVectorLayer *v2 = new QgsVectorLayer( "Point", "v1", "memory" );
  QgsVectorLayer *v3 = new QgsVectorLayer( "LineString", "v3", "memory" );
  QgsVectorLayer *v4 = new QgsVectorLayer( "none", "vvvv4", "memory" );

  p.addMapLayers( QList<QgsMapLayer *>() << r1 << r2 << r3 << v1 << v2 << v3 << v4 );

  // compatibleRasterLayers
  QVERIFY( QgsProcessingUtils::compatibleRasterLayers( nullptr ).isEmpty() );

  // sorted
  QStringList lIds;
  Q_FOREACH ( QgsRasterLayer *rl, QgsProcessingUtils::compatibleRasterLayers( &p ) )
    lIds << rl->name();
  QCOMPARE( lIds, QStringList() << "ar2" << "R1" << "zz" );

  // unsorted
  lIds.clear();
  Q_FOREACH ( QgsRasterLayer *rl, QgsProcessingUtils::compatibleRasterLayers( &p, false ) )
    lIds << rl->name();
  QCOMPARE( lIds, QStringList() << "R1" << "ar2" << "zz" );


  // compatibleVectorLayers
  QVERIFY( QgsProcessingUtils::compatibleVectorLayers( nullptr ).isEmpty() );

  // sorted
  lIds.clear();
  Q_FOREACH ( QgsVectorLayer *vl, QgsProcessingUtils::compatibleVectorLayers( &p ) )
    lIds << vl->name();
  QCOMPARE( lIds, QStringList() << "v1" << "v3" << "V4" << "vvvv4" );

  // unsorted
  lIds.clear();
  Q_FOREACH ( QgsVectorLayer *vl, QgsProcessingUtils::compatibleVectorLayers( &p, QList<QgsWkbTypes::GeometryType>(), false ) )
    lIds << vl->name();
  QCOMPARE( lIds, QStringList() << "V4" << "v1" << "v3" << "vvvv4" );

  // point only
  lIds.clear();
  Q_FOREACH ( QgsVectorLayer *vl, QgsProcessingUtils::compatibleVectorLayers( &p, QList<QgsWkbTypes::GeometryType>() << QgsWkbTypes::PointGeometry ) )
    lIds << vl->name();
  QCOMPARE( lIds, QStringList() << "v1" );

  // polygon only
  lIds.clear();
  Q_FOREACH ( QgsVectorLayer *vl, QgsProcessingUtils::compatibleVectorLayers( &p, QList<QgsWkbTypes::GeometryType>() << QgsWkbTypes::PolygonGeometry ) )
    lIds << vl->name();
  QCOMPARE( lIds, QStringList() << "V4" );

  // line only
  lIds.clear();
  Q_FOREACH ( QgsVectorLayer *vl, QgsProcessingUtils::compatibleVectorLayers( &p, QList<QgsWkbTypes::GeometryType>() << QgsWkbTypes::LineGeometry ) )
    lIds << vl->name();
  QCOMPARE( lIds, QStringList() << "v3" );

  // point and line only
  lIds.clear();
  Q_FOREACH ( QgsVectorLayer *vl, QgsProcessingUtils::compatibleVectorLayers( &p, QList<QgsWkbTypes::GeometryType>() << QgsWkbTypes::PointGeometry << QgsWkbTypes::LineGeometry ) )
    lIds << vl->name();
  QCOMPARE( lIds, QStringList() << "v1" << "v3" );


  // all layers
  QVERIFY( QgsProcessingUtils::compatibleLayers( nullptr ).isEmpty() );

  // sorted
  lIds.clear();
  Q_FOREACH ( QgsMapLayer *l, QgsProcessingUtils::compatibleLayers( &p ) )
    lIds << l->name();
  QCOMPARE( lIds, QStringList() << "ar2" << "R1" << "v1" << "v3" << "V4" << "vvvv4" <<  "zz" );

  // unsorted
  lIds.clear();
  Q_FOREACH ( QgsMapLayer *l, QgsProcessingUtils::compatibleLayers( &p, false ) )
    lIds << l->name();
  QCOMPARE( lIds, QStringList() << "R1" << "ar2" << "zz"  << "V4" << "v1" << "v3" << "vvvv4" );
}

void TestQgsProcessing::normalizeLayerSource()
{
  QCOMPARE( QgsProcessingUtils::normalizeLayerSource( "data\\layers\\test.shp" ), QString( "data/layers/test.shp" ) );
  QCOMPARE( QgsProcessingUtils::normalizeLayerSource( "data\\layers \"new\"\\test.shp" ), QString( "data/layers 'new'/test.shp" ) );
}

void TestQgsProcessing::context()
{
  QgsProcessingContext context;

  // simple tests for getters/setters
  context.setDefaultEncoding( "my_enc" );
  QCOMPARE( context.defaultEncoding(), QStringLiteral( "my_enc" ) );

  context.setFlags( QgsProcessingContext::Flags( 0 ) );
  QCOMPARE( context.flags(), QgsProcessingContext::Flags( 0 ) );

  QgsProject p;
  context.setProject( &p );
  QCOMPARE( context.project(), &p );

  context.setInvalidGeometryCheck( QgsFeatureRequest::GeometrySkipInvalid );
  QCOMPARE( context.invalidGeometryCheck(), QgsFeatureRequest::GeometrySkipInvalid );

  // layers to load on completion
  QgsVectorLayer *v1 = new QgsVectorLayer( "Polygon", "V1", "memory" );
  QgsVectorLayer *v2 = new QgsVectorLayer( "Polygon", "V2", "memory" );
  QVERIFY( context.layersToLoadOnCompletion().isEmpty() );
  QMap< QString, QgsProcessingContext::LayerDetails > layers;
  layers.insert( v1->id(), QgsProcessingContext::LayerDetails( QStringLiteral( "v1" ), &p ) );
  context.setLayersToLoadOnCompletion( layers );
  QCOMPARE( context.layersToLoadOnCompletion().count(), 1 );
  QCOMPARE( context.layersToLoadOnCompletion().keys().at( 0 ), v1->id() );
  QCOMPARE( context.layersToLoadOnCompletion().values().at( 0 ).name, QStringLiteral( "v1" ) );
  context.addLayerToLoadOnCompletion( v2->id(), QgsProcessingContext::LayerDetails( QStringLiteral( "v2" ), &p ) );
  QCOMPARE( context.layersToLoadOnCompletion().count(), 2 );
  QCOMPARE( context.layersToLoadOnCompletion().keys().at( 0 ), v1->id() );
  QCOMPARE( context.layersToLoadOnCompletion().values().at( 0 ).name, QStringLiteral( "v1" ) );
  QCOMPARE( context.layersToLoadOnCompletion().keys().at( 1 ), v2->id() );
  QCOMPARE( context.layersToLoadOnCompletion().values().at( 1 ).name, QStringLiteral( "v2" ) );
  layers.clear();
  layers.insert( v2->id(), QgsProcessingContext::LayerDetails( QStringLiteral( "v2" ), &p ) );
  context.setLayersToLoadOnCompletion( layers );
  QCOMPARE( context.layersToLoadOnCompletion().count(), 1 );
  QCOMPARE( context.layersToLoadOnCompletion().keys().at( 0 ), v2->id() );
  QCOMPARE( context.layersToLoadOnCompletion().values().at( 0 ).name, QStringLiteral( "v2" ) );
  context.addLayerToLoadOnCompletion( v1->id(), QgsProcessingContext::LayerDetails( QString(), &p ) );
  QCOMPARE( context.layersToLoadOnCompletion().count(), 2 );
  QCOMPARE( context.layersToLoadOnCompletion().keys().at( 0 ), v1->id() );
  QCOMPARE( context.layersToLoadOnCompletion().keys().at( 1 ), v2->id() );
  delete v1;
  delete v2;
}

void TestQgsProcessing::mapLayers()
{
  QString testDataDir = QStringLiteral( TEST_DATA_DIR ) + '/'; //defined in CmakeLists.txt
  QString raster = testDataDir + "landsat.tif";
  QString vector = testDataDir + "points.shp";

  // test loadMapLayerFromString with raster
  QgsMapLayer *l = QgsProcessingUtils::loadMapLayerFromString( raster );
  QVERIFY( l->isValid() );
  QCOMPARE( l->type(), QgsMapLayer::RasterLayer );
  delete l;

  //test with vector
  l = QgsProcessingUtils::loadMapLayerFromString( vector );
  QVERIFY( l->isValid() );
  QCOMPARE( l->type(), QgsMapLayer::VectorLayer );
  delete l;

  l = QgsProcessingUtils::loadMapLayerFromString( QString() );
  QVERIFY( !l );
  l = QgsProcessingUtils::loadMapLayerFromString( QStringLiteral( "so much room for activities!" ) );
  QVERIFY( !l );
  l = QgsProcessingUtils::loadMapLayerFromString( testDataDir + "multipoint.shp" );
  QVERIFY( l->isValid() );
  QCOMPARE( l->type(), QgsMapLayer::VectorLayer );
  delete l;
}

void TestQgsProcessing::mapLayerFromStore()
{
  // test mapLayerFromStore

  QgsMapLayerStore store;

  // add a bunch of layers to a project
  QString testDataDir = QStringLiteral( TEST_DATA_DIR ) + '/'; //defined in CmakeLists.txt
  QString raster1 = testDataDir + "tenbytenraster.asc";
  QString raster2 = testDataDir + "landsat.tif";
  QFileInfo fi1( raster1 );
  QgsRasterLayer *r1 = new QgsRasterLayer( fi1.filePath(), "R1" );
  QVERIFY( r1->isValid() );
  QFileInfo fi2( raster2 );
  QgsRasterLayer *r2 = new QgsRasterLayer( fi2.filePath(), "ar2" );
  QVERIFY( r2->isValid() );

  QgsVectorLayer *v1 = new QgsVectorLayer( "Polygon", "V4", "memory" );
  QgsVectorLayer *v2 = new QgsVectorLayer( "Point", "v1", "memory" );
  store.addMapLayers( QList<QgsMapLayer *>() << r1 << r2 << v1 << v2 );

  QVERIFY( ! QgsProcessingUtils::mapLayerFromStore( QString(), nullptr ) );
  QVERIFY( ! QgsProcessingUtils::mapLayerFromStore( QStringLiteral( "v1" ), nullptr ) );
  QVERIFY( ! QgsProcessingUtils::mapLayerFromStore( QString(), &store ) );
  QCOMPARE( QgsProcessingUtils::mapLayerFromStore( raster1, &store ), r1 );
  QCOMPARE( QgsProcessingUtils::mapLayerFromStore( raster2, &store ), r2 );
  QCOMPARE( QgsProcessingUtils::mapLayerFromStore( "R1", &store ), r1 );
  QCOMPARE( QgsProcessingUtils::mapLayerFromStore( "ar2", &store ), r2 );
  QCOMPARE( QgsProcessingUtils::mapLayerFromStore( "V4", &store ), v1 );
  QCOMPARE( QgsProcessingUtils::mapLayerFromStore( "v1", &store ), v2 );
  QCOMPARE( QgsProcessingUtils::mapLayerFromStore( r1->id(), &store ), r1 );
  QCOMPARE( QgsProcessingUtils::mapLayerFromStore( v1->id(), &store ), v1 );
}

void TestQgsProcessing::mapLayerFromString()
{
  // test mapLayerFromString

  QgsProcessingContext c;
  QgsProject p;

  // add a bunch of layers to a project
  QString testDataDir = QStringLiteral( TEST_DATA_DIR ) + '/'; //defined in CmakeLists.txt
  QString raster1 = testDataDir + "tenbytenraster.asc";
  QString raster2 = testDataDir + "landsat.tif";
  QFileInfo fi1( raster1 );
  QgsRasterLayer *r1 = new QgsRasterLayer( fi1.filePath(), "R1" );
  QVERIFY( r1->isValid() );
  QFileInfo fi2( raster2 );
  QgsRasterLayer *r2 = new QgsRasterLayer( fi2.filePath(), "ar2" );
  QVERIFY( r2->isValid() );

  QgsVectorLayer *v1 = new QgsVectorLayer( "Polygon", "V4", "memory" );
  QgsVectorLayer *v2 = new QgsVectorLayer( "Point", "v1", "memory" );
  p.addMapLayers( QList<QgsMapLayer *>() << r1 << r2 << v1 << v2 );

  // no project set yet
  QVERIFY( ! QgsProcessingUtils::mapLayerFromString( QString(), c ) );
  QVERIFY( ! QgsProcessingUtils::mapLayerFromString( QStringLiteral( "v1" ), c ) );

  c.setProject( &p );

  // layers from current project
  QVERIFY( ! QgsProcessingUtils::mapLayerFromString( QString(), c ) );
  QCOMPARE( QgsProcessingUtils::mapLayerFromString( raster1, c ), r1 );
  QCOMPARE( QgsProcessingUtils::mapLayerFromString( raster2, c ), r2 );
  QCOMPARE( QgsProcessingUtils::mapLayerFromString( "R1", c ), r1 );
  QCOMPARE( QgsProcessingUtils::mapLayerFromString( "ar2", c ), r2 );
  QCOMPARE( QgsProcessingUtils::mapLayerFromString( "V4", c ), v1 );
  QCOMPARE( QgsProcessingUtils::mapLayerFromString( "v1", c ), v2 );
  QCOMPARE( QgsProcessingUtils::mapLayerFromString( r1->id(), c ), r1 );
  QCOMPARE( QgsProcessingUtils::mapLayerFromString( v1->id(), c ), v1 );

  // check that layers in context temporary store are used
  QgsVectorLayer *v5 = new QgsVectorLayer( "Polygon", "V5", "memory" );
  QgsVectorLayer *v6 = new QgsVectorLayer( "Point", "v6", "memory" );
  c.temporaryLayerStore()->addMapLayers( QList<QgsMapLayer *>() << v5 << v6 );
  QCOMPARE( QgsProcessingUtils::mapLayerFromString( "V5", c ), v5 );
  QCOMPARE( QgsProcessingUtils::mapLayerFromString( "v6", c ), v6 );
  QCOMPARE( QgsProcessingUtils::mapLayerFromString( v5->id(), c ), v5 );
  QCOMPARE( QgsProcessingUtils::mapLayerFromString( v6->id(), c ), v6 );
  QVERIFY( ! QgsProcessingUtils::mapLayerFromString( "aaaaa", c ) );

  // if specified, check that layers can be loaded
  QVERIFY( ! QgsProcessingUtils::mapLayerFromString( "aaaaa", c ) );
  QString newRaster = testDataDir + "requires_warped_vrt.tif";
  // don't allow loading
  QVERIFY( ! QgsProcessingUtils::mapLayerFromString( newRaster, c, false ) );
  // allow loading
  QgsMapLayer *loadedLayer = QgsProcessingUtils::mapLayerFromString( newRaster, c, true );
  QVERIFY( loadedLayer->isValid() );
  QCOMPARE( loadedLayer->type(), QgsMapLayer::RasterLayer );
  // should now be in temporary store
  QCOMPARE( c.temporaryLayerStore()->mapLayer( loadedLayer->id() ), loadedLayer );

  // since it's now in temporary store, should be accessible even if we deny loading new layers
  QCOMPARE( QgsProcessingUtils::mapLayerFromString( newRaster, c, false ), loadedLayer );
}

void TestQgsProcessing::algorithm()
{
  DummyAlgorithm alg( "test" );
  DummyProvider *p = new DummyProvider( "p1" );
  QCOMPARE( alg.id(), QString( "test" ) );
  alg.setProvider( p );
  QCOMPARE( alg.provider(), p );
  QCOMPARE( alg.id(), QString( "p1:test" ) );

  QVERIFY( p->algorithms().isEmpty() );

  QSignalSpy providerRefreshed( p, &DummyProvider::algorithmsLoaded );
  p->refreshAlgorithms();
  QCOMPARE( providerRefreshed.count(), 1 );

  for ( int i = 0; i < 2; ++i )
  {
    QCOMPARE( p->algorithms().size(), 2 );
    QCOMPARE( p->algorithm( "alg1" )->name(), QStringLiteral( "alg1" ) );
    QCOMPARE( p->algorithm( "alg1" )->provider(), p );
    QCOMPARE( p->algorithm( "alg2" )->provider(), p );
    QCOMPARE( p->algorithm( "alg2" )->name(), QStringLiteral( "alg2" ) );
    QVERIFY( !p->algorithm( "aaaa" ) );
    QVERIFY( p->algorithms().contains( p->algorithm( "alg1" ) ) );
    QVERIFY( p->algorithms().contains( p->algorithm( "alg2" ) ) );

    // reload, then retest on next loop
    // must be safe for providers to reload their algorithms
    p->refreshAlgorithms();
    QCOMPARE( providerRefreshed.count(), 2 + i );
  }

  QgsProcessingRegistry r;
  r.addProvider( p );
  QCOMPARE( r.algorithms().size(), 2 );
  QVERIFY( r.algorithms().contains( p->algorithm( "alg1" ) ) );
  QVERIFY( r.algorithms().contains( p->algorithm( "alg2" ) ) );

  // algorithmById
  QCOMPARE( r.algorithmById( "p1:alg1" ), p->algorithm( "alg1" ) );
  QCOMPARE( r.algorithmById( "p1:alg2" ), p->algorithm( "alg2" ) );
  QVERIFY( !r.algorithmById( "p1:alg3" ) );
  QVERIFY( !r.algorithmById( "px:alg1" ) );

  //test that loading a provider triggers an algorithm refresh
  DummyProvider *p2 = new DummyProvider( "p2" );
  QVERIFY( p2->algorithms().isEmpty() );
  p2->load();
  QCOMPARE( p2->algorithms().size(), 2 );

  // test that adding a provider to the registry automatically refreshes algorithms (via load)
  DummyProvider *p3 = new DummyProvider( "p3" );
  QVERIFY( p3->algorithms().isEmpty() );
  r.addProvider( p3 );
  QCOMPARE( p3->algorithms().size(), 2 );
}

void TestQgsProcessing::features()
{
  QgsVectorLayer *layer = new QgsVectorLayer( "Point", "v1", "memory" );
  for ( int i = 1; i < 6; ++i )
  {
    QgsFeature f( i );
    f.setGeometry( QgsGeometry( new QgsPoint( 1, 2 ) ) );
    layer->dataProvider()->addFeatures( QgsFeatureList() << f );
  }

  QgsProject p;
  p.addMapLayer( layer );

  QgsProcessingContext context;
  context.setProject( &p );
  // disable check for geometry validity
  context.setFlags( QgsProcessingContext::Flags( 0 ) );

  std::function< QgsFeatureIds( QgsFeatureIterator it ) > getIds = []( QgsFeatureIterator it )
  {
    QgsFeature f;
    QgsFeatureIds ids;
    while ( it.nextFeature( f ) )
    {
      ids << f.id();
    }
    return ids;
  };

  std::unique_ptr< QgsProcessingParameterDefinition > def( new QgsProcessingParameterString( QStringLiteral( "layer" ) ) );
  QVariantMap params;
  params.insert( QStringLiteral( "layer" ), layer->id() );

  std::unique_ptr< QgsFeatureSource > source( QgsProcessingParameters::parameterAsSource( def.get(), params, context ) );

  // test with all features
  QgsFeatureIds ids = getIds( source->getFeatures() );
  QCOMPARE( ids, QgsFeatureIds() << 1 << 2 << 3 << 4 << 5 );
  QCOMPARE( source->featureCount(), 5L );

  // test with selected features
  params.insert( QStringLiteral( "layer" ), QVariant::fromValue( QgsProcessingFeatureSourceDefinition( layer->id(), true ) ) );
  layer->selectByIds( QgsFeatureIds() << 2 << 4 );
  source.reset( QgsProcessingParameters::parameterAsSource( def.get(), params, context ) );
  ids = getIds( source->getFeatures() );
  QCOMPARE( ids, QgsFeatureIds() << 2 << 4 );
  QCOMPARE( source->featureCount(), 2L );

  // selection, but not using selected features
  params.insert( QStringLiteral( "layer" ), QVariant::fromValue( QgsProcessingFeatureSourceDefinition( layer->id(), false ) ) );
  layer->selectByIds( QgsFeatureIds() << 2 << 4 );
  source.reset( QgsProcessingParameters::parameterAsSource( def.get(), params, context ) );
  ids = getIds( source->getFeatures() );
  QCOMPARE( ids, QgsFeatureIds() << 1 << 2 << 3 << 4 << 5 );
  QCOMPARE( source->featureCount(), 5L );

  // using selected features, but no selection
  params.insert( QStringLiteral( "layer" ), QVariant::fromValue( QgsProcessingFeatureSourceDefinition( layer->id(), true ) ) );
  layer->removeSelection();
  source.reset( QgsProcessingParameters::parameterAsSource( def.get(), params, context ) );
  ids = getIds( source->getFeatures() );
  QVERIFY( ids.isEmpty() );
  QCOMPARE( source->featureCount(), 0L );


  // test that feature request is honored
  params.insert( QStringLiteral( "layer" ), QVariant::fromValue( QgsProcessingFeatureSourceDefinition( layer->id(), false ) ) );
  source.reset( QgsProcessingParameters::parameterAsSource( def.get(), params, context ) );
  ids = getIds( source->getFeatures( QgsFeatureRequest().setFilterFids( QgsFeatureIds() << 1 << 3 << 5 ) ) );
  QCOMPARE( ids, QgsFeatureIds() << 1 << 3 << 5 );

  // count is only rough - but we expect (for now) to see full layer count
  QCOMPARE( source->featureCount(), 5L );

  //test that feature request is honored when using selections
  params.insert( QStringLiteral( "layer" ), QVariant::fromValue( QgsProcessingFeatureSourceDefinition( layer->id(), true ) ) );
  layer->selectByIds( QgsFeatureIds() << 2 << 4 );
  source.reset( QgsProcessingParameters::parameterAsSource( def.get(), params, context ) );
  ids = getIds( source->getFeatures( QgsFeatureRequest().setFlags( QgsFeatureRequest::NoGeometry ) ) );
  QCOMPARE( ids, QgsFeatureIds() << 2 << 4 );

  // test callback is hit when filtering invalid geoms
  bool encountered = false;
  std::function< void( const QgsFeature & ) > callback = [ &encountered ]( const QgsFeature & )
  {
    encountered = true;
  };

  context.setInvalidGeometryCheck( QgsFeatureRequest::GeometryAbortOnInvalid );
  context.setInvalidGeometryCallback( callback );
  QgsVectorLayer *polyLayer = new QgsVectorLayer( "Polygon", "v2", "memory" );
  QgsFeature f;
  f.setGeometry( QgsGeometry::fromWkt( QStringLiteral( "Polygon((0 0, 1 0, 0 1, 1 1, 0 0))" ) ) );
  polyLayer->dataProvider()->addFeatures( QgsFeatureList() << f );
  p.addMapLayer( polyLayer );
  params.insert( QStringLiteral( "layer" ), polyLayer->id() );

  source.reset( QgsProcessingParameters::parameterAsSource( def.get(), params, context ) );
  ids = getIds( source->getFeatures() );
  QVERIFY( encountered );

  encountered = false;
  context.setInvalidGeometryCheck( QgsFeatureRequest::GeometryNoCheck );
  source.reset( QgsProcessingParameters::parameterAsSource( def.get(), params, context ) );
  ids = getIds( source->getFeatures() );
  QVERIFY( !encountered );

}

void TestQgsProcessing::uniqueValues()
{
  QgsVectorLayer *layer = new QgsVectorLayer( "Point?field=a:integer&field=b:string", "v1", "memory" );
  for ( int i = 0; i < 6; ++i )
  {
    QgsFeature f( i );
    f.setAttributes( QgsAttributes() << i % 3 + 1 << QString( QChar( ( i % 3 ) + 65 ) ) );
    layer->dataProvider()->addFeatures( QgsFeatureList() << f );
  }

  QgsProcessingContext context;
  context.setFlags( QgsProcessingContext::Flags( 0 ) );

  QgsProject p;
  p.addMapLayer( layer );
  context.setProject( &p );

  std::unique_ptr< QgsProcessingParameterDefinition > def( new QgsProcessingParameterString( QStringLiteral( "layer" ) ) );
  QVariantMap params;
  params.insert( QStringLiteral( "layer" ), layer->id() );

  std::unique_ptr< QgsFeatureSource > source( QgsProcessingParameters::parameterAsSource( def.get(), params, context ) );

  // some bad checks
  QVERIFY( source->uniqueValues( -1 ).isEmpty() );
  QVERIFY( source->uniqueValues( 10001 ).isEmpty() );

  // good checks
  QSet< QVariant > vals = source->uniqueValues( 0 );
  QCOMPARE( vals.count(), 3 );
  QVERIFY( vals.contains( 1 ) );
  QVERIFY( vals.contains( 2 ) );
  QVERIFY( vals.contains( 3 ) );
  vals = source->uniqueValues( 1 );
  QCOMPARE( vals.count(), 3 );
  QVERIFY( vals.contains( QString( "A" ) ) );
  QVERIFY( vals.contains( QString( "B" ) ) );
  QVERIFY( vals.contains( QString( "C" ) ) );

  //using only selected features
  layer->selectByIds( QgsFeatureIds() << 1 << 2 << 4 );
  // but not using selection yet...
  source.reset( QgsProcessingParameters::parameterAsSource( def.get(), params, context ) );
  vals = source->uniqueValues( 0 );
  QCOMPARE( vals.count(), 3 );
  QVERIFY( vals.contains( 1 ) );
  QVERIFY( vals.contains( 2 ) );
  QVERIFY( vals.contains( 3 ) );
  vals = source->uniqueValues( 1 );
  QCOMPARE( vals.count(), 3 );
  QVERIFY( vals.contains( QString( "A" ) ) );
  QVERIFY( vals.contains( QString( "B" ) ) );
  QVERIFY( vals.contains( QString( "C" ) ) );

  // selection and using selection
  params.insert( QStringLiteral( "layer" ), QVariant::fromValue( QgsProcessingFeatureSourceDefinition( layer->id(), true ) ) );
  source.reset( QgsProcessingParameters::parameterAsSource( def.get(), params, context ) );
  QVERIFY( source->uniqueValues( -1 ).isEmpty() );
  QVERIFY( source->uniqueValues( 10001 ).isEmpty() );
  vals = source->uniqueValues( 0 );
  QCOMPARE( vals.count(), 2 );
  QVERIFY( vals.contains( 1 ) );
  QVERIFY( vals.contains( 2 ) );
  vals = source->uniqueValues( 1 );
  QCOMPARE( vals.count(), 2 );
  QVERIFY( vals.contains( QString( "A" ) ) );
  QVERIFY( vals.contains( QString( "B" ) ) );
}

void TestQgsProcessing::createIndex()
{
  QgsVectorLayer *layer = new QgsVectorLayer( "Point", "v1", "memory" );
  for ( int i = 1; i < 6; ++i )
  {
    QgsFeature f( i );
    f.setGeometry( QgsGeometry( new QgsPoint( i, 2 ) ) );
    layer->dataProvider()->addFeatures( QgsFeatureList() << f );
  }

  QgsProcessingContext context;
  QgsProject p;
  p.addMapLayer( layer );
  context.setProject( &p );

  std::unique_ptr< QgsProcessingParameterDefinition > def( new QgsProcessingParameterString( QStringLiteral( "layer" ) ) );
  QVariantMap params;
  params.insert( QStringLiteral( "layer" ), layer->id() );

  // disable selected features check
  std::unique_ptr< QgsFeatureSource > source( QgsProcessingParameters::parameterAsSource( def.get(), params, context ) );
  QVERIFY( source.get() );
  QgsSpatialIndex index( *source.get() );
  QList<QgsFeatureId> ids = index.nearestNeighbor( QgsPointXY( 2.1, 2 ), 1 );
  QCOMPARE( ids, QList<QgsFeatureId>() << 2 );

  // selected features check, but none selected
  params.insert( QStringLiteral( "layer" ), QVariant::fromValue( QgsProcessingFeatureSourceDefinition( layer->id(), true ) ) );
  source.reset( QgsProcessingParameters::parameterAsSource( def.get(), params, context ) );
  bool caught = false;
  try
  {
    index = QgsSpatialIndex( *source.get() );
    ids = index.nearestNeighbor( QgsPointXY( 2.1, 2 ), 1 );
  }
  catch ( ... )
  {
    caught = true;
  }
  QVERIFY( caught );

  // create selection
  layer->selectByIds( QgsFeatureIds() << 4 << 5 );
  source.reset( QgsProcessingParameters::parameterAsSource( def.get(), params, context ) );
  index = QgsSpatialIndex( *source.get() );
  ids = index.nearestNeighbor( QgsPointXY( 2.1, 2 ), 1 );
  QCOMPARE( ids, QList<QgsFeatureId>() << 4 );

  // selection but not using selection mode
  params.insert( QStringLiteral( "layer" ), QVariant::fromValue( QgsProcessingFeatureSourceDefinition( layer->id(), false ) ) );
  source.reset( QgsProcessingParameters::parameterAsSource( def.get(), params, context ) );
  index = QgsSpatialIndex( *source.get() );
  ids = index.nearestNeighbor( QgsPointXY( 2.1, 2 ), 1 );
  QCOMPARE( ids, QList<QgsFeatureId>() << 2 );
}

void TestQgsProcessing::createFeatureSink()
{
  QgsProcessingContext context;

  // empty destination
  QString destination;
  destination = QString();
  QgsVectorLayer *layer = nullptr;

  // should create a memory layer
  std::unique_ptr< QgsFeatureSink > sink( QgsProcessingUtils::createFeatureSink( destination, context, QgsFields(), QgsWkbTypes::Point, QgsCoordinateReferenceSystem() ) );
  QVERIFY( sink.get() );
  layer = qobject_cast< QgsVectorLayer *>( QgsProcessingUtils::mapLayerFromString( destination, context, false ) );
  QVERIFY( layer );
  QCOMPARE( static_cast< QgsProxyFeatureSink *>( sink.get() )->destinationSink(), layer->dataProvider() );
  QCOMPARE( layer->dataProvider()->name(), QStringLiteral( "memory" ) );
  QCOMPARE( destination, layer->id() );
  QCOMPARE( context.temporaryLayerStore()->mapLayer( layer->id() ), layer ); // layer should be in store
  QgsFeature f;
  QCOMPARE( layer->featureCount(), 0L );
  QVERIFY( sink->addFeature( f ) );
  QCOMPARE( layer->featureCount(), 1L );
  context.temporaryLayerStore()->removeAllMapLayers();
  layer = nullptr;

  // specific memory layer output
  destination = QStringLiteral( "memory:mylayer" );
  sink.reset( QgsProcessingUtils::createFeatureSink( destination, context, QgsFields(), QgsWkbTypes::Point, QgsCoordinateReferenceSystem() ) );
  QVERIFY( sink.get() );
  layer = qobject_cast< QgsVectorLayer *>( QgsProcessingUtils::mapLayerFromString( destination, context, false ) );
  QVERIFY( layer );
  QCOMPARE( static_cast< QgsProxyFeatureSink *>( sink.get() )->destinationSink(), layer->dataProvider() );
  QCOMPARE( layer->dataProvider()->name(), QStringLiteral( "memory" ) );
  QCOMPARE( layer->name(), QStringLiteral( "memory:mylayer" ) );
  QCOMPARE( destination, layer->id() );
  QCOMPARE( context.temporaryLayerStore()->mapLayer( layer->id() ), layer ); // layer should be in store
  QCOMPARE( layer->featureCount(), 0L );
  QVERIFY( sink->addFeature( f ) );
  QCOMPARE( layer->featureCount(), 1L );
  context.temporaryLayerStore()->removeAllMapLayers();
  layer = nullptr;

  // memory layer parameters
  destination = QStringLiteral( "memory:mylayer" );
  QgsFields fields;
  fields.append( QgsField( QStringLiteral( "my_field" ), QVariant::String, QString(), 100 ) );
  sink.reset( QgsProcessingUtils::createFeatureSink( destination, context, fields, QgsWkbTypes::PointZM, QgsCoordinateReferenceSystem::fromEpsgId( 3111 ) ) );
  QVERIFY( sink.get() );
  layer = qobject_cast< QgsVectorLayer *>( QgsProcessingUtils::mapLayerFromString( destination, context, false ) );
  QVERIFY( layer );
  QCOMPARE( static_cast< QgsProxyFeatureSink *>( sink.get() )->destinationSink(), layer->dataProvider() );
  QCOMPARE( layer->dataProvider()->name(), QStringLiteral( "memory" ) );
  QCOMPARE( layer->name(), QStringLiteral( "memory:mylayer" ) );
  QCOMPARE( layer->wkbType(), QgsWkbTypes::PointZM );
  QCOMPARE( layer->crs().authid(), QStringLiteral( "EPSG:3111" ) );
  QCOMPARE( layer->fields().size(), 1 );
  QCOMPARE( layer->fields().at( 0 ).name(), QStringLiteral( "my_field" ) );
  QCOMPARE( layer->fields().at( 0 ).type(), QVariant::String );
  QCOMPARE( destination, layer->id() );
  QCOMPARE( context.temporaryLayerStore()->mapLayer( layer->id() ), layer ); // layer should be in store
  QCOMPARE( layer->featureCount(), 0L );
  QVERIFY( sink->addFeature( f ) );
  QCOMPARE( layer->featureCount(), 1L );
  context.temporaryLayerStore()->removeAllMapLayers();
  layer = nullptr;

  // non memory layer output
  destination = QDir::tempPath() + "/create_feature_sink.tab";
  QString prevDest = destination;
  sink.reset( QgsProcessingUtils::createFeatureSink( destination, context, fields, QgsWkbTypes::Polygon, QgsCoordinateReferenceSystem::fromEpsgId( 3111 ) ) );
  QVERIFY( sink.get() );
  f = QgsFeature( fields );
  f.setGeometry( QgsGeometry::fromWkt( QStringLiteral( "Polygon((0 0, 0 1, 1 1, 1 0, 0 0 ))" ) ) );
  f.setAttributes( QgsAttributes() << "val" );
  QVERIFY( sink->addFeature( f ) );
  QCOMPARE( destination, prevDest );
  sink.reset( nullptr );
  layer = qobject_cast< QgsVectorLayer *>( QgsProcessingUtils::mapLayerFromString( destination, context, true ) );
  QVERIFY( layer->isValid() );
  QCOMPARE( layer->crs().authid(), QStringLiteral( "EPSG:3111" ) );
  QCOMPARE( layer->fields().size(), 1 );
  QCOMPARE( layer->fields().at( 0 ).name(), QStringLiteral( "my_field" ) );
  QCOMPARE( layer->fields().at( 0 ).type(), QVariant::String );
  QCOMPARE( layer->featureCount(), 1L );
  delete layer;
  layer = nullptr;

  // no extension, should default to shp
  destination = QDir::tempPath() + "/create_feature_sink2";
  prevDest = QDir::tempPath() + "/create_feature_sink2.shp";
  sink.reset( QgsProcessingUtils::createFeatureSink( destination, context, fields, QgsWkbTypes::Point25D, QgsCoordinateReferenceSystem::fromEpsgId( 3111 ) ) );
  QVERIFY( sink.get() );
  f.setGeometry( QgsGeometry::fromWkt( QStringLiteral( "PointZ(1 2 3)" ) ) );
  QVERIFY( sink->addFeature( f ) );
  QVERIFY( !layer );
  QCOMPARE( destination, prevDest );
  sink.reset( nullptr );
  layer = qobject_cast< QgsVectorLayer *>( QgsProcessingUtils::mapLayerFromString( destination, context, true ) );
  QCOMPARE( layer->wkbType(), QgsWkbTypes::Point25D );
  QCOMPARE( layer->crs().authid(), QStringLiteral( "EPSG:3111" ) );
  QCOMPARE( layer->fields().size(), 1 );
  QCOMPARE( layer->fields().at( 0 ).name(), QStringLiteral( "my_field" ) );
  QCOMPARE( layer->fields().at( 0 ).type(), QVariant::String );
  QCOMPARE( layer->featureCount(), 1L );
  delete layer;
  layer = nullptr;
}

void TestQgsProcessing::parameters()
{
  // test parameter utilities

  std::unique_ptr< QgsProcessingParameterDefinition > def;
  QVariantMap params;
  params.insert( QStringLiteral( "prop" ), QgsProperty::fromField( "a_field" ) );
  params.insert( QStringLiteral( "string" ), QStringLiteral( "a string" ) );
  params.insert( QStringLiteral( "double" ), 5.2 );
  params.insert( QStringLiteral( "int" ), 15 );
  params.insert( QStringLiteral( "bool" ), true );

  QgsProcessingContext context;

  // isDynamic
  QVERIFY( QgsProcessingParameters::isDynamic( params, QStringLiteral( "prop" ) ) );
  QVERIFY( !QgsProcessingParameters::isDynamic( params, QStringLiteral( "string" ) ) );
  QVERIFY( !QgsProcessingParameters::isDynamic( params, QStringLiteral( "bad" ) ) );

  // parameterAsString
  def.reset( new QgsProcessingParameterString( QStringLiteral( "string" ), QStringLiteral( "desc" ) ) );
  QCOMPARE( QgsProcessingParameters::parameterAsString( def.get(), params, context ), QStringLiteral( "a string" ) );
  def->setName( QStringLiteral( "double" ) );
  QCOMPARE( QgsProcessingParameters::parameterAsString( def.get(), params, context ).left( 3 ), QStringLiteral( "5.2" ) );
  def->setName( QStringLiteral( "int" ) );
  QCOMPARE( QgsProcessingParameters::parameterAsString( def.get(), params, context ), QStringLiteral( "15" ) );
  def->setName( QStringLiteral( "bool" ) );
  QCOMPARE( QgsProcessingParameters::parameterAsString( def.get(), params, context ), QStringLiteral( "true" ) );
  def->setName( QStringLiteral( "bad" ) );
  QCOMPARE( QgsProcessingParameters::parameterAsString( def.get(), params, context ), QString() );

  // string with dynamic property (feature not set)
  def->setName( QStringLiteral( "prop" ) );
  QCOMPARE( QgsProcessingParameters::parameterAsString( def.get(), params, context ), QString() );

  // correctly setup feature
  QgsFields fields;
  fields.append( QgsField( "a_field", QVariant::String, QString(), 30 ) );
  QgsFeature f( fields );
  f.setAttribute( 0, QStringLiteral( "field value" ) );
  context.expressionContext().setFeature( f );
  context.expressionContext().setFields( fields );
  def->setName( QStringLiteral( "prop" ) );
  QCOMPARE( QgsProcessingParameters::parameterAsString( def.get(), params, context ), QStringLiteral( "field value" ) );

  // as double
  def->setName( QStringLiteral( "double" ) );
  QCOMPARE( QgsProcessingParameters::parameterAsDouble( def.get(), params, context ), 5.2 );
  def->setName( QStringLiteral( "int" ) );
  QCOMPARE( QgsProcessingParameters::parameterAsDouble( def.get(), params, context ), 15.0 );
  f.setAttribute( 0, QStringLiteral( "6.2" ) );
  context.expressionContext().setFeature( f );
  def->setName( QStringLiteral( "prop" ) );
  QCOMPARE( QgsProcessingParameters::parameterAsDouble( def.get(), params, context ), 6.2 );

  // as int
  def->setName( QStringLiteral( "double" ) );
  QCOMPARE( QgsProcessingParameters::parameterAsInt( def.get(), params, context ), 5 );
  def->setName( QStringLiteral( "int" ) );
  QCOMPARE( QgsProcessingParameters::parameterAsInt( def.get(), params, context ), 15 );
  def->setName( QStringLiteral( "prop" ) );
  QCOMPARE( QgsProcessingParameters::parameterAsInt( def.get(), params, context ), 6 );

  // as bool
  def->setName( QStringLiteral( "double" ) );
  QCOMPARE( QgsProcessingParameters::parameterAsBool( def.get(), params, context ), true );
  def->setName( QStringLiteral( "int" ) );
  QCOMPARE( QgsProcessingParameters::parameterAsBool( def.get(), params, context ), true );
  def->setName( QStringLiteral( "bool" ) );
  QCOMPARE( QgsProcessingParameters::parameterAsBool( def.get(), params, context ), true );
  def->setName( QStringLiteral( "prop" ) );
  QCOMPARE( QgsProcessingParameters::parameterAsBool( def.get(), params, context ), true );
  f.setAttribute( 0, false );
  context.expressionContext().setFeature( f );
  def->setName( QStringLiteral( "prop" ) );
  QCOMPARE( QgsProcessingParameters::parameterAsBool( def.get(), params, context ), false );

  // as layer
  def->setName( QStringLiteral( "double" ) );
  QVERIFY( !QgsProcessingParameters::parameterAsLayer( def.get(), params, context ) );
  def->setName( QStringLiteral( "int" ) );
  QVERIFY( !QgsProcessingParameters::parameterAsLayer( def.get(),  params, context ) );
  def->setName( QStringLiteral( "bool" ) );
  QVERIFY( !QgsProcessingParameters::parameterAsLayer( def.get(), params, context ) );
  def->setName( QStringLiteral( "prop" ) );
  QVERIFY( !QgsProcessingParameters::parameterAsLayer( def.get(), params, context ) );

  QVERIFY( context.temporaryLayerStore()->mapLayers().isEmpty() );
  QString testDataDir = QStringLiteral( TEST_DATA_DIR ) + '/'; //defined in CmakeLists.txt
  f.setAttribute( 0, testDataDir + "/raster/band1_float32_noct_epsg4326.tif" );
  context.expressionContext().setFeature( f );
  def->setName( QStringLiteral( "prop" ) );
  QVERIFY( QgsProcessingParameters::parameterAsLayer( def.get(), params, context ) );
  // make sure layer was loaded
  QVERIFY( !context.temporaryLayerStore()->mapLayers().isEmpty() );

  // parameters as sinks

  QgsWkbTypes::Type wkbType = QgsWkbTypes::PolygonM;
  QgsCoordinateReferenceSystem crs = QgsCoordinateReferenceSystem( QStringLiteral( "epsg:3111" ) );
  QString destId;
  def->setName( QStringLiteral( "string" ) );
  params.insert( QStringLiteral( "string" ), QStringLiteral( "memory:mem" ) );
  std::unique_ptr< QgsFeatureSink > sink;
  sink.reset( QgsProcessingParameters::parameterAsSink( def.get(), params, fields, wkbType, crs, context, destId ) );
  QVERIFY( sink.get() );
  QgsVectorLayer *layer = qobject_cast< QgsVectorLayer *>( QgsProcessingUtils::mapLayerFromString( destId, context ) );
  QVERIFY( layer );
  QVERIFY( layer->isValid() );
  QCOMPARE( layer->fields().count(), 1 );
  QCOMPARE( layer->fields().at( 0 ).name(), QStringLiteral( "a_field" ) );
  QCOMPARE( layer->wkbType(), wkbType );
  QCOMPARE( layer->crs(), crs );

  // property defined sink destination
  params.insert( QStringLiteral( "prop" ), QgsProperty::fromExpression( "'memory:mem2'" ) );
  def->setName( QStringLiteral( "prop" ) );
  crs = QgsCoordinateReferenceSystem( QStringLiteral( "epsg:3113" ) );
  sink.reset( QgsProcessingParameters::parameterAsSink( def.get(), params, fields, wkbType, crs, context, destId ) );
  QVERIFY( sink.get() );
  layer = qobject_cast< QgsVectorLayer *>( QgsProcessingUtils::mapLayerFromString( destId, context ) );
  QVERIFY( layer );
  QVERIFY( layer->isValid() );
  QCOMPARE( layer->fields().count(), 1 );
  QCOMPARE( layer->fields().at( 0 ).name(), QStringLiteral( "a_field" ) );
  QCOMPARE( layer->wkbType(), wkbType );
  QCOMPARE( layer->crs(), crs );

  // QgsProcessingFeatureSinkDefinition as parameter
  QgsProject p;
  QgsProcessingOutputLayerDefinition fs( QStringLiteral( "test.shp" ) );
  fs.destinationProject = &p;
  QVERIFY( context.layersToLoadOnCompletion().isEmpty() );
  params.insert( QStringLiteral( "fs" ), QVariant::fromValue( fs ) );
  def->setName( QStringLiteral( "fs" ) );
  crs = QgsCoordinateReferenceSystem( QStringLiteral( "epsg:28356" ) );
  sink.reset( QgsProcessingParameters::parameterAsSink( def.get(), params, fields, wkbType, crs, context, destId ) );
  QVERIFY( sink.get() );
  QgsVectorFileWriter *writer = dynamic_cast< QgsVectorFileWriter *>( sink.get() );
  QVERIFY( writer );
  layer = qobject_cast< QgsVectorLayer *>( QgsProcessingUtils::mapLayerFromString( destId, context ) );
  QVERIFY( layer );
  QVERIFY( layer->isValid() );
  QCOMPARE( layer->wkbType(), wkbType );
  QCOMPARE( layer->crs(), crs );

  // make sure layer was automatically added to list to load on completion
  QCOMPARE( context.layersToLoadOnCompletion().size(), 1 );
  QCOMPARE( context.layersToLoadOnCompletion().keys().at( 0 ), destId );
  QCOMPARE( context.layersToLoadOnCompletion().values().at( 0 ).name, QStringLiteral( "desc" ) );
}

void TestQgsProcessing::algorithmParameters()
{
  DummyAlgorithm alg( "test" );
  alg.runParameterChecks();
}

void TestQgsProcessing::algorithmOutputs()
{
  DummyAlgorithm alg( "test" );
  alg.runOutputChecks();
}

void TestQgsProcessing::parameterGeneral()
{
  // test constructor
  QgsProcessingParameterBoolean param( "p1", "desc", true, true );
  QCOMPARE( param.name(), QString( "p1" ) );
  QCOMPARE( param.description(), QString( "desc" ) );
  QCOMPARE( param.defaultValue(), QVariant( true ) );
  QVERIFY( param.flags() & QgsProcessingParameterDefinition::FlagOptional );

  // test getters and setters
  param.setDescription( "p2" );
  QCOMPARE( param.description(), QString( "p2" ) );
  param.setDefaultValue( false );
  QCOMPARE( param.defaultValue(), QVariant( false ) );
  param.setFlags( QgsProcessingParameterDefinition::FlagHidden );
  QCOMPARE( param.flags(), QgsProcessingParameterDefinition::FlagHidden );
  param.setDefaultValue( true );
  QCOMPARE( param.defaultValue(), QVariant( true ) );
  param.setDefaultValue( QVariant() );
  QCOMPARE( param.defaultValue(), QVariant() );
}

void TestQgsProcessing::parameterBoolean()
{
  QgsProcessingContext context;

  // test no def
  QVariantMap params;
  params.insert( "no_def",  false );
  QCOMPARE( QgsProcessingParameters::parameterAsBool( nullptr, params, context ), false );
  params.insert( "no_def",  "false" );
  QCOMPARE( QgsProcessingParameters::parameterAsBool( nullptr, params, context ), false );
  params.insert( "no_def",  QVariant() );
  QCOMPARE( QgsProcessingParameters::parameterAsBool( nullptr, params, context ), false );
  params.remove( "no_def" );
  QCOMPARE( QgsProcessingParameters::parameterAsBool( nullptr, params, context ), false );

  // with defs

  std::unique_ptr< QgsProcessingParameterDefinition > def( new QgsProcessingParameterBoolean( "non_optional_default_false" ) );
  QVERIFY( def->checkValueIsAcceptable( false ) );
  QVERIFY( def->checkValueIsAcceptable( true ) );
  QVERIFY( def->checkValueIsAcceptable( "false" ) );
  QVERIFY( def->checkValueIsAcceptable( "true" ) );
  QVERIFY( !def->checkValueIsAcceptable( QVariant() ) );

  params.insert( "non_optional_default_false",  false );
  QCOMPARE( QgsProcessingParameters::parameterAsBool( def.get(), params, context ), false );
  params.insert( "non_optional_default_false",  true );
  QCOMPARE( QgsProcessingParameters::parameterAsBool( def.get(), params, context ), true );
  params.insert( "non_optional_default_false",  "true" );
  QCOMPARE( QgsProcessingParameters::parameterAsBool( def.get(), params, context ), true );
  params.insert( "non_optional_default_false",  "false" );
  QCOMPARE( QgsProcessingParameters::parameterAsBool( def.get(), params, context ), false );

  //non-optional - behavior is undefined, but internally default to false
  params.insert( "non_optional_default_false",  QVariant() );
  QCOMPARE( QgsProcessingParameters::parameterAsBool( def.get(), params, context ), false );
  params.remove( "non_optional_default_false" );
  QCOMPARE( QgsProcessingParameters::parameterAsBool( def.get(), params, context ), false );

  QCOMPARE( def->valueAsPythonString( false, context ), QStringLiteral( "False" ) );
  QCOMPARE( def->valueAsPythonString( true, context ), QStringLiteral( "True" ) );
  QCOMPARE( def->valueAsPythonString( "false", context ), QStringLiteral( "False" ) );
  QCOMPARE( def->valueAsPythonString( "true", context ), QStringLiteral( "True" ) );
  QCOMPARE( def->valueAsPythonString( QVariant::fromValue( QgsProperty::fromExpression( "\"a\"=1" ) ), context ), QStringLiteral( "QgsProperty.fromExpression('\"a\"=1')" ) );

  def.reset( new QgsProcessingParameterBoolean( "optional_default_true", QString(), true, true ) );

  QVERIFY( def->checkValueIsAcceptable( false ) );
  QVERIFY( def->checkValueIsAcceptable( true ) );
  QVERIFY( def->checkValueIsAcceptable( "false" ) );
  QVERIFY( def->checkValueIsAcceptable( "true" ) );
  QVERIFY( def->checkValueIsAcceptable( QVariant() ) );

  params.insert( "optional_default_true",  false );
  QCOMPARE( QgsProcessingParameters::parameterAsBool( def.get(), params, context ), false );
  params.insert( "optional_default_true",  true );
  QCOMPARE( QgsProcessingParameters::parameterAsBool( def.get(), params, context ), true );
  params.insert( "optional_default_true",  "true" );
  QCOMPARE( QgsProcessingParameters::parameterAsBool( def.get(), params, context ), true );
  params.insert( "optional_default_true",  "false" );
  QCOMPARE( QgsProcessingParameters::parameterAsBool( def.get(), params, context ), false );
  //optional - should be default
  params.insert( "optional_default_true",  QVariant() );
  QCOMPARE( QgsProcessingParameters::parameterAsBool( def.get(), params, context ), true );
  params.remove( "optional_default_true" );
  QCOMPARE( QgsProcessingParameters::parameterAsBool( def.get(), params, context ), true );

  def.reset( new QgsProcessingParameterBoolean( "optional_default_false", QString(), false, true ) );

  QVERIFY( def->checkValueIsAcceptable( false ) );
  QVERIFY( def->checkValueIsAcceptable( true ) );
  QVERIFY( def->checkValueIsAcceptable( "false" ) );
  QVERIFY( def->checkValueIsAcceptable( "true" ) );
  QVERIFY( def->checkValueIsAcceptable( QVariant() ) );
  QVERIFY( def->checkValueIsAcceptable( false ) );
  QVERIFY( def->checkValueIsAcceptable( true ) );
  QVERIFY( def->checkValueIsAcceptable( "false" ) );
  QVERIFY( def->checkValueIsAcceptable( "true" ) );
  QVERIFY( def->checkValueIsAcceptable( QVariant() ) );

  params.insert( "optional_default_false",  false );
  QCOMPARE( QgsProcessingParameters::parameterAsBool( def.get(), params, context ), false );
  params.insert( "optional_default_false",  true );
  QCOMPARE( QgsProcessingParameters::parameterAsBool( def.get(), params, context ), true );
  params.insert( "optional_default_false",  "true" );
  QCOMPARE( QgsProcessingParameters::parameterAsBool( def.get(), params, context ), true );
  params.insert( "optional_default_false",  "false" );
  QCOMPARE( QgsProcessingParameters::parameterAsBool( def.get(), params, context ), false );
  //optional - should be default
  params.insert( "optional_default_false",  QVariant() );
  QCOMPARE( QgsProcessingParameters::parameterAsBool( def.get(), params, context ), false );
  params.remove( "optional_default_false" );
  QCOMPARE( QgsProcessingParameters::parameterAsBool( def.get(), params, context ), false );

  def.reset( new QgsProcessingParameterBoolean( "non_optional_default_true", QString(), true, false ) );

  QVERIFY( def->checkValueIsAcceptable( false ) );
  QVERIFY( def->checkValueIsAcceptable( true ) );
  QVERIFY( def->checkValueIsAcceptable( "false" ) );
  QVERIFY( def->checkValueIsAcceptable( "true" ) );
  QVERIFY( !def->checkValueIsAcceptable( QVariant() ) );

  params.insert( "non_optional_default_true",  false );
  QCOMPARE( QgsProcessingParameters::parameterAsBool( def.get(), params, context ), false );
  params.insert( "non_optional_default_true",  true );
  QCOMPARE( QgsProcessingParameters::parameterAsBool( def.get(), params, context ), true );
  params.insert( "non_optional_default_true",  "true" );
  QCOMPARE( QgsProcessingParameters::parameterAsBool( def.get(), params, context ), true );
  params.insert( "non_optional_default_true",  "false" );
  QCOMPARE( QgsProcessingParameters::parameterAsBool( def.get(), params, context ), false );
  //non-optional - behavior is undefined, but internally fallback to default
  params.insert( "non_optional_default_true",  QVariant() );
  QCOMPARE( QgsProcessingParameters::parameterAsBool( def.get(), params, context ), true );
  params.remove( "non_optional_default_true" );
  QCOMPARE( QgsProcessingParameters::parameterAsBool( def.get(), params, context ), true );
}

void TestQgsProcessing::parameterCrs()
{
  // setup a context
  QgsProject p;
  p.setCrs( QgsCoordinateReferenceSystem::fromEpsgId( 28353 ) );
  QString testDataDir = QStringLiteral( TEST_DATA_DIR ) + '/'; //defined in CmakeLists.txt
  QString raster1 = testDataDir + "tenbytenraster.asc";
  QString raster2 = testDataDir + "landsat.tif";
  QFileInfo fi1( raster1 );
  QgsRasterLayer *r1 = new QgsRasterLayer( fi1.filePath(), "R1" );
  QgsVectorLayer *v1 = new QgsVectorLayer( "Polygon?crs=EPSG:3111", "V4", "memory" );
  p.addMapLayers( QList<QgsMapLayer *>() << v1 << r1 );
  QgsProcessingContext context;
  context.setProject( &p );

  // not optional!
  std::unique_ptr< QgsProcessingParameterCrs > def( new QgsProcessingParameterCrs( "non_optional", QString(), QString( "EPSG:3113" ), false ) );
  QVERIFY( !def->checkValueIsAcceptable( false ) );
  QVERIFY( !def->checkValueIsAcceptable( true ) );
  QVERIFY( !def->checkValueIsAcceptable( 5 ) );
  QVERIFY( def->checkValueIsAcceptable( "EPSG:12003" ) );
  QVERIFY( def->checkValueIsAcceptable( "EPSG:3111" ) );
  QVERIFY( !def->checkValueIsAcceptable( "" ) );
  QVERIFY( !def->checkValueIsAcceptable( QVariant() ) );

  // using map layer
  QVariantMap params;
  params.insert( "non_optional",  v1->id() );
  QCOMPARE( QgsProcessingParameters::parameterAsCrs( def.get(), params, context ).authid(), QString( "EPSG:3111" ) );
  QVERIFY( def->checkValueIsAcceptable( v1->id() ) );

  // special ProjectCrs string
  params.insert( "non_optional",  QStringLiteral( "ProjectCrs" ) );
  QCOMPARE( QgsProcessingParameters::parameterAsCrs( def.get(), params, context ).authid(), QString( "EPSG:28353" ) );
  QVERIFY( def->checkValueIsAcceptable( QStringLiteral( "ProjectCrs" ) ) );

  // string representing a project layer source
  params.insert( "non_optional", raster1 );
  QCOMPARE( QgsProcessingParameters::parameterAsCrs( def.get(), params, context ).authid(), QString( "EPSG:4326" ) );
  QVERIFY( def->checkValueIsAcceptable( raster1 ) );

  // string representing a non-project layer source
  params.insert( "non_optional", raster2 );
  QCOMPARE( QgsProcessingParameters::parameterAsCrs( def.get(), params, context ).authid(), QString( "EPSG:32633" ) );
  QVERIFY( def->checkValueIsAcceptable( raster2 ) );

  // string representation of a crs
  params.insert( "non_optional", QString( "EPSG:28355" ) );
  QCOMPARE( QgsProcessingParameters::parameterAsCrs( def.get(), params, context ).authid(), QString( "EPSG:28355" ) );
  QVERIFY( def->checkValueIsAcceptable( QStringLiteral( "EPSG:28355" ) ) );

  // nonsense string
  params.insert( "non_optional", QString( "i'm not a crs, and nothing you can do will make me one" ) );
  QVERIFY( !QgsProcessingParameters::parameterAsCrs( def.get(), params, context ).isValid() );

  QCOMPARE( def->valueAsPythonString( "EPSG:12003", context ), QStringLiteral( "'EPSG:12003'" ) );
  QCOMPARE( def->valueAsPythonString( "ProjectCrs", context ), QStringLiteral( "'ProjectCrs'" ) );
  QCOMPARE( def->valueAsPythonString( raster1, context ), QString( "'" ) + testDataDir + QStringLiteral( "tenbytenraster.asc'" ) );
  QCOMPARE( def->valueAsPythonString( r1->id(), context ), QString( "'" ) + testDataDir + QStringLiteral( "tenbytenraster.asc'" ) );
  QCOMPARE( def->valueAsPythonString( QVariant::fromValue( QgsProperty::fromExpression( "\"a\"=1" ) ), context ), QStringLiteral( "QgsProperty.fromExpression('\"a\"=1')" ) );

  // optional
  def.reset( new QgsProcessingParameterCrs( "optional", QString(), QString( "EPSG:3113" ), true ) );
  params.insert( "optional",  QVariant() );
  QCOMPARE( QgsProcessingParameters::parameterAsCrs( def.get(), params, context ).authid(), QString( "EPSG:3113" ) );
  QVERIFY( def->checkValueIsAcceptable( false ) );
  QVERIFY( def->checkValueIsAcceptable( true ) );
  QVERIFY( def->checkValueIsAcceptable( 5 ) );
  QVERIFY( def->checkValueIsAcceptable( "EPSG:12003" ) );
  QVERIFY( def->checkValueIsAcceptable( "EPSG:3111" ) );
  QVERIFY( def->checkValueIsAcceptable( "" ) );
  QVERIFY( def->checkValueIsAcceptable( QVariant() ) );
}

void TestQgsProcessing::parameterLayer()
{
  // setup a context
  QgsProject p;
  p.setCrs( QgsCoordinateReferenceSystem::fromEpsgId( 28353 ) );
  QString testDataDir = QStringLiteral( TEST_DATA_DIR ) + '/'; //defined in CmakeLists.txt
  QString raster1 = testDataDir + "tenbytenraster.asc";
  QString raster2 = testDataDir + "landsat.tif";
  QFileInfo fi1( raster1 );
  QgsRasterLayer *r1 = new QgsRasterLayer( fi1.filePath(), "R1" );
  QgsVectorLayer *v1 = new QgsVectorLayer( "Polygon?crs=EPSG:3111", "V4", "memory" );
  p.addMapLayers( QList<QgsMapLayer *>() << v1 << r1 );
  QgsProcessingContext context;
  context.setProject( &p );

  // not optional!
  std::unique_ptr< QgsProcessingParameterMapLayer > def( new QgsProcessingParameterMapLayer( "non_optional", QString(), QString( "EPSG:3113" ), false ) );
  QVERIFY( !def->checkValueIsAcceptable( false ) );
  QVERIFY( !def->checkValueIsAcceptable( true ) );
  QVERIFY( !def->checkValueIsAcceptable( 5 ) );
  QVERIFY( def->checkValueIsAcceptable( "layer12312312" ) );
  QVERIFY( def->checkValueIsAcceptable( QVariant::fromValue( r1 ) ) );
  QVERIFY( def->checkValueIsAcceptable( QVariant::fromValue( v1 ) ) );
  QVERIFY( !def->checkValueIsAcceptable( "" ) );
  QVERIFY( !def->checkValueIsAcceptable( QVariant() ) );

  // should be OK
  QVERIFY( def->checkValueIsAcceptable( "c:/Users/admin/Desktop/roads_clipped_transformed_v1_reprojected_final_clipped_aAAA.shp" ) );
  // ... unless we use context, when the check that the layer actually exists is performed
  QVERIFY( !def->checkValueIsAcceptable( "c:/Users/admin/Desktop/roads_clipped_transformed_v1_reprojected_final_clipped_aAAA.shp", &context ) );

  // using existing map layer ID
  QVariantMap params;
  params.insert( "non_optional",  v1->id() );
  QCOMPARE( QgsProcessingParameters::parameterAsLayer( def.get(), params, context )->id(), v1->id() );
  QVERIFY( def->checkValueIsAcceptable( v1->id() ) );
  QVERIFY( def->checkValueIsAcceptable( v1->id(), &context ) );

  // string representing a project layer source
  params.insert( "non_optional", raster1 );
  QVERIFY( def->checkValueIsAcceptable( raster1 ) );
  QCOMPARE( QgsProcessingParameters::parameterAsLayer( def.get(), params, context )->id(), r1->id() );
  // string representing a non-project layer source
  params.insert( "non_optional", raster2 );
  QVERIFY( def->checkValueIsAcceptable( raster2 ) );
  QCOMPARE( QgsProcessingParameters::parameterAsLayer( def.get(), params, context )->publicSource(), raster2 );

  // nonsense string
  params.insert( "non_optional", QString( "i'm not a layer, and nothing you can do will make me one" ) );
  QVERIFY( !QgsProcessingParameters::parameterAsLayer( def.get(), params, context ) );

  // layer
  params.insert( "non_optional", QVariant::fromValue( r1 ) );
  QCOMPARE( QgsProcessingParameters::parameterAsLayer( def.get(), params, context ), r1 );
  params.insert( "non_optional", QVariant::fromValue( v1 ) );
  QCOMPARE( QgsProcessingParameters::parameterAsLayer( def.get(), params, context ), v1 );

  QCOMPARE( def->valueAsPythonString( raster1, context ), QString( "'" ) + testDataDir + QStringLiteral( "tenbytenraster.asc'" ) );
  QCOMPARE( def->valueAsPythonString( r1->id(), context ), QString( "'" ) + testDataDir + QStringLiteral( "tenbytenraster.asc'" ) );
  QCOMPARE( def->valueAsPythonString( QVariant::fromValue( r1 ), context ), QString( "'" ) + testDataDir + QStringLiteral( "tenbytenraster.asc'" ) );
  QCOMPARE( def->valueAsPythonString( QVariant::fromValue( QgsProperty::fromExpression( "\"a\"=1" ) ), context ), QStringLiteral( "QgsProperty.fromExpression('\"a\"=1')" ) );

  // optional
  def.reset( new QgsProcessingParameterMapLayer( "optional", QString(), v1->id(), true ) );
  params.insert( "optional",  QVariant() );
  QCOMPARE( QgsProcessingParameters::parameterAsLayer( def.get(), params, context )->id(), v1->id() );
  QVERIFY( def->checkValueIsAcceptable( false ) );
  QVERIFY( def->checkValueIsAcceptable( true ) );
  QVERIFY( def->checkValueIsAcceptable( 5 ) );
  QVERIFY( def->checkValueIsAcceptable( "layer12312312" ) );
  QVERIFY( def->checkValueIsAcceptable( "c:/Users/admin/Desktop/roads_clipped_transformed_v1_reprojected_final_clipped_aAAA.shp" ) );
  QVERIFY( def->checkValueIsAcceptable( "" ) );
  QVERIFY( def->checkValueIsAcceptable( QVariant() ) );
  QVERIFY( def->checkValueIsAcceptable( QVariant::fromValue( r1 ) ) );
  QVERIFY( def->checkValueIsAcceptable( QVariant::fromValue( v1 ) ) );
}

void TestQgsProcessing::parameterExtent()
{
  // setup a context
  QgsProject p;
  QString testDataDir = QStringLiteral( TEST_DATA_DIR ) + '/'; //defined in CmakeLists.txt
  QString raster1 = testDataDir + "tenbytenraster.asc";
  QString raster2 = testDataDir + "landsat.tif";
  QFileInfo fi1( raster1 );
  QgsRasterLayer *r1 = new QgsRasterLayer( fi1.filePath(), "R1" );
  p.addMapLayers( QList<QgsMapLayer *>() << r1 );
  QgsProcessingContext context;
  context.setProject( &p );

  // not optional!
  std::unique_ptr< QgsProcessingParameterExtent > def( new QgsProcessingParameterExtent( "non_optional", QString(), QString( "1,2,3,4" ), false ) );
  QVERIFY( !def->checkValueIsAcceptable( false ) );
  QVERIFY( !def->checkValueIsAcceptable( true ) );
  QVERIFY( !def->checkValueIsAcceptable( 5 ) );
  QVERIFY( def->checkValueIsAcceptable( "1,2,3,4" ) );
  QVERIFY( !def->checkValueIsAcceptable( "" ) );
  QVERIFY( !def->checkValueIsAcceptable( QVariant() ) );

  // these checks require a context - otherwise we could potentially be referring to a layer source
  QVERIFY( def->checkValueIsAcceptable( "1,2,3" ) );
  QVERIFY( def->checkValueIsAcceptable( "1,2,3,a" ) );
  QVERIFY( !def->checkValueIsAcceptable( "1,2,3", &context ) );
  QVERIFY( !def->checkValueIsAcceptable( "1,2,3,a", &context ) );

  // using map layer
  QVariantMap params;
  params.insert( "non_optional",  r1->id() );
  QVERIFY( def->checkValueIsAcceptable( r1->id() ) );
  QgsRectangle ext = QgsProcessingParameters::parameterAsExtent( def.get(), params, context );
  QCOMPARE( ext, r1->extent() );

  // string representing a project layer source
  params.insert( "non_optional", raster1 );
  QVERIFY( def->checkValueIsAcceptable( raster1 ) );
  QCOMPARE( QgsProcessingParameters::parameterAsExtent( def.get(), params, context ),  r1->extent() );

  // string representing a non-project layer source
  params.insert( "non_optional", raster2 );
  QVERIFY( def->checkValueIsAcceptable( raster2 ) );
  ext = QgsProcessingParameters::parameterAsExtent( def.get(), params, context );
  QGSCOMPARENEAR( ext.xMinimum(), 781662.375000, 10 );
  QGSCOMPARENEAR( ext.xMaximum(), 793062.375000, 10 );
  QGSCOMPARENEAR( ext.yMinimum(),  3339523.125000, 10 );
  QGSCOMPARENEAR( ext.yMaximum(), 3350923.125000, 10 );

  // string representation of an extent
  params.insert( "non_optional", QString( "1.1,2.2,3.3,4.4" ) );
  QVERIFY( def->checkValueIsAcceptable( QStringLiteral( "1.1,2.2,3.3,4.4" ) ) );
  ext = QgsProcessingParameters::parameterAsExtent( def.get(), params, context );
  QGSCOMPARENEAR( ext.xMinimum(), 1.1, 0.001 );
  QGSCOMPARENEAR( ext.xMaximum(), 2.2, 0.001 );
  QGSCOMPARENEAR( ext.yMinimum(),  3.3, 0.001 );
  QGSCOMPARENEAR( ext.yMaximum(), 4.4, 0.001 );

  // nonsense string
  params.insert( "non_optional", QString( "i'm not a crs, and nothing you can do will make me one" ) );
  QVERIFY( QgsProcessingParameters::parameterAsExtent( def.get(), params, context ).isNull() );

  QCOMPARE( def->valueAsPythonString( "1,2,3,4", context ), QStringLiteral( "'1,2,3,4'" ) );
  QCOMPARE( def->valueAsPythonString( r1->id(), context ), QString( "'" ) + testDataDir + QStringLiteral( "tenbytenraster.asc'" ) );
  QCOMPARE( def->valueAsPythonString( QVariant::fromValue( r1 ), context ), QString( "'" ) + testDataDir + QStringLiteral( "tenbytenraster.asc'" ) );
  QCOMPARE( def->valueAsPythonString( raster2, context ), QString( "'" ) + testDataDir + QStringLiteral( "landsat.tif'" ) );
  QCOMPARE( def->valueAsPythonString( QVariant::fromValue( QgsProperty::fromExpression( "\"a\"=1" ) ), context ), QStringLiteral( "QgsProperty.fromExpression('\"a\"=1')" ) );

  // optional
  def.reset( new QgsProcessingParameterExtent( "optional", QString(), QString( "5,6,7,8" ), true ) );
  QVERIFY( def->checkValueIsAcceptable( false ) );
  QVERIFY( def->checkValueIsAcceptable( true ) );
  QVERIFY( def->checkValueIsAcceptable( 5 ) );
  QVERIFY( def->checkValueIsAcceptable( "1,2,3,4" ) );
  QVERIFY( def->checkValueIsAcceptable( "" ) );
  QVERIFY( def->checkValueIsAcceptable( QVariant() ) );

  // Extent is unique in that it will let you set invalid, whereas other
  // optional parameters become "default" when assigning invalid.
  params.insert( "optional",  QVariant() );
  ext = QgsProcessingParameters::parameterAsExtent( def.get(), params, context );
  QVERIFY( ext.isNull() );
}

void TestQgsProcessing::parameterPoint()
{
  QgsProcessingContext context;

  // not optional!
  std::unique_ptr< QgsProcessingParameterPoint > def( new QgsProcessingParameterPoint( "non_optional", QString(), QString( "1,2" ), false ) );
  QVERIFY( !def->checkValueIsAcceptable( false ) );
  QVERIFY( !def->checkValueIsAcceptable( true ) );
  QVERIFY( !def->checkValueIsAcceptable( 5 ) );
  QVERIFY( def->checkValueIsAcceptable( "1.1,2" ) );
  QVERIFY( !def->checkValueIsAcceptable( "1.1,a" ) );
  QVERIFY( !def->checkValueIsAcceptable( "layer12312312" ) );
  QVERIFY( !def->checkValueIsAcceptable( "" ) );
  QVERIFY( !def->checkValueIsAcceptable( QVariant() ) );

  // string representing a point
  QVariantMap params;
  params.insert( "non_optional", QString( "1.1,2.2" ) );
  QVERIFY( def->checkValueIsAcceptable( "1.1,2.2" ) );
  QgsPointXY point = QgsProcessingParameters::parameterAsPoint( def.get(), params, context );
  QGSCOMPARENEAR( point.x(), 1.1, 0.001 );
  QGSCOMPARENEAR( point.y(), 2.2, 0.001 );

  // nonsense string
  params.insert( "non_optional", QString( "i'm not a crs, and nothing you can do will make me one" ) );
  point = QgsProcessingParameters::parameterAsPoint( def.get(), params, context );
  QCOMPARE( point.x(), 0.0 );
  QCOMPARE( point.y(), 0.0 );

  QCOMPARE( def->valueAsPythonString( "1,2", context ), QStringLiteral( "'1,2'" ) );

  // optional
  def.reset( new QgsProcessingParameterPoint( "optional", QString(), QString( "5.1,6.2" ), true ) );
  QVERIFY( def->checkValueIsAcceptable( "1.1,2" ) );
  QVERIFY( !def->checkValueIsAcceptable( "1.1,a" ) );
  QVERIFY( !def->checkValueIsAcceptable( "layer12312312" ) );
  QVERIFY( def->checkValueIsAcceptable( "" ) );
  QVERIFY( def->checkValueIsAcceptable( QVariant() ) );

  params.insert( "optional",  QVariant() );
  point = QgsProcessingParameters::parameterAsPoint( def.get(), params, context );
  QGSCOMPARENEAR( point.x(), 5.1, 0.001 );
  QGSCOMPARENEAR( point.y(), 6.2, 0.001 );
}

void TestQgsProcessing::parameterFile()
{
  QgsProcessingContext context;

  // not optional!
  std::unique_ptr< QgsProcessingParameterFile > def( new QgsProcessingParameterFile( "non_optional", QString(), QgsProcessingParameterFile::File, QString(), QString( "abc.bmp" ), false ) );
  QVERIFY( !def->checkValueIsAcceptable( false ) );
  QVERIFY( !def->checkValueIsAcceptable( true ) );
  QVERIFY( !def->checkValueIsAcceptable( 5 ) );
  QVERIFY( def->checkValueIsAcceptable( "bricks.bmp" ) );
  QVERIFY( !def->checkValueIsAcceptable( "" ) );
  QVERIFY( !def->checkValueIsAcceptable( "  " ) );
  QVERIFY( !def->checkValueIsAcceptable( QVariant() ) );

  // string representing a file
  QVariantMap params;
  params.insert( "non_optional", QString( "def.bmp" ) );
  QCOMPARE( QgsProcessingParameters::parameterAsFile( def.get(), params, context ), QString( "def.bmp" ) );

  // with extension
  def.reset( new QgsProcessingParameterFile( "non_optional", QString(), QgsProcessingParameterFile::File, QStringLiteral( ".bmp" ), QString( "abc.bmp" ), false ) );
  QVERIFY( def->checkValueIsAcceptable( "bricks.bmp" ) );
  QVERIFY( def->checkValueIsAcceptable( "bricks.BMP" ) );
  QVERIFY( !def->checkValueIsAcceptable( "bricks.pcx" ) );

  QCOMPARE( def->valueAsPythonString( "bricks.bmp", context ), QStringLiteral( "'bricks.bmp'" ) );
  QCOMPARE( def->valueAsPythonString( QVariant::fromValue( QgsProperty::fromExpression( "\"a\"=1" ) ), context ), QStringLiteral( "QgsProperty.fromExpression('\"a\"=1')" ) );

  // optional
  def.reset( new QgsProcessingParameterFile( "optional", QString(), QgsProcessingParameterFile::File, QString(), QString( "gef.bmp" ),  true ) );
  QVERIFY( def->checkValueIsAcceptable( false ) );
  QVERIFY( def->checkValueIsAcceptable( true ) );
  QVERIFY( def->checkValueIsAcceptable( 5 ) );
  QVERIFY( def->checkValueIsAcceptable( "bricks.bmp" ) );
  QVERIFY( def->checkValueIsAcceptable( "" ) );
  QVERIFY( def->checkValueIsAcceptable( "  " ) );
  QVERIFY( def->checkValueIsAcceptable( QVariant() ) );

  params.insert( "optional",  QVariant() );
  QCOMPARE( QgsProcessingParameters::parameterAsFile( def.get(), params, context ), QString( "gef.bmp" ) );
}

void TestQgsProcessing::parameterMatrix()
{
  QgsProcessingContext context;

  // not optional!
  std::unique_ptr< QgsProcessingParameterMatrix > def( new QgsProcessingParameterMatrix( "non_optional", QString(), 3, false, QStringList(), QString( ), false ) );
  QVERIFY( !def->checkValueIsAcceptable( false ) );
  QVERIFY( !def->checkValueIsAcceptable( true ) );
  QVERIFY( def->checkValueIsAcceptable( 5 ) );
  QVERIFY( def->checkValueIsAcceptable( "1,2,3" ) );
  QVERIFY( !def->checkValueIsAcceptable( QVariantList() ) );
  QVERIFY( def->checkValueIsAcceptable( QVariantList() << 1 << 2 << 3 ) );
  QVERIFY( def->checkValueIsAcceptable( QVariantList() << ( QVariantList() << 1 << 2 << 3 ) << ( QVariantList() << 1 << 2 << 3 ) ) );
  QVERIFY( !def->checkValueIsAcceptable( "" ) );
  QVERIFY( !def->checkValueIsAcceptable( QVariant() ) );

  // list
  QVariantMap params;
  params.insert( "non_optional", QVariantList() << 1 << 2 << 3 );
  QCOMPARE( QgsProcessingParameters::parameterAsMatrix( def.get(), params, context ), QVariantList() << 1 << 2 << 3 );

  //string
  params.insert( "non_optional", QString( "4,5,6" ) );
  QCOMPARE( QgsProcessingParameters::parameterAsMatrix( def.get(), params, context ), QVariantList() << 4 << 5 << 6 );

  QCOMPARE( def->valueAsPythonString( 5, context ), QStringLiteral( "[5]" ) );
  QCOMPARE( def->valueAsPythonString( QVariantList() << 1 << 2 << 3, context ), QStringLiteral( "[1,2,3]" ) );
  QCOMPARE( def->valueAsPythonString( QVariantList() << ( QVariantList() << 1 << 2 << 3 ) << ( QVariantList() << 1 << 2 << 3 ), context ), QStringLiteral( "[1,2,3,1,2,3]" ) );
  QCOMPARE( def->valueAsPythonString( "1,2,3", context ), QStringLiteral( "[1,2,3]" ) );
  QCOMPARE( def->valueAsPythonString( QVariant::fromValue( QgsProperty::fromExpression( "\"a\"=1" ) ), context ), QStringLiteral( "QgsProperty.fromExpression('\"a\"=1')" ) );

  // optional
  def.reset( new QgsProcessingParameterMatrix( "optional", QString(), 3, false, QStringList(), QVariantList() << 4 << 5 << 6,  true ) );
  QVERIFY( def->checkValueIsAcceptable( 5 ) );
  QVERIFY( def->checkValueIsAcceptable( "1,2,3" ) );
  QVERIFY( def->checkValueIsAcceptable( QVariantList() << 1 << 2 << 3 ) );
  QVERIFY( def->checkValueIsAcceptable( QVariantList() ) );
  QVERIFY( def->checkValueIsAcceptable( "" ) );
  QVERIFY( def->checkValueIsAcceptable( QVariant() ) );

  params.insert( "optional",  QVariant() );
  QCOMPARE( QgsProcessingParameters::parameterAsMatrix( def.get(), params, context ), QVariantList() << 4 << 5 << 6 );
  def.reset( new QgsProcessingParameterMatrix( "optional", QString(), 3, false, QStringList(), QString( "1,2,3" ),  true ) );
  params.insert( "optional",  QVariant() );
  QCOMPARE( QgsProcessingParameters::parameterAsMatrix( def.get(), params, context ), QVariantList() << 1 << 2 << 3 );
}

void TestQgsProcessing::parameterLayerList()
{
  // setup a context
  QgsProject p;
  p.setCrs( QgsCoordinateReferenceSystem::fromEpsgId( 28353 ) );
  QString testDataDir = QStringLiteral( TEST_DATA_DIR ) + '/'; //defined in CmakeLists.txt
  QString raster1 = testDataDir + "tenbytenraster.asc";
  QString raster2 = testDataDir + "landsat.tif";
  QFileInfo fi1( raster1 );
  QgsRasterLayer *r1 = new QgsRasterLayer( fi1.filePath(), "R1" );
  QgsVectorLayer *v1 = new QgsVectorLayer( "Polygon?crs=EPSG:3111", "V4", "memory" );
  p.addMapLayers( QList<QgsMapLayer *>() << v1 << r1 );
  QgsProcessingContext context;
  context.setProject( &p );

  // not optional!
  std::unique_ptr< QgsProcessingParameterMultipleLayers > def( new QgsProcessingParameterMultipleLayers( "non_optional", QString(), QgsProcessingParameterDefinition::TypeAny, QString(), false ) );
  QVERIFY( !def->checkValueIsAcceptable( false ) );
  QVERIFY( !def->checkValueIsAcceptable( true ) );
  QVERIFY( !def->checkValueIsAcceptable( 5 ) );
  QVERIFY( def->checkValueIsAcceptable( "layer12312312" ) );
  QVERIFY( !def->checkValueIsAcceptable( "" ) );
  QVERIFY( !def->checkValueIsAcceptable( QVariant() ) );
  QVERIFY( def->checkValueIsAcceptable( QVariant::fromValue( r1 ) ) );
  QVERIFY( def->checkValueIsAcceptable( QVariant::fromValue( v1 ) ) );

  // should be OK
  QVERIFY( def->checkValueIsAcceptable( "c:/Users/admin/Desktop/roads_clipped_transformed_v1_reprojected_final_clipped_aAAA.shp" ) );
  QVERIFY( def->checkValueIsAcceptable( QStringList() << "c:/Users/admin/Desktop/roads_clipped_transformed_v1_reprojected_final_clipped_aAAA.shp" ) );
  QVERIFY( def->checkValueIsAcceptable( QVariantList() << "c:/Users/admin/Desktop/roads_clipped_transformed_v1_reprojected_final_clipped_aAAA.shp" ) );

  // ... unless we use context, when the check that the layer actually exists is performed
  QVERIFY( !def->checkValueIsAcceptable( "c:/Users/admin/Desktop/roads_clipped_transformed_v1_reprojected_final_clipped_aAAA.shp", &context ) );
  QVERIFY( !def->checkValueIsAcceptable( QStringList() << "c:/Users/admin/Desktop/roads_clipped_transformed_v1_reprojected_final_clipped_aAAA.shp", &context ) );
  QVERIFY( !def->checkValueIsAcceptable( QVariantList() << "c:/Users/admin/Desktop/roads_clipped_transformed_v1_reprojected_final_clipped_aAAA.shp", &context ) );


  // using existing map layer ID
  QVariantMap params;
  params.insert( "non_optional",  v1->id() );
  QCOMPARE( QgsProcessingParameters::parameterAsLayerList( def.get(), params, context ), QList< QgsMapLayer *>() << v1 );
  // using existing map layer
  params.insert( "non_optional",  QVariant::fromValue( v1 ) );
  QCOMPARE( QgsProcessingParameters::parameterAsLayerList( def.get(), params, context ), QList< QgsMapLayer *>() << v1 );

  // using two existing map layer ID
  params.insert( "non_optional",  QVariantList() << v1->id() << r1->id() );
  QCOMPARE( QgsProcessingParameters::parameterAsLayerList( def.get(), params, context ), QList< QgsMapLayer *>() << v1 << r1 );

  // using two existing map layers
  params.insert( "non_optional",  QVariantList() << QVariant::fromValue( v1 ) << QVariant::fromValue( r1 ) );
  QCOMPARE( QgsProcessingParameters::parameterAsLayerList( def.get(), params, context ), QList< QgsMapLayer *>() << v1 << r1 );

  // mix of existing layers and non project layer string
  params.insert( "non_optional",  QVariantList() << v1->id() << raster2 );
  QList< QgsMapLayer *> layers = QgsProcessingParameters::parameterAsLayerList( def.get(), params, context );
  QCOMPARE( layers.at( 0 ), v1 );
  QCOMPARE( layers.at( 1 )->publicSource(), raster2 );

  // empty string
  params.insert( "non_optional",  QString( "" ) );
  QVERIFY( QgsProcessingParameters::parameterAsLayerList( def.get(), params, context ).isEmpty() );

  // nonsense string
  params.insert( "non_optional", QString( "i'm not a layer, and nothing you can do will make me one" ) );
  QVERIFY( QgsProcessingParameters::parameterAsLayerList( def.get(), params, context ).isEmpty() );

  // with 2 min inputs
  def->setMinimumNumberInputs( 2 );
  QVERIFY( !def->checkValueIsAcceptable( false ) );
  QVERIFY( !def->checkValueIsAcceptable( true ) );
  QVERIFY( !def->checkValueIsAcceptable( 5 ) );
  QVERIFY( !def->checkValueIsAcceptable( "layer12312312" ) );
  QVERIFY( def->checkValueIsAcceptable( QStringList() << "layer12312312" << "layerB" ) );
  QVERIFY( def->checkValueIsAcceptable( QVariantList() << "layer12312312" << "layerB" ) );
  QVERIFY( !def->checkValueIsAcceptable( "" ) );
  QVERIFY( !def->checkValueIsAcceptable( QVariant() ) );
  QVERIFY( !def->checkValueIsAcceptable( "c:/Users/admin/Desktop/roads_clipped_transformed_v1_reprojected_final_clipped_aAAA.shp" ) );
  QVERIFY( !def->checkValueIsAcceptable( QStringList() << "c:/Users/admin/Desktop/roads_clipped_transformed_v1_reprojected_final_clipped_aAAA.shp" ) );
  QVERIFY( !def->checkValueIsAcceptable( QVariantList() << "c:/Users/admin/Desktop/roads_clipped_transformed_v1_reprojected_final_clipped_aAAA.shp" ) );
  QVERIFY( !def->checkValueIsAcceptable( "c:/Users/admin/Desktop/roads_clipped_transformed_v1_reprojected_final_clipped_aAAA.shp", &context ) );
  QVERIFY( !def->checkValueIsAcceptable( QStringList() << "c:/Users/admin/Desktop/roads_clipped_transformed_v1_reprojected_final_clipped_aAAA.shp", &context ) );
  QVERIFY( !def->checkValueIsAcceptable( QVariantList() << "c:/Users/admin/Desktop/roads_clipped_transformed_v1_reprojected_final_clipped_aAAA.shp", &context ) );

  def->setMinimumNumberInputs( 3 );
  QVERIFY( !def->checkValueIsAcceptable( QStringList() << "layer12312312" << "layerB" ) );
  QVERIFY( !def->checkValueIsAcceptable( QVariantList() << "layer12312312" << "layerB" ) );

  QCOMPARE( def->valueAsPythonString( "layer12312312", context ), QStringLiteral( "'layer12312312'" ) );
  QCOMPARE( def->valueAsPythonString( QVariant::fromValue( r1 ), context ), QStringLiteral( "['" ) + testDataDir + QStringLiteral( "tenbytenraster.asc']" ) );
  QCOMPARE( def->valueAsPythonString( r1->id(), context ), QStringLiteral( "['" ) + testDataDir + QStringLiteral( "tenbytenraster.asc']" ) );
  QCOMPARE( def->valueAsPythonString( QStringList() << r1->id() << raster2, context ), QStringLiteral( "['" ) + testDataDir + QStringLiteral( "tenbytenraster.asc','" ) + testDataDir + QStringLiteral( "landsat.tif']" ) );
  QCOMPARE( def->valueAsPythonString( QVariant::fromValue( QgsProperty::fromExpression( "\"a\"=1" ) ), context ), QStringLiteral( "QgsProperty.fromExpression('\"a\"=1')" ) );


  // optional with one default layer
  def.reset( new QgsProcessingParameterMultipleLayers( "optional", QString(), QgsProcessingParameterDefinition::TypeAny, v1->id(), true ) );
  QVERIFY( !def->checkValueIsAcceptable( false ) );
  QVERIFY( !def->checkValueIsAcceptable( true ) );
  QVERIFY( !def->checkValueIsAcceptable( 5 ) );
  QVERIFY( def->checkValueIsAcceptable( "layer12312312" ) );
  QVERIFY( def->checkValueIsAcceptable( "" ) );
  QVERIFY( def->checkValueIsAcceptable( QVariant() ) );

  // should be OK
  QVERIFY( def->checkValueIsAcceptable( "c:/Users/admin/Desktop/roads_clipped_transformed_v1_reprojected_final_clipped_aAAA.shp" ) );
  QVERIFY( def->checkValueIsAcceptable( QStringList() << "c:/Users/admin/Desktop/roads_clipped_transformed_v1_reprojected_final_clipped_aAAA.shp" ) );
  QVERIFY( def->checkValueIsAcceptable( QVariantList() << "c:/Users/admin/Desktop/roads_clipped_transformed_v1_reprojected_final_clipped_aAAA.shp" ) );

  // ... unless we use context, when the check that the layer actually exists is performed
  QVERIFY( !def->checkValueIsAcceptable( "c:/Users/admin/Desktop/roads_clipped_transformed_v1_reprojected_final_clipped_aAAA.shp", &context ) );
  QVERIFY( !def->checkValueIsAcceptable( QStringList() << "c:/Users/admin/Desktop/roads_clipped_transformed_v1_reprojected_final_clipped_aAAA.shp", &context ) );
  QVERIFY( !def->checkValueIsAcceptable( QVariantList() << "c:/Users/admin/Desktop/roads_clipped_transformed_v1_reprojected_final_clipped_aAAA.shp", &context ) );

  params.insert( "optional",  QVariant() );
  QCOMPARE( QgsProcessingParameters::parameterAsLayerList( def.get(), params, context ), QList< QgsMapLayer *>() << v1 );

  // optional with two default layers
  def.reset( new QgsProcessingParameterMultipleLayers( "optional", QString(), QgsProcessingParameterDefinition::TypeAny, QVariantList() << v1->id() << r1->publicSource(), true ) );
  params.insert( "optional",  QVariant() );
  QCOMPARE( QgsProcessingParameters::parameterAsLayerList( def.get(), params, context ), QList< QgsMapLayer *>() << v1 << r1 );

  // optional with one default direct layer
  def.reset( new QgsProcessingParameterMultipleLayers( "optional", QString(), QgsProcessingParameterDefinition::TypeAny, QVariant::fromValue( v1 ), true ) );
  QCOMPARE( QgsProcessingParameters::parameterAsLayerList( def.get(), params, context ), QList< QgsMapLayer *>() << v1 );

  // optional with two default direct layers
  def.reset( new QgsProcessingParameterMultipleLayers( "optional", QString(), QgsProcessingParameterDefinition::TypeAny, QVariantList() << QVariant::fromValue( v1 ) << QVariant::fromValue( r1 ), true ) );
  QCOMPARE( QgsProcessingParameters::parameterAsLayerList( def.get(), params, context ), QList< QgsMapLayer *>() << v1 << r1 );
}

void TestQgsProcessing::parameterNumber()
{
  QgsProcessingContext context;

  // not optional!
  std::unique_ptr< QgsProcessingParameterNumber > def( new QgsProcessingParameterNumber( "non_optional", QString(), QgsProcessingParameterNumber::Double, 5, false ) );
  QVERIFY( def->checkValueIsAcceptable( 5 ) );
  QVERIFY( def->checkValueIsAcceptable( "1.1" ) );
  QVERIFY( !def->checkValueIsAcceptable( "1.1,2" ) );
  QVERIFY( !def->checkValueIsAcceptable( "layer12312312" ) );
  QVERIFY( !def->checkValueIsAcceptable( "" ) );
  QVERIFY( !def->checkValueIsAcceptable( QVariant() ) );

  // string representing a number
  QVariantMap params;
  params.insert( "non_optional", QString( "1.1" ) );
  double number = QgsProcessingParameters::parameterAsDouble( def.get(), params, context );
  QGSCOMPARENEAR( number, 1.1, 0.001 );
  int iNumber = QgsProcessingParameters::parameterAsInt( def.get(), params, context );
  QCOMPARE( iNumber, 1 );

  // double
  params.insert( "non_optional", 1.1 );
  number = QgsProcessingParameters::parameterAsDouble( def.get(), params, context );
  QGSCOMPARENEAR( number, 1.1, 0.001 );
  iNumber = QgsProcessingParameters::parameterAsInt( def.get(), params, context );
  QCOMPARE( iNumber, 1 );

  // int
  params.insert( "non_optional", 1 );
  number = QgsProcessingParameters::parameterAsDouble( def.get(), params, context );
  QGSCOMPARENEAR( number, 1, 0.001 );
  iNumber = QgsProcessingParameters::parameterAsInt( def.get(), params, context );
  QCOMPARE( iNumber, 1 );

  // nonsense string
  params.insert( "non_optional", QString( "i'm not a number, and nothing you can do will make me one" ) );
  number = QgsProcessingParameters::parameterAsDouble( def.get(), params, context );
  QCOMPARE( number, 5.0 );
  iNumber = QgsProcessingParameters::parameterAsInt( def.get(), params, context );
  QCOMPARE( iNumber, 5 );

  // with min value
  def->setMinimum( 11 );
  QVERIFY( !def->checkValueIsAcceptable( 5 ) );
  QVERIFY( !def->checkValueIsAcceptable( "1.1" ) );
  QVERIFY( def->checkValueIsAcceptable( 25 ) );
  QVERIFY( def->checkValueIsAcceptable( "21.1" ) );
  // with max value
  def->setMaximum( 21 );
  QVERIFY( !def->checkValueIsAcceptable( 35 ) );
  QVERIFY( !def->checkValueIsAcceptable( "31.1" ) );
  QVERIFY( def->checkValueIsAcceptable( 15 ) );
  QVERIFY( def->checkValueIsAcceptable( "11.1" ) );

  QCOMPARE( def->valueAsPythonString( 5, context ), QStringLiteral( "5" ) );
  QCOMPARE( def->valueAsPythonString( QStringLiteral( "1.1" ), context ), QStringLiteral( "1.1" ) );
  QCOMPARE( def->valueAsPythonString( QVariant::fromValue( QgsProperty::fromExpression( "\"a\"=1" ) ), context ), QStringLiteral( "QgsProperty.fromExpression('\"a\"=1')" ) );

  // optional
  def.reset( new QgsProcessingParameterNumber( "optional", QString(), QgsProcessingParameterNumber::Double, 5.4, true ) );
  QVERIFY( def->checkValueIsAcceptable( 5 ) );
  QVERIFY( def->checkValueIsAcceptable( "1.1" ) );
  QVERIFY( def->checkValueIsAcceptable( "" ) );
  QVERIFY( def->checkValueIsAcceptable( QVariant() ) );

  params.insert( "optional",  QVariant() );
  number = QgsProcessingParameters::parameterAsDouble( def.get(), params, context );
  QGSCOMPARENEAR( number, 5.4, 0.001 );
  iNumber = QgsProcessingParameters::parameterAsInt( def.get(), params, context );
  QCOMPARE( iNumber, 5 );
  // unconvertible string
  params.insert( "optional",  QVariant( "aaaa" ) );
  number = QgsProcessingParameters::parameterAsDouble( def.get(), params, context );
  QGSCOMPARENEAR( number, 5.4, 0.001 );
  iNumber = QgsProcessingParameters::parameterAsInt( def.get(), params, context );
  QCOMPARE( iNumber, 5 );
}

void TestQgsProcessing::parameterRange()
{
  QgsProcessingContext context;

  // not optional!
  std::unique_ptr< QgsProcessingParameterRange > def( new QgsProcessingParameterRange( "non_optional", QString(), QgsProcessingParameterNumber::Double, QString( "5,6" ), false ) );
  QVERIFY( !def->checkValueIsAcceptable( 5 ) );
  QVERIFY( !def->checkValueIsAcceptable( "1.1" ) );
  QVERIFY( def->checkValueIsAcceptable( "1.1,2" ) );
  QVERIFY( !def->checkValueIsAcceptable( "1.1,2,3" ) );
  QVERIFY( !def->checkValueIsAcceptable( "layer12312312" ) );
  QVERIFY( !def->checkValueIsAcceptable( "1,a" ) );
  QVERIFY( def->checkValueIsAcceptable( QVariantList() << 1.1 << 2 ) );
  QVERIFY( !def->checkValueIsAcceptable( QVariantList() << 1.1 << 2 << 3 ) );
  QVERIFY( !def->checkValueIsAcceptable( "" ) );
  QVERIFY( !def->checkValueIsAcceptable( QVariant() ) );

  // string representing a range of numbers
  QVariantMap params;
  params.insert( "non_optional", QString( "1.1,1.2" ) );
  QList< double > range = QgsProcessingParameters::parameterAsRange( def.get(), params, context );
  QGSCOMPARENEAR( range.at( 0 ), 1.1, 0.001 );
  QGSCOMPARENEAR( range.at( 1 ), 1.2, 0.001 );

  // list
  params.insert( "non_optional", QVariantList() << 1.1 << 1.2 );
  range = QgsProcessingParameters::parameterAsRange( def.get(), params, context );
  QGSCOMPARENEAR( range.at( 0 ), 1.1, 0.001 );
  QGSCOMPARENEAR( range.at( 1 ), 1.2, 0.001 );

  // too many elements:
  params.insert( "non_optional", QString( "1.1,1.2,1.3" ) );
  range = QgsProcessingParameters::parameterAsRange( def.get(), params, context );
  QGSCOMPARENEAR( range.at( 0 ), 1.1, 0.001 );
  QGSCOMPARENEAR( range.at( 1 ), 1.2, 0.001 );
  params.insert( "non_optional", QVariantList() << 1.1 << 1.2 << 1.3 );
  range = QgsProcessingParameters::parameterAsRange( def.get(), params,  context );
  QGSCOMPARENEAR( range.at( 0 ), 1.1, 0.001 );
  QGSCOMPARENEAR( range.at( 1 ), 1.2, 0.001 );

  // not enough elements - don't care about the result, just don't crash!
  params.insert( "non_optional", QString( "1.1" ) );
  range = QgsProcessingParameters::parameterAsRange( def.get(), params, context );
  params.insert( "non_optional", QVariantList() << 1.1 );
  range = QgsProcessingParameters::parameterAsRange( def.get(), params, context );

  QCOMPARE( def->valueAsPythonString( "1.1,2", context ), QStringLiteral( "[1.1,2]" ) );
  QCOMPARE( def->valueAsPythonString( QVariantList() << 1.1 << 2, context ), QStringLiteral( "[1.1,2]" ) );
  QCOMPARE( def->valueAsPythonString( QVariant::fromValue( QgsProperty::fromExpression( "\"a\"=1" ) ), context ), QStringLiteral( "QgsProperty.fromExpression('\"a\"=1')" ) );

  // optional
  def.reset( new QgsProcessingParameterRange( "optional", QString(), QgsProcessingParameterNumber::Double, QString( "5.4,7.4" ), true ) );
  QVERIFY( def->checkValueIsAcceptable( "1.1,2" ) );
  QVERIFY( def->checkValueIsAcceptable( QVariantList() << 1.1 << 2 ) );
  QVERIFY( def->checkValueIsAcceptable( "" ) );
  QVERIFY( def->checkValueIsAcceptable( QVariant() ) );

  params.insert( "optional",  QVariant() );
  range = QgsProcessingParameters::parameterAsRange( def.get(), params, context );
  QGSCOMPARENEAR( range.at( 0 ), 5.4, 0.001 );
  QGSCOMPARENEAR( range.at( 1 ), 7.4, 0.001 );
}

void TestQgsProcessing::parameterRasterLayer()
{
  // setup a context
  QgsProject p;
  p.setCrs( QgsCoordinateReferenceSystem::fromEpsgId( 28353 ) );
  QString testDataDir = QStringLiteral( TEST_DATA_DIR ) + '/'; //defined in CmakeLists.txt
  QString raster1 = testDataDir + "tenbytenraster.asc";
  QString raster2 = testDataDir + "landsat.tif";
  QFileInfo fi1( raster1 );
  QgsRasterLayer *r1 = new QgsRasterLayer( fi1.filePath(), "R1" );
  QgsVectorLayer *v1 = new QgsVectorLayer( "Polygon?crs=EPSG:3111", "V4", "memory" );
  p.addMapLayers( QList<QgsMapLayer *>() << v1 << r1 );
  QgsProcessingContext context;
  context.setProject( &p );

  // not optional!
  std::unique_ptr< QgsProcessingParameterRasterLayer > def( new QgsProcessingParameterRasterLayer( "non_optional", QString(), QString( "EPSG:3113" ), false ) );
  QVERIFY( !def->checkValueIsAcceptable( false ) );
  QVERIFY( !def->checkValueIsAcceptable( true ) );
  QVERIFY( !def->checkValueIsAcceptable( 5 ) );
  QVERIFY( def->checkValueIsAcceptable( "layer12312312" ) );
  QVERIFY( !def->checkValueIsAcceptable( "" ) );
  QVERIFY( !def->checkValueIsAcceptable( QVariant() ) );
  QVERIFY( def->checkValueIsAcceptable( QVariant::fromValue( r1 ) ) );
  QVERIFY( !def->checkValueIsAcceptable( QVariant::fromValue( v1 ) ) );

  // should be OK
  QVERIFY( def->checkValueIsAcceptable( "c:/Users/admin/Desktop/roads_clipped_transformed_v1_reprojected_final_clipped_aAAA.tif" ) );
  // ... unless we use context, when the check that the layer actually exists is performed
  QVERIFY( !def->checkValueIsAcceptable( "c:/Users/admin/Desktop/roads_clipped_transformed_v1_reprojected_final_clipped_aAAA.tif", &context ) );

  // using existing map layer ID
  QVariantMap params;
  params.insert( "non_optional",  r1->id() );
  QCOMPARE( QgsProcessingParameters::parameterAsRasterLayer( def.get(), params, context )->id(), r1->id() );

  // using existing map layer
  params.insert( "non_optional",  QVariant::fromValue( r1 ) );
  QCOMPARE( QgsProcessingParameters::parameterAsRasterLayer( def.get(), params, context )->id(), r1->id() );

  // not raster layer
  params.insert( "non_optional",  v1->id() );
  QVERIFY( !QgsProcessingParameters::parameterAsRasterLayer( def.get(), params, context ) );

  // using existing vector layer
  params.insert( "non_optional",  QVariant::fromValue( v1 ) );
  QVERIFY( !QgsProcessingParameters::parameterAsRasterLayer( def.get(), params, context ) );

  // string representing a project layer source
  params.insert( "non_optional", raster1 );
  QCOMPARE( QgsProcessingParameters::parameterAsRasterLayer( def.get(), params, context )->id(), r1->id() );
  // string representing a non-project layer source
  params.insert( "non_optional", raster2 );
  QCOMPARE( QgsProcessingParameters::parameterAsRasterLayer( def.get(), params, context )->publicSource(), raster2 );

  // nonsense string
  params.insert( "non_optional", QString( "i'm not a layer, and nothing you can do will make me one" ) );
  QVERIFY( !QgsProcessingParameters::parameterAsRasterLayer( def.get(), params, context ) );

  QCOMPARE( def->valueAsPythonString( raster1, context ), QString( "'" ) + testDataDir + QStringLiteral( "tenbytenraster.asc'" ) );
  QCOMPARE( def->valueAsPythonString( r1->id(), context ), QString( "'" ) + testDataDir + QStringLiteral( "tenbytenraster.asc'" ) );
  QCOMPARE( def->valueAsPythonString( QVariant::fromValue( r1 ), context ), QString( "'" ) + testDataDir + QStringLiteral( "tenbytenraster.asc'" ) );
  QCOMPARE( def->valueAsPythonString( QVariant::fromValue( QgsProperty::fromExpression( "\"a\"=1" ) ), context ), QStringLiteral( "QgsProperty.fromExpression('\"a\"=1')" ) );

  // optional
  def.reset( new QgsProcessingParameterRasterLayer( "optional", QString(), r1->id(), true ) );
  QCOMPARE( QgsProcessingParameters::parameterAsRasterLayer( def.get(), params, context )->id(), r1->id() );
  QVERIFY( def->checkValueIsAcceptable( false ) );
  QVERIFY( def->checkValueIsAcceptable( true ) );
  QVERIFY( def->checkValueIsAcceptable( 5 ) );
  QVERIFY( def->checkValueIsAcceptable( "layer12312312" ) );
  QVERIFY( def->checkValueIsAcceptable( "c:/Users/admin/Desktop/roads_clipped_transformed_v1_reprojected_final_clipped_aAAA.tif" ) );
  QVERIFY( def->checkValueIsAcceptable( "" ) );
  QVERIFY( def->checkValueIsAcceptable( QVariant() ) );

  params.insert( "optional",  QVariant() );
  QCOMPARE( QgsProcessingParameters::parameterAsRasterLayer( def.get(), params, context )->id(), r1->id() );

  // optional with direct layer
  def.reset( new QgsProcessingParameterRasterLayer( "optional", QString(), QVariant::fromValue( r1 ), true ) );
  QCOMPARE( QgsProcessingParameters::parameterAsRasterLayer( def.get(), params, context )->id(), r1->id() );
}

void TestQgsProcessing::parameterEnum()
{
  QgsProcessingContext context;

  // not optional!
  std::unique_ptr< QgsProcessingParameterEnum > def( new QgsProcessingParameterEnum( "non_optional", QString(), QStringList() << "A" << "B" << "C", false, 2, false ) );
  QVERIFY( !def->checkValueIsAcceptable( false ) );
  QVERIFY( !def->checkValueIsAcceptable( true ) );
  QVERIFY( def->checkValueIsAcceptable( 1 ) );
  QVERIFY( def->checkValueIsAcceptable( "1" ) );
  QVERIFY( !def->checkValueIsAcceptable( "1,2" ) );
  QVERIFY( def->checkValueIsAcceptable( 0 ) );
  QVERIFY( !def->checkValueIsAcceptable( QVariantList() << 1 ) );
  QVERIFY( !def->checkValueIsAcceptable( QVariantList() << "a" ) );
  QVERIFY( !def->checkValueIsAcceptable( 15 ) );
  QVERIFY( !def->checkValueIsAcceptable( -1 ) );
  QVERIFY( !def->checkValueIsAcceptable( "layer12312312" ) );
  QVERIFY( !def->checkValueIsAcceptable( "" ) );
  QVERIFY( !def->checkValueIsAcceptable( QVariant() ) );

  // string representing a number
  QVariantMap params;
  params.insert( "non_optional", QString( "1" ) );
  int iNumber = QgsProcessingParameters::parameterAsEnum( def.get(), params, context );
  QCOMPARE( iNumber, 1 );

  // double
  params.insert( "non_optional", 2.2 );
  iNumber = QgsProcessingParameters::parameterAsEnum( def.get(), params, context );
  QCOMPARE( iNumber, 2 );

  // int
  params.insert( "non_optional", 1 );
  iNumber = QgsProcessingParameters::parameterAsEnum( def.get(), params, context );
  QCOMPARE( iNumber, 1 );

  // nonsense string
  params.insert( "non_optional", QString( "i'm not a number, and nothing you can do will make me one" ) );
  iNumber = QgsProcessingParameters::parameterAsEnum( def.get(), params, context );
  QCOMPARE( iNumber, 2 );

  // out of range
  params.insert( "non_optional", 4 );
  iNumber = QgsProcessingParameters::parameterAsEnum( def.get(), params, context );
  QCOMPARE( iNumber, 2 );

  QCOMPARE( def->valueAsPythonString( 5, context ), QStringLiteral( "5" ) );
  QCOMPARE( def->valueAsPythonString( QStringLiteral( "1.1" ), context ), QStringLiteral( "1" ) );
  QCOMPARE( def->valueAsPythonString( QVariant::fromValue( QgsProperty::fromExpression( "\"a\"=1" ) ), context ), QStringLiteral( "QgsProperty.fromExpression('\"a\"=1')" ) );

  // multiple
  def.reset( new QgsProcessingParameterEnum( "non_optional", QString(), QStringList() << "A" << "B" << "C", true, 5, false ) );
  QVERIFY( !def->checkValueIsAcceptable( false ) );
  QVERIFY( !def->checkValueIsAcceptable( true ) );
  QVERIFY( def->checkValueIsAcceptable( 1 ) );
  QVERIFY( def->checkValueIsAcceptable( "1" ) );
  QVERIFY( def->checkValueIsAcceptable( "1,2" ) );
  QVERIFY( def->checkValueIsAcceptable( 0 ) );
  QVERIFY( def->checkValueIsAcceptable( QVariantList() << 1 ) );
  QVERIFY( !def->checkValueIsAcceptable( QVariantList() << "a" ) );
  QVERIFY( !def->checkValueIsAcceptable( 15 ) );
  QVERIFY( !def->checkValueIsAcceptable( -1 ) );
  QVERIFY( !def->checkValueIsAcceptable( "layer12312312" ) );
  QVERIFY( !def->checkValueIsAcceptable( "" ) );
  QVERIFY( !def->checkValueIsAcceptable( QVariant() ) );

  params.insert( "non_optional", QString( "1,2" ) );
  QList< int > iNumbers = QgsProcessingParameters::parameterAsEnums( def.get(), params, context );
  QCOMPARE( iNumbers, QList<int>() << 1 << 2 );
  params.insert( "non_optional", QVariantList() << 0 << 2 );
  iNumbers = QgsProcessingParameters::parameterAsEnums( def.get(), params, context );
  QCOMPARE( iNumbers, QList<int>() << 0 << 2 );

  QCOMPARE( def->valueAsPythonString( QVariantList() << 1 << 2, context ), QStringLiteral( "[1,2]" ) );
  QCOMPARE( def->valueAsPythonString( QStringLiteral( "1,2" ), context ), QStringLiteral( "[1,2]" ) );

  // optional
  def.reset( new QgsProcessingParameterEnum( "optional", QString(), QStringList() << "a" << "b", false, 5, true ) );
  QVERIFY( def->checkValueIsAcceptable( 1 ) );
  QVERIFY( def->checkValueIsAcceptable( "1" ) );
  QVERIFY( !def->checkValueIsAcceptable( "1,2" ) );
  QVERIFY( def->checkValueIsAcceptable( 0 ) );
  QVERIFY( !def->checkValueIsAcceptable( QVariantList() << 1 ) );
  QVERIFY( !def->checkValueIsAcceptable( QVariantList() << "a" ) );
  QVERIFY( !def->checkValueIsAcceptable( 15 ) );
  QVERIFY( !def->checkValueIsAcceptable( -1 ) );
  QVERIFY( !def->checkValueIsAcceptable( "layer12312312" ) );
  QVERIFY( !def->checkValueIsAcceptable( "" ) );
  QVERIFY( def->checkValueIsAcceptable( QVariant() ) );

  params.insert( "optional",  QVariant() );
  iNumber = QgsProcessingParameters::parameterAsEnum( def.get(), params, context );
  QCOMPARE( iNumber, 5 );
  // unconvertible string
  params.insert( "optional",  QVariant( "aaaa" ) );
  iNumber = QgsProcessingParameters::parameterAsEnum( def.get(), params, context );
  QCOMPARE( iNumber, 5 );
  //optional with multiples
  def.reset( new QgsProcessingParameterEnum( "optional", QString(), QStringList() << "A" << "B" << "C", true, QVariantList() << 1 << 2, true ) );
  QVERIFY( def->checkValueIsAcceptable( 1 ) );
  QVERIFY( def->checkValueIsAcceptable( "1" ) );
  QVERIFY( def->checkValueIsAcceptable( "1,2" ) );
  QVERIFY( def->checkValueIsAcceptable( 0 ) );
  QVERIFY( def->checkValueIsAcceptable( QVariantList() << 1 ) );
  QVERIFY( !def->checkValueIsAcceptable( QVariantList() << "a" ) );
  QVERIFY( !def->checkValueIsAcceptable( 15 ) );
  QVERIFY( !def->checkValueIsAcceptable( -1 ) );
  QVERIFY( !def->checkValueIsAcceptable( "layer12312312" ) );
  QVERIFY( !def->checkValueIsAcceptable( "" ) );
  QVERIFY( def->checkValueIsAcceptable( QVariant() ) );

  params.insert( "optional",  QVariant() );
  iNumbers = QgsProcessingParameters::parameterAsEnums( def.get(), params, context );
  QCOMPARE( iNumbers, QList<int>() << 1 << 2 );
  def.reset( new QgsProcessingParameterEnum( "optional", QString(), QStringList() << "A" << "B" << "C", true, "1,2", true ) );
  params.insert( "optional",  QVariant() );
  iNumbers = QgsProcessingParameters::parameterAsEnums( def.get(), params, context );
  QCOMPARE( iNumbers, QList<int>() << 1 << 2 );
}

void TestQgsProcessing::parameterString()
{
  QgsProcessingContext context;

  // not optional!
  std::unique_ptr< QgsProcessingParameterString > def( new QgsProcessingParameterString( "non_optional", QString(), QString(), false, false ) );
  QVERIFY( def->checkValueIsAcceptable( 1 ) );
  QVERIFY( def->checkValueIsAcceptable( "test" ) );
  QVERIFY( !def->checkValueIsAcceptable( "" ) );
  QVERIFY( !def->checkValueIsAcceptable( QVariant() ) );

  // string
  QVariantMap params;
  params.insert( "non_optional", QString( "abcdef" ) );
  QCOMPARE( QgsProcessingParameters::parameterAsString( def.get(), params, context ), QString( "abcdef" ) );

  QCOMPARE( def->valueAsPythonString( 5, context ), QStringLiteral( "'5'" ) );
  QCOMPARE( def->valueAsPythonString( QStringLiteral( "abc" ), context ), QStringLiteral( "'abc'" ) );
  QCOMPARE( def->valueAsPythonString( QStringLiteral( "abc\ndef" ), context ), QStringLiteral( "'abc\\ndef'" ) );
  QCOMPARE( def->valueAsPythonString( QVariant::fromValue( QgsProperty::fromExpression( "\"a\"=1" ) ), context ), QStringLiteral( "QgsProperty.fromExpression('\"a\"=1')" ) );


  // optional
  def.reset( new QgsProcessingParameterString( "optional", QString(), QString( "default" ), false, true ) );
  QVERIFY( def->checkValueIsAcceptable( 1 ) );
  QVERIFY( def->checkValueIsAcceptable( "test" ) );
  QVERIFY( def->checkValueIsAcceptable( "" ) );
  QVERIFY( def->checkValueIsAcceptable( QVariant() ) );

  params.insert( "optional",  QVariant() );
  QCOMPARE( QgsProcessingParameters::parameterAsString( def.get(), params, context ), QString( "default" ) );
}

void TestQgsProcessing::parameterExpression()
{
  QgsProcessingContext context;

  // not optional!
  std::unique_ptr< QgsProcessingParameterExpression > def( new QgsProcessingParameterExpression( "non_optional", QString(), QString(), QString(), false ) );
  QVERIFY( def->checkValueIsAcceptable( 1 ) );
  QVERIFY( def->checkValueIsAcceptable( "test" ) );
  QVERIFY( !def->checkValueIsAcceptable( "" ) );
  QVERIFY( !def->checkValueIsAcceptable( QVariant() ) );

  // string
  QVariantMap params;
  params.insert( "non_optional", QString( "abcdef" ) );
  QCOMPARE( QgsProcessingParameters::parameterAsExpression( def.get(), params, context ), QString( "abcdef" ) );

  QCOMPARE( def->valueAsPythonString( 5, context ), QStringLiteral( "'5'" ) );
  QCOMPARE( def->valueAsPythonString( QStringLiteral( "abc" ), context ), QStringLiteral( "'abc'" ) );
  QCOMPARE( def->valueAsPythonString( QStringLiteral( "abc\ndef" ), context ), QStringLiteral( "'abc\\ndef'" ) );
  QCOMPARE( def->valueAsPythonString( QVariant::fromValue( QgsProperty::fromExpression( "\"a\"=1" ) ), context ), QStringLiteral( "QgsProperty.fromExpression('\"a\"=1')" ) );


  // optional
  def.reset( new QgsProcessingParameterExpression( "optional", QString(), QString( "default" ), QString(), true ) );
  QVERIFY( def->checkValueIsAcceptable( 1 ) );
  QVERIFY( def->checkValueIsAcceptable( "test" ) );
  QVERIFY( def->checkValueIsAcceptable( "" ) );
  QVERIFY( def->checkValueIsAcceptable( QVariant() ) );

  params.insert( "optional",  QVariant() );
  QCOMPARE( QgsProcessingParameters::parameterAsExpression( def.get(), params, context ), QString( "default" ) );
  // valid expression, should not fallback
  params.insert( "optional",  QVariant( "1+2" ) );
  QCOMPARE( QgsProcessingParameters::parameterAsExpression( def.get(), params, context ), QString( "1+2" ) );
  // invalid expression, should fallback
  params.insert( "optional",  QVariant( "1+" ) );
  QCOMPARE( QgsProcessingParameters::parameterAsExpression( def.get(), params, context ), QString( "default" ) );
}

void TestQgsProcessing::parameterField()
{
  QgsProcessingContext context;

  // not optional!
  std::unique_ptr< QgsProcessingParameterTableField > def( new QgsProcessingParameterTableField( "non_optional", QString(), QString(), QString(), QgsProcessingParameterTableField::Any, false, false ) );
  QVERIFY( def->checkValueIsAcceptable( 1 ) );
  QVERIFY( def->checkValueIsAcceptable( "test" ) );
  QVERIFY( !def->checkValueIsAcceptable( QStringList() << "a" << "b" ) );
  QVERIFY( !def->checkValueIsAcceptable( QVariantList() << "a" << "b" ) );
  QVERIFY( !def->checkValueIsAcceptable( "" ) );
  QVERIFY( !def->checkValueIsAcceptable( QVariant() ) );

  // string
  QVariantMap params;
  params.insert( "non_optional", QString( "a" ) );
  QStringList fields = QgsProcessingParameters::parameterAsFields( def.get(), params, context );
  QCOMPARE( fields, QStringList() << "a" );

  QCOMPARE( def->valueAsPythonString( QStringLiteral( "abc" ), context ), QStringLiteral( "'abc'" ) );
  QCOMPARE( def->valueAsPythonString( QVariant::fromValue( QgsProperty::fromExpression( "\"a\"=1" ) ), context ), QStringLiteral( "QgsProperty.fromExpression('\"a\"=1')" ) );

  // multiple
  def.reset( new QgsProcessingParameterTableField( "non_optional", QString(), QString(), QString(), QgsProcessingParameterTableField::Any, true, false ) );
  QVERIFY( def->checkValueIsAcceptable( 1 ) );
  QVERIFY( def->checkValueIsAcceptable( "test" ) );
  QVERIFY( def->checkValueIsAcceptable( QStringList() << "a" << "b" ) );
  QVERIFY( def->checkValueIsAcceptable( QVariantList() << "a" << "b" ) );
  QVERIFY( !def->checkValueIsAcceptable( "" ) );
  QVERIFY( !def->checkValueIsAcceptable( QVariant() ) );

  params.insert( "non_optional", QString( "a;b" ) );
  fields = QgsProcessingParameters::parameterAsFields( def.get(), params, context );
  QCOMPARE( fields, QStringList() << "a" << "b" );
  params.insert( "non_optional", QVariantList() << "a" << "b" );
  fields = QgsProcessingParameters::parameterAsFields( def.get(), params, context );
  QCOMPARE( fields, QStringList() << "a" << "b" );

  QCOMPARE( def->valueAsPythonString( QStringList() << "a" << "b", context ), QStringLiteral( "['a','b']" ) );
  QCOMPARE( def->valueAsPythonString( QStringList() << "a" << "b", context ), QStringLiteral( "['a','b']" ) );

  // optional
  def.reset( new QgsProcessingParameterTableField( "optional", QString(), QString( "def" ), QString(), QgsProcessingParameterTableField::Any, false, true ) );
  QVERIFY( def->checkValueIsAcceptable( 1 ) );
  QVERIFY( def->checkValueIsAcceptable( "test" ) );
  QVERIFY( !def->checkValueIsAcceptable( QStringList() << "a" << "b" ) );
  QVERIFY( !def->checkValueIsAcceptable( QVariantList() << "a" << "b" ) );
  QVERIFY( def->checkValueIsAcceptable( "" ) );
  QVERIFY( def->checkValueIsAcceptable( QVariant() ) );

  params.insert( "optional",  QVariant() );
  fields = QgsProcessingParameters::parameterAsFields( def.get(), params, context );
  QCOMPARE( fields, QStringList() << "def" );

  // optional, no default
  def.reset( new QgsProcessingParameterTableField( "optional", QString(), QVariant(), QString(), QgsProcessingParameterTableField::Any, false, true ) );
  params.insert( "optional",  QVariant() );
  fields = QgsProcessingParameters::parameterAsFields( def.get(), params, context );
  QVERIFY( fields.isEmpty() );

  //optional with multiples
  def.reset( new QgsProcessingParameterTableField( "optional", QString(), QString( "abc;def" ), QString(), QgsProcessingParameterTableField::Any, true, true ) );
  QVERIFY( def->checkValueIsAcceptable( 1 ) );
  QVERIFY( def->checkValueIsAcceptable( "test" ) );
  QVERIFY( def->checkValueIsAcceptable( QStringList() << "a" << "b" ) );
  QVERIFY( def->checkValueIsAcceptable( QVariantList() << "a" << "b" ) );
  QVERIFY( def->checkValueIsAcceptable( "" ) );
  QVERIFY( def->checkValueIsAcceptable( QVariant() ) );

  params.insert( "optional",  QVariant() );
  fields = QgsProcessingParameters::parameterAsFields( def.get(), params, context );
  QCOMPARE( fields, QStringList() << "abc" << "def" );
  def.reset( new QgsProcessingParameterTableField( "optional", QString(), QVariantList() << "abc" << "def", QString(), QgsProcessingParameterTableField::Any, true, true ) );
  params.insert( "optional",  QVariant() );
  fields = QgsProcessingParameters::parameterAsFields( def.get(), params, context );
  QCOMPARE( fields, QStringList() << "abc" << "def" );
}

void TestQgsProcessing::parameterFeatureSource()
{
  // setup a context
  QgsProject p;
  p.setCrs( QgsCoordinateReferenceSystem::fromEpsgId( 28353 ) );
  QString testDataDir = QStringLiteral( TEST_DATA_DIR ) + '/'; //defined in CmakeLists.txt
  QString vector1 = testDataDir + "multipoint.shp";
  QString raster = testDataDir + "landsat.tif";
  QFileInfo fi1( raster );
  QgsRasterLayer *r1 = new QgsRasterLayer( fi1.filePath(), "R1" );
  QgsVectorLayer *v1 = new QgsVectorLayer( "Polygon?crs=EPSG:3111", "V4", "memory" );
  p.addMapLayers( QList<QgsMapLayer *>() << v1 << r1 );
  QgsProcessingContext context;
  context.setProject( &p );

  // not optional!
  std::unique_ptr< QgsProcessingParameterFeatureSource > def( new QgsProcessingParameterFeatureSource( "non_optional", QString(), QList< int >() << QgsProcessingParameterDefinition::TypeVectorAny, QString( "EPSG:3113" ), false ) );
  QVERIFY( !def->checkValueIsAcceptable( false ) );
  QVERIFY( !def->checkValueIsAcceptable( true ) );
  QVERIFY( !def->checkValueIsAcceptable( 5 ) );
  QVERIFY( def->checkValueIsAcceptable( "layer12312312" ) );
  QVERIFY( !def->checkValueIsAcceptable( "" ) );
  QVERIFY( !def->checkValueIsAcceptable( QVariant() ) );
  QVERIFY( def->checkValueIsAcceptable( QgsProcessingFeatureSourceDefinition( "layer1231123" ) ) );
  QVERIFY( def->checkValueIsAcceptable( QVariant::fromValue( v1 ) ) );
  QVERIFY( !def->checkValueIsAcceptable( QVariant::fromValue( r1 ) ) );

  // should be OK
  QVERIFY( def->checkValueIsAcceptable( "c:/Users/admin/Desktop/roads_clipped_transformed_v1_reprojected_final_clipped_aAAA.shp" ) );
  // ... unless we use context, when the check that the layer actually exists is performed
  QVERIFY( !def->checkValueIsAcceptable( "c:/Users/admin/Desktop/roads_clipped_transformed_v1_reprojected_final_clipped_aAAA.shp", &context ) );

  // using existing map layer ID
  QVariantMap params;
  params.insert( "non_optional",  v1->id() );
  QCOMPARE( QgsProcessingParameters::parameterAsVectorLayer( def.get(), params, context )->id(), v1->id() );

  // using existing layer
  params.insert( "non_optional",  QVariant::fromValue( v1 ) );
  QCOMPARE( QgsProcessingParameters::parameterAsVectorLayer( def.get(), params, context )->id(), v1->id() );

  // not vector layer
  params.insert( "non_optional",  r1->id() );
  QVERIFY( !QgsProcessingParameters::parameterAsVectorLayer( def.get(), params, context ) );

  // using existing non-vector layer
  params.insert( "non_optional",  QVariant::fromValue( r1 ) );
  QVERIFY( !QgsProcessingParameters::parameterAsVectorLayer( def.get(), params, context ) );

  // string representing a layer source
  params.insert( "non_optional", vector1 );
  QCOMPARE( QgsProcessingParameters::parameterAsVectorLayer( def.get(), params, context )->publicSource(), vector1 );

  // nonsense string
  params.insert( "non_optional", QString( "i'm not a layer, and nothing you can do will make me one" ) );
  QVERIFY( !QgsProcessingParameters::parameterAsVectorLayer( def.get(), params, context ) );

  QCOMPARE( def->valueAsPythonString( QStringLiteral( "abc" ), context ), QStringLiteral( "'abc'" ) );
  QCOMPARE( def->valueAsPythonString( QVariant::fromValue( QgsProcessingFeatureSourceDefinition( "abc" ) ), context ), QStringLiteral( "QgsProcessingFeatureSourceDefinition('abc', False)" ) );
  QCOMPARE( def->valueAsPythonString( QVariant::fromValue( QgsProcessingFeatureSourceDefinition( QgsProperty::fromValue( "abc" ), true ) ), context ), QStringLiteral( "QgsProcessingFeatureSourceDefinition('abc', True)" ) );
  QCOMPARE( def->valueAsPythonString( QVariant::fromValue( QgsProcessingFeatureSourceDefinition( QgsProperty::fromExpression( "\"abc\" || \"def\"" ) ) ), context ), QStringLiteral( "QgsProcessingFeatureSourceDefinition(QgsProperty.fromExpression('\"abc\" || \"def\"'), False)" ) );
  QCOMPARE( def->valueAsPythonString( QVariant::fromValue( QgsProperty::fromExpression( "\"a\"=1" ) ), context ), QStringLiteral( "QgsProperty.fromExpression('\"a\"=1')" ) );

  // optional
  def.reset( new QgsProcessingParameterFeatureSource( "optional", QString(), QList< int >() << QgsProcessingParameterDefinition::TypeVectorAny, v1->id(), true ) );
  params.insert( "optional",  QVariant() );
  QCOMPARE( QgsProcessingParameters::parameterAsVectorLayer( def.get(), params,  context )->id(), v1->id() );
  QVERIFY( def->checkValueIsAcceptable( false ) );
  QVERIFY( def->checkValueIsAcceptable( true ) );
  QVERIFY( def->checkValueIsAcceptable( 5 ) );
  QVERIFY( def->checkValueIsAcceptable( "layer12312312" ) );
  QVERIFY( def->checkValueIsAcceptable( "c:/Users/admin/Desktop/roads_clipped_transformed_v1_reprojected_final_clipped_aAAA.shp" ) );
  QVERIFY( def->checkValueIsAcceptable( "" ) );
  QVERIFY( def->checkValueIsAcceptable( QVariant() ) );
  QVERIFY( def->checkValueIsAcceptable( QgsProcessingFeatureSourceDefinition( "layer1231123" ) ) );

  //optional with direct layer default
  def.reset( new QgsProcessingParameterFeatureSource( "optional", QString(), QList< int >() << QgsProcessingParameterDefinition::TypeVectorAny, QVariant::fromValue( v1 ), true ) );
  QCOMPARE( QgsProcessingParameters::parameterAsVectorLayer( def.get(), params,  context )->id(), v1->id() );
}

void TestQgsProcessing::parameterFeatureSink()
{
  // setup a context
  QgsProject p;
  p.setCrs( QgsCoordinateReferenceSystem::fromEpsgId( 28353 ) );
  QgsProcessingContext context;
  context.setProject( &p );

  // not optional!
  std::unique_ptr< QgsProcessingParameterFeatureSink > def( new QgsProcessingParameterFeatureSink( "non_optional", QString(), QgsProcessingParameterDefinition::TypeVectorAny, QString( "EPSG:3113" ), false ) );
  QVERIFY( !def->checkValueIsAcceptable( false ) );
  QVERIFY( !def->checkValueIsAcceptable( true ) );
  QVERIFY( !def->checkValueIsAcceptable( 5 ) );
  QVERIFY( def->checkValueIsAcceptable( "layer12312312" ) );
  QVERIFY( !def->checkValueIsAcceptable( "" ) );
  QVERIFY( !def->checkValueIsAcceptable( QVariant() ) );
  QVERIFY( def->checkValueIsAcceptable( QgsProcessingOutputLayerDefinition( "layer1231123" ) ) );

  // should be OK with or without context - it's an output layer!
  QVERIFY( def->checkValueIsAcceptable( "c:/Users/admin/Desktop/roads_clipped_transformed_v1_reprojected_final_clipped_aAAA.shp" ) );
  QVERIFY( def->checkValueIsAcceptable( "c:/Users/admin/Desktop/roads_clipped_transformed_v1_reprojected_final_clipped_aAAA.shp", &context ) );

  QCOMPARE( def->valueAsPythonString( QStringLiteral( "abc" ), context ), QStringLiteral( "'abc'" ) );
  QCOMPARE( def->valueAsPythonString( QVariant::fromValue( QgsProcessingOutputLayerDefinition( "abc" ) ), context ), QStringLiteral( "QgsProcessingOutputLayerDefinition('abc')" ) );
  QCOMPARE( def->valueAsPythonString( QVariant::fromValue( QgsProcessingOutputLayerDefinition( QgsProperty::fromValue( "abc" ) ) ), context ), QStringLiteral( "QgsProcessingOutputLayerDefinition('abc')" ) );
  QCOMPARE( def->valueAsPythonString( QVariant::fromValue( QgsProcessingOutputLayerDefinition( QgsProperty::fromExpression( "\"abc\" || \"def\"" ) ) ), context ), QStringLiteral( "QgsProcessingOutputLayerDefinition(QgsProperty.fromExpression('\"abc\" || \"def\"'))" ) );
  QCOMPARE( def->valueAsPythonString( QVariant::fromValue( QgsProperty::fromExpression( "\"a\"=1" ) ), context ), QStringLiteral( "QgsProperty.fromExpression('\"a\"=1')" ) );

  // optional
  def.reset( new QgsProcessingParameterFeatureSink( "optional", QString(), QgsProcessingParameterDefinition::TypeVectorAny, QString(), true ) );
  QVERIFY( !def->checkValueIsAcceptable( false ) );
  QVERIFY( !def->checkValueIsAcceptable( true ) );
  QVERIFY( !def->checkValueIsAcceptable( 5 ) );
  QVERIFY( def->checkValueIsAcceptable( "layer12312312" ) );
  QVERIFY( def->checkValueIsAcceptable( "c:/Users/admin/Desktop/roads_clipped_transformed_v1_reprojected_final_clipped_aAAA.shp" ) );
  QVERIFY( def->checkValueIsAcceptable( "" ) );
  QVERIFY( def->checkValueIsAcceptable( QVariant() ) );
  QVERIFY( def->checkValueIsAcceptable( QgsProcessingOutputLayerDefinition( "layer1231123" ) ) );

  // test hasGeometry
  QVERIFY( QgsProcessingParameterFeatureSink( "test", QString(), QgsProcessingParameterDefinition::TypeAny ).hasGeometry() );
  QVERIFY( QgsProcessingParameterFeatureSink( "test", QString(), QgsProcessingParameterDefinition::TypeVectorAny ).hasGeometry() );
  QVERIFY( QgsProcessingParameterFeatureSink( "test", QString(), QgsProcessingParameterDefinition::TypeVectorPoint ).hasGeometry() );
  QVERIFY( QgsProcessingParameterFeatureSink( "test", QString(), QgsProcessingParameterDefinition::TypeVectorLine ).hasGeometry() );
  QVERIFY( QgsProcessingParameterFeatureSink( "test", QString(), QgsProcessingParameterDefinition::TypeVectorPolygon ).hasGeometry() );
  QVERIFY( !QgsProcessingParameterFeatureSink( "test", QString(), QgsProcessingParameterDefinition::TypeRaster ).hasGeometry() );
  QVERIFY( !QgsProcessingParameterFeatureSink( "test", QString(), QgsProcessingParameterDefinition::TypeFile ).hasGeometry() );
  QVERIFY( QgsProcessingParameterFeatureSink( "test", QString(), QgsProcessingParameterDefinition::TypeTable ).hasGeometry() );

}

void TestQgsProcessing::parameterRasterOut()
{
  // setup a context
  QgsProject p;
  p.setCrs( QgsCoordinateReferenceSystem::fromEpsgId( 28353 ) );
  QgsProcessingContext context;
  context.setProject( &p );

  // not optional!
  std::unique_ptr< QgsProcessingParameterRasterOutput > def( new QgsProcessingParameterRasterOutput( "non_optional", QString(), QString(), false ) );
  QVERIFY( !def->checkValueIsAcceptable( false ) );
  QVERIFY( !def->checkValueIsAcceptable( true ) );
  QVERIFY( !def->checkValueIsAcceptable( 5 ) );
  QVERIFY( def->checkValueIsAcceptable( "layer12312312" ) );
  QVERIFY( !def->checkValueIsAcceptable( "" ) );
  QVERIFY( !def->checkValueIsAcceptable( QVariant() ) );
  QVERIFY( def->checkValueIsAcceptable( QgsProcessingOutputLayerDefinition( "layer1231123" ) ) );

  // should be OK with or without context - it's an output layer!
  QVERIFY( def->checkValueIsAcceptable( "c:/Users/admin/Desktop/roads_clipped_transformed_v1_reprojected_final_clipped_aAAA.tif" ) );
  QVERIFY( def->checkValueIsAcceptable( "c:/Users/admin/Desktop/roads_clipped_transformed_v1_reprojected_final_clipped_aAAA.tif", &context ) );

  QVariantMap params;
  params.insert( "non_optional", "test.tif" );
  QCOMPARE( QgsProcessingParameters::parameterAsRasterOutputLayer( def.get(), params, context ), QStringLiteral( "test.tif" ) );
  params.insert( "non_optional", QgsProcessingOutputLayerDefinition( "test.tif" ) );
  QCOMPARE( QgsProcessingParameters::parameterAsRasterOutputLayer( def.get(), params, context ), QStringLiteral( "test.tif" ) );

  QCOMPARE( def->valueAsPythonString( QStringLiteral( "abc" ), context ), QStringLiteral( "'abc'" ) );
  QCOMPARE( def->valueAsPythonString( QVariant::fromValue( QgsProcessingOutputLayerDefinition( "abc" ) ), context ), QStringLiteral( "QgsProcessingOutputLayerDefinition('abc')" ) );
  QCOMPARE( def->valueAsPythonString( QVariant::fromValue( QgsProcessingOutputLayerDefinition( QgsProperty::fromValue( "abc" ) ) ), context ), QStringLiteral( "QgsProcessingOutputLayerDefinition('abc')" ) );
  QCOMPARE( def->valueAsPythonString( QVariant::fromValue( QgsProcessingOutputLayerDefinition( QgsProperty::fromExpression( "\"abc\" || \"def\"" ) ) ), context ), QStringLiteral( "QgsProcessingOutputLayerDefinition(QgsProperty.fromExpression('\"abc\" || \"def\"'))" ) );
  QCOMPARE( def->valueAsPythonString( QVariant::fromValue( QgsProperty::fromExpression( "\"a\"=1" ) ), context ), QStringLiteral( "QgsProperty.fromExpression('\"a\"=1')" ) );

  // optional
  def.reset( new QgsProcessingParameterRasterOutput( "optional", QString(), QString( "default.tif" ), true ) );
  QVERIFY( !def->checkValueIsAcceptable( false ) );
  QVERIFY( !def->checkValueIsAcceptable( true ) );
  QVERIFY( !def->checkValueIsAcceptable( 5 ) );
  QVERIFY( def->checkValueIsAcceptable( "layer12312312" ) );
  QVERIFY( def->checkValueIsAcceptable( "c:/Users/admin/Desktop/roads_clipped_transformed_v1_reprojected_final_clipped_aAAA.tif" ) );
  QVERIFY( def->checkValueIsAcceptable( "" ) );
  QVERIFY( def->checkValueIsAcceptable( QVariant() ) );
  QVERIFY( def->checkValueIsAcceptable( QgsProcessingOutputLayerDefinition( "layer1231123" ) ) );

  params.insert( "optional", QVariant() );
  QCOMPARE( QgsProcessingParameters::parameterAsRasterOutputLayer( def.get(), params, context ), QStringLiteral( "default.tif" ) );

}

void TestQgsProcessing::parameterFileOut()
{
  // setup a context
  QgsProject p;
  p.setCrs( QgsCoordinateReferenceSystem::fromEpsgId( 28353 ) );
  QgsProcessingContext context;
  context.setProject( &p );

  // not optional!
  std::unique_ptr< QgsProcessingParameterFileOutput > def( new QgsProcessingParameterFileOutput( "non_optional", QString(), QStringLiteral( "BMP files (*.bmp)" ), QString(), false ) );
  QCOMPARE( def->fileFilter(), QStringLiteral( "BMP files (*.bmp)" ) );
  def->setFileFilter( QStringLiteral( "PCX files (*.pcx)" ) );
  QCOMPARE( def->fileFilter(), QStringLiteral( "PCX files (*.pcx)" ) );

  QVERIFY( !def->checkValueIsAcceptable( false ) );
  QVERIFY( !def->checkValueIsAcceptable( true ) );
  QVERIFY( !def->checkValueIsAcceptable( 5 ) );
  QVERIFY( def->checkValueIsAcceptable( "layer12312312" ) );
  QVERIFY( !def->checkValueIsAcceptable( "" ) );
  QVERIFY( !def->checkValueIsAcceptable( QVariant() ) );
  QVERIFY( def->checkValueIsAcceptable( QgsProcessingOutputLayerDefinition( "layer1231123" ) ) );

  // should be OK with or without context - it's an output file!
  QVERIFY( def->checkValueIsAcceptable( "c:/Users/admin/Desktop/roads_clipped_transformed_v1_reprojected_final_clipped_aAAA.txt" ) );
  QVERIFY( def->checkValueIsAcceptable( "c:/Users/admin/Desktop/roads_clipped_transformed_v1_reprojected_final_clipped_aAAA.txt", &context ) );

  QVariantMap params;
  params.insert( "non_optional", "test.txt" );
  QCOMPARE( QgsProcessingParameters::parameterAsFileOutput( def.get(), params, context ), QStringLiteral( "test.txt" ) );
  params.insert( "non_optional", QgsProcessingOutputLayerDefinition( "test.txt" ) );
  QCOMPARE( QgsProcessingParameters::parameterAsFileOutput( def.get(), params, context ), QStringLiteral( "test.txt" ) );

  QCOMPARE( def->valueAsPythonString( QStringLiteral( "abc" ), context ), QStringLiteral( "'abc'" ) );
  QCOMPARE( def->valueAsPythonString( QVariant::fromValue( QgsProcessingOutputLayerDefinition( "abc" ) ), context ), QStringLiteral( "QgsProcessingOutputLayerDefinition('abc')" ) );
  QCOMPARE( def->valueAsPythonString( QVariant::fromValue( QgsProcessingOutputLayerDefinition( QgsProperty::fromValue( "abc" ) ) ), context ), QStringLiteral( "QgsProcessingOutputLayerDefinition('abc')" ) );
  QCOMPARE( def->valueAsPythonString( QVariant::fromValue( QgsProcessingOutputLayerDefinition( QgsProperty::fromExpression( "\"abc\" || \"def\"" ) ) ), context ), QStringLiteral( "QgsProcessingOutputLayerDefinition(QgsProperty.fromExpression('\"abc\" || \"def\"'))" ) );
  QCOMPARE( def->valueAsPythonString( QVariant::fromValue( QgsProperty::fromExpression( "\"a\"=1" ) ), context ), QStringLiteral( "QgsProperty.fromExpression('\"a\"=1')" ) );


  // optional
  def.reset( new QgsProcessingParameterFileOutput( "optional", QString(), QString(), QString( "default.txt" ), true ) );
  QVERIFY( !def->checkValueIsAcceptable( false ) );
  QVERIFY( !def->checkValueIsAcceptable( true ) );
  QVERIFY( !def->checkValueIsAcceptable( 5 ) );
  QVERIFY( def->checkValueIsAcceptable( "layer12312312" ) );
  QVERIFY( def->checkValueIsAcceptable( "c:/Users/admin/Desktop/roads_clipped_transformed_v1_reprojected_final_clipped_aAAA.txt" ) );
  QVERIFY( def->checkValueIsAcceptable( "" ) );
  QVERIFY( def->checkValueIsAcceptable( QVariant() ) );
  QVERIFY( def->checkValueIsAcceptable( QgsProcessingOutputLayerDefinition( "layer1231123" ) ) );

  params.insert( "optional", QVariant() );
  QCOMPARE( QgsProcessingParameters::parameterAsFileOutput( def.get(), params, context ), QStringLiteral( "default.txt" ) );
}

void TestQgsProcessing::parameterFolderOut()
{
  // setup a context
  QgsProject p;
  QgsProcessingContext context;
  context.setProject( &p );

  // not optional!
  std::unique_ptr< QgsProcessingParameterFolderOutput > def( new QgsProcessingParameterFolderOutput( "non_optional", QString(), QString(), false ) );

  QVERIFY( !def->checkValueIsAcceptable( false ) );
  QVERIFY( !def->checkValueIsAcceptable( true ) );
  QVERIFY( !def->checkValueIsAcceptable( 5 ) );
  QVERIFY( def->checkValueIsAcceptable( "asdasd" ) );
  QVERIFY( !def->checkValueIsAcceptable( "" ) );
  QVERIFY( !def->checkValueIsAcceptable( QVariant() ) );

  // should be OK with or without context - it's an output folder!
  QVERIFY( def->checkValueIsAcceptable( "c:/Users/admin/Desktop/" ) );
  QVERIFY( def->checkValueIsAcceptable( "c:/Users/admin/Desktop/", &context ) );

  QVariantMap params;
  params.insert( "non_optional", "c:/mine" );
  QCOMPARE( QgsProcessingParameters::parameterAsFileOutput( def.get(), params, context ), QStringLiteral( "c:/mine" ) );

  QCOMPARE( def->valueAsPythonString( QStringLiteral( "abc" ), context ), QStringLiteral( "'abc'" ) );
  QCOMPARE( def->valueAsPythonString( QVariant::fromValue( QgsProperty::fromExpression( "\"a\"=1" ) ), context ), QStringLiteral( "QgsProperty.fromExpression('\"a\"=1')" ) );

  // optional
  def.reset( new QgsProcessingParameterFolderOutput( "optional", QString(), QString( "c:/junk" ), true ) );
  QVERIFY( !def->checkValueIsAcceptable( false ) );
  QVERIFY( !def->checkValueIsAcceptable( true ) );
  QVERIFY( !def->checkValueIsAcceptable( 5 ) );
  QVERIFY( def->checkValueIsAcceptable( "layer12312312" ) );
  QVERIFY( def->checkValueIsAcceptable( "c:/Users/admin/Desktop/" ) );
  QVERIFY( def->checkValueIsAcceptable( "" ) );
  QVERIFY( def->checkValueIsAcceptable( QVariant() ) );

  params.insert( "optional", QVariant() );
  QCOMPARE( QgsProcessingParameters::parameterAsFileOutput( def.get(), params, context ), QStringLiteral( "c:/junk" ) );
}

void TestQgsProcessing::checkParamValues()
{
  DummyAlgorithm a( "asd" );
  a.checkParameterVals();
}

void TestQgsProcessing::combineLayerExtent()
{
  QgsRectangle ext = QgsProcessingUtils::combineLayerExtents( QList< QgsMapLayer *>() );
  QVERIFY( ext.isNull() );

  QString testDataDir = QStringLiteral( TEST_DATA_DIR ) + '/'; //defined in CmakeLists.txt

  QString raster1 = testDataDir + "tenbytenraster.asc";
  QString raster2 = testDataDir + "landsat.tif";
  QFileInfo fi1( raster1 );
  QgsRasterLayer *r1 = new QgsRasterLayer( fi1.filePath(), "R1" );
  QFileInfo fi2( raster2 );
  QgsRasterLayer *r2 = new QgsRasterLayer( fi2.filePath(), "R2" );

  ext = QgsProcessingUtils::combineLayerExtents( QList< QgsMapLayer *>() << r1 );
  QGSCOMPARENEAR( ext.xMinimum(), 1535375.000000, 10 );
  QGSCOMPARENEAR( ext.xMaximum(), 1535475, 10 );
  QGSCOMPARENEAR( ext.yMinimum(), 5083255, 10 );
  QGSCOMPARENEAR( ext.yMaximum(), 5083355, 10 );

  ext = QgsProcessingUtils::combineLayerExtents( QList< QgsMapLayer *>() << r1 << r2 );
  QGSCOMPARENEAR( ext.xMinimum(), 781662, 10 );
  QGSCOMPARENEAR( ext.xMaximum(), 1535475, 10 );
  QGSCOMPARENEAR( ext.yMinimum(), 3339523, 10 );
  QGSCOMPARENEAR( ext.yMaximum(), 5083355, 10 );

  // with reprojection
  ext = QgsProcessingUtils::combineLayerExtents( QList< QgsMapLayer *>() << r1 << r2, QgsCoordinateReferenceSystem::fromEpsgId( 3785 ) );
  QGSCOMPARENEAR( ext.xMinimum(), 1995320, 10 );
  QGSCOMPARENEAR( ext.xMaximum(), 2008833, 10 );
  QGSCOMPARENEAR( ext.yMinimum(), 3523084, 10 );
  QGSCOMPARENEAR( ext.yMaximum(), 3536664, 10 );
}

void TestQgsProcessing::processingFeatureSource()
{
  QString sourceString = QStringLiteral( "test.shp" );
  QgsProcessingFeatureSourceDefinition fs( sourceString, true );
  QCOMPARE( fs.source.staticValue().toString(), sourceString );
  QVERIFY( fs.selectedFeaturesOnly );

  // test storing QgsProcessingFeatureSource in variant and retrieving
  QVariant fsInVariant = QVariant::fromValue( fs );
  QVERIFY( fsInVariant.isValid() );

  QgsProcessingFeatureSourceDefinition fromVar = qvariant_cast<QgsProcessingFeatureSourceDefinition>( fsInVariant );
  QCOMPARE( fromVar.source.staticValue().toString(), sourceString );
  QVERIFY( fromVar.selectedFeaturesOnly );

  // test evaluating parameter as source
  QgsVectorLayer *layer = new QgsVectorLayer( "Point", "v1", "memory" );
  QgsFeature f( 10001 );
  f.setGeometry( QgsGeometry( new QgsPoint( 1, 2 ) ) );
  layer->dataProvider()->addFeatures( QgsFeatureList() << f );

  QgsProject p;
  p.addMapLayer( layer );
  QgsProcessingContext context;
  context.setProject( &p );

  // first using static string definition
  std::unique_ptr< QgsProcessingParameterDefinition > def( new QgsProcessingParameterString( QStringLiteral( "layer" ) ) );
  QVariantMap params;
  params.insert( QStringLiteral( "layer" ), QgsProcessingFeatureSourceDefinition( layer->id(), false ) );
  std::unique_ptr< QgsFeatureSource > source( QgsProcessingParameters::parameterAsSource( def.get(), params, context ) );
  // can't directly match it to layer, so instead just get the feature and test that it matches what we expect
  QgsFeature f2;
  QVERIFY( source.get() );
  QVERIFY( source->getFeatures().nextFeature( f2 ) );
  QCOMPARE( f2.geometry(), f.geometry() );

  // next using property based definition
  params.insert( QStringLiteral( "layer" ), QgsProcessingFeatureSourceDefinition( QgsProperty::fromExpression( QStringLiteral( "trim('%1' + ' ')" ).arg( layer->id() ) ), false ) );
  source.reset( QgsProcessingParameters::parameterAsSource( def.get(), params, context ) );
  // can't directly match it to layer, so instead just get the feature and test that it matches what we expect
  QVERIFY( source.get() );
  QVERIFY( source->getFeatures().nextFeature( f2 ) );
  QCOMPARE( f2.geometry(), f.geometry() );
}

void TestQgsProcessing::processingFeatureSink()
{
  QString sinkString( QStringLiteral( "test.shp" ) );
  QgsProject p;
  QgsProcessingOutputLayerDefinition fs( sinkString, &p );
  QCOMPARE( fs.sink.staticValue().toString(), sinkString );
  QCOMPARE( fs.destinationProject, &p );

  // test storing QgsProcessingFeatureSink in variant and retrieving
  QVariant fsInVariant = QVariant::fromValue( fs );
  QVERIFY( fsInVariant.isValid() );

  QgsProcessingOutputLayerDefinition fromVar = qvariant_cast<QgsProcessingOutputLayerDefinition>( fsInVariant );
  QCOMPARE( fromVar.sink.staticValue().toString(), sinkString );
  QCOMPARE( fromVar.destinationProject, &p );

  // test evaluating parameter as sink
  QgsProcessingContext context;
  context.setProject( &p );

  // first using static string definition
  std::unique_ptr< QgsProcessingParameterDefinition > def( new QgsProcessingParameterString( QStringLiteral( "layer" ) ) );
  QVariantMap params;
  params.insert( QStringLiteral( "layer" ), QgsProcessingOutputLayerDefinition( "memory:test", nullptr ) );
  QString dest;
  std::unique_ptr< QgsFeatureSink > sink( QgsProcessingParameters::parameterAsSink( def.get(), params, QgsFields(), QgsWkbTypes::Point, QgsCoordinateReferenceSystem( "EPSG:3111" ), context, dest ) );
  QVERIFY( sink.get() );
  QgsVectorLayer *layer = qobject_cast< QgsVectorLayer *>( QgsProcessingUtils::mapLayerFromString( dest, context, false ) );
  QVERIFY( layer );
  QCOMPARE( layer->crs().authid(), QStringLiteral( "EPSG:3111" ) );

  // next using property based definition
  params.insert( QStringLiteral( "layer" ), QgsProcessingOutputLayerDefinition( QgsProperty::fromExpression( QStringLiteral( "trim('memory' + ':test2')" ) ), nullptr ) );
  sink.reset( QgsProcessingParameters::parameterAsSink( def.get(), params, QgsFields(), QgsWkbTypes::Point, QgsCoordinateReferenceSystem( "EPSG:3113" ), context, dest ) );
  QVERIFY( sink.get() );
  QgsVectorLayer *layer2 = qobject_cast< QgsVectorLayer *>( QgsProcessingUtils::mapLayerFromString( dest, context, false ) );
  QVERIFY( layer2 );
  QCOMPARE( layer2->crs().authid(), QStringLiteral( "EPSG:3113" ) );


  // non optional sink
  def.reset( new QgsProcessingParameterFeatureSink( QStringLiteral( "layer" ), QString(), QgsProcessingParameterDefinition::TypeAny, QVariant(), false ) );
  QVERIFY( def->checkValueIsAcceptable( QStringLiteral( "memory:test" ) ) );
  QVERIFY( def->checkValueIsAcceptable( QgsProcessingOutputLayerDefinition( "memory:test" ) ) );
  QVERIFY( def->checkValueIsAcceptable( QgsProperty::fromValue( "memory:test" ) ) );
  QVERIFY( !def->checkValueIsAcceptable( QString() ) );
  QVERIFY( !def->checkValueIsAcceptable( QVariant() ) );
  QVERIFY( !def->checkValueIsAcceptable( 5 ) );
  params.insert( QStringLiteral( "layer" ), QStringLiteral( "memory:test" ) );
  sink.reset( QgsProcessingParameters::parameterAsSink( def.get(), params, QgsFields(), QgsWkbTypes::Point, QgsCoordinateReferenceSystem( "EPSG:3113" ), context, dest ) );
  QVERIFY( sink.get() );

  // optional sink
  def.reset( new QgsProcessingParameterFeatureSink( QStringLiteral( "layer" ), QString(), QgsProcessingParameterDefinition::TypeAny, QVariant(), true ) );
  QVERIFY( def->checkValueIsAcceptable( QStringLiteral( "memory:test" ) ) );
  QVERIFY( def->checkValueIsAcceptable( QgsProcessingOutputLayerDefinition( "memory:test" ) ) );
  QVERIFY( def->checkValueIsAcceptable( QgsProperty::fromValue( "memory:test" ) ) );
  QVERIFY( def->checkValueIsAcceptable( QString() ) );
  QVERIFY( def->checkValueIsAcceptable( QVariant() ) );
  QVERIFY( !def->checkValueIsAcceptable( 5 ) );
  params.insert( QStringLiteral( "layer" ), QStringLiteral( "memory:test" ) );
  sink.reset( QgsProcessingParameters::parameterAsSink( def.get(), params, QgsFields(), QgsWkbTypes::Point, QgsCoordinateReferenceSystem( "EPSG:3113" ), context, dest ) );
  QVERIFY( sink.get() );
  // optional sink, not set - should be no sink
  params.insert( QStringLiteral( "layer" ), QVariant() );
  sink.reset( QgsProcessingParameters::parameterAsSink( def.get(), params, QgsFields(), QgsWkbTypes::Point, QgsCoordinateReferenceSystem( "EPSG:3113" ), context, dest ) );
  QVERIFY( !sink.get() );

  //.... unless there's a default set
  def.reset( new QgsProcessingParameterFeatureSink( QStringLiteral( "layer" ), QString(), QgsProcessingParameterDefinition::TypeAny, QStringLiteral( "memory:defaultlayer" ), true ) );
  params.insert( QStringLiteral( "layer" ), QVariant() );
  sink.reset( QgsProcessingParameters::parameterAsSink( def.get(), params, QgsFields(), QgsWkbTypes::Point, QgsCoordinateReferenceSystem( "EPSG:3113" ), context, dest ) );
  QVERIFY( sink.get() );
}

void TestQgsProcessing::algorithmScope()
{
  QgsProcessingContext pc;

  // no alg
  std::unique_ptr< QgsExpressionContextScope > scope( QgsExpressionContextUtils::processingAlgorithmScope( nullptr, QVariantMap(), pc ) );
  QVERIFY( scope.get() );

  // with alg
  std::unique_ptr< QgsProcessingAlgorithm > alg( new DummyAlgorithm( "alg1" ) );
  QVariantMap params;
  params.insert( QStringLiteral( "a_param" ), 5 );
  scope.reset( QgsExpressionContextUtils::processingAlgorithmScope( alg.get(), params, pc ) );
  QVERIFY( scope.get() );
  QCOMPARE( scope->variable( QStringLiteral( "algorithm_id" ) ).toString(), alg->id() );

  QgsExpressionContext context;
  context.appendScope( scope.release() );
  QgsExpression exp( "parameter('bad')" );
  QVERIFY( !exp.evaluate( &context ).isValid() );
  QgsExpression exp2( "parameter('a_param')" );
  QCOMPARE( exp2.evaluate( &context ).toInt(), 5 );
}

void TestQgsProcessing::validateInputCrs()
{
  DummyAlgorithm alg( "test" );
  alg.runValidateInputCrsChecks();
}

void TestQgsProcessing::generateIteratingDestination()
{
  QgsProcessingContext context;
  QCOMPARE( QgsProcessingUtils::generateIteratingDestination( "memory:x", 1, context ).toString(), QStringLiteral( "memory:x_1" ) );
  QCOMPARE( QgsProcessingUtils::generateIteratingDestination( "memory:x", 2, context ).toString(), QStringLiteral( "memory:x_2" ) );
  QCOMPARE( QgsProcessingUtils::generateIteratingDestination( "ape.shp", 1, context ).toString(), QStringLiteral( "ape_1.shp" ) );
  QCOMPARE( QgsProcessingUtils::generateIteratingDestination( "ape.shp", 2, context ).toString(), QStringLiteral( "ape_2.shp" ) );
  QCOMPARE( QgsProcessingUtils::generateIteratingDestination( "/home/bif.o/ape.shp", 2, context ).toString(), QStringLiteral( "/home/bif.o/ape_2.shp" ) );

  QgsProject p;
  QgsProcessingOutputLayerDefinition def;
  def.sink = QgsProperty::fromValue( "ape.shp" );
  def.destinationProject = &p;
  QVariant res = QgsProcessingUtils::generateIteratingDestination( def, 2, context );
  QVERIFY( res.canConvert<QgsProcessingOutputLayerDefinition>() );
  QgsProcessingOutputLayerDefinition fromVar = qvariant_cast<QgsProcessingOutputLayerDefinition>( res );
  QCOMPARE( fromVar.sink.staticValue().toString(), QStringLiteral( "ape_2.shp" ) );
  QCOMPARE( fromVar.destinationProject, &p );

  def.sink = QgsProperty::fromExpression( "'ape' || '.shp'" );
  res = QgsProcessingUtils::generateIteratingDestination( def, 2, context );
  QVERIFY( res.canConvert<QgsProcessingOutputLayerDefinition>() );
  fromVar = qvariant_cast<QgsProcessingOutputLayerDefinition>( res );
  QCOMPARE( fromVar.sink.staticValue().toString(), QStringLiteral( "ape_2.shp" ) );
  QCOMPARE( fromVar.destinationProject, &p );
}

void TestQgsProcessing::asPythonCommand()
{
  DummyAlgorithm alg( "test" );
  alg.runAsPythonCommandChecks();
}

void TestQgsProcessing::modelerAlgorithm()
{
  //static value source
  QgsProcessingModelAlgorithm::ChildParameterSource svSource = QgsProcessingModelAlgorithm::ChildParameterSource::fromStaticValue( 5 );
  QCOMPARE( svSource.source(), QgsProcessingModelAlgorithm::ChildParameterSource::StaticValue );
  QCOMPARE( svSource.staticValue().toInt(), 5 );
  svSource.setStaticValue( 7 );
  QCOMPARE( svSource.staticValue().toInt(), 7 );
  svSource = QgsProcessingModelAlgorithm::ChildParameterSource::fromModelParameter( "a" );
  // check that calling setStaticValue flips source to StaticValue
  QCOMPARE( svSource.source(), QgsProcessingModelAlgorithm::ChildParameterSource::ModelParameter );
  svSource.setStaticValue( 7 );
  QCOMPARE( svSource.staticValue().toInt(), 7 );
  QCOMPARE( svSource.source(), QgsProcessingModelAlgorithm::ChildParameterSource::StaticValue );

  // model parameter source
  QgsProcessingModelAlgorithm::ChildParameterSource mpSource = QgsProcessingModelAlgorithm::ChildParameterSource::fromModelParameter( "a" );
  QCOMPARE( mpSource.source(), QgsProcessingModelAlgorithm::ChildParameterSource::ModelParameter );
  QCOMPARE( mpSource.parameterName(), QStringLiteral( "a" ) );
  mpSource.setParameterName( "b" );
  QCOMPARE( mpSource.parameterName(), QStringLiteral( "b" ) );
  mpSource = QgsProcessingModelAlgorithm::ChildParameterSource::fromStaticValue( 5 );
  // check that calling setParameterName flips source to ModelParameter
  QCOMPARE( mpSource.source(), QgsProcessingModelAlgorithm::ChildParameterSource::StaticValue );
  mpSource.setParameterName( "c" );
  QCOMPARE( mpSource.parameterName(), QStringLiteral( "c" ) );
  QCOMPARE( mpSource.source(), QgsProcessingModelAlgorithm::ChildParameterSource::ModelParameter );

  // child alg output source
  QgsProcessingModelAlgorithm::ChildParameterSource oSource = QgsProcessingModelAlgorithm::ChildParameterSource::fromChildOutput( "a", "b" );
  QCOMPARE( oSource.source(), QgsProcessingModelAlgorithm::ChildParameterSource::ChildOutput );
  QCOMPARE( oSource.outputChildId(), QStringLiteral( "a" ) );
  QCOMPARE( oSource.outputName(), QStringLiteral( "b" ) );
  oSource.setOutputChildId( "c" );
  QCOMPARE( oSource.outputChildId(), QStringLiteral( "c" ) );
  oSource.setOutputName( "d" );
  QCOMPARE( oSource.outputName(), QStringLiteral( "d" ) );
  oSource = QgsProcessingModelAlgorithm::ChildParameterSource::fromStaticValue( 5 );
  // check that calling setOutputChildId flips source to ChildOutput
  QCOMPARE( oSource.source(), QgsProcessingModelAlgorithm::ChildParameterSource::StaticValue );
  oSource.setOutputChildId( "c" );
  QCOMPARE( oSource.outputChildId(), QStringLiteral( "c" ) );
  QCOMPARE( oSource.source(), QgsProcessingModelAlgorithm::ChildParameterSource::ChildOutput );
  oSource = QgsProcessingModelAlgorithm::ChildParameterSource::fromStaticValue( 5 );
  // check that calling setOutputName flips source to ChildOutput
  QCOMPARE( oSource.source(), QgsProcessingModelAlgorithm::ChildParameterSource::StaticValue );
  oSource.setOutputName( "d" );
  QCOMPARE( oSource.outputName(), QStringLiteral( "d" ) );
  QCOMPARE( oSource.source(), QgsProcessingModelAlgorithm::ChildParameterSource::ChildOutput );

  QgsProcessingModelAlgorithm::ChildAlgorithm child( QStringLiteral( "some_id" ) );
  QCOMPARE( child.algorithmId(), QStringLiteral( "some_id" ) );
  QVERIFY( !child.algorithm() );
  child.setAlgorithmId( QStringLiteral( "native:centroids" ) );
  QVERIFY( child.algorithm() );
  QCOMPARE( child.algorithm()->id(), QStringLiteral( "native:centroids" ) );
  child.setDescription( QStringLiteral( "desc" ) );
  QCOMPARE( child.description(), QStringLiteral( "desc" ) );
  QVERIFY( child.isActive() );
  child.setActive( false );
  QVERIFY( !child.isActive() );
  child.setPosition( QPointF( 1, 2 ) );
  QCOMPARE( child.position(), QPointF( 1, 2 ) );
  QVERIFY( child.parametersCollapsed() );
  child.setParametersCollapsed( false );
  QVERIFY( !child.parametersCollapsed() );
  QVERIFY( child.outputsCollapsed() );
  child.setOutputsCollapsed( false );
  QVERIFY( !child.outputsCollapsed() );

  child.setChildId( QStringLiteral( "my_id" ) );
  QCOMPARE( child.childId(), QStringLiteral( "my_id" ) );

  child.setDependencies( QStringList() << "a" << "b" );
  QCOMPARE( child.dependencies(), QStringList() << "a" << "b" );

  QMap< QString, QgsProcessingModelAlgorithm::ChildParameterSource > sources;
  sources.insert( QStringLiteral( "a" ), QgsProcessingModelAlgorithm::ChildParameterSource::fromStaticValue( 5 ) );
  child.setParameterSources( sources );
  QCOMPARE( child.parameterSources().value( QStringLiteral( "a" ) ).staticValue().toInt(), 5 );
  child.addParameterSource( QStringLiteral( "b" ), QgsProcessingModelAlgorithm::ChildParameterSource::fromStaticValue( 7 ) );
  QCOMPARE( child.parameterSources().value( QStringLiteral( "a" ) ).staticValue().toInt(), 5 );
  QCOMPARE( child.parameterSources().value( QStringLiteral( "b" ) ).staticValue().toInt(), 7 );

  QgsProcessingModelAlgorithm alg( "test", "testGroup" );
  QCOMPARE( alg.name(), QStringLiteral( "test" ) );
  QCOMPARE( alg.displayName(), QStringLiteral( "test" ) );
  QCOMPARE( alg.group(), QStringLiteral( "testGroup" ) );

  // child algorithms
  QMap<QString, QgsProcessingModelAlgorithm::ChildAlgorithm> algs;
  QgsProcessingModelAlgorithm::ChildAlgorithm a1;
  a1.setDescription( QStringLiteral( "alg1" ) );
  QgsProcessingModelAlgorithm::ChildAlgorithm a2;
  a2.setDescription( QStringLiteral( "alg2" ) );
  algs.insert( QStringLiteral( "a" ), a1 );
  algs.insert( QStringLiteral( "b" ), a2 );
  alg.setChildAlgorithms( algs );
  QCOMPARE( alg.childAlgorithms().count(), 2 );
  QCOMPARE( alg.childAlgorithms().value( QStringLiteral( "a" ) ).description(), QStringLiteral( "alg1" ) );
  QCOMPARE( alg.childAlgorithms().value( QStringLiteral( "b" ) ).description(), QStringLiteral( "alg2" ) );
  QgsProcessingModelAlgorithm::ChildAlgorithm a3;
  a3.setChildId( QStringLiteral( "c" ) );
  a3.setDescription( QStringLiteral( "alg3" ) );
  QCOMPARE( alg.addChildAlgorithm( a3 ), QStringLiteral( "c" ) );
  QCOMPARE( alg.childAlgorithms().count(), 3 );
  QCOMPARE( alg.childAlgorithms().value( QStringLiteral( "a" ) ).description(), QStringLiteral( "alg1" ) );
  QCOMPARE( alg.childAlgorithms().value( QStringLiteral( "b" ) ).description(), QStringLiteral( "alg2" ) );
  QCOMPARE( alg.childAlgorithms().value( QStringLiteral( "c" ) ).description(), QStringLiteral( "alg3" ) );
  QCOMPARE( alg.childAlgorithm( "a" ).description(), QStringLiteral( "alg1" ) );
  QCOMPARE( alg.childAlgorithm( "b" ).description(), QStringLiteral( "alg2" ) );
  QCOMPARE( alg.childAlgorithm( "c" ).description(), QStringLiteral( "alg3" ) );
  // initially non-existent
  QVERIFY( alg.childAlgorithm( "d" ).description().isEmpty() );
  alg.childAlgorithm( "d" ).setDescription( QStringLiteral( "alg4" ) );
  QCOMPARE( alg.childAlgorithm( "d" ).description(), QStringLiteral( "alg4" ) );

  // generating child ids
  QgsProcessingModelAlgorithm::ChildAlgorithm c1;
  c1.setAlgorithmId( QStringLiteral( "buffer" ) );
  c1.generateChildId( alg );
  QCOMPARE( c1.childId(), QStringLiteral( "buffer_1" ) );
  QCOMPARE( alg.addChildAlgorithm( c1 ), QStringLiteral( "buffer_1" ) );
  QgsProcessingModelAlgorithm::ChildAlgorithm c2;
  c2.setAlgorithmId( QStringLiteral( "buffer" ) );
  c2.generateChildId( alg );
  QCOMPARE( c2.childId(), QStringLiteral( "buffer_2" ) );
  QCOMPARE( alg.addChildAlgorithm( c2 ), QStringLiteral( "buffer_2" ) );
  QgsProcessingModelAlgorithm::ChildAlgorithm c3;
  c3.setAlgorithmId( QStringLiteral( "centroid" ) );
  c3.generateChildId( alg );
  QCOMPARE( c3.childId(), QStringLiteral( "centroid_1" ) );
  QCOMPARE( alg.addChildAlgorithm( c3 ), QStringLiteral( "centroid_1" ) );
  QgsProcessingModelAlgorithm::ChildAlgorithm c4;
  c4.setAlgorithmId( QStringLiteral( "centroid" ) );
  c4.setChildId( QStringLiteral( "centroid_1" ) );// dupe id
  QCOMPARE( alg.addChildAlgorithm( c4 ), QStringLiteral( "centroid_2" ) );
  QCOMPARE( alg.childAlgorithm( QStringLiteral( "centroid_2" ) ).childId(), QStringLiteral( "centroid_2" ) );

  // test canExecute
  QgsProcessingModelAlgorithm alg2( "test", "testGroup" );
  QVERIFY( alg2.canExecute() );
  QgsProcessingModelAlgorithm::ChildAlgorithm c5;
  c5.setAlgorithmId( "native:centroids" );
  alg2.addChildAlgorithm( c5 );
  QVERIFY( alg2.canExecute() );
  // non-existing alg
  QgsProcessingModelAlgorithm::ChildAlgorithm c6;
  c6.setAlgorithmId( "i'm not an alg" );
  alg2.addChildAlgorithm( c6 );
  QVERIFY( !alg2.canExecute() );
}

QGSTEST_MAIN( TestQgsProcessing )
#include "testqgsprocessing.moc"
