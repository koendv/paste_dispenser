
The is a remix of the ["zapta" Arduino Motorized SMT Solder Paste Dispenser](https://www.thingiverse.com/thing:1119914) , with the following mods:

- add holes for solder-side pins of the stepper controller board.
- convert 28BYJ-48 stepper motor from [unipolar to bipolar](http://www.jangeox.be/2013/10/change-unipolar-28byj-48-to-bipolar.html) for more torque
- use an [aluminum 4mm to 5mm shaft coupler](https://www.google.com/search?&q=%2B%22D19*L25%22+Shaft+Coupling+Aluminium+flexible+%2B%224x5%22+aliexpress) instead of a 3D-printed shaft coupler

![knurled threaded tapered brass insert](https://github.com/koendv/paste_dispenser/raw/master/openscad/inserts.jpg)

This build requires 5 M4 threaded inserts, style *IUB-M4-2*. 
The file m4_threaded_insert.scad allows you to chose between inserts from [McMaster-Carr](https://www.mcmaster.com/94180a353) (US) and from [Amazon](https://www.amazon.co.uk/dp/B075XHV3GH/) (Europe). Edit m4_threaded_insert.scad and uncomment the threaded insert used. Alternatively, add your own.



