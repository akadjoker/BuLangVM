fill
====

Fill buffer with value.

.. code-block:: bulang

    buf = @(10, BUFFER_UINT8);
    buf.fill(255);  // All bytes = 0xFF

    buf = @(10, BUFFER_FLOAT);
    buf.fill(3.14);  // All floats = 3.14

