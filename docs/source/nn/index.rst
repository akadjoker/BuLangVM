Neural Networks (nn) Module
===========================

Machine learning and neural network functions.

.. code-block:: bulang

   import nn;

Activation Functions
--------------------

Common neural network activation functions.

.. list-table::
   :widths: 25 75
   :header-rows: 1

   * - Function
     - Description
   * - :doc:`sigmoid`
     - Sigmoid activation: 1 / (1 + exp(-x))
   * - :doc:`relu`
     - ReLU: max(0, x)
   * - :doc:`leaky_relu`
     - Leaky ReLU: x > 0 ? x : alpha * x
   * - :doc:`elu`
     - ELU: x > 0 ? x : alpha * (exp(x) - 1)
   * - :doc:`swish`
     - Swish: x * sigmoid(x)
   * - :doc:`gelu`
     - Gaussian Error Linear Unit
   * - :doc:`mish`
     - Mish: x * tanh(softplus(x))
   * - :doc:`softplus`
     - Softplus: log(1 + exp(x))

.. toctree::
   :hidden:
   
   sigmoid
   relu
   leaky_relu
   elu
   swish
   gelu
   mish
   softplus

Derivatives
-----------

Activation function derivatives for backpropagation.

.. list-table::
   :widths: 25 75
   :header-rows: 1

   * - Function
     - Description
   * - :doc:`sigmoid_d`
     - Sigmoid derivative
   * - :doc:`relu_d`
     - ReLU derivative
   * - :doc:`tanh_d`
     - Tanh derivative

.. toctree::
   :hidden:
   
   sigmoid_d
   relu_d
   tanh_d

Loss Functions
--------------

Loss functions for training.

.. list-table::
   :widths: 25 75
   :header-rows: 1

   * - Function
     - Description
   * - :doc:`mse`
     - Mean Squared Error
   * - :doc:`bce`
     - Binary Cross Entropy

.. toctree::
   :hidden:
   
   mse
   bce

Normalization
-------------

Data normalization functions.

.. list-table::
   :widths: 25 75
   :header-rows: 1

   * - Function
     - Description
   * - :doc:`normalize`
     - Scale value to [0, 1]
   * - :doc:`denormalize`
     - Scale back from [0, 1]

.. toctree::
   :hidden:
   
   normalize
   denormalize

Image I/O
---------

Load and save BMP images for neural network input.

.. list-table::
   :widths: 25 75
   :header-rows: 1

   * - Function
     - Description
   * - :doc:`loadImage`
     - Load BMP image (returns width, height, channels, pixels)
   * - :doc:`saveImage`
     - Save pixel data as BMP image

.. toctree::
   :hidden:
   
   loadImage
   saveImage

Network Class
-------------

Build, train, and use neural networks.

.. list-table::
   :widths: 25 75
   :header-rows: 1

   * - Method
     - Description
   * - :doc:`Network`
     - Create a new neural network
   * - :doc:`add`
     - Add a dense (fully connected) layer
   * - :doc:`input`
     - Set CNN input dimensions
   * - :doc:`addConv2D`
     - Add 2D convolutional layer
   * - :doc:`addMaxPool`
     - Add max pooling layer
   * - :doc:`flatSize`
     - Get flattened size after conv layers
   * - :doc:`compile`
     - Configure optimizer and loss function
   * - :doc:`fit`
     - Train the network
   * - :doc:`predict`
     - Run inference
   * - :doc:`save`
     - Save model to file
   * - :doc:`load`
     - Load model from file
   * - :doc:`summary`
     - Print network architecture
   * - :doc:`layers`
     - Get layer information

.. toctree::
   :hidden:
   
   Network
   add
   input
   addConv2D
   addMaxPool
   flatSize
   compile
   fit
   predict
   save
   load
   summary
   layers

Supported Options
-----------------

.. list-table::
   :widths: 30 70
   :header-rows: 1

   * - Category
     - Options
   * - **Activations**
     - ``relu``, ``sigmoid``, ``tanh``, ``softmax``, ``identity``, ``mish``
   * - **Optimizers**
     - ``adam``, ``sgd``, ``rmsprop``, ``adagrad``
   * - **Loss**
     - ``mse``, ``bce``, ``cross_entropy``

Examples
--------

**MLP (Multi-Layer Perceptron):**

.. code-block:: bulang

   import nn;
   
   var net = Network();
   
   // 2 inputs → 8 hidden → 1 output (XOR problem)
   net.add(2, 8, "tanh");
   net.add(8, 1, "sigmoid");
   
   net.compile("adam", "bce", 0.01);
   
   var X = [[0,0], [0,1], [1,0], [1,1]];
   var Y = [[0], [1], [1], [0]];
   
   net.fit(X, Y, 1000, 4);  // 1000 epochs, batch size 4
   
   var result = net.predict([1, 0]);
   print(result);  // ~[1.0]

**CNN (Convolutional Neural Network):**

.. code-block:: bulang

   import nn;
   
   var net = Network();
   
   // MNIST-style: 28x28 grayscale
   net.input(28, 28, 1);
   net.addConv2D(8, 3, 3, "relu");   // 8 filters 3x3
   net.addMaxPool(2, 2);              // 2x2 pooling
   net.add(net.flatSize(), 64, "relu");
   net.add(64, 10, "softmax");        // 10 classes
   
   net.compile("adam", "cross_entropy", 0.001);
   net.fit(images, labels, 10, 32);
   
   net.save("mnist_cnn.bunn");

.. warning::

   CNN training requires minimum 8x8 input images. Smaller images will cause errors.

**Image Classification:**

.. code-block:: bulang

   import nn;
   
   // Load image
   var (w, h, c, pixels) = nn.loadImage("digit.bmp");
   
   // Load trained model
   var net = Network();
   net.load("classifier.bunn");
   
   // Predict
   var result = net.predict(pixels);
   print("Predicted class:", result);

**Save/Load Model:**

.. code-block:: bulang

   // Save after training
   net.save("model.bunn");
   
   // Load later
   var net2 = Network();
   net2.load("model.bunn");
   var result = net2.predict(input);
