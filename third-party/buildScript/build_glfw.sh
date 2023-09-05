#!/bin/sh
workdir=$(cd $(dirname $0); pwd)/../glfw

package=glfw

############ build ${package} ############
echo "--- build ${package} ---"

cd "$workdir/" && \
mkdir -p "$workdir/build" && cd "$workdir/build/" && \
cmake -DCMAKE_CXX_STANDARD=17 -DCMAKE_INSTALL_PREFIX="$workdir/../deploy/$package" ../ && \
make -j 12 && \
make install

if [[ $? -ne 0 ]]; then
    echo "ERROR: Failed to build ${package}"
    exit -1
fi

echo "success!"
