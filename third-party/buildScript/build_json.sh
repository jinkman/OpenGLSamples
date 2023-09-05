#!/bin/sh
workdir=$(cd $(dirname $0); pwd)/../json

package=json

############ build ${package} ############
echo "--- build ${package} ---"

cd "$workdir/" && \
mkdir -p "$workdir/build" && cd "$workdir/build/" && \
cmake -DJSON_BuildTests=OFF -DCMAKE_INSTALL_PREFIX="$workdir/../deploy/$package" ../ && \
make -j 12 && \
make install

if [[ $? -ne 0 ]]; then
    echo "ERROR: Failed to build ${package}"
    exit -1
fi

echo "success!"