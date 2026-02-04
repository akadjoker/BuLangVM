info
====

Get socket information.

.. code-block:: bulang

    info = socket.info(sock);
    write("Type: {}", info["type"]);        // "tcp_client", "tcp_server", "udp"
    write("Port: {}", info["port"]);
    write("Connected: {}", info["connected"]);
    write("Blocking: {}", info["blocking"]);

