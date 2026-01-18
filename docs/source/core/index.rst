Core Functions
==============


Output Functions
----------------

Functions for printing and displaying data.

.. list-table::
   :widths: 20 80
   :header-rows: 1

   * - Function
     - Description
   * - :doc:`print`
     - Print formatted string with newline
   * - :doc:`write`
     - Print formatted string without newline
   * - :doc:`format`
     - Format string with ``{}`` placeholders

.. toctree::
   :hidden:
   
   print
   write
   format
   clock

Debug Functions
---------------

Debugging and development utilities.

.. list-table::
   :widths: 20 80
   :header-rows: 1

   * - Function
     - Description
   * - :doc:`print_stack`
     - Print current VM stack state for debugging

.. toctree::
   :hidden:
   
   print_stack

System Functions
----------------

Low-level system operations.

.. list-table::
   :widths: 20 80
   :header-rows: 1

   * - Function
     - Description
   * - :doc:`_gc`
     - Manually trigger garbage collection cycle

.. toctree::
   :hidden:
   
   _gc

Examples
--------

**Basic Output:**

.. code-block:: bulang

   print("Hello, World!");           // With newline
   write("Loading... ");              // Without newline
   
**String Formatting:**

.. code-block:: bulang

   name = "Alice";
   score = 95;
   msg = format("Player {} scored {}", name, score);
   print(msg);  // "Player Alice scored 95"

**Debugging:**

.. code-block:: bulang

   print_stack("Checkpoint 1");  // See VM state
   _gc();                        // Force garbage collection