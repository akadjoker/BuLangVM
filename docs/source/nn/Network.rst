Network
=======

Create a new neural network.

Syntax
------

.. code-block:: bulang

   var net = Network()

Returns
-------

Network
   A new empty neural network instance.

Example
-------

.. code-block:: bulang

   import nn;
   
   var net = Network();
   
   // Add layers
   net.add(2, 8, "relu");
   net.add(8, 1, "sigmoid");
   
   // Configure
   net.compile("adam", "bce", 0.01);
