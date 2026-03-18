Crypto Module
=============

Cryptographic functions and encoding utilities.

.. code-block:: bulang

   import crypto;

Base64 Encoding
---------------

Encode/decode data in Base64 format.

.. list-table::
   :widths: 25 75
   :header-rows: 1

   * - Function
     - Description
   * - :doc:`base64_encode`
     - Encode string to Base64
   * - :doc:`base64_decode`
     - Decode Base64 to string

.. toctree::
   :hidden:
   
   base64_encode
   base64_decode

Hex Encoding
------------

Encode/decode data in hexadecimal format.

.. list-table::
   :widths: 25 75
   :header-rows: 1

   * - Function
     - Description
   * - :doc:`hex_encode`
     - Encode string to hexadecimal
   * - :doc:`hex_decode`
     - Decode hexadecimal to string

.. toctree::
   :hidden:
   
   hex_encode
   hex_decode

Hash Functions
--------------

Generate cryptographic hashes.

.. list-table::
   :widths: 25 75
   :header-rows: 1

   * - Function
     - Description
   * - :doc:`md5`
     - Calculate MD5 hash (128-bit)
   * - :doc:`sha1`
     - Calculate SHA-1 hash (160-bit)
   * - :doc:`sha256`
     - Calculate SHA-256 hash (256-bit)
   * - :doc:`crc32`
     - Calculate CRC32 checksum (integer)

.. toctree::
   :hidden:
   
   md5
   sha1
   sha256
   crc32

UUID Generation
---------------

Generate unique identifiers.

.. list-table::
   :widths: 25 75
   :header-rows: 1

   * - Function
     - Description
   * - :doc:`uuid`
     - Generate UUID v4 string

.. toctree::
   :hidden:
   
   uuid

Examples
--------

**Base64 Encoding:**

.. code-block:: bulang

   import crypto;
   
   var encoded = crypto.base64_encode("Hello World");
   print(encoded);  // "SGVsbG8gV29ybGQ="
   
   var decoded = crypto.base64_decode(encoded);
   print(decoded);  // "Hello World"

**Hex Encoding:**

.. code-block:: bulang

   var hex = crypto.hex_encode("ABC");
   print(hex);  // "414243"
   
   var text = crypto.hex_decode(hex);
   print(text);  // "ABC"

**Hashing:**

.. code-block:: bulang

   var h1 = crypto.md5("password");
   var h2 = crypto.sha1("password");
   var h3 = crypto.sha256("password");
   var checksum = crypto.crc32("data");

**UUID Generation:**

.. code-block:: bulang

   var id = crypto.uuid();
   print(id);  // "550e8400-e29b-41d4-a716-446655440000"
