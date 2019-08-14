
The is a remix of the ["zapta" Arduino Motorized SMT Solder Paste Dispenser](https://www.thingiverse.com/thing:1119914) , with the following mods:

- stepper controller board with support for footswitch
- motor mount with holes for solder-side pins of the stepper controller board.
- 28BYJ-48 stepper motor converted from [unipolar to bipolar](http://www.jangeox.be/2013/10/change-unipolar-28byj-48-to-bipolar.html) for more torque
- [aluminum 4mm to 5mm shaft coupler](https://www.google.com/search?&q=%2B%22D19*L25%22+Shaft+Coupling+Aluminium+flexible+%2B%224x5%22+aliexpress) instead of a 3D-printed ABS shaft coupler

![knurled threaded tapered brass insert](https://github.com/koendv/paste_dispenser/raw/master/openscad/inserts.jpg)

This build requires 5 M4 threaded inserts, type *IUB-M4-2*. You push these heat-set inserts in with a hot soldering iron. You may want to practice on [insert_practice.stl](https://github.com/koendv/paste_dispenser/blob/master/openscad/mcmaster-94180A353/insert_practice.stl) first.

The file m4_threaded_insert.scad allows you to choose between [McMaster-Carr 94180a353](https://www.mcmaster.com/94180a353) (McMaster, US) and [Aerzetix C19143](https://www.amazon.co.uk/dp/B075XHV3GH/) (Amazon, Europe) inserts. Before generating .stl files in [openscad](http://www.openscad.org), edit m4_threaded_insert.scad and uncomment the threaded insert used. Alternatively, add your own.



