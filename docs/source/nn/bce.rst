bce
===

Binary Cross Entropy loss function.

Syntax
------

.. code-block:: bulang

   nn.bce(predicted, target)

Parameters
----------

``predicted`` : number
   Predicted probability (0-1).

``target`` : number
   Target value (0 or 1).

Returns
-------

number
   Binary cross entropy loss.

Example
-------

.. code-block:: bulang

   import nn;
   
   var loss = nn.bce(0.9, 1.0);
   print(loss);  // ~0.105
