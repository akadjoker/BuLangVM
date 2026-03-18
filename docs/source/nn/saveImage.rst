saveImage
=========

Save pixel data as a BMP image file.

Syntax
------

.. code-block:: bulang

   nn.saveImage(path, width, height, channels, pixels)

Parameters
----------

``path`` : string
   Output file path.

``width`` : int
   Image width.

``height`` : int
   Image height.

``channels`` : int
   Number of channels (3 for RGB, 4 for RGBA).

``pixels`` : array
   Flat array of pixel values [0, 1].

Example
-------

.. code-block:: bulang

   import nn;
   
   // Create a 2x2 red image
   var pixels = [
       1, 0, 0,  // Red
       1, 0, 0,  // Red
       1, 0, 0,  // Red
       1, 0, 0   // Red
   ];
   
   nn.saveImage("red.bmp", 2, 2, 3, pixels);
