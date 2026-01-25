Buffer Methods
==============

Methods for binary data manipulation using typed buffers in BuLang.

Information Methods
-------------------

Query buffer state and properties.

.. list-table::
   :widths: 20 80
   :header-rows: 1

   * - Method
     - Description
   * - :doc:`length`
     - Get buffer capacity (number of elements)
   * - :doc:`tell`
     - Get current cursor position
   * - :doc:`remaining`
     - Get bytes remaining from cursor to end

.. toctree::
   :hidden:
   
   length
   tell
   remaining

Cursor Control
--------------

Navigate and position within the buffer.

.. list-table::
   :widths: 20 80
   :header-rows: 1

   * - Method
     - Description
   * - :doc:`seek`
     - Move cursor to absolute position
   * - :doc:`skip`
     - Move cursor forward/backward (relative)
   * - :doc:`rewind`
     - Reset cursor to start (position 0)

.. toctree::
   :hidden:
   
   seek
   skip
   rewind

Read Methods
------------

Read data from buffer at current cursor position.

.. list-table::
   :widths: 20 80
   :header-rows: 1

   * - Method
     - Description
   * - :doc:`readByte`
     - Read single byte (0-255)
   * - :doc:`readFloat`
     - Read 32-bit float
   * - :doc:`readDouble`
     - Read 64-bit double
   * - :doc:`readString`
     - Read string of specified length

.. toctree::
   :hidden:
   
   readByte
   readFloat
   readDouble
   readString

Write Methods
-------------

Write data to buffer at current cursor position.

.. list-table::
   :widths: 20 80
   :header-rows: 1

   * - Method
     - Description
   * - :doc:`writeByte`
     - Write single byte
   * - :doc:`writeFloat`
     - Write 32-bit float
   * - :doc:`writeDouble`
     - Write 64-bit double
   * - :doc:`writeString`
     - Write string bytes (no null terminator)

.. toctree::
   :hidden:
   
   writeByte
   writeFloat
   writeDouble
   writeString

Buffer Operations
-----------------

Manipulate buffer contents.

.. list-table::
   :widths: 20 80
   :header-rows: 1

   * - Method
     - Description
   * - :doc:`clear`
     - Zero all bytes
   * - :doc:`fill`
     - Fill buffer with value
   * - :doc:`copy`
     - Copy data from another buffer
   * - :doc:`slice`
     - Create new buffer with copied data range

.. toctree::
   :hidden:
   
   clear
   fill
   copy
   slice

File Operations
---------------

Save buffer data to disk.

.. list-table::
   :widths: 20 80
   :header-rows: 1

   * - Method
     - Description
   * - :doc:`save`
     - Save buffer to file (raw binary)

.. toctree::
   :hidden:
   
   save

Examples
--------

**Creating and Writing:**

.. code-block:: bulang

   buf = @(100, BUFFER_UINT8);
   buf.writeByte(0xFF);
   buf.writeFloat(3.14);
   buf.writeString("Hello");

**Reading Data:**

.. code-block:: bulang

   buf.rewind();              // Start from beginning
   byte = buf.readByte();     // Read byte
   val = buf.readFloat();     // Read float
   text = buf.readString(5);  // Read 5 bytes as string

**Buffer Manipulation:**

.. code-block:: bulang

   buf = @(100, BUFFER_FLOAT);
   buf.fill(0.0);                    // Zero all
   buf.clear();                      // Also zeros
   sub = buf.slice(10, 50);          // Extract portion

**Cursor Navigation:**

.. code-block:: bulang

   buf.seek(0);              // Go to start
   buf.skip(10);             // Forward 10 bytes
   pos = buf.tell();         // Get position
   left = buf.remaining();   // Bytes left

**Copying Between Buffers:**

.. code-block:: bulang

   src = @(100, BUFFER_UINT8);
   dst = @(100, BUFFER_UINT8);
   dst.copy(0, src, 10, 50);  // Copy 50 elements

**Saving to File:**

.. code-block:: bulang

   buf = @(1000, BUFFER_FLOAT);
   // ... fill with data ...
   buf.save("data.bin");