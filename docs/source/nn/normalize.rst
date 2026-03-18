normalize
=========

Normalize a value to the range [0, 1].

Syntax
------

.. code-block:: bulang

   nn.normalize(value, min, max)

Parameters
----------

``value`` : number
   Value to normalize.

``min`` : number
   Minimum of the original range.

``max`` : number
   Maximum of the original range.

Returns
-------

number
   Normalized value in [0, 1].

Example
-------

.. code-block:: bulang

   import nn;
   
   // Normalize pixel (0-255) to (0-1)
   var pixel = nn.normalize(128, 0, 255);
   print(pixel);  // ~0.502
