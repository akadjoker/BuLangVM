elu
===

Exponential Linear Unit activation function.

Syntax
------

.. code-block:: bulang

   nn.elu(x, alpha)

Parameters
----------

``x`` : number
   Input value.

``alpha`` : number
   Scale for negative values (typically 1.0).

Returns
-------

number
   x if x > 0, else alpha * (exp(x) - 1)

Example
-------

.. code-block:: bulang

   import nn;
   
   print(nn.elu(2, 1.0));    // 2
   print(nn.elu(-1, 1.0));   // ~-0.632
