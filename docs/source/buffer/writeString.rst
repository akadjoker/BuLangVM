writeString
===========

Write string bytes (no null terminator).

.. code-block:: bulang

    buf = @(100, BUFFER_UINT8);
    buf.writeString("Hello");  // Writes 5 bytes

