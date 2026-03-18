swish
=====

Swish activation function (self-gated).

Syntax
------

.. code-block:: bulang

   nn.swish(x)

Parameters
----------

``x`` : number
   Input value.

Returns
-------

number
   x * sigmoid(x)

Example
-------

.. code-block:: bulang

   import nn;
   
   print(nn.swish(2));    // ~1.76
   print(nn.swish(-2));   // ~-0.24
