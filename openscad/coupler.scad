// 28BYJ-48 stepper motor to M4 threaded shaft coupler. 

// This prints half a coupler. To get a shaft coupler you need to print two of these.
// After printing remove the support from the screw holes by hand, with a 3mm drill.
// Clamp between stepper shaft and leadscrew using four M3 x 10mm screws 

include <NopSCADlib/lib.scad>;
include  <util.scad>

coupler_height = 25.0;
coupler_width = 19.0;
coupler_slot_width = 1.0;
coupler_inradius = coupler_width/2 * cos(180/8);

eps1 = 0.001;
eps2 = 2 * eps1;
$fn = 120;
layer_thickness = 0.5;

// --- 28BYJ-48 stepper
// shaft diameter
stepper_shaft_dia = 5.0 + 0.7; 
// Length of hole for shaft 1.
stepper_shaft_height = 10;
// Width between the flat sides of the motor shaft hole.
stepper_shaft_width = 3.0 + 0.5;
// Height of full round hole at the motor end.
stepper_shaft_base_height = 3.5;

// --- M4 threaded shaft end
// Inner diameter of hole for shaft 2. This one fits the M4 threaded shaft.
leadscrew_dia = 4.0 + 0.5;  
// Length of hole for shaft 2.
leadscrew_height = 12;

// --- M3 Clamping screws
// Diameter of the screw holes.
screw_hole_diameter = 3.3;
// Diameter of the inset for the screws head.
screw_head_diameter = 5.7;
// Diameter of the inset for the screws nuts.
screw_nut_diameter = 6.5;
// Distance between screw head and nut.
screw_head_to_nut = 7.0;
// Offset of the screws' centers from the coupler's center.
hole_offset_from_center = 5.0;
// Offset of the screws' centers from the coupler ends.
hole_offset_from_end = 5.0;

module coupler_body() {
    rotate([0, 0, 180/8])
    cylinder(h = coupler_height, d = coupler_width, $fn = 8);
}

module halfcoupler_body() {
    translate([coupler_height/2 , 0, 0])
    difference() {
        rotate([0, -90, 0])
        coupler_body();
        translate([-2 * coupler_height, - coupler_width, -coupler_slot_width/2])
        cube([3 * coupler_height, 2 * coupler_width, 2 * coupler_width]);
    }
}

module clamping_screw() {
    union() {
        cylinder(d = screw_hole_diameter, h = coupler_width);
        translate([0, 0, -coupler_width])
        cylinder(d = screw_head_diameter, h = coupler_width);
        translate([-screw_head_diameter/2, -coupler_width, -coupler_width])
        cube([screw_head_diameter, coupler_width, coupler_width]);
    }
}

module clamping_nut() {
    union() {
        cylinder(d = screw_hole_diameter, h = coupler_width);
        translate([0, 0, -coupler_width])
        cylinder(d = screw_nut_diameter, h = coupler_width, $fn = 6);
        translate([-screw_nut_diameter/2, 0, -coupler_width])
        cube([screw_nut_diameter, coupler_width, coupler_width]);
    }
}

module single_layer_support() {
    intersection() {
        translate([-coupler_height/2, -coupler_width/2, -screw_head_to_nut/2 - eps1])
        cube([coupler_height, coupler_width, layer_thickness]);
        halfcoupler_body();
    }
}

module coupler_leadscrew() {
    translate([coupler_height/2 - leadscrew_height, 0, 0])
    rotate([0, 90, 0])
    rotate([0, 0, 180/8])
    cylinder_outer(h = 2 * leadscrew_height, d = leadscrew_dia, fn = 8); // octagonal
    //cylinder(h = 2 * leadscrew_height, d = leadscrew_dia); // round
}

module stepper_shaft() {
    translate([-coupler_height/2 - eps2, 0, 0])
    rotate([90, 0, 0])
    rotate([0, 90, 0]) {
        translate([0, 0, -eps1])
        cylinder(h = stepper_shaft_base_height + eps2 , d = stepper_shaft_dia);
        intersection() {
            cylinder(h = stepper_shaft_height , d = stepper_shaft_dia);
            translate([-stepper_shaft_dia/2, -stepper_shaft_width/2, stepper_shaft_base_height])
            cube([stepper_shaft_dia, stepper_shaft_width, stepper_shaft_height]);
        }
    }
}

module halfcoupler() {
    difference() {
        union() {
            difference() {
                halfcoupler_body();
                translate([coupler_height/2 - hole_offset_from_end, hole_offset_from_center, -screw_head_to_nut/2 ])
                clamping_nut();
                translate([-coupler_height/2 + hole_offset_from_end, hole_offset_from_center, -screw_head_to_nut/2])
                clamping_nut();
                translate([coupler_height/2 - hole_offset_from_end, -hole_offset_from_center, -screw_head_to_nut/2])
                clamping_screw();
                translate([-coupler_height/2 + hole_offset_from_end, -hole_offset_from_center, -screw_head_to_nut/2])
                clamping_screw();
            }
            single_layer_support();
        }
        coupler_leadscrew();
        stepper_shaft();
    }
}

// model of an assembled shaft coupler in the motor mount cavity 
// to check everything fits

module clearance_check() {
    // cavity in motor mount
    cavity_height = 28;
    cavity_diameter = 22;
    %translate([-cavity_height/2, 0, 0])
    rotate([0, 90, 0])
    difference() {
        translate([-cavity_diameter, -cavity_diameter, 0])
        cube([2 * cavity_diameter, 2 * cavity_diameter,  cavity_height]);
        translate([0, 0, -cavity_height/2])
        cylinder(h = cavity_height * 2, d = cavity_diameter);
    }

    // motor shaft
    shaft_dia = 5.0; 
    shaft_height = 10;
    shaft_width = 3.0;
    shaft_base_height = 3.5;
    translate([-coupler_height/2 - eps2, 0, 0])
    rotate([0, 90, 0])
    color("Gray")
    intersection() {
        cylinder(h = shaft_height, d = shaft_dia);
        union() {
            translate([-shaft_dia/2, -shaft_dia/2, 0])
            cube([shaft_dia, shaft_dia, shaft_base_height]);
            translate([-shaft_width/2, -shaft_dia/2, 0])
            cube([shaft_width, shaft_dia, shaft_height]);
        }
    }
    
    // leadscrew
    color("Gray")
    translate([coupler_height/2 - leadscrew_height, 0, 0])
    rotate([0, 90, 0])
    cylinder(h = coupler_height, d = 4.0);
    
    // clamping screws 
    delta_x = coupler_height/2 - hole_offset_from_end;
    delta_y = hole_offset_from_center;
    for (x = [delta_x, -delta_x])
    for (y = [delta_y, -delta_y])
    mirror([0, 0, y>0?1:0])    
    translate([x, y, 0]) {
        translate([0, 0, -screw_head_to_nut/2])
        mirror([0, 0, 1])
        screw(M3_pan_screw, 10);
        translate([0, 0, screw_head_to_nut/2])
        nut(M3_nut);
    }
    
    // two halfcouplers
    rotate([180, 0, 0])
    halfcoupler();
    halfcoupler();
}

// choose between printer-ready halfcoupler and assembled model.

if (true) {
    translate([0, 0, coupler_inradius])
    halfcoupler();
}
else {
    clearance_check();
};

// not truncated
