# A Short Introduction To G-Code

## What is G-Code ?

G-code is a standard of code for giving instructions to CNC machines in general, that has been developed over the course of several decades. It mainly contains instructions to move a toolhead along the 3 spatial axis, and activate different tool functionalities. It is not specific to 3D printers or extruders in general, but has many commands to run all sorts of industrial tools. Most of the main commands start with the letter « G », hence the name G-code. 


## Writing you own G-Code

Usually, we use a software that acts as a slicer and will slice your 3D model into layers, thus creating a path for the toolhead to go through to build the desired object. G-code instructions are then automatically generated and you don’t have to worry about them.

However, there might be situations were you want to control the printer in ways that your software will not usually allow : you may want to slice your model along a custom path, or modify something on the fly, or simply quickly arrange for the printer to resume mid-process following an encountered problem, by getting rid of part of the code and choosing an adequate restarting position. Understanding G-code and its possibilities might get you to think of more options with the 3D printer than those which are available through a software.


## Useful G-Code variables

Before detailing the main commands, we must first talk about the main variables :

* `X`, `Y`, and `Z` designate the 3 spatial axis which caracterise the **toolhead's position** in millimeters,
* `E` designates the **extruder's angular position** in the 3D printer's case,
* `F` designates the **displacement speed**, in mm/min.

## Main G-Code commands

### G1

G1 is used to linearly go to a position from the current one. You only have to specified the variables which have to be changed (others will keep their value, though re-specifying them will cause no harm), be it `X`, `Y`, `Z` or `E`. You also have to specify a displacement speed `F` for this motion. For instance : 

```
G1 X100.0000 Y100.000 Z15.0000 F1000
```

This will move the toolhead towards (100,100,15) at a speed of 1 meter per minute.

Note that you only have to specify the speed once if you do not wish to change it **in a series of G1** commands, like so : 

```
G1 X100.0000 Y100.000 Z15.0000 F1000
G1 X200.0000
G1 Y200.0000
```

This will move to (100,100,15), then (200,100,15) and finally (200,200,15) at 1 meter per minute.

However, if you use another command in between, you absolutely have to set the speed again.


### G92

G92 is used to define the current position as having certain values. For instance, if you would like to tell the printer to consider the current position as the origin, you would run : 

```
G92 X0 Y0 Z0
```

This indicates that the current position becomes the new (0,0,0). Of course, setting non-zero values as well as setting the value `E` for the extruder is also acceptable.

If you place an object on the printer's plate (such as a scale) and want to print an object on top of it, you have to use this command to offset the `Z` axis. Imagine your object is 80mm high, then you would run the following while being at the home position :

```
G92 Z-80
```

Then, when the toolhead is instructed to go to `Z=0`, it will effectively go to `Z` so that `Z + offset = 0`, that is to say `Z = 80`, printing on top of your object.


### G28

G28 brings you home no matter the circumstances. Regardless of any offset previously set, it will roll back all axis until it hits the stopping pot on each of them.

### M221

M221 is a command that is more specific to our particular 3D printer. It concerns the motor driving the extruder.

It is useful to set the motor's rotation speed as a percentage of a certain standard speed by using the variable `S` like so :

```
M221 S65
```

This will set the motor's speed to 65% of the base speed. Speeds upwards of the base speed are also possible, like 200% for instance.
