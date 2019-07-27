include <param.scad>
include <util.scad>
include <assembly.scad>

model = 1; // overridden in generate.sh
if (model == 1)
    body();
else if (model == 2)
    syringe_holder();
else if (model == 3)
    plunger();
else if (model == 4)
    anti_rotation();
else if (model == 5)
    assembly();
else if (model == 6) {
    $vpd=700;
    $vpr=[90, 0, 90];
    $vpt=[0, 0, 0];
    assembly();
    }
else assert(false);

// not truncated
