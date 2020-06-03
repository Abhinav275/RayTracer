# Implementation of Ray Tracing for image rendering in C++
This project implements an image rendering pipeline in C++ using ray tracing.

## Features Implemented
 - [X] Blinn-Phong illumination model
 - [X] Attenuation
 - [X] Depth Cueing
 - [X] Soft Shadows
 - [X] Texture Rendering
 - [X] Reflection
 - [X] Transparency

## Implementation details

### Code Compilation and running

First compile the code and then run rayCaster with the input to render an image with given specifications.

```
$ make
$ ./rayCaster <input-file>.txt
```

The image generated would be in ASCII PPM format and can be opened up in a text editor to read different pixel values. You can read more about ASCII PPM format [here][1]

### Input format

The input text file is expected to follow a certain format. It should initially mention the camera details like ``` eye position```, ```view direction```, ```up direction```, ``` horizontal field of view``` and ``` image width and height```

Then it should mention the ``` background color ``` and the ``` light positions ```. The program can support multiple light positions. The light can be both positional or directional. The code can also support attentuated light sources.

After defining these components, we can define the specifications of various objects in the scene.

### Object Definition

Before defining any object details, make sure to define the respective ```material details```. Changing ```material details``` can make the objects, transparent, opaque, reflective, metallic, plastic etc.

The program can render spheres and triangles. Using these two different object types, we can extend the scenes to make various different objects.

#### Spheres

``` Spheres``` can be defined by giving ``` center coordinates ``` and ``` radius ```. 

#### Triangles

``` Triangles ``` are much more complicated. We first need to define ``` vertices ``` of the triangle and then the ``` face ``` of the triangle. The normal of the triangle is determined by the order of the vertices.

### Template Rendering

We can also render a ```template``` over the ```sphere``` or the ```triangle```. For rendering a ```template``` over the object, the ```template file name``` should be mentioned in the input file right before the material of the object. For triangles, you also need to define ```template vertices``` along with ```traingle vertices``` while defining ```faces```.

## Example Image

For better understanding of input format and template rendering kindly go through the showcase.txt. To run the code

```
./rayCaster showcase.txt
```

This will generate a showcase.ppm in the same folder.




[1]: http://netpbm.sourceforge.net/doc/ppm.html



