receive
=======

Receive data from TCP socket. Returns string or nil.

.. code-block:: bulang

    data = socket.receive(sock, 4096);
    if data != nil 
    {
        write("Received: {}", data);
    }

