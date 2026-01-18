slice
=====

Create new buffer with copied data.

.. code-block:: bulang

    buf = @(100, BUFFER_UINT8);
    // ... fill data ...

    // Extract middle portion
    sub = buf.slice(25, 75);  // New buffer with 50 elements

