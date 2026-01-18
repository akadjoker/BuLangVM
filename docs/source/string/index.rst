String Methods
==============

Methods for string manipulation and processing in BuLang.

Case Conversion
---------------

Change string case.

.. list-table::
   :widths: 20 80
   :header-rows: 1

   * - Method
     - Description
   * - :doc:`upper`
     - Convert to uppercase
   * - :doc:`lower`
     - Convert to lowercase

.. toctree::
   :hidden:
   
   upper
   lower

Search Methods
--------------

Find substrings and check patterns.

.. list-table::
   :widths: 20 80
   :header-rows: 1

   * - Method
     - Description
   * - :doc:`indexOf`
     - Find first occurrence of substring (returns -1 if not found)
   * - :doc:`startsWith`
     - Check if string starts with prefix
   * - :doc:`endsWith`
     - Check if string ends with suffix

.. toctree::
   :hidden:
   
   indexOf
   startsWith
   endsWith

Extraction Methods
------------------

Extract parts of strings.

.. list-table::
   :widths: 20 80
   :header-rows: 1

   * - Method
     - Description
   * - :doc:`at`
     - Get character at index
   * - :doc:`sub`
     - Extract substring from start to end index

.. toctree::
   :hidden:
   
   at
   sub

Transformation Methods
----------------------

Modify and transform strings.

.. list-table::
   :widths: 20 80
   :header-rows: 1

   * - Method
     - Description
   * - :doc:`replace`
     - Replace all occurrences of old with new
   * - :doc:`trim`
     - Remove leading and trailing whitespace
   * - :doc:`repeat`
     - Repeat string count times
   * - :doc:`split`
     - Split string into array by delimiter

.. toctree::
   :hidden:
   
   replace
   trim
   repeat
   split

Examples
--------

**Case Conversion:**

.. code-block:: bulang

   name = "John Doe";
   upper = name.upper();  // "JOHN DOE"
   lower = name.lower();  // "john doe"

**Searching Strings:**

.. code-block:: bulang

   text = "Hello World Hello";
   
   // Find substring
   index = text.indexOf("World");     // 6
   index = text.indexOf("Hello", 1);  // 12 (start from index 1)
   index = text.indexOf("Bye");       // -1 (not found)
   
   // Check prefix/suffix
   if (text.startsWith("Hello")) 
   {
       write("Starts with Hello!");
   }
   
   if (text.endsWith("Hello"))
    {
       write("Ends with Hello!");
   }

**Extracting Substrings:**

.. code-block:: bulang

   text = "Hello World";
   
   // Get characters
   first = text.at(0);    // "H"
   last = text.at(10);    // "d"
   
   // Extract substring
   hello = text.sub(0, 5);   // "Hello"
   world = text.sub(6, 11);  // "World"

**String Transformation:**

.. code-block:: bulang

   // Replace text
   text = "Hello World";
   result = text.replace("World", "BuLang");  // "Hello BuLang"
   
   // Trim whitespace
   messy = "  Hello  ";
   clean = messy.trim();  // "Hello"
   
   // Repeat string
   laugh = "Ha".repeat(3);    // "HaHaHa"
   line = "-".repeat(20);     // "--------------------"

**Splitting Strings:**

.. code-block:: bulang

   // Split by delimiter
   csv = "apple,banana,orange";
   fruits = csv.split(",");  // ["apple", "banana", "orange"]
   
   // Split by space
   sentence = "Hello World";
   words = sentence.split(" ");  // ["Hello", "World"]
   
   // Split into characters
   text = "abc";
   chars = text.split("");  // ["a", "b", "c"]

**CSV Parser Example:**

.. code-block:: bulang

   csvLine = "John,Doe,30,Engineer";
   fields = csvLine.split(",");
   
   firstName = fields[0];   // "John"
   lastName = fields[1];    // "Doe"
   age = fields[2];         // "30"
   job = fields[3];         // "Engineer"
   
   write("{} {} is {} years old", firstName, lastName, age);

**Command Parser:**

.. code-block:: bulang

   input = "  MOVE NORTH  ";
   command = input.trim().lower();
   
   if (command.startsWith("move")) 
   {
       parts = command.split(" ");
       direction = parts[1];
       write("Moving {}", direction);
   }

**URL Builder:**

.. code-block:: bulang

   base = "https://api.example.com";
   endpoint = "/users";
   params = "?page=1&limit=10";
   
   url = base + endpoint + params;
   write(url);

**Text Processing:**

.. code-block:: bulang

   text = "The quick brown fox";
   
   // Find all occurrences of "o"
   index = 0;
   while (true)
    {
       pos = text.indexOf("o", index);
       if (pos == -1) break;
       
       write("Found 'o' at position {}", pos);
       index = pos + 1;
   }

**Template String:**

.. code-block:: bulang

   template = "Hello {name}, you have {count} messages";
   
   result = template.replace("{name}", "Alice");
   result = result.replace("{count}", "5");
   
   write(result);  // "Hello Alice, you have 5 messages"

**File Extension Check:**

.. code-block:: bulang

   filename = "document.txt";
   
   if (filename.endsWith(".txt")) 
   {
       write("Text file");
   } elif (filename.endsWith(".png") || filename.endsWith(".jpg")) 
   {
       write("Image file");
   }

**String Builder:**

.. code-block:: bulang

   // Build a header
   title = "Report";
   width = 20;
   
   line = "=".repeat(width);
   padding = ((width - len(title)) / 2);
   spaces = " ".repeat(padding);
   
   write(line);
   write(spaces + title);
   write(line);