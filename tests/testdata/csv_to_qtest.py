import os
import csv


with open('geom_data.csv', 'rb') as f:
    print 'QTest::addColumn<QString>( "wkt" );'
    print 'QTest::addColumn<QString>( "validWkt" );'
    print 'QTest::addColumn<int>( "numPoints" );'
    print 'QTest::addColumn<int>( "numGeometries" );'
    print 'QTest::addColumn<int>( "numRings" );'
    print 'QTest::addColumn<bool>( "isClosed" );'
    print 'QTest::addColumn<QString>( "centroid" );'
    print 'QTest::addColumn<double>( "xMin" );'
    print 'QTest::addColumn<double>( "yMin" );'
    print 'QTest::addColumn<double>( "xMax" );'
    print 'QTest::addColumn<double>( "yMax" );'
    print 'QTest::addColumn<double>( "area" );'    
    print 'QTest::addColumn<double>( "length" );'
    print 'QTest::addColumn<double>( "perimeter" );'
      
    reader = csv.DictReader(f)
    for i, row in enumerate(reader):
        print 'QTest::newRow( "geometry {}" ) << "{}"'.format(i,row['wkt'])
        print '             << "{}"'.format(row['valid_wkt'])
        print '             << {} << {} << {}'.format(row['num_points'],row['num_geometries'],row['num_rings'])
        print '             << {}'.format( row['is_closed'] == '1' and 'true' or 'false' )
        print '             << "{}" << {} << {} << {} << {}'.format( row['centroid'], float(row['x_min']), float(row['y_min']), float(row['x_max']), float(row['y_max'] ))
        print '             << {} << {} << {};'.format( float(row['area']), float(row['length']), float(row['perimeter']) )

