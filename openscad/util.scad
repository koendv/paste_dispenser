/* circumscribed (outer) polygons, polygonal cylinders, and cones */

module cylinder_outer(h = 0, r = 0, d = 0, fn = 3) {
   fudge = 1/cos(180/fn);
   if (r == 0) cylinder(h=h, d=d*fudge, $fn=fn);
   else cylinder(h=h, r=r*fudge, $fn=fn);
}

module cone_outer(h = 0, r1 = 0, r2 = 0, d1 = 0, d2 = 0, fn = 3) {
   fudge = 1/cos(180/fn);
   if ((r1 == 0) && (r2 == 0)) cylinder(h=h, d1=d1*fudge, d2=d2*fudge, $fn=fn);
   else cylinder(h=h, r1=r1*fudge, r2=r2*fudge, $fn=fn);
}

module circle_outer(d = 0, r = 0, fn = 3) {
    fudge = 1/cos(180/fn);
    if (r == 0) circle(d = d*fudge, $fn=fn);
    else circle(r = r*fudge, $fn=fn);
}

/* create two (washers, screws, standoffs) separated by a distance */

module twice() {
    translate([0, stepper_screw_spacing/2, 0])
    children();
    translate([0, -stepper_screw_spacing/2, 0])
    children();
}

/* make cutout */

module cutout(cut = true) {
    if (cut) 
        difference() {
            children();
            translate([0, 0, -100])
            cube([200, 200, 200]);
        }
    else
        children();
}

/* not truncated */
