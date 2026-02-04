stat
====

Get file/directory information.

.. code-block:: bulang

    info = fs.stat("data.txt");
    if info != nil 
    {
        write("Size: {} bytes", info["size"]);
        write("Is file: {}", info["isfile"]);
        write("Is dir: {}", info["isdir"]);
        // Unix only:
        // write("Mode: {}", info["mode"]);
        // write("Modified: {}", info["mtime"]);

    }

