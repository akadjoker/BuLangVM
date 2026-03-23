# Neural Network Module

```bulang
import nn;
```

Neural network library with MLP and CNN support, built on MiniDNN/Eigen.

## Activation Functions

| Function | Arguments | Returns | Description |
|----------|-----------|---------|-------------|
| `sigmoid` | `x: number` | `number` | σ(x) = 1/(1+e^(-x)) |
| `relu` | `x: number` | `number` | max(0, x) |
| `leaky_relu` | `x: number`, `alpha?: number` | `number` | max(αx, x) |
| `elu` | `x: number`, `alpha?: number` | `number` | x if x>0, else α(e^x-1) |
| `swish` | `x: number` | `number` | x · sigmoid(x) |
| `silu` | `x: number` | `number` | x · sigmoid(x) (alias) |
| `gelu` | `x: number` | `number` | 0.5x(1+tanh(√(2/π)(x+0.044715x³))) |
| `softplus` | `x: number` | `number` | ln(1+e^x) |
| `mish` | `x: number` | `number` | x · tanh(softplus(x)) |
| `step` | `x: number` | `number` | 0 if x<0, else 1 |

## Derivatives

| Function | Arguments | Returns | Description |
|----------|-----------|---------|-------------|
| `sigmoid_d` | `x: number` | `number` | σ(x)(1-σ(x)) |
| `relu_d` | `x: number` | `number` | 0 if x<0, else 1 |
| `tanh_d` | `x: number` | `number` | 1-tanh²(x) |

## Loss Functions

| Function | Arguments | Returns | Description |
|----------|-----------|---------|-------------|
| `mse` | `predicted: array`, `actual: array` | `number` | Mean Squared Error |
| `bce` | `predicted: array`, `actual: array` | `number` | Binary Cross-Entropy |

## Data Utilities

| Function | Arguments | Returns | Description |
|----------|-----------|---------|-------------|
| `normalize` | `data: array`, `min`, `max` | `array` | Normalize to [0,1] |
| `denormalize` | `data: array`, `min`, `max` | `array` | Reverse normalization |

## Image Functions

| Function | Arguments | Returns | Description |
|----------|-----------|---------|-------------|
| `loadImage` | `path: string`, `channels?: int` | `Float64Array` | Load image as flat array |
| `saveImage` | `path`, `data`, `w`, `h`, `channels` | `bool` | Save array as image |
| `loadImageData` | `path: string` | `map` | Load image with metadata |
| `getImageData` | `data`, `w`, `h`, `channels` | `map` | Create image data map |

### Image Data Format

```bulang
{
    data: Float64Array,  // Pixel values [0,255]
    width: int,
    height: int,
    channels: int        // 1=gray, 3=RGB, 4=RGBA
}
```

## Network Class

### Constructor

```bulang
var net = nn.Network();
```

### Layer Methods

| Method | Arguments | Description |
|--------|-----------|-------------|
| `input` | `size: int` | Set input layer size |
| `add` | `size: int`, `activation: string` | Add dense layer |
| `addConv2D` | `w, h, channels, filters, kernelW, kernelH` | Add conv layer |
| `addMaxPool` | `w, h, channels, poolW, poolH` | Add pooling layer |
| `flatSize` | `w, h, channels` | Calculate flattened size |

### Training Methods

| Method | Arguments | Description |
|--------|-----------|-------------|
| `compile` | `loss`, `optimizer`, `learningRate` | Configure training |
| `fit` | `x, y, epochs, batchSize, verbose?` | Train network |
| `predict` | `input: array` | Get prediction |

### Info Methods

| Method | Arguments | Returns |
|--------|-----------|---------|
| `layers` | none | `array` of layer info |
| `summary` | none | Prints architecture |

### Persistence

| Method | Arguments | Description |
|--------|-----------|-------------|
| `save` | `path: string` | Save weights to file |
| `load` | `path: string` | Load weights from file |

## Supported Options

**Activations:** `"sigmoid"`, `"relu"`, `"tanh"`, `"softmax"`, `"identity"`

**Loss:** `"mse"`, `"crossentropy"`

**Optimizer:** `"sgd"`, `"adam"`, `"rmsprop"`

## Examples

### XOR Network (MLP)

```bulang
import nn;

var net = nn.Network();
net.input(2);
net.add(4, "relu");
net.add(1, "sigmoid");
net.compile("mse", "adam", 0.1);

// Training data
var x = [[0,0], [0,1], [1,0], [1,1]];
var y = [[0], [1], [1], [0]];

net.fit(x, y, 1000, 4, true);

// Test
print(net.predict([0, 1]));  // ~1
print(net.predict([1, 1]));  // ~0
```

### CNN for Images

```bulang
import nn;

var net = nn.Network();

// Input: 28x28 grayscale
net.addConv2D(28, 28, 1, 8, 3, 3);    // -> 26x26x8
net.addMaxPool(26, 26, 8, 2, 2);       // -> 13x13x8

net.addConv2D(13, 13, 8, 16, 3, 3);   // -> 11x11x16
net.addMaxPool(11, 11, 16, 2, 2);      // -> 5x5x16

// Flatten and dense
var flatSize = net.flatSize(5, 5, 16); // 400
net.add(64, "relu");
net.add(10, "softmax");

net.compile("crossentropy", "adam", 0.001);
net.summary();

// Train (x: list of flat images, y: one-hot labels)
net.fit(trainX, trainY, 10, 32, true);

// Save trained model
net.save("model.bin");
```

### Loading Images

```bulang
import nn;

// Load grayscale 28x28
var img = nn.loadImage("digit.png", 1);
// img is Float64Array of 784 values

// Load with metadata
var imgData = nn.loadImageData("photo.png");
print("Size: " + str(imgData.width) + "x" + str(imgData.height));
print("Channels: " + str(imgData.channels));

// Normalize for network input
var normalized = nn.normalize(img.toArray(), 0, 255);
```

### Full Training Pipeline

```bulang
import nn;
import fs;

// Prepare data
var data = load_training_data();
var trainX = data.images;
var trainY = data.labels;

// Create model
var model = nn.Network();
model.input(784);
model.add(128, "relu");
model.add(64, "relu");
model.add(10, "softmax");
model.compile("crossentropy", "adam", 0.001);

// Train with progress
model.fit(trainX, trainY, 20, 32, true);

// Evaluate
var correct = 0;
for (var i = 0; i < len(testX); i++) {
    var pred = model.predict(testX[i]);
    if (argmax(pred) == argmax(testY[i])) {
        correct = correct + 1;
    }
}
print("Accuracy: " + str(correct / len(testX) * 100) + "%");

// Save
model.save("trained_model.bin");
```

## Notes

- Input data should be normalized [0,1] for best results
- Use `softmax` output with `crossentropy` for classification
- Use `sigmoid` output with `mse` for binary classification
- CNN requires flattening before dense layers
- `fit()` with verbose=true shows epoch progress
- Models are saved in binary format (weights only)
