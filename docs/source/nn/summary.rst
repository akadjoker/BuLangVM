summary
=======

Print a summary of the network architecture.

Syntax
------

.. code-block:: bulang

   net.summary()

Example
-------

.. code-block:: bulang

   import nn;
   
   var net = Network();
   net.add(2, 8, "relu");
   net.add(8, 4, "relu");
   net.add(4, 1, "sigmoid");
   net.compile("adam", "bce", 0.01);
   
   net.summary();
   
   // Output:
   // Layer 1: Dense (2 → 8) relu
   // Layer 2: Dense (8 → 4) relu
   // Layer 3: Dense (4 → 1) sigmoid
