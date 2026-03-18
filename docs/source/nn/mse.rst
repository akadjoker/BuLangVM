mse
===

Mean Squared Error loss function.

Syntax
------

.. code-block:: bulang

   nn.mse(predicted, target)

Parameters
----------

``predicted`` : number
   Predicted value.

``target`` : number
   Target value.

Returns
-------

number
   (predicted - target)²

Example
-------

.. code-block:: bulang

   import nn;
   
   var loss = nn.mse(0.8, 1.0);
   print(loss);  // 0.04
