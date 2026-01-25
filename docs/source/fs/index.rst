Filesystem Operations
=====================

High-level filesystem operations for text files and directories.

File Reading & Writing
----------------------

Simple text file operations.

.. list-table::
   :widths: 20 80
   :header-rows: 1

   * - Function
     - Description
   * - :doc:`read`
     - Read entire file as string
   * - :doc:`write`
     - Write string to file (overwrites)
   * - :doc:`append`
     - Append string to file

.. toctree::
   :hidden:
   
   read
   write
   append

File Management
---------------

Delete and query files.

.. list-table::
   :widths: 20 80
   :header-rows: 1

   * - Function
     - Description
   * - :doc:`remove`
     - Delete file
   * - :doc:`stat`
     - Get file/directory information (size, type, etc.)

.. toctree::
   :hidden:
   
   remove
   stat

Directory Operations
--------------------

Create, remove, and list directories.

.. list-table::
   :widths: 20 80
   :header-rows: 1

   * - Function
     - Description
   * - :doc:`mkdir`
     - Create directory
   * - :doc:`rmdir`
     - Remove empty directory
   * - :doc:`list`
     - List directory contents

.. toctree::
   :hidden:
   
   mkdir
   rmdir
   list

Examples
--------

**Reading and Writing Files:**

.. code-block:: bulang

   // Write entire file
   fs.write("config.txt", "option=true");
   
   // Read entire file
   content = fs.read("config.txt");
   if (content != nil) {
       write(content);
   }
   
   // Append to log
   fs.append("log.txt", "New entry\n");

**Directory Operations:**

.. code-block:: bulang

   // Create directory
   fs.mkdir("data");
   fs.mkdir("saves/profile1");
   
   // List contents
   files = fs.list(".");
   foreach (file in files) {
       write(file);
   }
   
   // Remove directory
   fs.rmdir("old_data");

**File Information:**

.. code-block:: bulang

   info = fs.stat("data.txt");
   if (info != nil) {
       write("Size: {} bytes", info["size"]);
       write("Is file: {}", info["isfile"]);
       write("Is dir: {}", info["isdir"]);
       
       // Unix only:
       // write("Mode: {}", info["mode"]);
       // write("Modified: {}", info["mtime"]);
   }

**File Management:**

.. code-block:: bulang

   // Delete file
   fs.remove("temp.txt");
   
   // Check if file exists
   info = fs.stat("config.txt");
   if (info != nil && info["isfile"]) {
       write("File exists!");
   }

**Config File Example:**

.. code-block:: bulang

   // Save config
   config = "width=800\nheight=600\nfullscreen=false";
   fs.write("game.cfg", config);
   
   // Load config
   data = fs.read("game.cfg");
   if (data != nil) {
       lines = data.split("\n");
       foreach (line in lines) {
           write(line);
       }
   }

**Log File Example:**

.. code-block:: bulang

   // Initialize log
   fs.write("game.log", "=== Game Started ===\n");
   
   // Add entries
   fs.append("game.log", "Player joined\n");
   fs.append("game.log", "Level loaded\n");
   fs.append("game.log", "Game saved\n");