save
====

Save the trained model to a file.

Syntax
------

.. code-block:: bulang

   net.save(path)

Parameters
----------

``path`` : string
   Output file path (use ``.bunn`` extension).

Example
-------

.. code-block:: bulang

   import nn;
   
   var net = Network();
   net.add(2, 8, "relu");
   net.add(8, 1, "sigmoid");
   net.compile("adam", "bce", 0.01);
   
   // Train...
   net.fit(X, Y, 1000, 4);
   
   // Save model
   net.save("model.bunn");
