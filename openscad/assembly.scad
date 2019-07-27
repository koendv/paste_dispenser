/*
 * Assembly drawing
 */

include <param.scad>
include <body.scad>
include <motor.scad>
include <plunger.scad>
include <anti-rotation.scad>
include <syringe.scad>

include <cyl_head_bolt.scad> // from nutsnbolts library

cut = true;
show_syringe_contents = false;

antirotation_length = 100; // excluding threaded parts

module hex_pillar() {
    linear_extrude(height = hex_standoff_length)
    circle_outer(d = hex_standoff_dia, fn = 6);
}

// valid values for position: 0 ... 1. 
// realistic values for position: 0.3 ... 0.7
module assembly(position = 0.5) {
    
    //body
    cutout(cut)
    body();
    
    // linear stepper
    translate([0, 0, body_height])
    color("silver")
    cutout(cut)
    motor();
    
    // hex pillars
    color("silver")
    twice() hex_pillar();
    
    // top M4 screws
    translate([0, 0, body_height + stepper_base_thickness])
    color("silver")
    twice() screw("M4x10");
    
    // syringe
    translate([0, 0, -syringe_barrel_length])
    syringe(model_contents = show_syringe_contents);
    
    // syringe holder
    translate([0, 0, -syringe_flange_thickness])
    mirror([0, 0, 1])
    cutout(cut)
    syringe_holder();

    // bottom M4 screws
    translate([0, 0, -syringe_flange_thickness - syringe_holder_base_height])
    mirror([0, 0, 1])
    color("silver")
    twice() screw("M4x10");
    
    // guide for anti-rotation 
    color("silver")
    translate([0, stepper_screw_spacing/2, body_height])
    cylinder(d = stepper_screw_dia, h = antirotation_length);

    // leadscrew position
    leadscrew_position = max(min(position, 1.0), 0.0); // constrain to values between 0.0 and 1.0
    leadscrew_pos = stepper_base_thickness + stepper_height - leadscrew_length * position;
    //echo ("leadscrew position:", leadscrew_pos);
    
    // plunger total height
    plunger_height = plunger_h1 + plunger_h2 + plunger_h3 + plunger_h4;
    echo ("plunger height:", plunger_height);
    
    // plunger, leadscrew and anti-rotation device
    translate([0, 0, leadscrew_pos]) {
        
        // leadscrew
        translate([0, 0, leadscrew_offset])
        color("navy")
        leadscrew();
        
        // hex nut in plunger
        if (1)
        color("silver")
        translate([0, 0, leadscrew_offset + leadscrew_tip_length - leadscrew_nut_height])
        //cube([leadscrew_nut_width, leadscrew_nut_width, leadscrew_nut_height], center = true); // square nut
        cylinder_outer(d = leadscrew_nut_width, h = leadscrew_nut_height, fn = 6); // hex nut
        
        // plunger
        if (1)
        cutout(cut)
        plunger();
        
        // anti-rotation
        translate([0, 0, leadscrew_offset + leadscrew_length + leadscrew_tip_length - antirotation_height ]) {
        anti_rotation();
        translate([antirotation_width/2, stepper_screw_spacing/4, antirotation_height/2])
        rotate([0, 90, 0])
        color("silver")    
        screw("M3x16");
        translate([-antirotation_width/2, stepper_screw_spacing/4, antirotation_height/2])
        rotate([0, -90, 0])
        rotate([0, 0, 30])
        color("silver")
        nut("M3");
        }
    }

}

//assembly();

//not truncated
