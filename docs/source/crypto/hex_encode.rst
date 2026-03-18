hex_encode
==========

Encode a string to hexadecimal format.

Syntax
------

.. code-block:: bulang

   crypto.hex_encode(data)

Parameters
----------

``data`` : string
   The string to encode.

Returns
-------

string
   Hexadecimal encoded string (lowercase).

Example
-------

.. code-block:: bulang

   import crypto;
   
   var hex = crypto.hex_encode("ABC");
   print(hex);  // "414243"
   
   var hex2 = crypto.hex_encode("Hello");
   print(hex2);  // "48656c6c6f"
