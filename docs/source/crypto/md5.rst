md5
===

Calculate the MD5 hash of a string.

Syntax
------

.. code-block:: bulang

   crypto.md5(data)

Parameters
----------

``data`` : string
   The string to hash.

Returns
-------

string
   32-character hexadecimal MD5 hash.

Example
-------

.. code-block:: bulang

   import crypto;
   
   var hash = crypto.md5("hello");
   print(hash);  // "5d41402abc4b2a76b9719d911017c592"

.. warning::

   MD5 is cryptographically broken and should not be used for security purposes.
   Use SHA-256 for secure hashing.
