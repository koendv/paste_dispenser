//eps1 = 0.001;
//eps2 = 2 * 0.001;

pcb_solderside_components = 2.0; // room needed on solder side for pins of through-hole components
pcb_thickness =1.6;
pcb_width = 30;
pcb_length = 40;
clearance_fit = 0.5;

// Model of pcb, with pins sticking out.


// The pins vector is generated as follows:
// In kicad pcbnew: File, Plot, Generate Drill File, File Format Excellon format, Drill units millimeters, Zeroes format decimal format, PTH and NPTH holes in single file, Drill origin auxiliary axis. 
// The resulting .drl drill file in kicad/paste_dispenser/gerbers/ contains the drill sizes and the coordinates of all drill holes.
// The header will look similar to this:
//
// METRIC,TZ
// T1C0.406
// T2C0.610
// T3C1.000
// T4C1.100
// 
// These are drill sizes.
// Everything with a drill < 1mm (T1 and T2) are vias. Discard. That leaves T3 and T4.
// Edit the file:
// replace X with [
// replace Y with ,
// replace eol with ],
// So a line like this
// X15.772Y28.043
// becomes this:
// [15.772,28.043],
// and paste the result in this openscad script.

pins = [
[28.98, 13.184],
[28.98, 10.644],
[31.52, 13.184],
[31.52, 10.644],
[34.06, 13.184],
[34.06, 10.644],
[15.772, 28.043],
[18.312, 28.043],
[20.852, 28.043],
[23.392, 28.043],
[25.932, 28.043],
[34.06, 24.868],
[34.06, 22.328],
[34.06, 19.788],
[34.06, 17.248],
[37.87, 24.868],
[37.87, 22.328],
[37.87, 19.788],
[37.87, 17.248],
[37.87, 14.708],
[37.87, 12.168],
[21.36, 6.58],
[21.36, 2.08],
[27.86, 6.58],
[27.86, 2.08],
[11.708, 6.58],
[11.708, 2.08],
[18.208, 6.58],
[18.208, 2.08],
[31.012, 6.58],
[31.012, 2.08],
[37.512, 6.58],
[37.512, 2.08],
[2.056, 6.58],
[2.056, 2.08],
[8.556, 6.58],
[8.556, 2.08],
];

//cluster_distance = 2.55; // Determine experimentally, initial value 2.55
cluster_distance_x = 0.5;
cluster_distance_y = 4.55; // FDM: favor joining pcb pins on the same vertical

// Omron B3F-1000 pushbutton on 1.6mm FR4:
// pins stick out 1.9mm
// pin thichness 0.7mm

pad_dia = 1.5;
pin_top_dia = 0.7;

// put a pin at coordinate x
module pcb_pin(x) {
    translate(x)
    rotate([0, 0, 45])
    union () {
    cylinder(r1 = pad_dia, r2 = pin_top_dia/2, h = pcb_solderside_components, $fn = 4);
    translate([0, 0, -pcb_thickness + eps1])
    cylinder(r = pad_dia, h = pcb_thickness, $fn = 4);
    }
}

// join pins that are clustered
module pcb_pins() {
    // Iterate over pins, and do a hull() of all pins less than cluster_distance apart
    for (p1 = pins)
    hull()
    for (p2 = pins)
    //if (abs(p1[0] - p2[0]) + abs(p1[1] - p2[1]) <= cluster_distance)
    if (abs(p1[0] - p2[0]) <= cluster_distance_x && abs(p1[1] - p2[1]) <= cluster_distance_y)
    pcb_pin(p2);    
}

// pcb with pins at back, to be used as body cutout.
module pcb() {
    translate([-pcb_length/2, -pcb_width/2, 0]) {
        
        // holes for solder-side pins
        translate([0, 0, pcb_thickness])
        translate([0, pcb_width])
        mirror([0, 1, 0])
        pcb_pins();
        translate([0, 0, -eps1])
        linear_extrude(height = pcb_thickness + eps2)
        translate([-clearance_fit/2, -clearance_fit/2])
        square([pcb_length + clearance_fit, pcb_width + clearance_fit]);
    }
}

//pcb();
//pcb_pins();

// not truncated
