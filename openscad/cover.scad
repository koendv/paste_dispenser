// pcb cover
// needs more aesthetics.

$fn = 120;
eps1 = 0.001;
slack = 0.4; // slack around components
wall_thickness = 2.0;
cover_height = 8.0;
pcb_thickness = 1.6;
pcb_size_x = 40;
pcb_size_y = 30;

// hole for pinheader. parameters are number of rows, number of columns.

module pinheader(row, col) {
    pitch = 2.54;
    translate(- [pitch/2, pitch/2])
    square([row * pitch, col * pitch]);
}

// hole for pushbutton knob.

module button() {
    circle(d = 3.5);
}

// hole for led.

module led() {
    square([2, 1.25], true);
}

module corner_side() {
    hull() {
        cube([wall_thickness, 2 * wall_thickness, eps1]);
        translate([0, 0, cover_height])
        cube([wall_thickness, cover_height + 2 * wall_thickness, eps1]);
    }
}

module corner() {
    translate([0, -wall_thickness, 0])
    corner_side();
    translate([wall_thickness, -wall_thickness, 0])
    rotate([0, 0, 90])
    corner_side();
    translate([-wall_thickness, 0, pcb_thickness])
    cube([wall_thickness, wall_thickness, cover_height - pcb_thickness]);
}

module corners() {
    rotate([0, 0, 270])
    corner();
    translate([pcb_size_x, 0, 0])
    rotate([0, 0, 0])
    corner();
    translate([0, pcb_size_y, 0])
    rotate([0, 0, 180])
    corner();
    translate([pcb_size_x, pcb_size_y, 0])
    rotate([0, 0, 90])
    corner();    
}

module cover() {
    translate([0, 0, cover_height])
    difference() {
        translate([-wall_thickness, -wall_thickness, 0])
        cube([pcb_size_x + 2 * wall_thickness, pcb_size_y + 2 * wall_thickness, wall_thickness]);
        translate([0, 0, -1])
        linear_extrude(height = 10) 
        offset(r  =  slack) { 
            // positioning data from .drl drill file 
            translate(([21.36, 6.58]+[27.86, 2.08])/2) button(); // 4 pushbuttons
            translate(([11.708, 6.58]+[18.208, 2.08])/2) button();
            translate(([31.012, 6.58]+[37.512, 2.08])/2)  button();
            translate(([2.056, 6.58]+[8.556, 2.08])/2) button();
            translate([28.98, 10.644]) pinheader(3, 2);  // icp
            translate([15.772, 28.043]) pinheader(5, 1); // stepper
            translate([34.06, 17.248]) pinheader(1, 4);  // oled. XXX add support for oled screen
            translate([37.87, 12.168]) pinheader(1, 6);  // ext vertical header
            translate([34.06, 12.168]) pinheader(2, 6);  // dummy header, to avoid thin slivers
            
            
            // positioning data from .pos pick-and-place file 
            translate([30.377, 27.916]) led(); // led
        }
    }
    corners();
}

// choose between ready-to-print cover, and assembly of cover and pcb.
if (false) {
    // cover, positioned for fdm printing
    translate([pcb_size_x + wall_thickness, wall_thickness, cover_height + pcb_thickness])
    rotate([0, 180, 0])
    cover();
}
else {
    // assembly of cover and pcb
    cover();
 
    //   Flow to generate .stl file of pcb:
    // - In kicad: In pcbnew: File... Export... STEP
    // - In FreeCad: 
    // File... Open ... (select file exported from kicad)
    // Edit... Select All
    // File... Export... STL

    color("LimeGreen")
    import("paste_dispenser.stl", convexity=20);
}
    
// not truncated
