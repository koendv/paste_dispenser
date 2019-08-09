// Test part for practicing threaded inserts insertion using a 
// solder iron..

$fn=32;

h1=8;
h2=10;
w=24;

eps1 = 0.001;
eps2 = 2*eps1;

// Hole for a M4 metal insert, mcmaster part number 94180A353.
// h is the total depth for the screw hole. Already includes an 
// extra eps1 at the opening side.
module m4_threaded_insert(h) {
  // Adding tweaks for compensate for my printer's tolerace.
  A = 5.3 + 0.3;
  B = 5.94 + 0.4;
  L = 7.9;
  D = 5.0;
  translate([0, 0, -eps1]) {
    // NOTE: diameter are compensated to actual print results.
    // May vary among printers.
    cylinder(d1=B, d2=A, h=eps1+L*2/3, $f2=32);
    cylinder(d=A, h=eps1+L, $f2=32);
    translate([0, 0, L-eps1]) cylinder(d=D, h=h+eps1-L, $f2=32);
  }
}

module m4_inserts() {
    translate([6, 6, -eps1]) m4_threaded_insert(h2+2*eps2);
    translate([18, 6, -eps1]) m4_threaded_insert(h2+2*eps2);
    translate([6, 18, -eps1]) m4_threaded_insert(h2+2*eps2);
    translate([18, 18, -eps1]) m4_threaded_insert(h2+2*eps2);
}

module main() {
  difference() {
    cube([w, w, h2]);
    m4_inserts();
      
    translate([-8, -8, 0])
    rotate([0, 0, 45])
    cube([w, w, w], center=true);
  }
}

translate([0, 0, h2]) mirror ([0, 0, 1]) main();
// not truncated
