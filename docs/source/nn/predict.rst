predict
=======

Run inference on input data.

Syntax
------

.. code-block:: bulang

   var result = net.predict(input)

Parameters
----------

``input`` : array
   Input data (1D array matching network input size).

Returns
-------

array
   Output predictions.

Example
-------

.. code-block:: bulang

   import nn;
   
   var net = Network();
   net.add(2, 8, "relu");
   net.add(8, 1, "sigmoid");
   net.compile("adam", "bce", 0.01);
   
   // ... train the network ...
   
   var result = net.predict([1, 0]);
   print(result);  // [0.98] (close to 1)
