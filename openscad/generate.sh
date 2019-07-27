#!/bin/bash -x
#OPENSCAD=openscad
OPENSCAD=~/Applications/OpenSCAD.app/Contents/MacOS/OpenSCAD

# number of fragments when rendering a circle 
FN=128

$OPENSCAD -o body.stl -Dbatch=true -Dmodel=1 -D\$fn=$FN generate.scad
$OPENSCAD -o syringe_holder.stl -Dbatch=true -Dmodel=2 -D\$fn=$FN generate.scad
$OPENSCAD -o plunger.stl -Dbatch=true -Dmodel=3 -D\$fn=$FN generate.scad
$OPENSCAD -o anti-rotation.stl -Dbatch=true -Dmodel=4 -D\$fn=$FN generate.scad
$OPENSCAD -o assembly.stl -Dbatch=true -Dmodel=5 -D\$fn=$FN generate.scad
$OPENSCAD -o assembly-drawing.png -Dbatch=true -Dmodel=6 -D\$fn=$FN generate.scad
#not truncated
