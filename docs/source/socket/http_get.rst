http_get
========

Perform HTTP GET request.

.. code-block:: bulang

    // Simple GET
    response = socket.http_get("http://api.example.com/data");

    // With options
    response = socket.http_get("http://api.example.com/users", {
        params: {
            page: 1,
            limit: 10
        },
        headers: {
            "Authorization": "Bearer token123",
            "Accept": "application/json"
        },
        timeout: 30,
        user_agent: "MyApp/1.0"

    });

    // Access response
    if response["success"] {
        print("Status: {}", response["status_code"]);
        print("Body: {}", response["body"]);
        print("Headers: {}", response["headers"]["Content-Type"]);

    }

