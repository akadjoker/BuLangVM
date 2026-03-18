addConv2D
=========

Add a 2D convolutional layer.

Syntax
------

.. code-block:: bulang

   net.addConv2D(filters, kernel_w, kernel_h, activation)

Parameters
----------

``filters`` : int
   Number of output filters.

``kernel_w`` : int
   Kernel width.

``kernel_h`` : int
   Kernel height.

``activation`` : string
   Activation function (usually ``"relu"``).

Example
-------

.. code-block:: bulang

   import nn;
   
   var net = Network();
   net.input(28, 28, 1);
   
   // First conv layer: 8 filters, 3x3 kernel
   net.addConv2D(8, 3, 3, "relu");
   
   // Second conv layer: 16 filters, 3x3 kernel
   net.addConv2D(16, 3, 3, "relu");
