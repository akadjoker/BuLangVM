recvfrom
========

Receive UDP packet.

.. code-block:: bulang

    packet = socket.recvfrom(udp, 1024);
    if packet != nil 
    {
        write("From: {}:{}", packet["host"], packet["port"]);
        write("Data: {}", packet["data"]);
    }

