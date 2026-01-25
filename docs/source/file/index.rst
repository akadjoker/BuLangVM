File Operations
===============

Binary file I/O operations in BuLang.

File Management
---------------

Open, close, and manage files.

.. list-table::
   :widths: 20 80
   :header-rows: 1

   * - Function
     - Description
   * - :doc:`open`
     - Open file for reading/writing (returns file ID)
   * - :doc:`close`
     - Close file (auto-saves if modified)
   * - :doc:`save`
     - Save changes to disk
   * - :doc:`size`
     - Get file size in bytes
   * - :doc:`tell`
     - Get cursor position
    

.. toctree::
   :hidden:
   
   open
   close
   save
   size
   tell

Cursor Control
--------------

Navigate within the file.

.. list-table::
   :widths: 20 80
   :header-rows: 1

   * - Function
     - Description
   * - :doc:`seek`
     - Move cursor to absolute position

.. toctree::
   :hidden:
   
   seek

Read Functions
--------------

Read data from file at current cursor position.

.. list-table::
   :widths: 20 80
   :header-rows: 1

   * - Function
     - Description
   * - :doc:`read_byte`
     - Read single byte (0-255)
   * - :doc:`read_bool`
     - Read boolean
   * - :doc:`read_int`
     - Read 32-bit integer
   * - :doc:`read_float`
     - Read 32-bit float
   * - :doc:`read_double`
     - Read 64-bit double
   * - :doc:`read_string`
     - Read string (length-prefixed)

.. toctree::
   :hidden:
   
   read_byte
   read_bool
   read_int
   read_float
   read_double
   read_string

Write Functions
---------------

Write data to file at current cursor position.

.. list-table::
   :widths: 20 80
   :header-rows: 1

   * - Function
     - Description
   * - :doc:`write_byte`
     - Write single byte (0-255)
   * - :doc:`write_bool`
     - Write boolean as byte
   * - :doc:`write_int`
     - Write 32-bit integer
   * - :doc:`write_float`
     - Write 32-bit float
   * - :doc:`write_double`
     - Write 64-bit double
   * - :doc:`write_string`
     - Write string (length-prefixed)

.. toctree::
   :hidden:
   
   write_byte
   write_bool
   write_int
   write_float
   write_double
   write_string

Utility Functions
-----------------

String formatting utility.

.. list-table::
   :widths: 20 80
   :header-rows: 1

   * - Function
     - Description
   * - :doc:`format`
     - Format string with ``{}`` placeholders

.. toctree::
   :hidden:
   
   format

Examples
--------

**Opening and Closing Files:**

.. code-block:: bulang

   f = file.open("data.bin", "rw");  // Read-write
   // ... work with file ...
   file.close(f);                    // Auto-saves

   // Different modes
   f = file.open("save.dat", "w");   // Write only
   f = file.open("load.dat", "r");   // Read only

**Writing Data:**

.. code-block:: bulang

   f = file.open("game.sav", "w");
   
   file.write_int(f, 1234);
   file.write_float(f, 99.5);
   file.write_bool(f, true);
   file.write_string(f, "Player Name");
   
   file.save(f);
   file.close(f);

**Reading Data:**

.. code-block:: bulang

   f = file.open("game.sav", "r");
   
   score = file.read_int(f);
   health = file.read_float(f);
   active = file.read_bool(f);
   name = file.read_string(f);
   
   file.close(f);

**Seeking and File Info:**

.. code-block:: bulang

   f = file.open("data.bin", "r");
   
   bytes = file.size(f);      // Get file size
   file.seek(f, 0);           // Go to start
   file.seek(f, 100);         // Go to position 100
   
   file.close(f);

**Save Game Example:**

.. code-block:: bulang

   // Save
   f = file.open("save.dat", "w");
   file.write_string(f, player.name);
   file.write_int(f, player.score);
   file.write_int(f, player.level);
   file.write_float(f, player.health);
   file.close(f);
   
   // Load
   f = file.open("save.dat", "r");
   player.name = file.read_string(f);
   player.score = file.read_int(f);
   player.level = file.read_int(f);
   player.health = file.read_float(f);
   file.close(f);