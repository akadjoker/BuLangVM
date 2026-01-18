http_post
=========

Perform HTTP POST request.

.. code-block:: bulang

    // Form data
    response = socket.http_post("http://api.example.com/login", {
        data: {
            username: "admin",
            password: "secret"
        }

    });

    // JSON data (auto-serialized)
    response = socket.http_post("http://api.example.com/users", {
        json: {
            name: "John Doe",
            email: "john@example.com",
            age: 30
        },
        headers: {
            "Authorization": "Bearer token"
        }

    });

    // Raw string
    response = socket.http_post("http://api.example.com/webhook", {
        data: '{"event": "user.created"}',
        headers: {
            "Content-Type": "application/json"
        }

    });

