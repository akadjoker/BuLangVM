ftime
=====

Format timestamp as string.

.. code-block:: bulang

    // Default ISO format
    str = time.ftime(time.now());
    // "2025-01-18 14:30:00"

    // Custom format
    str = time.ftime(time.now(), "%Y/%m/%d %H:%M");
    // "2025/01/18 14:30"

