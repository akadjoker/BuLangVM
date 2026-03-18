denormalize
===========

Scale a normalized value back to original range.

Syntax
------

.. code-block:: bulang

   nn.denormalize(value, min, max)

Parameters
----------

``value`` : number
   Normalized value (0-1).

``min`` : number
   Minimum of the target range.

``max`` : number
   Maximum of the target range.

Returns
-------

number
   Denormalized value.

Example
-------

.. code-block:: bulang

   import nn;
   
   // Denormalize (0-1) back to (0-255)
   var pixel = nn.denormalize(0.5, 0, 255);
   print(pixel);  // 127.5
