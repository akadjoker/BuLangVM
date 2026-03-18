BuLang Syntax
=============

Complete language syntax reference.

.. warning::

   **Closures** are limited to **1 level of nesting** for performance reasons.
   A closure can capture variables from its immediate parent scope only.

Variables
---------

.. code-block:: bulang

   var name = "Alice";
   var age = 30;
   var score = 95.5;
   var active = true;
   var nothing = nil;
   
   // Multiple declarations
   var a, b, c;
   var x = 1, y = 2, z = 3;
   
   // Multi-return unpacking
   var (width, height) = getSize();

Data Types
----------

.. code-block:: bulang

   // Integers
   var integer = 42;
   var hex = 0xFF;
   var negative = -7;

   // Floats/Doubles
   var pi = 3.14159;
   var small = 0.001;

   // Strings
   var text = "Hello World";
   var escaped = "Line1\nLine2\tTabbed";
   var verbatim = @"C:\Users\path";    // No escape processing
   var interpolated = f"Name: {name}"; // String interpolation

   // Booleans
   var flag = true;
   var off = false;

   // Nil
   var empty = nil;

   // Arrays
   var numbers = [1, 2, 3, 4, 5];
   var mixed = [1, "two", 3.0, true];

   // Maps
   var player = {
       "name": "Hero",
       "hp": 100,
       "level": 5
   };

Truthy/Falsy Values
-------------------

Only these values are **falsy**:

- ``nil``
- ``false``
- ``0`` (integer zero)

Everything else is **truthy**, including:

- Empty string ``""`` (truthy!)
- Empty array ``[]`` (truthy!)
- Empty map ``{}`` (truthy!)

Operators
---------

Arithmetic
^^^^^^^^^^

.. code-block:: bulang

   a + b      // Addition
   a - b      // Subtraction
   a * b      // Multiplication
   a / b      // Division
   a % b      // Modulo
   -a         // Negation

Comparison
^^^^^^^^^^

.. code-block:: bulang

   a == b     // Equal
   a != b     // Not equal
   a < b      // Less than
   a > b      // Greater than
   a <= b     // Less or equal
   a >= b     // Greater or equal

Logical
^^^^^^^

.. code-block:: bulang

   !a         // NOT
   a && b     // AND (short-circuit)
   a || b     // OR (short-circuit)

Bitwise
^^^^^^^

.. code-block:: bulang

   a & b      // AND
   a | b      // OR
   a ^ b      // XOR
   ~a         // NOT (complement)
   a << n     // Left shift
   a >> n     // Right shift

Increment/Decrement
^^^^^^^^^^^^^^^^^^^

.. code-block:: bulang

   ++i        // Prefix increment
   --i        // Prefix decrement
   i++        // Postfix increment
   i--        // Postfix decrement

Compound Assignment
^^^^^^^^^^^^^^^^^^^

.. code-block:: bulang

   x += 1;    x -= 1;    x *= 2;    x /= 2;    x %= 3;

Ternary Operator
^^^^^^^^^^^^^^^^

.. code-block:: bulang

   var result = condition ? valueIfTrue : valueIfFalse;

Control Flow
------------

If Statement
^^^^^^^^^^^^

.. code-block:: bulang

   if (score > 90) {
       print("Excellent!");
   } elif (score > 70) {
       print("Good!");
   } else {
       print("Keep trying!");
   }

While Loop
^^^^^^^^^^

.. code-block:: bulang

   var i = 0;
   while (i < 5) {
       print(i);
       i++;
   }

Do-While Loop
^^^^^^^^^^^^^

.. code-block:: bulang

   var i = 0;
   do {
       print(i);
       i++;
   } while (i < 5);

For Loop (C-style)
^^^^^^^^^^^^^^^^^^

.. code-block:: bulang

   for (var i = 0; i < 10; i++) {
       print(i);
   }
   
   // Infinite loop
   for (;;) {
       if (done) break;
   }

Foreach Loop
^^^^^^^^^^^^

.. code-block:: bulang

   // Arrays
   foreach (item in [1, 2, 3, 4, 5]) {
       print(item);
   }
   
   // Maps
   foreach (key in myMap) {
       print(key, myMap[key]);
   }
   
   // Strings (iterates characters)
   foreach (char in "hello") {
       print(char);
   }

Loop (Infinite)
^^^^^^^^^^^^^^^

.. code-block:: bulang

   loop {
       // Only exits via break
       if (condition) break;
   }

Switch/Case
^^^^^^^^^^^

.. code-block:: bulang

   switch (value) {
       case 1:
           print("One");
       case 2:
           print("Two");
       default:
           print("Other");
   }

.. note::

   Cases **auto-exit** — there is no fall-through. Do not use ``break``.

Break and Continue
^^^^^^^^^^^^^^^^^^

.. code-block:: bulang

   // Break - exit loop
   for (var i = 0; i < 10; i++) {
       if (i == 5) break;
       print(i);
   }

   // Continue - skip iteration
   for (var i = 0; i < 10; i++) {
       if (i % 2 == 0) continue;
       print(i);  // Only odd numbers
   }

Functions
---------

Basic Function
^^^^^^^^^^^^^^

.. code-block:: bulang

   def add(a, b) {
       return a + b;
   }

   var result = add(10, 20);  // 30

Multiple Return Values
^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: bulang

   def getPosition() {
       return (100, 200);
   }

   var (x, y) = getPosition();

Recursive Function
^^^^^^^^^^^^^^^^^^

.. code-block:: bulang

   def factorial(n) {
       if (n <= 1) return 1;
       return n * factorial(n - 1);
   }

   var result = factorial(5);  // 120

Closures (1 Level Only)
-----------------------

.. warning::

   Closures can only capture variables from their **immediate parent scope**.
   Deeper nesting is not supported for performance reasons.

.. code-block:: bulang

   def makeCounter() {
       var count = 0;
       
       def increment() {
           count++;        // Captures 'count' from parent
           return count;
       }
       
       return increment;
   }

   var counter = makeCounter();
   print(counter());  // 1
   print(counter());  // 2
   print(counter());  // 3

Classes
-------

Class Declaration
^^^^^^^^^^^^^^^^^

.. code-block:: bulang

   class Animal {
       var name;
       var age;
       var sound = "...";   // Default value (literal only)

       def init(name, age) {
           self.name = name;
           self.age = age;
       }

       def speak() {
           print(f"{self.name} says {self.sound}");
       }
   }

   var dog = Animal("Rex", 5);
   dog.speak();

.. note::

   - Use ``self`` to access instance fields and methods
   - ``this`` is an alias for ``self``
   - Field defaults must be literals (int, float, string, bool, nil)
   - Complex initialization must be done in ``init()``

Inheritance (`:` Syntax)
^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: bulang

   class Dog : Animal {
       var breed;

       def init(name, age, breed) {
           super.init(name, age);   // Call parent constructor
           self.breed = breed;
           self.sound = "Woof!";
       }
       
       def fetch() {
           print(f"{self.name} fetches the ball!");
       }
   }

   var buddy = Dog("Buddy", 3, "Labrador");
   buddy.speak();   // "Buddy says Woof!"
   buddy.fetch();   // "Buddy fetches the ball!"

.. warning::

   - Inheritance uses ``:`` (NOT ``<`` or ``extends``)
   - Single inheritance only
   - Use ``super.method()`` to call parent methods

Structs
-------

Lightweight value containers (no methods, no inheritance):

.. code-block:: bulang

   struct Vec2 {
       var x, y;
   }

   struct Color {
       var r, g, b, a;
   }

   // Instantiation (positional arguments)
   var pos = Vec2(100, 200);
   print(pos.x);   // 100
   
   var red = Color(255, 0, 0, 255);

Enums
-----

Named integer constants:

.. code-block:: bulang

   enum Direction {
       Up,        // 0
       Down,      // 1
       Left,      // 2
       Right      // 3
   }
   
   enum HttpStatus {
       OK = 200,
       NotFound = 404,
       Error = 500
   }
   
   // Access
   var dir = Direction.Up;
   print(HttpStatus.NotFound);  // 404

Arrays
------

.. code-block:: bulang

   // Create
   var arr = [1, 2, 3, 4, 5];
   var empty = [];

   // Access (0-indexed, negative from end)
   var first = arr[0];
   var last = arr[-1];

   // Modify
   arr[0] = 10;

   // Methods
   arr.push(6);           // Add to end
   var x = arr.pop();     // Remove and return last
   arr.insert(0, 99);     // Insert at index
   arr.remove(0);         // Remove at index
   arr.reverse();         // Reverse in place
   arr.sort();            // Sort in place
   arr.clear();           // Remove all elements
   
   // Info
   var size = arr.length();
   var idx = arr.indexOf(3);
   var has = arr.contains(5);

Maps
----

.. code-block:: bulang

   // Create
   var player = {
       "name": "Hero",
       "hp": 100,
       "level": 5
   };

   // Access (both syntaxes work)
   var name = player["name"];
   var hp = player.hp;

   // Modify
   player["score"] = 1000;
   player.level = 6;

   // Methods
   var hasHp = player.has("hp");
   var keys = player.keys();
   var vals = player.values();
   player.remove("score");
   player.clear();

Buffers
-------

Typed binary buffers for efficient data storage:

.. code-block:: bulang

   // Create buffer: @(count, type)
   // Types: "byte", "int", "float", "double"
   
   var bytes = @(100, "byte");      // 100 bytes
   var ints = @(50, "int");         // 50 integers
   var floats = @(100, "float");    // 100 floats
   var doubles = @(100, "double");  // 100 doubles

   // Access
   bytes[0] = 255;
   var val = ints[10];

   // Methods
   var size = bytes.size();
   bytes.fill(0);                   // Fill with value
   bytes.resize(200);               // Resize buffer

Strings
-------

.. code-block:: bulang

   // Regular string
   var text = "Hello World";
   
   // Escape sequences
   var escaped = "Line1\nLine2\tTabbed";
   
   // Verbatim string (no escapes)
   var path = @"C:\Users\name\file.txt";
   
   // Interpolated string
   var name = "Alice";
   var greeting = f"Hello, {name}!";
   var math = f"2 + 2 = {2 + 2}";
   
   // Methods
   var upper = text.upper();
   var lower = text.lower();
   var trimmed = text.trim();
   var parts = text.split(" ");
   var idx = text.indexof("World");
   var starts = text.startswith("Hello");
   var ends = text.endswith("World");
   var sub = text.sub(0, 5);
   var replaced = text.replace("World", "BuLang");

Processes (Coroutines)
----------------------

Game-oriented concurrency primitive:

.. code-block:: bulang

   process Enemy(startX, startY) {
       // Private variables auto-bind from parameters
       x = startX;
       y = startY;
       
       loop {
           // Game logic
           x += velx;
           y += vely;
           
           DrawCircle(x, y, 10, RED);
           
           frame;  // Yield until next tick
       }
   }

   // Spawn multiple instances
   Enemy(100, 200);
   Enemy(200, 300);
   Enemy(300, 400);

Process Keywords
^^^^^^^^^^^^^^^^

.. code-block:: bulang

   frame;         // Yield for one frame (100%)
   frame(50);     // Yield for 50% of a frame
   exit;          // Terminate process
   exit(1);       // Terminate with error code

Exception Handling
------------------

Try-Catch-Finally
^^^^^^^^^^^^^^^^^

.. code-block:: bulang

   try {
       var data = riskyOperation();
   } catch (e) {
       print(f"Error: {e}");
   } finally {
       cleanup();   // Always runs
   }

Throw Exception
^^^^^^^^^^^^^^^

.. code-block:: bulang

   def divide(a, b) {
       if (b == 0) {
           throw "Division by zero!";
       }
       return a / b;
   }

   try {
       var result = divide(10, 0);
   } catch (e) {
       print(f"Caught: {e}");
   }

Modules
-------

Import
^^^^^^

.. code-block:: bulang

   // Import with namespace
   import math;
   var x = math.lerp(0, 100, 0.5);

   // Import multiple
   import time, file, socket;
   
   // Import all
   import *;

Using (Flat Import)
^^^^^^^^^^^^^^^^^^^

.. code-block:: bulang

   import math;
   using math;
   
   // Now access without prefix
   var x = lerp(0, 100, 0.5);
   var pi = PI;

Include
^^^^^^^

.. code-block:: bulang

   // Inline another source file
   include "utils.bu";

Comments
--------

.. code-block:: bulang

   // Single line comment

   /* Multi-line
      comment block */

   var x = 10;  // Inline comment

Built-in Math Functions
-----------------------

These are keywords (no import needed):

.. code-block:: bulang

   sin(x)       cos(x)       tan(x)
   asin(x)      acos(x)      atan(x)
   atan2(y, x)
   sqrt(x)      pow(x, y)    exp(x)
   log(x)       abs(x)
   floor(x)     ceil(x)
   deg(x)       rad(x)

Built-in Functions
------------------

.. code-block:: bulang

   print(args...)    // Print values
   len(value)        // Length of array/string/map/buffer
   free(value)       // Mark for garbage collection
   clock()           // High-resolution CPU time
   _gc()             // Force garbage collection cycle

Labels and Goto
---------------

.. code-block:: bulang

   // Labels
   myLabel:
       print("at label");
   
   // Goto (unconditional jump)
   goto myLabel;
   
   // Gosub (call and return)
   gosub mySubroutine;
   print("returned");
   
   mySubroutine:
       print("in subroutine");
       return;   // Returns to after gosub
