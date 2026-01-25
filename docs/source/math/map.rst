map
===

Remap value from one range to another.

.. code-block:: bulang

    // Map 0-1023 (sensor) to 0-100 (percentage)
    percent = math.map(sensor, 0, 1023, 0, 100);

