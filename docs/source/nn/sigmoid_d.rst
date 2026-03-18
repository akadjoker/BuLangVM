sigmoid_d
=========

Sigmoid derivative for backpropagation.

Syntax
------

.. code-block:: bulang

   nn.sigmoid_d(x)

Parameters
----------

``x`` : number
   Input value.

Returns
-------

number
   sigmoid(x) * (1 - sigmoid(x))

Example
-------

.. code-block:: bulang

   import nn;
   
   print(nn.sigmoid_d(0));    // 0.25
