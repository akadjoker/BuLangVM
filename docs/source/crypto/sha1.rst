sha1
====

Calculate the SHA-1 hash of a string.

Syntax
------

.. code-block:: bulang

   crypto.sha1(data)

Parameters
----------

``data`` : string
   The string to hash.

Returns
-------

string
   40-character hexadecimal SHA-1 hash.

Example
-------

.. code-block:: bulang

   import crypto;
   
   var hash = crypto.sha1("hello");
   print(hash);  // "aaf4c61ddcc5e8a2dabede0f3b482cd9aea9434d"

.. warning::

   SHA-1 is considered weak for cryptographic purposes.
   Use SHA-256 for secure hashing.
