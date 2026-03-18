loadImage
=========

Load a BMP image file for neural network input.

Syntax
------

.. code-block:: bulang

   var (width, height, channels, pixels) = nn.loadImage(path)

Parameters
----------

``path`` : string
   Path to BMP image file.

Returns
-------

Multiple values:

``width`` : int
   Image width in pixels.

``height`` : int
   Image height in pixels.

``channels`` : int
   Number of channels (3 for RGB, 4 for RGBA).

``pixels`` : array
   Flat array of normalized pixel values [0, 1].

Supported Formats
-----------------

- 24-bit BMP (RGB)
- 32-bit BMP (RGBA with alpha)

Example
-------

.. code-block:: bulang

   import nn;
   
   var (w, h, c, pixels) = nn.loadImage("sprite.bmp");
   
   print("Size:", w, "x", h);
   print("Channels:", c);
   print("Pixels:", pixels.length());
   
   // Use pixels as network input
   var result = net.predict(pixels);
