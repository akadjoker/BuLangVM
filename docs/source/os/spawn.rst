spawn
=====

Spawn process asynchronously. Returns process ID.

.. code-block:: bulang

    pid = os.spawn("python", "script.py", "--verbose");
    write("Started process {}", pid);

