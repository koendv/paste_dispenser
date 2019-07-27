include <param.scad>

/* 
 * rough model of a syringe and contents. 
 * note syringe model is determined in param.scad by including either 5ml_syringe.scad or 10ml_syringe.scad
 */
 
 // given syringe diameter and volume of the contents of the syringe, determine the stroke of the plunger
 syringe_stroke = syringe_volume / (PI * syringe_int_dia * syringe_int_dia / 4) * 1000;
 
 module syringe(model_contents = true) {
     color("LightBlue", 0.25)
     %union() { // make syringe transparent so we can see plunger and solder paste
         difference() {
            // syringe outside wall
            translate([0, 0, syringe_barrel_length - syringe_empty_length])
            cylinder(d = syringe_ext_dia, h = syringe_empty_length);
            // syringe inside wall
            translate([0, 0, syringe_barrel_length - syringe_empty_length + eps1])
            cylinder(d = syringe_int_dia, h = syringe_empty_length);
            
         }
         // syringe luer tip
         cylinder(d = syringe_luer_dia, h = syringe_barrel_length - syringe_empty_length);
         // flange
         translate([-syringe_ext_dia/2, -syringe_flange_dia/2, syringe_barrel_length - syringe_flange_thickness])
         cube([syringe_ext_dia, syringe_flange_dia, syringe_flange_thickness]);
    }
    // model of syringe contents
    if (model_contents) {
        color("DarkGrey", 0.5)
        translate([0, 0, syringe_barrel_length - syringe_empty_length])
        cylinder(d = syringe_int_dia - eps2, h = syringe_stroke);
    }
}

if (0) {
   syringe();
}
// not truncated 
