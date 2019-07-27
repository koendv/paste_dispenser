include <param.scad>
include <util.scad>
include <cyl_head_bolt.scad>

/* 
 * Plunger.
 * Note plunger has an insert: a stainless steel nut that needs to be inserted during the build process.
 */

module torus(r1 = 0, r2 = 0) {
    rotate_extrude()
    translate([r1, 0, 0])
    circle(r = r2);
}

module chamfered_cylinder(d = 0, d1 = 0, d2 = 0, h = 0, chamfer = 0) {
    chamfer1 = min(h/2, chamfer);
    hull() {
        translate([0, 0, h - chamfer1])
        torus(r1 = max(d, d2)/2 - chamfer, r2 = chamfer1);
        translate([0, 0, chamfer1])
        torus(r1 = max(d, d1)/2 - chamfer, r2 = chamfer1);
    }
} 

module plunger_body() {
    
    hull() {
        translate([0, 0, plunger_h2 + plunger_h3 + plunger_h4])    
        chamfered_cylinder(d = plunger_dia1, h = plunger_h1 + eps1, chamfer = 1);

        translate([0, 0, plunger_h3 + plunger_h4])  
        cylinder(d1 = plunger_dia3, h = eps1);
    }
       
    translate([0, 0, plunger_h4])  
    cylinder(d = plunger_dia3, h = plunger_h3 + eps1);
        
    chamfer = clearance_fit;
    
    // chamfer where piston meets shaft
    translate([0, 0, plunger_h4])
    cylinder(d1 = plunger_dia3 + chamfer, d2 = plunger_dia3, h = chamfer/2);
    
    // chamfer at top of piston
    translate([0, 0, plunger_h4 - chamfer/2])
    cylinder(d1 = plunger_dia4, d2 = plunger_dia4 - chamfer, h = chamfer/2);
    
    // chamfer at piston bottom
    translate([0, 0, chamfer/2])
    cylinder(d = plunger_dia4, h = plunger_h4 - chamfer);

    // rest of piston
    cylinder(d1 = plunger_dia4 - chamfer, d2 = plunger_dia4, h = chamfer/2);
}

// distance between tip of leadscrew and plunger tip
leadscrew_offset = plunger_h2 + plunger_h3 + plunger_h4;

module plunger_hole() {
    
    // hole for leadscrew
    translate([0, 0, leadscrew_offset + leadscrew_tip_length])
    cylinder(d = leadscrew_dia + clearance_fit, h = plunger_h1 + eps2);
    
    // hole for leadscrew tip
    translate([0, 0, leadscrew_offset - 2 * clearance_fit])
    cylinder(d = leadscrew_tip_dia + clearance_fit, h = leadscrew_tip_length + 2 * clearance_fit + eps1);

    // hole for M3 hex nut on leadscrew tip
    translate([0, 0, leadscrew_offset + leadscrew_tip_length - leadscrew_nut_height])
    //cube([leadscrew_nut_width + clearance_fit, leadscrew_nut_width + clearance_fit, leadscrew_nut_height + clearance_fit], center=true); // square nut
    cylinder_outer(d = leadscrew_nut_width + clearance_fit, h = leadscrew_nut_height + clearance_fit, fn = 6); // hex nut

}

module plunger() {
    difference() {
        plunger_body();
        plunger_hole();
    }
}

if (1) {
    cutout()
    plunger();
}

// not truncated 

 
