Networking
==========

Socket programming, TCP/UDP communication, and HTTP utilities.

System Functions
----------------

Initialize and manage socket system.

.. list-table::
   :widths: 20 80
   :header-rows: 1

   * - Function
     - Description
   * - :doc:`init`
     - Initialize socket system (Windows only, automatic on Unix)
   * - :doc:`quit`
     - Cleanup socket system

.. toctree::
   :hidden:
   
   init
   quit

TCP Sockets
-----------

TCP connection and server management.

.. list-table::
   :widths: 20 80
   :header-rows: 1

   * - Function
     - Description
   * - :doc:`tcp_connect`
     - Connect to TCP server (returns socket ID)
   * - :doc:`tcp_listen`
     - Create TCP server (returns socket ID)
   * - :doc:`tcp_accept`
     - Accept incoming connection (returns client socket or nil)
   * - :doc:`send`
     - Send data on TCP socket (returns bytes sent)
   * - :doc:`receive`
     - Receive data from TCP socket (returns string or nil)
   * - :doc:`close`
     - Close socket connection

.. toctree::
   :hidden:
   
   tcp_connect
   tcp_listen
   tcp_accept
   send
   receive
   close

UDP Sockets
-----------

UDP datagram communication.

.. list-table::
   :widths: 20 80
   :header-rows: 1

   * - Function
     - Description
   * - :doc:`udp_create`
     - Create UDP socket (returns socket ID)
   * - :doc:`sendto`
     - Send UDP packet to specific host/port
   * - :doc:`recvfrom`
     - Receive UDP packet (returns data with sender info)

.. toctree::
   :hidden:
   
   udp_create
   sendto
   recvfrom

Socket Configuration
--------------------

Configure socket behavior.

.. list-table::
   :widths: 20 80
   :header-rows: 1

   * - Function
     - Description
   * - :doc:`set_blocking`
     - Set blocking/non-blocking mode
   * - :doc:`set_nodelay`
     - Set TCP_NODELAY (disable Nagle's algorithm for low latency)

.. toctree::
   :hidden:
   
   set_blocking
   set_nodelay

Socket Information
------------------

Query socket state and properties.

.. list-table::
   :widths: 20 80
   :header-rows: 1

   * - Function
     - Description
   * - :doc:`info`
     - Get socket information (type, port, connected, blocking)
   * - :doc:`resolve`
     - Resolve hostname to IP address
   * - :doc:`is_connected`
     - Check if socket is connected
   * - :doc:`get_local_ip`
     - Get local machine IP address

.. toctree::
   :hidden:
   
   info
   resolve
   is_connected
   get_local_ip

HTTP Utilities
--------------

High-level HTTP operations.

.. list-table::
   :widths: 20 80
   :header-rows: 1

   * - Function
     - Description
   * - :doc:`http_get`
     - Perform HTTP GET request
   * - :doc:`http_post`
     - Perform HTTP POST request
   * - :doc:`download_file`
     - Download file to disk

.. toctree::
   :hidden:
   
   http_get
   http_post
   download_file

Network Utilities
-----------------

Additional networking tools.

.. list-table::
   :widths: 20 80
   :header-rows: 1

   * - Function
     - Description
   * - :doc:`ping`
     - Check if host is reachable

.. toctree::
   :hidden:
   
   ping

Examples
--------

**TCP Client:**

.. code-block:: bulang

   // Connect to server
   sock = socket.tcp_connect("example.com", 80);
   
   if (sock != nil) {
       // Send HTTP request
       socket.send(sock, "GET / HTTP/1.1\r\nHost: example.com\r\n\r\n");
       
       // Receive response
       response = socket.receive(sock, 4096);
       if (response != nil) {
           print(response);
       }
       
       // Close connection
       socket.close(sock);
   }

**TCP Server:**

.. code-block:: bulang

   // Create server on port 8080
   server = socket.tcp_listen(8080, 10);
   socket.set_blocking(server, false);
   
   print("Server listening on port 8080");
   
   while (true) {
       // Check for new connections
       client = socket.tcp_accept(server);
       
       if (client != nil) {
           print("Client connected!");
           
           // Handle client
           data = socket.receive(client, 1024);
           if (data != nil) {
               print("Received: {}", data);
               socket.send(client, "Hello, Client!");
           }
           
           socket.close(client);
       }
       
       // Small delay
       time.sleep(0.01);
   }

**UDP Communication:**

.. code-block:: bulang

   // Create UDP socket on port 5000
   udp = socket.udp_create(5000);
   socket.set_blocking(udp, false);
   
   // Send packet
   socket.sendto(udp, "Hello", "192.168.1.100", 5001);
   
   // Receive packets
   while (true) {
       packet = socket.recvfrom(udp, 1024);
       
       if (packet != nil) {
           print("From: {}:{}", packet["host"], packet["port"]);
           print("Data: {}", packet["data"]);
       }
       
       time.sleep(0.01);
   }

**HTTP GET Request:**

.. code-block:: bulang

   // Simple GET
   response = socket.http_get("https://api.example.com/data");
   
   if (response["success"]) {
       print("Status: {}", response["status_code"]);
       print("Body: {}", response["body"]);
   }
   
   // With options
   response = socket.http_get("https://api.example.com/users", {
       params: {page: 1, limit: 10},
       headers: {"Authorization": "Bearer token123"},
       timeout: 30
   });

**HTTP POST Request:**

.. code-block:: bulang

   // JSON data
   response = socket.http_post("https://api.example.com/users", {
       json: {
           name: "John Doe",
           email: "john@example.com"
       },
       headers: {
           "Authorization": "Bearer token"
       }
   });
   
   if (response["success"]) {
       print("User created!");
   }

**Download File:**

.. code-block:: bulang

   success = socket.download_file(
       "https://example.com/file.zip",
       "./downloads/file.zip"
   );
   
   if (success) {
       print("Download complete!");
   }

**Network Utilities:**

.. code-block:: bulang

   // Check if host is reachable
   if (socket.ping("google.com", 80, 2)) {
       print("Host is up!");
   }
   
   // Get local IP
   myIp = socket.get_local_ip();
   print("My IP: {}", myIp);
   
   // Resolve hostname
   ip = socket.resolve("google.com");
   print("Google IP: {}", ip);

**Chat Server Example:**

.. code-block:: bulang

   socket.init();
   
   server = socket.tcp_listen(9000, 10);
   socket.set_blocking(server, false);
   clients = [];
   
   print("Chat server running on port 9000");
   
   while (true) {
       // Accept new clients
       client = socket.tcp_accept(server);
       if (client != nil) {
           clients.push(client);
           print("Client joined. Total: {}", len(clients));
       }
       
       // Handle messages
       for (i = 0; i < len(clients); i = i + 1) {
           data = socket.receive(clients[i], 1024);
           
           if (data != nil) {
               // Broadcast to all clients
               for (j = 0; j < len(clients); j = j + 1) {
                   socket.send(clients[j], data);
               }
           }
       }
       
       time.sleep(0.01);
   }

**API Client Example:**

.. code-block:: bulang

   // Login
   loginResp = socket.http_post("https://api.game.com/login", {
       json: {
           username: "player1",
           password: "secret"
       }
   });
   
   token = loginResp["body"]["token"];
   
   // Get player stats
   statsResp = socket.http_get("https://api.game.com/stats", {
       headers: {"Authorization": "Bearer " + token}
   });
   
   print("Score: {}", statsResp["body"]["score"]);