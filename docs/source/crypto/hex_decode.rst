hex_decode
==========

Decode a hexadecimal encoded string.

Syntax
------

.. code-block:: bulang

   crypto.hex_decode(hex)

Parameters
----------

``hex`` : string
   The hexadecimal string to decode.

Returns
-------

string
   Decoded string.

Example
-------

.. code-block:: bulang

   import crypto;
   
   var text = crypto.hex_decode("414243");
   print(text);  // "ABC"
   
   var hello = crypto.hex_decode("48656c6c6f");
   print(hello);  // "Hello"
