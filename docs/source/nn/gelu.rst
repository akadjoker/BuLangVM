gelu
====

Gaussian Error Linear Unit activation function.

Syntax
------

.. code-block:: bulang

   nn.gelu(x)

Parameters
----------

``x`` : number
   Input value.

Returns
-------

number
   GELU approximation.

Example
-------

.. code-block:: bulang

   import nn;
   
   print(nn.gelu(1));    // ~0.841
   print(nn.gelu(-1));   // ~-0.159

.. note::

   GELU is commonly used in transformer architectures like BERT and GPT.
