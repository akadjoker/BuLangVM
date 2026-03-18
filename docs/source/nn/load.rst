load
====

Load a trained model from a file.

Syntax
------

.. code-block:: bulang

   net.load(path)

Parameters
----------

``path`` : string
   Path to saved model file (``.bunn``).

Example
-------

.. code-block:: bulang

   import nn;
   
   var net = Network();
   net.load("model.bunn");
   
   // Ready for inference
   var result = net.predict([1, 0]);
   print(result);
