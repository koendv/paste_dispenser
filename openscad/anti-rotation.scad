
include <param.scad>
include <util.scad>
include <cyl_head_bolt.scad> // from nutsnbolts library
//min_wall_thickness = 4.0;

tab_length = 8.0;
screw_dia = 3.0;
nut_dia = 5.5;
nut_height = 2.0;
antirotation_height = nut_dia + 2 * min_wall_thickness;
antirotation_width = stepper_screw_dia + 2 * clearance_fit + 4 * min_wall_thickness;

module holes() {

    circle_outer(d = leadscrew_dia + tight_fit, fn = 6);
    translate([0, stepper_screw_spacing/2, 0])
    rotate([0, 0, 45/2])
    circle_outer(d = stepper_screw_dia + 2 * clearance_fit, fn = 8);
}

module anti_rotation() {
    difference() {
        linear_extrude(height = antirotation_height, convexity=10)
        difference() {
            offset(r = min_wall_thickness * 2)
            hull() {
                circle(d = stepper_screw_dia + 2 * clearance_fit);
                translate([0, stepper_screw_spacing/2, 0])
                circle(d = stepper_screw_dia + 2 * clearance_fit);
            }
            holes();
            // slot
            square([2 * clearance_fit, stepper_screw_spacing], center = true);
            
        }
        translate([-(stepper_screw_dia + clearance_fit + 2* min_wall_thickness - nut_height + eps1), stepper_screw_spacing/4, antirotation_height/2])
        rotate([0, 90, 0]) {
            rotate([0, 0, 45/2])
            cylinder_outer(d = screw_dia + clearance_fit, h = stepper_screw_spacing, fn = 8);
            rotate([0, 0, 30])
            cylinder_outer(d = nut_dia + clearance_fit, h = nut_height, fn = 6);
        }
         
    }
}

//anti_rotation();
      
// not truncated
