include <param.scad>
include <pcb.scad>
include <util.scad>

// hex standoffs
module hex_standoffs () {
  twice() circle_outer(d = hex_standoff_dia + tight_fit, fn = 6); // check standoff diameter
}

// body
module base_pattern() {
    hull() {
        circle(d = stepper_width);
        twice() {circle (d = stepper_length - stepper_screw_spacing);}
        // make sure there is min_wall_thickness around stepper, syringe and standoffs.
        offset(r = min_wall_thickness) {
            circle (d = stepper_bottom_dia);
            circle (d = syringe_int_dia);
            hex_standoffs ();
        }
    };          
}

// put pcb right at edge of body
pcb_xpos =  max(stepper_width/2, stepper_bottom_dia/2 + min_wall_thickness, syringe_int_dia/2 + min_wall_thickness) - eps2; 

module body() {
    difference() {
        linear_extrude(height = body_height, convexity = 10) {
            difference() {
                hull() {
                    base_pattern();
                    // put rounded corners left and right of pcb
                    translate([pcb_xpos - min_wall_thickness, -pcb_length/2])
                    circle(r = min_wall_thickness);
                    translate([pcb_xpos - min_wall_thickness, +pcb_length/2])
                    circle(r = min_wall_thickness);
                }
                // 
                hex_standoffs ();
            }
        }
        // cutouts in body for pcb pins
        translate([pcb_xpos + pcb_thickness - eps1, 0, pcb_width/2])
        rotate([0, -90, 0])
        rotate([0, 0, 90])
        pcb();
        // holes in body for stepper motor and plunger
        // hole for stepper motor
        translate([0, 0, body_height - stepper_bottom_height])
        cylinder(d = stepper_bottom_dia, h = stepper_bottom_height + eps1);
        // hole for plunger
        cylinder(d = syringe_int_dia, h = body_height - stepper_bottom_height + eps1);
        // internal chamfer at 45 degree
        dia_diff = stepper_bottom_dia - syringe_int_dia;
        // if syringe_int_dia is smaller than stepper_bottom_dia
        translate([0, 0, body_height - stepper_bottom_height - dia_diff/2])
        cylinder (d1 = syringe_int_dia, d2 = stepper_bottom_dia, h = dia_diff/2 + eps1);
        // if syringe_int_dia is bigger than stepper_bottom_dia
        translate([0, 0, body_height - stepper_bottom_height])
        cylinder (d1 = syringe_int_dia, d2 = stepper_bottom_dia, h = -dia_diff/2 - eps1); 
    }
}

module screw_heads () {
    screw_head_dia = 2 * stepper_screw_dia + 1; // valid washer size for M2, M2.5, M3, M3.5, M4.
    twice() {cylinder(d = screw_head_dia, h = syringe_holder_total_height);}           
}

module syringe_holder() {
    difference() {
        hull() {
            linear_extrude(height = syringe_holder_base_height)
            base_pattern();
            linear_extrude(height = syringe_holder_total_height)
            circle(d = syringe_ext_dia + 2 * clearance_fit + 2 * min_wall_thickness);
        }
        translate([0, 0, -eps1])
        cylinder(d = syringe_ext_dia + 2 * clearance_fit, h = syringe_holder_total_height + eps2);
        // holes for screws
        twice() {cylinder(d = stepper_screw_dia, h = syringe_holder_total_height + eps2);}
        // holes for screw heads and washers
        translate([0, 0, syringe_holder_base_height])
        screw_heads();
    }
}

if (0) {
    cutout()
    body();
    
    translate([0, 0, -10])
    mirror([0, 0, 1])
    syringe_holder();
}
                
// not truncated
