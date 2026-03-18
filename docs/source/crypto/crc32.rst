crc32
=====

Calculate the CRC32 checksum of a string.

Syntax
------

.. code-block:: bulang

   crypto.crc32(data)

Parameters
----------

``data`` : string
   The string to checksum.

Returns
-------

int
   32-bit CRC32 checksum as integer.

Example
-------

.. code-block:: bulang

   import crypto;
   
   var checksum = crypto.crc32("hello");
   print(checksum);  // 907060870
   
   // Verify data integrity
   var data = "important data";
   var expected = crypto.crc32(data);
   
   // Later, verify
   if crypto.crc32(data) == expected {
       print("Data is intact");
   }

.. note::

   CRC32 is designed for error detection, not cryptographic security.
   Use it for data integrity checks, not security purposes.
