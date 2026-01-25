tcp_listen
==========

Create TCP server. Returns socket ID.

.. code-block:: bulang

    server = socket.tcp_listen(8080, 10);
    socket.set_blocking(server, false);

    while true 
    {
        client = socket.tcp_accept(server);
        if client != nil 
        {
            print("Client connected!");
        }
    }

