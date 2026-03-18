softplus
========

Softplus activation function.

Syntax
------

.. code-block:: bulang

   nn.softplus(x)

Parameters
----------

``x`` : number
   Input value.

Returns
-------

number
   log(1 + exp(x))

Example
-------

.. code-block:: bulang

   import nn;
   
   print(nn.softplus(0));    // ~0.693
   print(nn.softplus(2));    // ~2.13

.. note::

   Softplus is a smooth approximation of ReLU.
