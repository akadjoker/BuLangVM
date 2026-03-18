leaky_relu
==========

Leaky ReLU activation function.

Syntax
------

.. code-block:: bulang

   nn.leaky_relu(x, alpha)

Parameters
----------

``x`` : number
   Input value.

``alpha`` : number
   Slope for negative values (typically 0.01).

Returns
-------

number
   x if x > 0, else alpha * x

Example
-------

.. code-block:: bulang

   import nn;
   
   print(nn.leaky_relu(5, 0.01));    // 5
   print(nn.leaky_relu(-3, 0.01));   // -0.03
