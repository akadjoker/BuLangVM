relu_d
======

ReLU derivative for backpropagation.

Syntax
------

.. code-block:: bulang

   nn.relu_d(x)

Parameters
----------

``x`` : number
   Input value.

Returns
-------

number
   1 if x > 0, else 0

Example
-------

.. code-block:: bulang

   import nn;
   
   print(nn.relu_d(5));    // 1
   print(nn.relu_d(-3));   // 0
