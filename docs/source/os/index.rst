Operating System
================

OS-level operations for process management, environment variables, and system interaction.

System Information
------------------

Query system and platform details.

.. list-table::
   :widths: 20 80
   :header-rows: 1

   * - Property/Function
     - Description
   * - :doc:`os.platform`
     - Current platform string (windows, linux, macos, unix, android, unknown)
   * - :doc:`getcwd`
     - Get current working directory
   * - :doc:`getenv`
     - Get environment variable

.. toctree::
   :hidden:
   
   os.platform
   getcwd
   getenv

Environment & Directory
-----------------------

Modify environment and navigate directories.

.. list-table::
   :widths: 20 80
   :header-rows: 1

   * - Function
     - Description
   * - :doc:`setenv`
     - Set environment variable
   * - :doc:`chdir`
     - Change working directory

.. toctree::
   :hidden:
   
   setenv
   chdir

Process Execution
-----------------

Execute commands and manage processes.

.. list-table::
   :widths: 20 80
   :header-rows: 1

   * - Function
     - Description
   * - :doc:`execute`
     - Execute command synchronously (blocks until complete)
   * - :doc:`spawn`
     - Spawn process asynchronously (returns process ID)
   * - :doc:`spawn_shell`
     - Execute command via system shell
   * - :doc:`spawn_capture`
     - Execute and capture output
   * - :doc:`wait`
     - Wait for process to complete (returns exit code)
   * - :doc:`kill`
     - Terminate process

.. toctree::
   :hidden:
   
   execute
   spawn
   spawn_shell
   spawn_capture
   wait
   kill

Examples
--------

**System Information:**

.. code-block:: bulang

   // Check platform
   if (os.platform == "windows") 
   {
       print("Running on Windows");
   }
   
   // Get current directory
   cwd = os.getcwd();
   write("Working directory: {}", cwd);
   
   // Get environment variable
   home = os.getenv("HOME");
   path = os.getenv("PATH");

**Directory Navigation:**

.. code-block:: bulang

   // Change directory
   os.chdir("/home/user/project");
   
   // Verify
   write("Now in: {}", os.getcwd());

**Environment Variables:**

.. code-block:: bulang

   // Set environment variable
   os.setenv("MY_VAR", "value");
   os.setenv("DEBUG", "true");
   
   // Read it back
   debug = os.getenv("DEBUG");

**Synchronous Execution:**

.. code-block:: bulang

   // Execute and wait (blocks)
   exitCode = os.execute("ls -la");
   
   if (exitCode == 0) 
   {
       print("Command succeeded");
   } else 
   {
       write("Command failed: {}", exitCode);
   }

**Asynchronous Execution:**

.. code-block:: bulang

   // Start process without waiting
   pid = os.spawn("python", "script.py", "--verbose");
   write("Started process {}", pid);
   
   // Do other work...
   
   // Wait for completion
   exitCode = os.wait(pid);
   write("Process finished with code {}", exitCode);

**Capture Output:**

.. code-block:: bulang

   // Execute and get output
   result = os.spawn_capture("git status");
   
   write("Output: {}", result["output"]);
   write("Exit code: {}", result["code"]);
   
   if (result["code"] == 0) 
   {
       // Parse output...
   }

**Shell Commands:**

.. code-block:: bulang

   // Use shell features (pipes, etc.)
   pid = os.spawn_shell("echo 'Hello' | grep H");
   os.wait(pid);

**Build System Example:**

.. code-block:: bulang

   // Change to project directory
   os.chdir("/home/user/myproject");
   
   // Run build
   print("Building...");
   result = os.spawn_capture("make all");
   
   if (result["code"] == 0) 
   {
       print("Build successful!");
       print(result["output"]);
   } else 
   {
       print("Build failed!");
       print(result["output"]);
   }

**Process Management:**

.. code-block:: bulang

   // Start background process
   pid = os.spawn("server", "--port", "8080");
   print("Server started: {}", pid);
   
   // Later, if needed...
   // os.kill(pid);

**Cross-Platform Path:**

.. code-block:: bulang

   if (os.platform == "windows") 
   {
       os.chdir("C:\\Projects\\MyGame");
   } else 
   {
       os.chdir("/home/user/projects/mygame");
   }