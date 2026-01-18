wait
====

Wait for process to complete. Returns exit code.

.. code-block:: bulang

    pid = os.spawn("converter", "input.txt");
    exitCode = os.wait(pid);
    if (exitCode == 0)
    {
        print("Success!");

    }

