Time Functions
==============

Time measurement, delays, and timestamp manipulation.

Current Time
------------

Get current time in various formats.

.. list-table::
   :widths: 20 80
   :header-rows: 1

   * - Function
     - Description
   * - :doc:`now`
     - Current Unix timestamp (seconds since epoch)
   * - :doc:`now_ms`
     - Current timestamp in milliseconds
   * - :doc:`current`
     - High-precision clock (nanosecond resolution)

.. toctree::
   :hidden:
   
   now
   now_ms
   current

Time Delays
-----------

Pause execution.

.. list-table::
   :widths: 20 80
   :header-rows: 1

   * - Function
     - Description
   * - :doc:`sleep`
     - Pause execution for seconds
   * - :doc:`sleep_ms`
     - Pause execution for milliseconds

.. toctree::
   :hidden:
   
   sleep
   sleep_ms

Time Formatting
---------------

Format and parse timestamps.

.. list-table::
   :widths: 20 80
   :header-rows: 1

   * - Function
     - Description
   * - :doc:`ftime`
     - Format timestamp as string
   * - :doc:`parse`
     - Parse string to timestamp
   * - :doc:`date`
     - Decompose timestamp into components (year, month, day, etc.)

.. toctree::
   :hidden:
   
   ftime
   parse
   date

Time Calculations
-----------------

Calculate time differences.

.. list-table::
   :widths: 20 80
   :header-rows: 1

   * - Function
     - Description
   * - :doc:`diff`
     - Calculate difference between timestamps

.. toctree::
   :hidden:
   
   diff

Examples
--------

**Getting Current Time:**

.. code-block:: bulang

   // Unix timestamp (seconds)
   timestamp = time.now();        // e.g., 1705593600
   
   // Milliseconds
   ms = time.now_ms();            // e.g., 1705593600000
   
   // High-precision (nanoseconds)
   precise = time.current();

**Measuring Elapsed Time:**

.. code-block:: bulang

   start = time.current();
   
   // ... do some work ...
   for (i = 0; i < 1000000; i = i + 1) {
       // computation
   }
   
   elapsed = time.current() - start;
   write("Took {} nanoseconds", elapsed);

**Using Delays:**

.. code-block:: bulang

   write("Starting...");
   time.sleep(2);           // Wait 2 seconds
   write("2 seconds later");
   
   time.sleep(0.5);         // Wait 500ms
   time.sleep_ms(100);      // Wait 100ms exactly

**Formatting Time:**

.. code-block:: bulang

   // Default ISO format
   now = time.now();
   formatted = time.ftime(now);
   // "2025-01-18 14:30:00"
   
   // Custom format
   custom = time.ftime(now, "%Y/%m/%d %H:%M");
   // "2025/01/18 14:30"
   
   custom = time.ftime(now, "%B %d, %Y");
   // "January 18, 2025"

**Parsing Time:**

.. code-block:: bulang

   // Parse date string
   timestamp = time.parse("2025-01-18 14:30:00", "%Y-%m-%d %H:%M:%S");
   
   // Parse different format
   timestamp = time.parse("18/01/2025", "%d/%m/%Y");

**Date Components:**

.. code-block:: bulang

   d = time.date();  // Current time
   
   write("Year: {}", d["year"]);       // 2025
   write("Month: {}", d["month"]);     // 1-12
   write("Day: {}", d["day"]);         // 1-31
   write("Hour: {}", d["hour"]);       // 0-23
   write("Minute: {}", d["minute"]);   // 0-59
   write("Second: {}", d["second"]);   // 0-59
   write("Weekday: {}", d["weekday"]); // 0-6 (Sunday=0)
   write("Yearday: {}", d["yearday"]); // 1-366

**Time Differences:**

.. code-block:: bulang

   start_time = time.now();
   
   // ... do work ...
   time.sleep(5);
   
   end_time = time.now();
   elapsed = time.diff(end_time, start_time);
   
   write("Took {} seconds", elapsed);

**Benchmark Example:**

.. code-block:: bulang

   write("Benchmarking...");
   
   start = time.current();
   
   // Test code
   sum = 0;
   for (i = 0; i < 1000000; i = i + 1) {
       sum = sum + i;
   }
   
   elapsed = time.current() - start;
   seconds = elapsed / 1000000000.0;  // Convert to seconds
   
   write("Took {:.6f} seconds", seconds);

**Timeout System:**

.. code-block:: bulang

   timeout = 10;  // 10 seconds
   start = time.now();
   
   while (true) {
       // Check timeout
       if (time.diff(time.now(), start) > timeout) {
           write("Timeout!");
           break;
       }
       
       // Do work...
       time.sleep(0.1);
   }

**Frame Rate Counter:**

.. code-block:: bulang

   frameCount = 0;
   startTime = time.now();
   
   while (true) {
       // Render frame...
       frameCount = frameCount + 1;
       
       // Calculate FPS every second
       elapsed = time.diff(time.now(), startTime);
       if (elapsed >= 1.0) {
           fps = frameCount / elapsed;
           write("FPS: {:.2f}", fps);
           
           frameCount = 0;
           startTime = time.now();
       }
       
       time.sleep_ms(16);  // ~60 FPS
   }

**Log with Timestamp:**

.. code-block:: bulang

   def log(message) {
       timestamp = time.ftime(time.now(), "%Y-%m-%d %H:%M:%S");
       write("[{}] {}", timestamp, message);
   }
   
   log("Application started");
   log("Loading resources...");
   log("Ready!");

**Stopwatch Class:**

.. code-block:: bulang

   class Stopwatch {
       var startTime;
       var running;
       
       def init() {
           self.startTime = 0;
           self.running = false;
       }
       
       def start() {
           self.startTime = time.current();
           self.running = true;
       }
       
       def stop() {
           self.running = false;
       }
       
       def elapsed() {
           if (!self.running) return 0;
           return (time.current() - self.startTime) / 1000000000.0;
       }
   }
   
   watch = Stopwatch();
   watch.start();
   
   time.sleep(2.5);
   
   write("Elapsed: {:.3f} seconds", watch.elapsed());

**Scheduled Tasks:**

.. code-block:: bulang

   lastUpdate = time.now();
   updateInterval = 5;  // Every 5 seconds
   
   while (true) 
   {
       now = time.now();
       
       if (time.diff(now, lastUpdate) >= updateInterval) 
       {
           write("Running scheduled task...");
           lastUpdate = now;
       }
       
       time.sleep(0.1);
   }