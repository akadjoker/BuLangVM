mish
====

Mish activation function.

Syntax
------

.. code-block:: bulang

   nn.mish(x)

Parameters
----------

``x`` : number
   Input value.

Returns
-------

number
   x * tanh(softplus(x))

Example
-------

.. code-block:: bulang

   import nn;
   
   print(nn.mish(2));    // ~1.94
   print(nn.mish(-2));   // ~-0.25

.. note::

   Mish often provides better results than ReLU in deep networks.
