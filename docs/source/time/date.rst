date
====

Decompose timestamp into components.

.. code-block:: bulang

    d = time.date();  // Current time
    // Returns map with: year, month, day, hour, minute, second, weekday, yearday

    print(d["year"]);    // 2025
    print(d["month"]);   // 1-12
    print(d["day"]);     // 1-31
    print(d["weekday"]); // 0-6 (Sunday=0)

