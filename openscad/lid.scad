// pcb cover

/* 
 Flow to generate .stl file of pcb:
 - In kicad: In pcbnew: File... Export... STEP
 - In FreeCad: 
    File... Open ... (select file exported from kicad)
    Edit... Select All
    File... Export... STL
 */

import("paste_dispenser.stl", convexity=20);
$fn = 120;

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

module lid() {
    difference() {
        cube([40, 30, 2]);
        translate([0, 0, -1])
        linear_extrude(height = 10) 
        offset(r  =  0.25) { 
            translate(([21.36, 6.58]+[27.86, 2.08])/2) button(); // 4 pushbuttons
            translate(([11.708, 6.58]+[18.208, 2.08])/2) button();
            translate(([31.012, 6.58]+[37.512, 2.08])/2)  button();
            translate(([2.056, 6.58]+[8.556, 2.08])/2) button();
            translate([28.98, 10.644]) pinheader(3, 2);  // icp
            translate([15.772, 28.043]) pinheader(5, 1); // stepper
            translate([34.06, 17.248]) pinheader(1, 4);  // oled
            translate([37.87, 12.168]) pinheader(1, 6);  //ext vertical header
            translate([30.377, 27.916]) led(); // led
        }
    }
}

translate([0, 0, 6.5])
color("grey")
lid();

// not truncated
