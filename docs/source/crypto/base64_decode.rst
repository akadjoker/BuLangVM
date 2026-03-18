base64_decode
=============

Decode a Base64 encoded string.

Syntax
------

.. code-block:: bulang

   crypto.base64_decode(encoded)

Parameters
----------

``encoded`` : string
   The Base64 encoded string to decode.

Returns
-------

string
   Decoded string.

Example
-------

.. code-block:: bulang

   import crypto;
   
   var decoded = crypto.base64_decode("SGVsbG8gV29ybGQ=");
   print(decoded);  // "Hello World"
