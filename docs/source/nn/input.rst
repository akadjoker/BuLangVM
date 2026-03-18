input
=====

Set the input dimensions for a CNN.

Syntax
------

.. code-block:: bulang

   net.input(width, height, channels)

Parameters
----------

``width`` : int
   Input width in pixels.

``height`` : int
   Input height in pixels.

``channels`` : int
   Number of channels (1 for grayscale, 3 for RGB).

Example
-------

.. code-block:: bulang

   import nn;
   
   var net = Network();
   
   // MNIST-style input
   net.input(28, 28, 1);
   
   // RGB image
   net.input(32, 32, 3);

.. warning::

   Minimum input size for CNN training is 8x8 pixels.
