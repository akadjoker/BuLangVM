spawn_capture
=============

Execute and capture output.

.. code-block:: bulang

    result = os.spawn_capture("git status");
    write("Output: {}", result["output"]);
    write("Exit code: {}", result["code"]);

