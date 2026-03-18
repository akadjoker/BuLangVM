fit
===

Train the network on data.

Syntax
------

.. code-block:: bulang

   net.fit(X, Y, epochs, batch_size)

Parameters
----------

``X`` : array
   Training inputs (2D array of samples).

``Y`` : array
   Training targets (2D array of outputs).

``epochs`` : int
   Number of training iterations.

``batch_size`` : int
   Samples per batch.

Example
-------

.. code-block:: bulang

   import nn;
   
   var net = Network();
   net.add(2, 8, "tanh");
   net.add(8, 1, "sigmoid");
   net.compile("adam", "bce", 0.01);
   
   // XOR training data
   var X = [[0,0], [0,1], [1,0], [1,1]];
   var Y = [[0], [1], [1], [0]];
   
   // Train for 1000 epochs
   net.fit(X, Y, 1000, 4);
