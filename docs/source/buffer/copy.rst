copy
====

Copy data from another buffer.

.. code-block:: bulang

    src = @(100, BUFFER_UINT8);
    dst = @(100, BUFFER_UINT8);

    // Copy 50 elements from src[10:] to dst[0:]
    dst.copy(0, src, 10, 50);

