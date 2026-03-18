uuid
====

Generate a random UUID v4 string.

Syntax
------

.. code-block:: bulang

   crypto.uuid()

Parameters
----------

None.

Returns
-------

string
   UUID v4 string in format ``xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx``.

Example
-------

.. code-block:: bulang

   import crypto;
   
   var id = crypto.uuid();
   print(id);  // "550e8400-e29b-41d4-a716-446655440000"
   
   // Generate unique IDs for objects
   class Entity {
       var id;
       def init() {
           self.id = crypto.uuid();
       }
   }
   
   var player = Entity();
   print(player.id);

.. note::

   UUID v4 uses random numbers. While collisions are theoretically possible,
   they are extremely unlikely in practice.
