add
===

Add a dense (fully connected) layer to the network.

Syntax
------

.. code-block:: bulang

   net.add(inputs, outputs, activation)

Parameters
----------

``inputs`` : int
   Number of input neurons.

``outputs`` : int
   Number of output neurons.

``activation`` : string
   Activation function: ``"relu"``, ``"sigmoid"``, ``"tanh"``, ``"softmax"``, ``"identity"``, ``"mish"``

Example
-------

.. code-block:: bulang

   import nn;
   
   var net = Network();
   
   // 2 inputs → 8 hidden → 1 output
   net.add(2, 8, "tanh");
   net.add(8, 1, "sigmoid");
