Math Functions
==============

Mathematical functions and utilities for BuLang.

Basic Math
----------

Fundamental mathematical operations.

.. list-table::
   :widths: 20 80
   :header-rows: 1

   * - Function
     - Description
   * - ``abs()``
     - Absolute value
   * - ``sqrt()``
     - Square root
   * - ``pow()``
     - Power (x^y)
   * - ``exp()``
     - Exponential (e^x)
   * - ``log()``
     - Natural logarithm
   * - :doc:`log2`
     - Logarithm base 2
   * - :doc:`log10`
     - Logarithm base 10
   * - ``floor()``
     - Round down to integer
   * - ``ceil()``
     - Round up to integer

.. toctree::
   :hidden:
   
   log2
   log10

Trigonometry
------------

Trigonometric functions (angles in radians).

.. list-table::
   :widths: 20 80
   :header-rows: 1

   * - Function
     - Description
   * - ``sin()``
     - Sine
   * - ``cos()``
     - Cosine
   * - ``tan()``
     - Tangent
   * - ``asin()``
     - Arc sine
   * - ``acos()``
     - Arc cosine
   * - ``atan()``
     - Arc tangent
   * - ``atan2()``
     - Arc tangent of y/x (2 arguments)
   * - ``deg()``
     - Convert radians to degrees
   * - ``rad()``
     - Convert degrees to radians

Hyperbolic Functions
--------------------

Hyperbolic trigonometric functions.

.. list-table::
   :widths: 20 80
   :header-rows: 1

   * - Function
     - Description
   * - :doc:`sinh`
     - Hyperbolic sine
   * - :doc:`cosh`
     - Hyperbolic cosine
   * - :doc:`tanh`
     - Hyperbolic tangent

.. toctree::
   :hidden:
   
   sinh
   cosh
   tanh

Utility Functions
-----------------

Helper functions for common operations.

.. list-table::
   :widths: 20 80
   :header-rows: 1

   * - Function
     - Description
   * - :doc:`min`
     - Return minimum value
   * - :doc:`max`
     - Return maximum value
   * - :doc:`clamp`
     - Constrain value to range
   * - :doc:`sign`
     - Return -1, 0, or 1 based on sign
   * - :doc:`hypot`
     - Calculate hypotenuse (safe from overflow)

.. toctree::
   :hidden:
   
   min
   max
   clamp
   sign
   hypot

Interpolation
-------------

Smooth value transitions and easing functions.

.. list-table::
   :widths: 20 80
   :header-rows: 1

   * - Function
     - Description
   * - :doc:`lerp`
     - Linear interpolation between two values
   * - :doc:`smoothstep`
     - Smooth hermite interpolation
   * - :doc:`smootherstep`
     - Even smoother Ken Perlin interpolation
   * - :doc:`hermite`
     - Hermite curve interpolation

.. toctree::
   :hidden:
   
   lerp
   smoothstep
   smootherstep
   hermite

Range Mapping
-------------

Transform values between ranges.

.. list-table::
   :widths: 20 80
   :header-rows: 1

   * - Function
     - Description
   * - :doc:`map`
     - Remap value from one range to another
   * - :doc:`repeat`
     - Wrap value within range [0, length]
   * - :doc:`ping_pong`
     - Ping-pong value back and forth

.. toctree::
   :hidden:
   
   map
   repeat
   ping_pong

Random Numbers
--------------

Generate random values.

.. list-table::
   :widths: 20 80
   :header-rows: 1

   * - Function
     - Description
   * - :doc:`rand`
     - Generate random float [0.0, 1.0] or within range
   * - :doc:`irand`
     - Generate random integer
   * - :doc:`seed`
     - Set RNG seed for deterministic results

.. toctree::
   :hidden:
   
   rand
   irand
   seed

Examples
--------

**Basic Math:**

.. code-block:: bulang

   // Basic operations
   a = math.abs(-5);           // 5
   b = math.sqrt(16);          // 4.0
   c = math.pow(2, 8);         // 256
   d = math.floor(3.7);        // 3
   e = math.ceil(3.2);         // 4

**Trigonometry:**

.. code-block:: bulang

   angle = math.rad(45);       // Convert 45° to radians
   x = math.cos(angle);
   y = math.sin(angle);
   
   // Arc tangent with two arguments
   direction = math.atan2(dy, dx);
   degrees = math.deg(direction);

**Clamping and Ranges:**

.. code-block:: bulang

   // Keep health between 0 and 100
   health = math.clamp(health, 0, 100);
   
   // Map sensor value to percentage
   percent = math.map(sensor, 0, 1023, 0, 100);
   
   // Get sign of velocity
   dir = math.sign(velocity);  // -1, 0, or 1

**Interpolation:**

.. code-block:: bulang

   // Linear interpolation
   pos = math.lerp(start, end, 0.5);
   
   // Smooth interpolation
   t = math.smoothstep(0, 1, time);
   smooth_pos = math.lerp(start, end, t);

**Random Numbers:**

.. code-block:: bulang

   // Set seed for reproducible results
   math.seed(12345);
   
   // Random float 0.0 to 1.0
   random = math.rand();
   
   // Random float in range
   speed = math.rand(5.0, 10.0);
   
   // Random integer
   dice = math.irand(1, 6);

**Animation:**

.. code-block:: bulang

   // Ping-pong between 0 and 10
   wave = math.ping_pong(time, 10);
   
   // Loop animation 0 to 360
   angle = math.repeat(time * 45, 360);

**Distance Calculation:**

.. code-block:: bulang

   // Safe distance calculation
   dx = x2 - x1;
   dy = y2 - y1;
   distance = math.hypot(dx, dy);