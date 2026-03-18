tanh_d
======

Tanh derivative for backpropagation.

Syntax
------

.. code-block:: bulang

   nn.tanh_d(x)

Parameters
----------

``x`` : number
   Input value.

Returns
-------

number
   1 - tanh(x)²

Example
-------

.. code-block:: bulang

   import nn;
   
   print(nn.tanh_d(0));    // 1.0
   print(nn.tanh_d(1));    // ~0.42
