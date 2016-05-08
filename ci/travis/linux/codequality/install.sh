mkdir build
cd build

ABS_CLAZY_PATH=`cd "$TRAVIS_BUILD_DIR/clazy/lib"; pwd`

export CLAZY_TESTS="assert-with-side-effects"
export CLAZY_TESTS="$CLAZY_TESTS,detaching-temporary"

# too many results for now
export CLAZY_TESTS="$CLAZY_TESTS,container-anti-pattern"

export CLAZY_TESTS="$CLAZY_TESTS,bogus-dynamic-cast"
export CLAZY_TESTS="$CLAZY_TESTS,qdeleteall"
export CLAZY_TESTS="$CLAZY_TESTS,isempty-vs-count"
export CLAZY_TESTS="$CLAZY_TESTS,qstring-left"
export CLAZY_TESTS="$CLAZY_TESTS,qgetenv"
export CLAZY_TESTS="$CLAZY_TESTS,unused-non-trivial-variable"
export CLAZY_TESTS="$CLAZY_TESTS,temporary-iterator"
export CLAZY_TESTS="$CLAZY_TESTS,writing-to-temporary"

# TODO
# export CLAZY_TESTS="$CLAZY_TESTS,qstring-arg"

# Seems broken on clang 3.8:
#export CLAZY_TESTS="$CLAZY_TESTS,missing-qobject-macro"
# Flags all QVERIFY( some pointer ) uses
#export CLAZY_TESTS="$CLAZY_TESTS,implicit-casts"
# Incorrectly flags QgsFields interation
#export CLAZY_TESTS="$CLAZY_TESTS,foreach"
# Requires Qt5 toDouble/toInt QStringRef implementations
#export CLAZY_TESTS="$CLAZY_TESTS,qstring-ref"
# Requires API break:
#export CLAZY_TESTS="$CLAZY_TESTS,inefficient-qlist-soft"

export CLANG_WARNINGS="-Wall \
                    -Wno-c++98-compat \
                    -Wno-exit-time-destructors \
                    -Wno-padded \
                    -Wno-documentation \
                    -Wno-c++98-compat-pedantic \
                    -Wno-documentation-unknown-command \
                    -Wno-shadow \
                    -Wno-deprecated \
                    -Wno-global-constructors \
                    -Wno-switch-enum \
                    -Wno-conversion \
                    -Wno-sign-conversion \
                    -Wno-weak-vtables \
                    -Wno-cast-align \
                    -Wno-covered-switch-default \
                    -Wno-unreachable-code \
                    -Wno-missing-prototypes \
                    -Wno-reserved-id-macro \
                    -Wimplicit-fallthrough \
                    -Wno-disabled-macro-expansion \
                    -Wno-missing-variable-declarations \
                    -Wno-float-equal \
                    -Wno-old-style-cast \
                    -Wno-return-type-c-linkage \
                    -Wno-overloaded-virtual"

export CXX="clang++-${LLVM_VERSION}"
export CC="clang-${LLVM_VERSION}"
export PATH=${HOME}/osgeo4travis/bin:${PATH}
export PYTHONPATH=${HOME}/osgeo4travis/lib/python3.3/site-packages/

cmake --version
${CC} --version
${CXX} --version

# Include this line for debug reasons
#      -DCMAKE_BUILD_TYPE=RelWithDebInfo \
#
cmake \
      -DCMAKE_PREFIX_PATH=/home/travis/osgeo4travis \
      -DWITH_STAGED_PLUGINS=OFF \
      -DWITH_GRASS=ON \
      -DSUPPRESS_QT_WARNINGS=ON \
      -DENABLE_MODELTEST=OFF \
      -DENABLE_PGTEST=OFF \
      -DWITH_QSPATIALITE=ON \
      -DWITH_QWTPOLAR=OFF \
      -DWITH_APIDOC=OFF \
      -DWITH_ASTYLE=ON \
      -DWITH_SERVER=ON \
      -DWITH_INTERNAL_YAML=OFF \
      -DENABLE_QT5=ON \
      -DENABLE_PYTHON3=ON \
      -DDISABLE_DEPRECATED=ON \
      -DCXX_EXTRA_FLAGS="$CLANG_WARNINGS -Xclang -load -Xclang $ABS_CLAZY_PATH/ClangLazy.so -Xclang -add-plugin -Xclang clang-lazy -Xclang -plugin-arg-clang-lazy -Xclang $CLAZY_TESTS" \
      ..

