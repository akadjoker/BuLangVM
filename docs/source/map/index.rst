Map Methods
===========

Methods for manipulating hash maps (dictionaries) in BuLang.

Query Methods
-------------

Check and retrieve map information.

.. list-table::
   :widths: 20 80
   :header-rows: 1

   * - Method
     - Description
   * - :doc:`has`
     - Check if map contains key
   * - :doc:`keys`
     - Get array of all keys
   * - :doc:`values`
     - Get array of all values

.. toctree::
   :hidden:
   
   has
   keys
   values

Modification Methods
--------------------

Add, remove, and modify map entries.

.. list-table::
   :widths: 20 80
   :header-rows: 1

   * - Method
     - Description
   * - :doc:`remove`
     - Remove key-value pair
   * - :doc:`clear`
     - Remove all key-value pairs

.. toctree::
   :hidden:
   
   remove
   clear

Examples
--------

**Creating and Accessing Maps:**

.. code-block:: bulang

   // Create map
   player = {
       name: "Alice",
       age: 30,
       score: 1000
   };
   
   // Access values
   name = player["name"];    // "Alice"
   age = player.age;         // 30 (alternative syntax)

**Checking Keys:**

.. code-block:: bulang

   map = {name: "Alice", age: 30};
   
   if (map.has("name")) {
       print("Name: {}", map["name"]);
   }
   
   if (!map.has("city")) {
       map["city"] = "NYC";
   }

**Getting Keys and Values:**

.. code-block:: bulang

   map = {name: "Alice", age: 30, city: "NYC"};
   
   // Iterate over keys
   foreach (key in map.keys()) {
       write("{}: {}", key, map[key]);
   }
   
   // Get all values
   vals = map.values();  // ["Alice", 30, "NYC"]

**Modifying Maps:**

.. code-block:: bulang

   map = {a: 1, b: 2, c: 3};
   
   // Add new entry
   map["d"] = 4;
   
   // Remove entry
   map.remove("b");  // {a: 1, c: 3, d: 4}
   
   // Clear all
   map.clear();      // {}

**Inventory System Example:**

.. code-block:: bulang

   inventory = {
       "sword": 1,
       "potion": 5,
       "gold": 100
   };
   
   // Check item
   if (inventory.has("sword")) {
       print("You have a sword!");
   }
   
   // Add item
   if (!inventory.has("shield")) {
       inventory["shield"] = 1;
   }
   
   // Remove item
   inventory.remove("potion");
   
   // List all items
   foreach (item in inventory.keys()) {
       write("{}: {}", item, inventory[item]);
   }

**Player Stats Example:**

.. code-block:: bulang

   stats = {
       hp: 100,
       mp: 50,
       str: 15,
       def: 10
   };
   
   // Get all stat names
   statNames = stats.keys();
   
   // Get all stat values
   statValues = stats.values();
   
   // Print stats
   for (stat in statNames) {
       write("{}: {}", stat, stats[stat]);
   }