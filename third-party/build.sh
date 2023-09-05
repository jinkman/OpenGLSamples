#!/bin/sh
workdir=$(cd $(dirname $0); pwd)

third_party_list=(
    assimp
    glfw
    glm
    freetype
    opencv-4.0
    json
    soil
)

for i in ${third_party_list[@]}
do
    path="${workdir}/buildScript"
    echo "============== build $i ==========="
    sh "${path}/build_$i.sh"
    if [[ $? -ne 0 ]]; then
        echo "ERROR: Failed to build $i"
        exit -1
    fi
done

echo "build all success!"