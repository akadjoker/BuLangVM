relu
====

Rectified Linear Unit activation function.

Syntax
------

.. code-block:: bulang

   nn.relu(x)

Parameters
----------

``x`` : number
   Input value.

Returns
-------

number
   max(0, x)

Example
-------

.. code-block:: bulang

   import nn;
   
   print(nn.relu(5));    // 5
   print(nn.relu(-3));   // 0
   print(nn.relu(0));    // 0
