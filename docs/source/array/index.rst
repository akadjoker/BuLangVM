Array Methods
=============

Methods for manipulating dynamic arrays in BuLang.

Access Methods
--------------

Get elements from arrays.

.. list-table::
   :widths: 20 80
   :header-rows: 1

   * - Method
     - Description
   * - :doc:`first`
     - Get first element (or nil if empty)
   * - :doc:`last`
     - Get last element (or nil if empty)
   * - :doc:`back`
     - Get last item without removing it

.. toctree::
   :hidden:
   
   first
   last
   back

Modification Methods
--------------------

Add, remove, and modify array elements.

.. list-table::
   :widths: 20 80
   :header-rows: 1

   * - Method
     - Description
   * - :doc:`push`
     - Add item to end (chainable)
   * - :doc:`pop`
     - Remove and return last item
   * - :doc:`insert`
     - Insert item at specific index
   * - :doc:`remove`
     - Remove element at index
   * - :doc:`fill`
     - Fill all elements with value
   * - :doc:`reverse`
     - Reverse array in-place (chainable)

.. toctree::
   :hidden:
   
   push
   pop
   insert
   remove
   fill
   reverse

Search Methods
--------------

Find elements and check existence.

.. list-table::
   :widths: 20 80
   :header-rows: 1

   * - Method
     - Description
   * - :doc:`contains`
     - Check if array contains value
   * - :doc:`find`
     - Find first index of value (returns -1 if not found)

.. toctree::
   :hidden:
   
   contains
   find

Utility Methods
---------------

Array manipulation and transformation.

.. list-table::
   :widths: 20 80
   :header-rows: 1

   * - Method
     - Description
   * - :doc:`concat`
     - Concatenate arrays (creates new array)

.. toctree::
   :hidden:
   
   concat

Examples
--------

**Basic Operations:**

.. code-block:: bulang

   arr = [1, 2, 3];
   arr.push(4);           // [1, 2, 3, 4]
   last = arr.pop();      // 4, arr is [1, 2, 3]
   first = arr.first();   // 1

**Searching:**

.. code-block:: bulang

   arr = [10, 20, 30];
   has = arr.contains(20);    // true
   index = arr.find(30);      // 2
   index = arr.find(99);      // -1

**Manipulation:**

.. code-block:: bulang

   arr = [1, 2, 3];
   arr.insert(1, 99);     // [1, 99, 2, 3]
   arr.remove(2);         // [1, 99, 3]
   arr.reverse();         // [3, 99, 1]

**Chaining:**

.. code-block:: bulang

   arr = [1, 2, 3];
   arr.push(4).push(5).reverse();  // [5, 4, 3, 2, 1]