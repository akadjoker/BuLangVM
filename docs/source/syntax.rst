BuLang Syntax
=============

Complete language syntax reference.

Variables
---------

.. code-block:: bulang

   var name = "Alice";
   var age = 30;
   var score = 95.5;
   var active = true;
   var nothing = nil;

Data Types
----------

.. code-block:: bulang

   // Numbers
   var integer = 42;
   var hex = 0xFF;
   var binary = 0b1010;
   var float = 3.14;

   // Strings
   var text = "Hello World";

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
       name: "Hero",
       hp: 100,
       level: 5
   };

Operators
---------

Arithmetic
^^^^^^^^^^

.. code-block:: bulang

   a + b    // Addition
   a - b    // Subtraction
   a * b    // Multiplication
   a / b    // Division
   a % b    // Modulo
   -a       // Negation

Comparison
^^^^^^^^^^

.. code-block:: bulang

   a == b   // Equal
   a != b   // Not equal
   a < b    // Less than
   a > b    // Greater than
   a <= b   // Less or equal
   a >= b   // Greater or equal

Logical
^^^^^^^

.. code-block:: bulang

   !a       // NOT
   a && b   // AND
   a || b   // OR

Control Flow
------------

If Statement
^^^^^^^^^^^^

.. code-block:: bulang

   if (score > 90) 
   {
       print("Excellent!");
   } elif (score > 70) 
   {
       print("Good!");
   } else 
   {
       print("Keep trying!");
   }

While Loop
^^^^^^^^^^

.. code-block:: bulang

   i = 0;
   while (i < 5) 
   {
       print(i);
       i = i + 1;
   }

   i = 0;
   do
   {
      print(i);
       i = i + 1;
    }while(i<5);    

For Loop
^^^^^^^^

.. code-block:: bulang

   for (i = 0; i < 10; i = i + 1)
   {
       print(i);
   }


For-each loop
^^^^^^^^^^^^^

.. code-block:: bulang

   forecah(item in [1, 2, 3, 4, 5])
   {
       print(item);
   }


Switch/Case
^^^^^^^^^^^

.. code-block:: bulang

   switch (value) 
   {
       case 1:
           print("One");
        
       case 2:
           print("Two");
         
       default:
           print("Other");
   }

Break and Continue
^^^^^^^^^^^^^^^^^^

.. code-block:: bulang

   // Break - exit loop
   for (var i=0;i<10;i++)
   {
       if (i == 5) break;
       print(i);
   }

   // Continue - skip iteration
   for (var i=0;i<10;i++)
   {
       if (i % 2 == 0) continue;
       print(i);  // Only odd numbers
   }

Functions
---------

Basic Function
^^^^^^^^^^^^^^

.. code-block:: bulang

   def soma(a, b) 
   {
       return a + b;
   }

   result = soma(10, 20);  // 30

Recursive Function
^^^^^^^^^^^^^^^^^^

.. code-block:: bulang

   def factorial(n) 
   {
       if (n <= 1) return 1;
       return n * factorial(n - 1);
   }

   result = factorial(5);  // 120


Classes
-------

Basic Class
^^^^^^^^^^^

.. code-block:: bulang

   class Animal 
   {
       var nome;

       def init(n) 
       {
           self.nome = n;
       }

       def som() 
       {
           return "nada";
       }
   }

   bicho = Animal("Rex");
   print(bicho.nome);  // "Rex"

Inheritance
^^^^^^^^^^^

.. code-block:: bulang

   class Cao : Animal 
   {
       def init(n) 
       {
           super.init(n);
       }

       def som() 
       {
           return "au au";
       }
   }

   cao = Cao("Bobby");
   print(cao.som());  // "au au"

Processes
------------------------

Basic Process
^^^^^^^^^^^^^

.. code-block:: bulang

   import raylib;

   process player() 
   {
       var x = 100;
       var y = 100;

       loop
       {
           if (IsKeyDown(KEY_RIGHT)) { x += 5; }
           if (IsKeyDown(KEY_LEFT)) { x -= 5; }

           DrawRectangle(x, y, 50, 50, RED);
           frame;  // Wait for next frame
       }
   }

   InitWindow(800, 600, "Game");
   player();  // Start process

Multiple Processes
^^^^^^^^^^^^^^^^^^

.. code-block:: bulang

   process enemy(x, y) 
   {
       loop
       {
           x += 2;
           DrawCircle(x, y, 10, RED);
           frame;
       }
   }

   enemy(100, 200);
   enemy(200, 300);
   enemy(300, 400);

Fibers
------

.. code-block:: bulang

   def worker() 
   {
       for (var i=0;i<10;i++)
       {
           write("Working: {}", i);
           yield;  // Pause and return control
       }
   }

   process player()
   {
    fiber worker;
   }

Closures
--------

.. code-block:: bulang

   def makeCounter() 
   {
       var count = 0;

       def inc() 
       {
           count = count + 1;
           return count;
       };

       return inc;
   }

   counter = makeCounter();
   print(counter());  // 1
   print(counter());  // 2
   print(counter());  // 3

Exception Handling
------------------

Try-Catch
^^^^^^^^^

.. code-block:: bulang

   try 
   {
       file = open("data.txt", "r");
       data = file.read();
       file.close();
   } catch (e) 
   {
       write("Error: {}", e);
   }

Throw Exception
^^^^^^^^^^^^^^^

.. code-block:: bulang

   def divide(a, b) 
   {
       if (b == 0) 
       {
           throw "Division by zero!";
       }
       return a / b;
   }

   try 
   {
       result = divide(10, 0);
   } catch (e) 
   {
       print("Caught: {}", e);
   }

Modules
-------

Import Module
^^^^^^^^^^^^^

.. code-block:: bulang

   // Import with namespace
   import math;
   result = math.sqrt(25);

   // Import multiple
   import time, file, socket;

Using (Flat Import)
^^^^^^^^^^^^^^^^^^^

.. code-block:: bulang

   // Import without namespace
   using math;
   result = sqrt(25);  // Direct access

Comments
--------

.. code-block:: bulang

   // Single line comment

   // Also single line

   /* Multi-line
      comment block */

   var x = 10;  // Inline comment

Arrays
------

.. code-block:: bulang

   // Create
   arr = [1, 2, 3, 4, 5];

   // Access
   first = arr[0];
   last = arr[-1];

   // Modify
   arr[0] = 10;

   // Methods
   arr.push(6);
   last = arr.pop();
   arr.reverse();

Maps
----

.. code-block:: bulang

   // Create
   player = {
       name: "Hero",
       hp: 100,
       level: 5
   };

   // Access
   name = player["name"];
   hp = player.hp;  // Alternative syntax

   // Modify
   player["score"] = 1000;
   player.level = 6;

   // Check key
   if (player.has("hp")) {
       print("HP: {}", player.hp);
   }

String Operations
-----------------

.. code-block:: bulang

   // Concatenation
   full = "Hello" + " " + "World";

   // Methods
   upper = text.upper();
   lower = text.lower();
   parts = text.split(",");
   trimmed = text.trim();

   // Format
   msg = format("Score: {}, Time: {}s", 100, 45.5);

