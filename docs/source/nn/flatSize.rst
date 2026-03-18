flatSize
========

Get the flattened size after convolutional layers.

Syntax
------

.. code-block:: bulang

   var size = net.flatSize()

Returns
-------

int
   Number of neurons after flattening conv output.

Example
-------

.. code-block:: bulang

   import nn;
   
   var net = Network();
   net.input(28, 28, 1);
   net.addConv2D(8, 3, 3, "relu");
   net.addMaxPool(2, 2);
   
   // Get flattened size for dense layer
   var flat = net.flatSize();
   net.add(flat, 64, "relu");
   net.add(64, 10, "softmax");

.. note::

   Call ``flatSize()`` after all conv/pool layers but before dense layers.
