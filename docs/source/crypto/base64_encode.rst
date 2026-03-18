base64_encode
=============

Encode a string to Base64 format.

Syntax
------

.. code-block:: bulang

   crypto.base64_encode(data)

Parameters
----------

``data`` : string
   The string to encode.

Returns
-------

string
   Base64 encoded string.

Example
-------

.. code-block:: bulang

   import crypto;
   
   var encoded = crypto.base64_encode("Hello World");
   print(encoded);  // "SGVsbG8gV29ybGQ="
   
   // Encode binary-like data
   var token = crypto.base64_encode("user:password");
   print(token);  // "dXNlcjpwYXNzd29yZA=="
