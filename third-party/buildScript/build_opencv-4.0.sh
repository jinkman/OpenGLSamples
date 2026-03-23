#!/bin/sh
workdir=$(cd $(dirname $0); pwd)/../opencv-4.0

package=opencv-4.0

############ build ${package} ############
echo "--- build ${package} ---"

cd "$workdir/" && \
mkdir -p "$workdir/build" && cd "$workdir/build/" && \
cmake -DCMAKE_POLICY_VERSION_MINIMUM=3.5 -DCMAKE_CXX_STANDARD=17 -DBUILD_ZLIB=OFF -DENABLE_PRECOMPILED_HEADERS=OFF -DWITH_FFMPEG=OFF -DWITH_PNG=OFF -DBUILD_PERF_TESTS=OFF -DBUILD_TESTS=OFF -DCMAKE_INSTALL_PREFIX="$workdir/../deploy/$package" ../ && \
make -j 12 && \
make install

if [[ $? -ne 0 ]]; then
    echo "ERROR: Failed to build ${package}"
    exit -1
fi

echo "success!"
