// process variables

clearance_fit = 0.5;
tight_fit = 0.2;
min_wall_thickness = 2.0; // minimum desired wall thickness, conservative value

// general variables
//$fn = 128; // high resolution for rendering
$fn = 32; // low resolution when designing
eps1 = 0.001;
eps2 = 2 * eps1;

// Choose syringe model
//include <10ml_syringe.scad>
include <5ml_chipquik.scad>

// Choose canstack stepper model
//include <haydon-25000.scad>
//include <haydon-37000.scad>
include <robotdigg-35-byz.scad> // Robotdigg 35mm

// controller pcb dimensions
// pcb is 40x30x1.6mm. 
pcb_width = 30.0;
pcb_length = 40.0;
pcb_thickness = 1.6;
pcb_solderside_components = 1.5; // room needed on solder side for pins of through-hole components

// hex standoffs in body - these are what we screw syringe holder and stepper on
hex_standoff_dia = 7.0; // distance between flats
hex_standoff_length = 30.0; // same as, or larger than, pcb width

// body dimensions
body_height = max(pcb_width, hex_standoff_length); 

// syringe holder
syringe_holder_base_height = 3.0;
syringe_holder_total_height = 6.0;

// not truncated
