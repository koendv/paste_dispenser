include <param.scad>
include <util.scad>

/*
 * model of a can-stack non-captive linear stepper 
 */

module motor (position = 0.5) {
    
    // stepper base plate
    linear_extrude(height = stepper_base_thickness)
    difference() {
        hull() {
            twice() circle(d = stepper_length - stepper_screw_spacing);
            circle(d = stepper_width);
        }
        twice() circle(d = stepper_screw_dia);
    }
    
    // stepper body
    translate([0, 0, stepper_base_thickness])
    cylinder(d = stepper_width, h = stepper_height );
    
    // stepper front cap
    translate([0, 0, -stepper_bottom_height])
    cylinder(d = stepper_bottom_dia, h = stepper_bottom_height);
    
}

module leadscrew() {
    // leadscrew
    translate([0, 0, leadscrew_tip_length])
    cylinder(d = leadscrew_dia, h = leadscrew_length);
    
    // leadscrew threaded tip
    cylinder(d = leadscrew_tip_dia, h = leadscrew_tip_length + eps1);
}

if (0) {
    motor();
    translate([0, 0, -leadscrew_length/2])
    leadscrew();
}

// not truncated
