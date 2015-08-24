export ASAN_OPTIONS=detect_leaks=1
#No Python tests due to ASAN incompatibility
xvfb-run ctest -V -E 'qgis_openstreetmaptest|qgis_wcsprovidertest|Py' -S ./qgis-test-travis.ctest --output-on-failure

