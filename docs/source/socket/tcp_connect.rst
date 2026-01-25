tcp_connect
===========

Connect to TCP server. Returns socket ID.

.. code-block:: bulang

    sock = socket.tcp_connect("example.com", 80);
    if sock != nil 
    {
        socket.send(sock, "GET / HTTP/1.1\r\n\r\n");
        response = socket.receive(sock);
        socket.close(sock);
    }

