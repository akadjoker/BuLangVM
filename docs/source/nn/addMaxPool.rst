addMaxPool
==========

Add a max pooling layer.

Syntax
------

.. code-block:: bulang

   net.addMaxPool(pool_w, pool_h)

Parameters
----------

``pool_w`` : int
   Pooling window width.

``pool_h`` : int
   Pooling window height.

Example
-------

.. code-block:: bulang

   import nn;
   
   var net = Network();
   net.input(28, 28, 1);
   net.addConv2D(8, 3, 3, "relu");
   
   // 2x2 max pooling (reduces dimensions by half)
   net.addMaxPool(2, 2);
