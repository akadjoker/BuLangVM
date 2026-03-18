sigmoid
=======

Sigmoid activation function.

Syntax
------

.. code-block:: bulang

   nn.sigmoid(x)

Parameters
----------

``x`` : number
   Input value.

Returns
-------

number
   Value in range (0, 1).

Formula
-------

.. math::

   \sigma(x) = \frac{1}{1 + e^{-x}}

Example
-------

.. code-block:: bulang

   import nn;
   
   print(nn.sigmoid(0));    // 0.5
   print(nn.sigmoid(2));    // ~0.88
   print(nn.sigmoid(-2));   // ~0.12
