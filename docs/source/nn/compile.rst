compile
=======

Configure the optimizer and loss function.

Syntax
------

.. code-block:: bulang

   net.compile(optimizer, loss, learning_rate)

Parameters
----------

``optimizer`` : string
   Optimizer: ``"adam"``, ``"sgd"``, ``"rmsprop"``, ``"adagrad"``

``loss`` : string
   Loss function: ``"mse"``, ``"bce"``, ``"cross_entropy"``

``learning_rate`` : number
   Learning rate (e.g., 0.01, 0.001).

Example
-------

.. code-block:: bulang

   import nn;
   
   var net = Network();
   net.add(2, 8, "relu");
   net.add(8, 1, "sigmoid");
   
   // Binary classification
   net.compile("adam", "bce", 0.01);
   
   // Multi-class classification
   // net.compile("adam", "cross_entropy", 0.001);
   
   // Regression
   // net.compile("sgd", "mse", 0.01);
