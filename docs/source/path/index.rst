Path Operations
===============

Cross-platform path manipulation and queries.

Path Parsing
------------

Extract components from paths.

.. list-table::
   :widths: 20 80
   :header-rows: 1

   * - Function
     - Description
   * - :doc:`basename`
     - Get filename portion of path
   * - :doc:`dirname`
     - Get directory portion of path
   * - :doc:`extname`
     - Get file extension (including dot)

.. toctree::
   :hidden:
   
   basename
   dirname
   extname

Path Construction
-----------------

Build and normalize paths.

.. list-table::
   :widths: 20 80
   :header-rows: 1

   * - Function
     - Description
   * - :doc:`join`
     - Join path components (cross-platform)
   * - :doc:`normalize`
     - Normalize path (resolve . and ..)

.. toctree::
   :hidden:
   
   join
   normalize

Path Queries
------------

Check path existence and type.

.. list-table::
   :widths: 20 80
   :header-rows: 1

   * - Function
     - Description
   * - :doc:`exists`
     - Check if path exists
   * - :doc:`isfile`
     - Check if path is regular file
   * - :doc:`isdir`
     - Check if path is directory

.. toctree::
   :hidden:
   
   exists
   isfile
   isdir

Examples
--------

**Parsing Paths:**

.. code-block:: bulang

   fullpath = "/home/user/documents/report.pdf";
   
   // Extract components
   filename = path.basename(fullpath);  // "report.pdf"
   directory = path.dirname(fullpath);  // "/home/user/documents"
   extension = path.extname(fullpath);  // ".pdf"

**Building Paths:**

.. code-block:: bulang

   // Cross-platform join
   savePath = path.join("data", "saves", "player.dat");
   // Windows: "data\saves\player.dat"
   // Unix: "data/saves/player.dat"
   
   // Multiple components
   configPath = path.join("app", "config", "settings.json");

**Normalizing Paths:**

.. code-block:: bulang

   messy = "data/../config/./settings.txt";
   clean = path.normalize(messy);
   // "config/settings.txt"
   
   relative = "./images/../assets/sprite.png";
   normalized = path.normalize(relative);
   // "assets/sprite.png"

**Checking Existence:**

.. code-block:: bulang

   if (path.exists("config.txt")) 
   {
       write("Config found!");
   }
   
   if (path.isfile("data.bin")) 
   {
       write("Data file exists");
   }
   
   if (path.isdir("saves")) 
   {
       write("Saves directory exists");
   }

**File Type Detection:**

.. code-block:: bulang

   filepath = "document.tar.gz";
   ext = path.extname(filepath);  // ".gz"
   
   if (ext == ".txt" || ext == ".md") 
   {
       write("Text file");
   } elif (ext == ".png" || ext == ".jpg") 
   {
       write("Image file");
   }

**Save System Example:**

.. code-block:: bulang

   // Build save path
   saveDir = path.join("data", "saves");
   savePath = path.join(saveDir, "player.sav");
   
   // Ensure directory exists
   if (!path.isdir(saveDir))
    {
       fs.mkdir(saveDir);
   }
   
   // Check if save exists
   if (path.exists(savePath)) 
   {
       write("Loading existing save...");
       // Load game...
   } else
    {
       write("Starting new game...");
       // New game...
   }

**Asset Loading:**

.. code-block:: bulang

   // Build asset paths
   texturePath = path.join("assets", "textures", "player.png");
   soundPath = path.join("assets", "sounds", "jump.wav");
   
   // Verify assets exist
   if (!path.isfile(texturePath))
    {
       write("Missing texture: {}", texturePath);
   }
   
   if (!path.isfile(soundPath)) 
   {
       write("Missing sound: {}", soundPath);
   }

**Configuration Files:**

.. code-block:: bulang

   // Try multiple config locations
   configs = [
       path.join(os.getcwd(), "config.json"),
       path.join(os.getenv("HOME"), ".myapp", "config.json"),
       "/etc/myapp/config.json"
   ];
   
   configPath = nil;
   for (candidate in configs)
    {
       if (path.exists(candidate)) 
       {
           configPath = candidate;
           break;
       }
   }
   
   if (configPath != nil)
    {
       write("Using config: {}", configPath);
   }

**File Processing:**

.. code-block:: bulang

   files = fs.list(".");
   
   for (file in files)
    {
       if (path.isfile(file))
        {
           ext = path.extname(file);
           name = path.basename(file);
           
           write("File: {} (type: {})", name, ext);
       }
   }