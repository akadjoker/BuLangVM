layers
======

Get information about network layers.

Syntax
------

.. code-block:: bulang

   var info = net.layers()

Returns
-------

array
   Array of layer information objects.

Example
-------

.. code-block:: bulang

   import nn;
   
   var net = Network();
   net.add(2, 8, "relu");
   net.add(8, 1, "sigmoid");
   
   var info = net.layers();
   
   for layer in info {
       print("Type:", layer["type"]);
       print("In:", layer["in"]);
       print("Out:", layer["out"]);
   }
