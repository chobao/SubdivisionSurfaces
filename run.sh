#!/bin/bash
#declare -a array=('./res/monsterfrog.obj')
res_dir="./res"
array=(`ls ${res_dir}/*.obj`) # visualization all models
arraylength=${#array[@]}
exe_path="./build/src/mainSubdivisionSurface"
vertex_shader="./src/visualization/model_v.glsl"
fragment_shader="./src/visualization/model_f.glsl"
for((i=0; i<${arraylength}; i++));
do
        echo ${exe_path} ${array[${i}]} ${vertex_shader} ${fragment_shader}
        ${exe_path} ${array[${i}]} ${vertex_shader} ${fragment_shader}
done
