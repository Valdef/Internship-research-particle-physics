Simulation of the Spaghetti Calorimeter developed for the LumiPS system.
Current version is 18x18x18 cm^3, with 0.5 mm fibres spaced by 0.5 mm.
Options are availible to chance size and add a tungsten plate absorber at the front.

Install:

1. delete the build directory
2. mkdir build directory
3. inside the build directory run 'cmake ..'
4. change output file path in src/RunAction.cc
5. run make inside the build directory
6. run ./exampleB4c to run the simulation

Steps 5 and 6 must be done after making any changes in the code.


To change between visualation and bulk run:

1. open exampleB4c.cc 
2. comment out either line 144 or 145
3. run1.mac must be changed in both the build directory and main directory when making changes to it.


To change detector geometry:

1. open src/DetectorConstruction.cc
2. changing sizes is simple as long as:
    a. calorThickness is always a multiple of 18 mm
    b. calorXY is divisible by 1 mm
    c. errors will come up when trying to run if the geometry doesn't work


To change beam properties:

1. open src/PrimaryActionGenerator.cc
2. generally self explanatory what does what
    a. particle type and number is changed at the top of the file
    b. to change beam spread or energy look near the bottom

