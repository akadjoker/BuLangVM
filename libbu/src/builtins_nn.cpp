#include "interpreter.hpp"

#ifdef BU_ENABLE_NN
#include "platform.hpp"
#include <cmath>
#include <algorithm>

// ============================================
// ACTIVATION FUNCTIONS
// ============================================

// sigmoid(x) = 1 / (1 + exp(-x))
static int native_nn_sigmoid(Interpreter *vm, int argCount, Value *args)
{
    if (argCount != 1 || !args[0].isNumber())
    {
        vm->runtimeError("sigmoid expects 1 number argument");
        return 0;
    }
    double x = args[0].asNumber();
    double result = 1.0 / (1.0 + std::exp(-x));
    vm->push(vm->makeDouble(result));
    return 1;
}

// relu(x) = max(0, x)
static int native_nn_relu(Interpreter *vm, int argCount, Value *args)
{
    if (argCount != 1 || !args[0].isNumber())
    {
        vm->runtimeError("relu expects 1 number argument");
        return 0;
    }
    double x = args[0].asNumber();
    vm->push(vm->makeDouble(x > 0 ? x : 0));
    return 1;
}

// leaky_relu(x, alpha=0.01)
static int native_nn_leaky_relu(Interpreter *vm, int argCount, Value *args)
{
    if (argCount < 1 || !args[0].isNumber())
    {
        vm->runtimeError("leaky_relu expects at least 1 number argument");
        return 0;
    }
    double x = args[0].asNumber();
    double alpha = (argCount >= 2 && args[1].isNumber()) ? args[1].asNumber() : 0.01;
    vm->push(vm->makeDouble(x > 0 ? x : alpha * x));
    return 1;
}

// elu(x, alpha=1.0) = x > 0 ? x : alpha * (exp(x) - 1)
static int native_nn_elu(Interpreter *vm, int argCount, Value *args)
{
    if (argCount < 1 || !args[0].isNumber())
    {
        vm->runtimeError("elu expects at least 1 number argument");
        return 0;
    }
    double x = args[0].asNumber();
    double alpha = (argCount >= 2 && args[1].isNumber()) ? args[1].asNumber() : 1.0;
    vm->push(vm->makeDouble(x > 0 ? x : alpha * (std::exp(x) - 1.0)));
    return 1;
}

// swish(x) = x * sigmoid(x) = x / (1 + exp(-x))
static int native_nn_swish(Interpreter *vm, int argCount, Value *args)
{
    if (argCount != 1 || !args[0].isNumber())
    {
        vm->runtimeError("swish expects 1 number argument");
        return 0;
    }
    double x = args[0].asNumber();
    double result = x / (1.0 + std::exp(-x));
    vm->push(vm->makeDouble(result));
    return 1;
}

// gelu(x) - Gaussian Error Linear Unit (aproximação rápida)
// gelu(x) ≈ 0.5 * x * (1 + tanh(sqrt(2/π) * (x + 0.044715 * x³)))
static int native_nn_gelu(Interpreter *vm, int argCount, Value *args)
{
    if (argCount != 1 || !args[0].isNumber())
    {
        vm->runtimeError("gelu expects 1 number argument");
        return 0;
    }
    double x = args[0].asNumber();
    const double sqrt_2_pi = 0.7978845608028654;  // sqrt(2/π)
    double x3 = x * x * x;
    double inner = sqrt_2_pi * (x + 0.044715 * x3);
    double result = 0.5 * x * (1.0 + std::tanh(inner));
    vm->push(vm->makeDouble(result));
    return 1;
}

// softplus(x) = log(1 + exp(x))
static int native_nn_softplus(Interpreter *vm, int argCount, Value *args)
{
    if (argCount != 1 || !args[0].isNumber())
    {
        vm->runtimeError("softplus expects 1 number argument");
        return 0;
    }
    double x = args[0].asNumber();
    // Numerical stability: for large x, softplus(x) ≈ x
    double result = (x > 20) ? x : std::log(1.0 + std::exp(x));
    vm->push(vm->makeDouble(result));
    return 1;
}

// silu(x) = x * sigmoid(x) (same as swish)
static int native_nn_silu(Interpreter *vm, int argCount, Value *args)
{
    return native_nn_swish(vm, argCount, args);
}

// mish(x) = x * tanh(softplus(x)) = x * tanh(ln(1 + e^x))
static int native_nn_mish(Interpreter *vm, int argCount, Value *args)
{
    if (argCount != 1 || !args[0].isNumber())
    {
        vm->runtimeError("mish expects 1 number argument");
        return 0;
    }
    double x = args[0].asNumber();
    double sp = (x > 20) ? x : std::log(1.0 + std::exp(x));
    double result = x * std::tanh(sp);
    vm->push(vm->makeDouble(result));
    return 1;
}

// ============================================
// DERIVATIVE FUNCTIONS (for backprop)
// ============================================

// sigmoid_derivative(x) = sigmoid(x) * (1 - sigmoid(x))
// If input is already sigmoid output: d = x * (1 - x)
static int native_nn_sigmoid_derivative(Interpreter *vm, int argCount, Value *args)
{
    if (argCount != 1 || !args[0].isNumber())
    {
        vm->runtimeError("sigmoid_derivative expects 1 number argument");
        return 0;
    }
    double x = args[0].asNumber();
    double sig = 1.0 / (1.0 + std::exp(-x));
    double result = sig * (1.0 - sig);
    vm->push(vm->makeDouble(result));
    return 1;
}

// relu_derivative(x) = 1 if x > 0 else 0
static int native_nn_relu_derivative(Interpreter *vm, int argCount, Value *args)
{
    if (argCount != 1 || !args[0].isNumber())
    {
        vm->runtimeError("relu_derivative expects 1 number argument");
        return 0;
    }
    double x = args[0].asNumber();
    vm->push(vm->makeDouble(x > 0 ? 1.0 : 0.0));
    return 1;
}

// tanh_derivative(x) = 1 - tanh²(x)
static int native_nn_tanh_derivative(Interpreter *vm, int argCount, Value *args)
{
    if (argCount != 1 || !args[0].isNumber())
    {
        vm->runtimeError("tanh_derivative expects 1 number argument");
        return 0;
    }
    double x = args[0].asNumber();
    double t = std::tanh(x);
    vm->push(vm->makeDouble(1.0 - t * t));
    return 1;
}

// ============================================
// LOSS FUNCTIONS
// ============================================

// mse(predicted, target) = (predicted - target)²
static int native_nn_mse(Interpreter *vm, int argCount, Value *args)
{
    if (argCount != 2 || !args[0].isNumber() || !args[1].isNumber())
    {
        vm->runtimeError("mse expects 2 number arguments");
        return 0;
    }
    double pred = args[0].asNumber();
    double target = args[1].asNumber();
    double diff = pred - target;
    vm->push(vm->makeDouble(diff * diff));
    return 1;
}

// binary_cross_entropy(predicted, target)
// -[target * log(pred) + (1-target) * log(1-pred)]
static int native_nn_binary_cross_entropy(Interpreter *vm, int argCount, Value *args)
{
    if (argCount != 2 || !args[0].isNumber() || !args[1].isNumber())
    {
        vm->runtimeError("binary_cross_entropy expects 2 number arguments");
        return 0;
    }
    double pred = args[0].asNumber();
    double target = args[1].asNumber();
    
    // Clip to avoid log(0)
    const double eps = 1e-15;
    pred = std::max(eps, std::min(1.0 - eps, pred));
    
    double result = -(target * std::log(pred) + (1.0 - target) * std::log(1.0 - pred));
    vm->push(vm->makeDouble(result));
    return 1;
}

// ============================================
// UTILITY FUNCTIONS
// ============================================

// normalize(x, min, max) -> [0, 1]
static int native_nn_normalize(Interpreter *vm, int argCount, Value *args)
{
    if (argCount != 3)
    {
        vm->runtimeError("normalize expects 3 arguments (x, min, max)");
        return 0;
    }
    double x = args[0].asNumber();
    double min = args[1].asNumber();
    double max = args[2].asNumber();
    
    if (max == min)
    {
        vm->push(vm->makeDouble(0.5));
        return 1;
    }
    
    double result = (x - min) / (max - min);
    vm->push(vm->makeDouble(result));
    return 1;
}

// denormalize(x, min, max) -> original scale
static int native_nn_denormalize(Interpreter *vm, int argCount, Value *args)
{
    if (argCount != 3)
    {
        vm->runtimeError("denormalize expects 3 arguments (x, min, max)");
        return 0;
    }
    double x = args[0].asNumber();
    double min = args[1].asNumber();
    double max = args[2].asNumber();
    
    double result = x * (max - min) + min;
    vm->push(vm->makeDouble(result));
    return 1;
}

// step(x, threshold=0) -> 0 or 1
static int native_nn_step(Interpreter *vm, int argCount, Value *args)
{
    if (argCount < 1 || !args[0].isNumber())
    {
        vm->runtimeError("step expects at least 1 number argument");
        return 0;
    }
    double x = args[0].asNumber();
    double threshold = (argCount >= 2 && args[1].isNumber()) ? args[1].asNumber() : 0.0;
    vm->push(vm->makeDouble(x >= threshold ? 1.0 : 0.0));
    return 1;
}

// ============================================
// MINIDNN NEURAL NETWORK CLASS
// ============================================

#ifdef BU_ENABLE_MINIDNN

#include <MiniDNN.h>
#include <vector>
#include <string>

namespace {

// Activation enum for layer creation
enum class ActivationType { RELU, SIGMOID, TANH, SOFTMAX, IDENTITY, MISH };

// Loss enum
enum class LossType { MSE, BINARY_CROSS_ENTROPY, MULTI_CROSS_ENTROPY };

// Optimizer enum
enum class OptimizerType { SGD, ADAM, RMSPROP, ADAGRAD };

// Layer type enum
enum class LayerType { DENSE, CONV2D, MAXPOOL, FLATTEN };

// Layer info for save/load
struct LayerInfo
{
    LayerType type = LayerType::DENSE;
    int inSize = 0;
    int outSize = 0;
    ActivationType activation = ActivationType::RELU;
    
    // For Conv2D/MaxPool
    int inWidth = 0;
    int inHeight = 0;
    int inChannels = 0;
    int outChannels = 0;
    int filterWidth = 0;
    int filterHeight = 0;
};

// Network wrapper structure
struct NetworkData
{
    MiniDNN::Network net;
    OptimizerType optimizerType = OptimizerType::ADAM;
    LossType lossType = LossType::MSE;
    double learningRate = 0.001;
    bool initialized = false;
    int lastInputSize = 0;
    int lastOutputSize = 0;
    
    // Track current spatial dimensions for CNN
    int currentWidth = 0;
    int currentHeight = 0;
    int currentChannels = 0;
    
    // Layer architecture for save/load
    std::vector<LayerInfo> layerInfos;
    
    // Optimizer instances
    MiniDNN::Adam* adamOpt = nullptr;
    MiniDNN::SGD* sgdOpt = nullptr;
    MiniDNN::RMSProp* rmspropOpt = nullptr;
    MiniDNN::AdaGrad* adagradOpt = nullptr;
    
    ~NetworkData()
    {
        delete adamOpt;
        delete sgdOpt;
        delete rmspropOpt;
        delete adagradOpt;
    }
};

static NetworkData* as_network(void* instance)
{
    return static_cast<NetworkData*>(instance);
}

// Constructor: Network()
static void* network_ctor(Interpreter* vm, int argCount, Value* args)
{
    (void)vm; (void)argCount; (void)args;
    return new NetworkData();
}

// Destructor
static void network_dtor(Interpreter* vm, void* instance)
{
    (void)vm;
    delete as_network(instance);
}

// Helper: parse activation string
static ActivationType parse_activation(const char* str)
{
    if (!str) return ActivationType::RELU;
    if (strcmp(str, "relu") == 0) return ActivationType::RELU;
    if (strcmp(str, "sigmoid") == 0) return ActivationType::SIGMOID;
    if (strcmp(str, "tanh") == 0) return ActivationType::TANH;
    if (strcmp(str, "softmax") == 0) return ActivationType::SOFTMAX;
    if (strcmp(str, "identity") == 0 || strcmp(str, "linear") == 0) return ActivationType::IDENTITY;
    if (strcmp(str, "mish") == 0) return ActivationType::MISH;
    return ActivationType::RELU;
}

// Method: add(in_size, out_size, activation="relu")
// Adds a fully connected layer
static int network_add(Interpreter* vm, void* instance, int argCount, Value* args)
{
    NetworkData* nd = as_network(instance);
    
    if (argCount < 2 || !args[0].isNumber() || !args[1].isNumber())
    {
        vm->runtimeError("add() expects (in_size, out_size, [activation])");
        return 0;
    }
    
    int inSize = (int)args[0].asNumber();
    int outSize = (int)args[1].asNumber();
    
    ActivationType act = ActivationType::RELU;
    if (argCount >= 3 && args[2].isString())
    {
        act = parse_activation(args[2].asString()->chars());
    }
    
    // Track sizes for init
    if (nd->lastInputSize == 0) nd->lastInputSize = inSize;
    nd->lastOutputSize = outSize;
    
    // Store layer info for save/load
    LayerInfo info;
    info.type = LayerType::DENSE;
    info.inSize = inSize;
    info.outSize = outSize;
    info.activation = act;
    nd->layerInfos.push_back(info);
    
    // Create layer with appropriate activation
    switch (act)
    {
        case ActivationType::RELU:
            nd->net.add_layer(new MiniDNN::FullyConnected<MiniDNN::ReLU>(inSize, outSize));
            break;
        case ActivationType::SIGMOID:
            nd->net.add_layer(new MiniDNN::FullyConnected<MiniDNN::Sigmoid>(inSize, outSize));
            break;
        case ActivationType::TANH:
            nd->net.add_layer(new MiniDNN::FullyConnected<MiniDNN::Tanh>(inSize, outSize));
            break;
        case ActivationType::SOFTMAX:
            nd->net.add_layer(new MiniDNN::FullyConnected<MiniDNN::Softmax>(inSize, outSize));
            break;
        case ActivationType::IDENTITY:
            nd->net.add_layer(new MiniDNN::FullyConnected<MiniDNN::Identity>(inSize, outSize));
            break;
        case ActivationType::MISH:
            nd->net.add_layer(new MiniDNN::FullyConnected<MiniDNN::Mish>(inSize, outSize));
            break;
    }
    
    vm->push(vm->makeNil());
    return 1;
}

// Method: input(width, height, channels=1)
// Sets input dimensions for CNN
static int network_input(Interpreter* vm, void* instance, int argCount, Value* args)
{
    NetworkData* nd = as_network(instance);
    
    if (argCount < 2 || !args[0].isNumber() || !args[1].isNumber())
    {
        vm->runtimeError("input() expects (width, height, [channels=1])");
        return 0;
    }
    
    nd->currentWidth = (int)args[0].asNumber();
    nd->currentHeight = (int)args[1].asNumber();
    nd->currentChannels = (argCount >= 3 && args[2].isNumber()) ? (int)args[2].asNumber() : 1;
    nd->lastInputSize = nd->currentWidth * nd->currentHeight * nd->currentChannels;
    
    vm->push(vm->makeNil());
    return 1;
}

// Method: addConv2D(filters, filter_width, filter_height, activation="relu")
// Adds a convolutional layer
static int network_addConv2D(Interpreter* vm, void* instance, int argCount, Value* args)
{
    NetworkData* nd = as_network(instance);
    
    if (nd->currentWidth == 0 || nd->currentHeight == 0)
    {
        vm->runtimeError("addConv2D(): Call input() first to set image dimensions");
        return 0;
    }
    
    if (argCount < 3 || !args[0].isNumber() || !args[1].isNumber() || !args[2].isNumber())
    {
        vm->runtimeError("addConv2D() expects (filters, filter_width, filter_height, [activation])");
        return 0;
    }
    
    int outChannels = (int)args[0].asNumber();
    int filterWidth = (int)args[1].asNumber();
    int filterHeight = (int)args[2].asNumber();
    
    ActivationType act = ActivationType::RELU;
    if (argCount >= 4 && args[3].isString())
    {
        act = parse_activation(args[3].asString()->chars());
    }
    
    // Store layer info
    LayerInfo info;
    info.type = LayerType::CONV2D;
    info.inWidth = nd->currentWidth;
    info.inHeight = nd->currentHeight;
    info.inChannels = nd->currentChannels;
    info.outChannels = outChannels;
    info.filterWidth = filterWidth;
    info.filterHeight = filterHeight;
    info.activation = act;
    nd->layerInfos.push_back(info);
    
    // Create convolutional layer
    switch (act)
    {
        case ActivationType::RELU:
            nd->net.add_layer(new MiniDNN::Convolutional<MiniDNN::ReLU>(
                nd->currentWidth, nd->currentHeight, nd->currentChannels,
                outChannels, filterWidth, filterHeight));
            break;
        case ActivationType::SIGMOID:
            nd->net.add_layer(new MiniDNN::Convolutional<MiniDNN::Sigmoid>(
                nd->currentWidth, nd->currentHeight, nd->currentChannels,
                outChannels, filterWidth, filterHeight));
            break;
        case ActivationType::TANH:
            nd->net.add_layer(new MiniDNN::Convolutional<MiniDNN::Tanh>(
                nd->currentWidth, nd->currentHeight, nd->currentChannels,
                outChannels, filterWidth, filterHeight));
            break;
        case ActivationType::IDENTITY:
            nd->net.add_layer(new MiniDNN::Convolutional<MiniDNN::Identity>(
                nd->currentWidth, nd->currentHeight, nd->currentChannels,
                outChannels, filterWidth, filterHeight));
            break;
        default:
            nd->net.add_layer(new MiniDNN::Convolutional<MiniDNN::ReLU>(
                nd->currentWidth, nd->currentHeight, nd->currentChannels,
                outChannels, filterWidth, filterHeight));
            break;
    }
    
    // Update dimensions (valid convolution)
    nd->currentWidth = nd->currentWidth - filterWidth + 1;
    nd->currentHeight = nd->currentHeight - filterHeight + 1;
    nd->currentChannels = outChannels;
    nd->lastOutputSize = nd->currentWidth * nd->currentHeight * nd->currentChannels;
    
    vm->push(vm->makeNil());
    return 1;
}

// Method: addMaxPool(pool_width, pool_height)
// Adds a max pooling layer
static int network_addMaxPool(Interpreter* vm, void* instance, int argCount, Value* args)
{
    NetworkData* nd = as_network(instance);
    
    if (nd->currentWidth == 0 || nd->currentHeight == 0)
    {
        vm->runtimeError("addMaxPool(): Call input() first to set image dimensions");
        return 0;
    }
    
    if (argCount < 2 || !args[0].isNumber() || !args[1].isNumber())
    {
        vm->runtimeError("addMaxPool() expects (pool_width, pool_height)");
        return 0;
    }
    
    int poolWidth = (int)args[0].asNumber();
    int poolHeight = (int)args[1].asNumber();
    
    // Store layer info
    LayerInfo info;
    info.type = LayerType::MAXPOOL;
    info.inWidth = nd->currentWidth;
    info.inHeight = nd->currentHeight;
    info.inChannels = nd->currentChannels;
    info.filterWidth = poolWidth;
    info.filterHeight = poolHeight;
    nd->layerInfos.push_back(info);
    
    // Create max pooling layer (uses Identity activation)
    nd->net.add_layer(new MiniDNN::MaxPooling<MiniDNN::Identity>(
        nd->currentWidth, nd->currentHeight, nd->currentChannels,
        poolWidth, poolHeight));
    
    // Update dimensions
    nd->currentWidth = nd->currentWidth / poolWidth;
    nd->currentHeight = nd->currentHeight / poolHeight;
    nd->lastOutputSize = nd->currentWidth * nd->currentHeight * nd->currentChannels;
    
    vm->push(vm->makeNil());
    return 1;
}

// Method: flatten()
// Returns the current flattened size (for transitioning to dense layers)
static int network_flatten(Interpreter* vm, void* instance, int argCount, Value* args)
{
    (void)argCount; (void)args;
    NetworkData* nd = as_network(instance);
    
    int flatSize = nd->currentWidth * nd->currentHeight * nd->currentChannels;
    if (flatSize == 0)
    {
        flatSize = nd->lastOutputSize;
    }
    
    vm->push(vm->makeDouble((double)flatSize));
    return 1;
}

// Method: compile(optimizer="adam", loss="mse", learning_rate=0.001)
static int network_compile(Interpreter* vm, void* instance, int argCount, Value* args)
{
    NetworkData* nd = as_network(instance);
    
    // Parse optimizer
    if (argCount >= 1 && args[0].isString())
    {
        const char* opt = args[0].asString()->chars();
        if (strcmp(opt, "sgd") == 0) nd->optimizerType = OptimizerType::SGD;
        else if (strcmp(opt, "adam") == 0) nd->optimizerType = OptimizerType::ADAM;
        else if (strcmp(opt, "rmsprop") == 0) nd->optimizerType = OptimizerType::RMSPROP;
        else if (strcmp(opt, "adagrad") == 0) nd->optimizerType = OptimizerType::ADAGRAD;
    }
    
    // Parse loss
    if (argCount >= 2 && args[1].isString())
    {
        const char* loss = args[1].asString()->chars();
        if (strcmp(loss, "mse") == 0) nd->lossType = LossType::MSE;
        else if (strcmp(loss, "bce") == 0 || strcmp(loss, "binary_cross_entropy") == 0) 
            nd->lossType = LossType::BINARY_CROSS_ENTROPY;
        else if (strcmp(loss, "cross_entropy") == 0 || strcmp(loss, "categorical_cross_entropy") == 0)
            nd->lossType = LossType::MULTI_CROSS_ENTROPY;
    }
    
    // Parse learning rate
    if (argCount >= 3 && args[2].isNumber())
    {
        nd->learningRate = args[2].asNumber();
    }
    
    // Set output layer based on loss
    switch (nd->lossType)
    {
        case LossType::MSE:
            nd->net.set_output(new MiniDNN::RegressionMSE());
            break;
        case LossType::BINARY_CROSS_ENTROPY:
            nd->net.set_output(new MiniDNN::BinaryClassEntropy());
            break;
        case LossType::MULTI_CROSS_ENTROPY:
            nd->net.set_output(new MiniDNN::MultiClassEntropy());
            break;
    }
    
    // Initialize network
    nd->net.init(0, 0.01, 42);
    nd->initialized = true;
    
    // Create optimizer
    switch (nd->optimizerType)
    {
        case OptimizerType::SGD:
            nd->sgdOpt = new MiniDNN::SGD();
            nd->sgdOpt->m_lrate = nd->learningRate;
            break;
        case OptimizerType::ADAM:
            nd->adamOpt = new MiniDNN::Adam();
            nd->adamOpt->m_lrate = nd->learningRate;
            break;
        case OptimizerType::RMSPROP:
            nd->rmspropOpt = new MiniDNN::RMSProp();
            nd->rmspropOpt->m_lrate = nd->learningRate;
            break;
        case OptimizerType::ADAGRAD:
            nd->adagradOpt = new MiniDNN::AdaGrad();
            nd->adagradOpt->m_lrate = nd->learningRate;
            break;
    }
    
    vm->push(vm->makeNil());
    return 1;
}

// ============================================
// TypedArray helper - uses BufferType, no string comparison
// ============================================

// TypedArray data structure (matches builtins_array.cpp)
struct TypedArrayData
{
    BufferType type;
    int count;
    int capacity;
    int elementSize;
    uint8* data;
};

// Get any TypedArray (Float32Array, Float64Array, etc.) - no string comparison!
static bool get_typed_array(const Value& val, TypedArrayData** out)
{
    if (!val.isNativeClassInstance()) return false;
    
    NativeClassInstance* inst = val.asNativeClassInstance();
    if (!inst || !inst->userData) return false;
    
    // Just cast - the caller will check the BufferType
    *out = static_cast<TypedArrayData*>(inst->userData);
    return true;
}

// Read typed array data into Eigen column vector
static bool typed_array_to_col(TypedArrayData* ta, Eigen::MatrixXd& mat, int col)
{
    switch (ta->type)
    {
        case BufferType::DOUBLE:
        {
            double* data = reinterpret_cast<double*>(ta->data);
            for (int i = 0; i < ta->count; i++)
                mat(i, col) = data[i];
            return true;
        }
        case BufferType::FLOAT:
        {
            float* data = reinterpret_cast<float*>(ta->data);
            for (int i = 0; i < ta->count; i++)
                mat(i, col) = static_cast<double>(data[i]);
            return true;
        }
        case BufferType::INT32:
        {
            int32* data = reinterpret_cast<int32*>(ta->data);
            for (int i = 0; i < ta->count; i++)
                mat(i, col) = static_cast<double>(data[i]);
            return true;
        }
        case BufferType::UINT32:
        {
            uint32* data = reinterpret_cast<uint32*>(ta->data);
            for (int i = 0; i < ta->count; i++)
                mat(i, col) = static_cast<double>(data[i]);
            return true;
        }
        default:
            return false;  // Unsupported type for ML
    }
}

// Convert BuLang data to Eigen matrix
// Supports: Array, Float64Array, Float32Array, Int32Array, Uint32Array
static bool data_to_matrix(Value& val, Eigen::MatrixXd& mat, int expectedRows)
{
    // Case 1: Single TypedArray (1D data, single sample)
    TypedArrayData* ta = nullptr;
    if (get_typed_array(val, &ta))
    {
        if (expectedRows > 0 && ta->count != expectedRows) return false;
        mat.resize(ta->count, 1);
        return typed_array_to_col(ta, mat, 0);
    }
    
    // Case 2: Regular Array
    if (!val.isArray()) return false;
    
    ArrayInstance* arr = val.asArray();
    int nSamples = (int)arr->values.size();
    if (nSamples == 0) return false;
    
    // Case 2a: Array of TypedArrays (fast path!)
    TypedArrayData* firstTA = nullptr;
    if (get_typed_array(arr->values[0], &firstTA))
    {
        int features = firstTA->count;
        if (expectedRows > 0 && features != expectedRows) return false;
        
        mat.resize(features, nSamples);
        
        for (int i = 0; i < nSamples; i++)
        {
            TypedArrayData* sample = nullptr;
            if (!get_typed_array(arr->values[i], &sample)) return false;
            if (sample->count != features) return false;
            if (!typed_array_to_col(sample, mat, i)) return false;
        }
        return true;
    }
    
    // Case 2b: 2D array of arrays
    if (arr->values[0].isArray())
    {
        ArrayInstance* first = arr->values[0].asArray();
        int features = (int)first->values.size();
        
        if (expectedRows > 0 && features != expectedRows) return false;
        
        mat.resize(features, nSamples);
        
        for (int i = 0; i < nSamples; i++)
        {
            if (!arr->values[i].isArray()) return false;
            ArrayInstance* sample = arr->values[i].asArray();
            if ((int)sample->values.size() != features) return false;
            
            for (int j = 0; j < features; j++)
            {
                if (!sample->values[j].isNumber()) return false;
                mat(j, i) = sample->values[j].asNumber();
            }
        }
        return true;
    }
    
    // Case 2c: 1D array of numbers (single sample)
    if (arr->values[0].isNumber())
    {
        mat.resize(nSamples, 1);
        for (int i = 0; i < nSamples; i++)
        {
            if (!arr->values[i].isNumber()) return false;
            mat(i, 0) = arr->values[i].asNumber();
        }
        return true;
    }
    
    return false;
}

// Helper: Convert Eigen matrix to BuLang array (for predict output)
static Value matrix_to_array(Interpreter* vm, const Eigen::MatrixXd& mat)
{
    int rows = (int)mat.rows();
    int cols = (int)mat.cols();
    
    if (cols == 1)
    {
        // Single sample: return 1D array
        Value arrVal = vm->makeArray();
        ArrayInstance* arr = arrVal.asArray();
        for (int i = 0; i < rows; i++)
        {
            arr->values.push(vm->makeDouble(mat(i, 0)));
        }
        return arrVal;
    }
    else
    {
        // Multiple samples: return 2D array
        Value outerVal = vm->makeArray();
        ArrayInstance* outer = outerVal.asArray();
        for (int c = 0; c < cols; c++)
        {
            Value innerVal = vm->makeArray();
            ArrayInstance* inner = innerVal.asArray();
            for (int r = 0; r < rows; r++)
            {
                inner->values.push(vm->makeDouble(mat(r, c)));
            }
            outer->values.push(innerVal);
        }
        return outerVal;
    }
}

// Method: fit(x, y, epochs=10, batch_size=32)
// Supports: Array, Float64Array, Float32Array, or array of Float64Array/Float32Array
static int network_fit(Interpreter* vm, void* instance, int argCount, Value* args)
{
    NetworkData* nd = as_network(instance);
    
    if (!nd->initialized)
    {
        vm->runtimeError("Network not compiled. Call compile() first.");
        return 0;
    }
    
    if (argCount < 2)
    {
        vm->runtimeError("fit() expects (x_data, y_data, [epochs], [batch_size])");
        return 0;
    }
    
    int epochs = (argCount >= 3 && args[2].isNumber()) ? (int)args[2].asNumber() : 10;
    int batchSize = (argCount >= 4 && args[3].isNumber()) ? (int)args[3].asNumber() : 32;
    
    // Convert data to Eigen matrices (supports Array, Float64Array, Float32Array)
    Eigen::MatrixXd x, y;
    
    if (!data_to_matrix(args[0], x, nd->lastInputSize))
    {
        vm->runtimeError("fit(): Invalid x data format or dimension mismatch");
        return 0;
    }
    
    if (!data_to_matrix(args[1], y, nd->lastOutputSize))
    {
        vm->runtimeError("fit(): Invalid y data format or dimension mismatch");
        return 0;
    }
    
    if (x.cols() != y.cols())
    {
        vm->runtimeError("fit(): x and y must have the same number of samples");
        return 0;
    }
    
    // Train
    bool success = false;
    switch (nd->optimizerType)
    {
        case OptimizerType::SGD:
            success = nd->net.fit(*nd->sgdOpt, x, y, batchSize, epochs);
            break;
        case OptimizerType::ADAM:
            success = nd->net.fit(*nd->adamOpt, x, y, batchSize, epochs);
            break;
        case OptimizerType::RMSPROP:
            success = nd->net.fit(*nd->rmspropOpt, x, y, batchSize, epochs);
            break;
        case OptimizerType::ADAGRAD:
            success = nd->net.fit(*nd->adagradOpt, x, y, batchSize, epochs);
            break;
    }
    
    vm->push(vm->makeBool(success));
    return 1;
}

// Method: predict(x) -> array
// Supports: Array, Float64Array, Float32Array
static int network_predict(Interpreter* vm, void* instance, int argCount, Value* args)
{
    NetworkData* nd = as_network(instance);
    
    if (!nd->initialized)
    {
        vm->runtimeError("Network not compiled. Call compile() first.");
        return 0;
    }
    
    if (argCount < 1)
    {
        vm->runtimeError("predict() expects input data");
        return 0;
    }
    
    Eigen::MatrixXd x;
    if (!data_to_matrix(args[0], x, nd->lastInputSize))
    {
        vm->runtimeError("predict(): Invalid input format or dimension mismatch");
        return 0;
    }
    
    Eigen::MatrixXd result = nd->net.predict(x);
    vm->push(matrix_to_array(vm, result));
    return 1;
}

// Method: layers() -> number of layers
static int network_layers(Interpreter* vm, void* instance, int argCount, Value* args)
{
    (void)argCount; (void)args;
    NetworkData* nd = as_network(instance);
    vm->pushInt(nd->net.num_layers());
    return 1;
}

// Method: summary() -> prints network info
static int network_summary(Interpreter* vm, void* instance, int argCount, Value* args)
{
    (void)argCount; (void)args;
    NetworkData* nd = as_network(instance);
    
    OsPrintf("Network Summary\n");
    OsPrintf("===============\n");
    OsPrintf("Layers: %d\n", nd->net.num_layers());
    OsPrintf("Input size: %d\n", nd->lastInputSize);
    OsPrintf("Output size: %d\n", nd->lastOutputSize);
    OsPrintf("Initialized: %s\n", nd->initialized ? "yes" : "no");
    
    const char* optName = "unknown";
    switch (nd->optimizerType)
    {
        case OptimizerType::SGD: optName = "SGD"; break;
        case OptimizerType::ADAM: optName = "Adam"; break;
        case OptimizerType::RMSPROP: optName = "RMSProp"; break;
        case OptimizerType::ADAGRAD: optName = "AdaGrad"; break;
    }
    OsPrintf("Optimizer: %s (lr=%.6f)\n", optName, nd->learningRate);
    
    const char* lossName = "unknown";
    switch (nd->lossType)
    {
        case LossType::MSE: lossName = "MSE"; break;
        case LossType::BINARY_CROSS_ENTROPY: lossName = "Binary Cross Entropy"; break;
        case LossType::MULTI_CROSS_ENTROPY: lossName = "Categorical Cross Entropy"; break;
    }
    OsPrintf("Loss: %s\n", lossName);
    
    vm->push(vm->makeNil());
    return 1;
}

// Helper: Add layer to network given layer info
static void add_layer_to_network(NetworkData* nd, const LayerInfo& info)
{
    if (info.type == LayerType::CONV2D)
    {
        switch (info.activation)
        {
            case ActivationType::RELU:
                nd->net.add_layer(new MiniDNN::Convolutional<MiniDNN::ReLU>(
                    info.inWidth, info.inHeight, info.inChannels,
                    info.outChannels, info.filterWidth, info.filterHeight));
                break;
            case ActivationType::SIGMOID:
                nd->net.add_layer(new MiniDNN::Convolutional<MiniDNN::Sigmoid>(
                    info.inWidth, info.inHeight, info.inChannels,
                    info.outChannels, info.filterWidth, info.filterHeight));
                break;
            case ActivationType::TANH:
                nd->net.add_layer(new MiniDNN::Convolutional<MiniDNN::Tanh>(
                    info.inWidth, info.inHeight, info.inChannels,
                    info.outChannels, info.filterWidth, info.filterHeight));
                break;
            default:
                nd->net.add_layer(new MiniDNN::Convolutional<MiniDNN::ReLU>(
                    info.inWidth, info.inHeight, info.inChannels,
                    info.outChannels, info.filterWidth, info.filterHeight));
                break;
        }
    }
    else if (info.type == LayerType::MAXPOOL)
    {
        nd->net.add_layer(new MiniDNN::MaxPooling<MiniDNN::Identity>(
            info.inWidth, info.inHeight, info.inChannels,
            info.filterWidth, info.filterHeight));
    }
    else // DENSE
    {
        switch (info.activation)
        {
            case ActivationType::RELU:
                nd->net.add_layer(new MiniDNN::FullyConnected<MiniDNN::ReLU>(info.inSize, info.outSize));
                break;
            case ActivationType::SIGMOID:
                nd->net.add_layer(new MiniDNN::FullyConnected<MiniDNN::Sigmoid>(info.inSize, info.outSize));
                break;
            case ActivationType::TANH:
                nd->net.add_layer(new MiniDNN::FullyConnected<MiniDNN::Tanh>(info.inSize, info.outSize));
                break;
            case ActivationType::SOFTMAX:
                nd->net.add_layer(new MiniDNN::FullyConnected<MiniDNN::Softmax>(info.inSize, info.outSize));
                break;
            case ActivationType::IDENTITY:
                nd->net.add_layer(new MiniDNN::FullyConnected<MiniDNN::Identity>(info.inSize, info.outSize));
                break;
            case ActivationType::MISH:
                nd->net.add_layer(new MiniDNN::FullyConnected<MiniDNN::Mish>(info.inSize, info.outSize));
                break;
        }
    }
}

// Method: save(filename) -> saves network architecture and weights to file
// Format v3: [magic][version][nlayers][inputSize][outputSize][optimizerType][lossType][lr]
//            [currentWidth][currentHeight][currentChannels]
//            [layer_infos...][params...]
// LayerInfo: [type][activation][inSize][outSize][inWidth][inHeight][inChannels][outChannels][filterW][filterH]
static int network_save(Interpreter* vm, void* instance, int argCount, Value* args)
{
    NetworkData* nd = as_network(instance);
    
    if (argCount < 1 || !args[0].isString())
    {
        vm->runtimeError("save() expects a filename string");
        return 0;
    }
    
    if (!nd->initialized)
    {
        vm->runtimeError("save(): Network not initialized");
        return 0;
    }
    
    const char* filename = args[0].asString()->chars();
    
    // Get parameters from network
    std::vector<std::vector<MiniDNN::Scalar>> params = nd->net.get_parameters();
    
    // Calculate total size
    size_t totalSize = 0;
    totalSize += 4;  // magic "BUNN"
    totalSize += 4;  // version (3)
    totalSize += 4;  // nlayers
    totalSize += 4;  // inputSize
    totalSize += 4;  // outputSize
    totalSize += 4;  // optimizerType
    totalSize += 4;  // lossType
    totalSize += 8;  // learningRate (double)
    totalSize += 4;  // currentWidth (for CNN state)
    totalSize += 4;  // currentHeight
    totalSize += 4;  // currentChannels
    
    // Size for layer architecture (10 int32s per layer)
    // type, activation, inSize, outSize, inWidth, inHeight, inChannels, outChannels, filterW, filterH
    totalSize += nd->layerInfos.size() * (4 * 10);
    
    // Size for each layer's params
    for (size_t i = 0; i < params.size(); i++)
    {
        totalSize += 4;  // param count for this layer
        totalSize += params[i].size() * sizeof(MiniDNN::Scalar);
    }
    
    // Allocate buffer
    std::vector<uint8_t> buffer(totalSize);
    uint8_t* ptr = buffer.data();
    
    // Write magic
    memcpy(ptr, "BUNN", 4); ptr += 4;
    
    // Write version (3 = full CNN support)
    int32_t version = 3;
    memcpy(ptr, &version, 4); ptr += 4;
    
    // Write nlayers
    int32_t nlayers = (int32_t)nd->layerInfos.size();
    memcpy(ptr, &nlayers, 4); ptr += 4;
    
    // Write inputSize
    int32_t inputSize = nd->lastInputSize;
    memcpy(ptr, &inputSize, 4); ptr += 4;
    
    // Write outputSize
    int32_t outputSize = nd->lastOutputSize;
    memcpy(ptr, &outputSize, 4); ptr += 4;
    
    // Write optimizerType
    int32_t optType = (int32_t)nd->optimizerType;
    memcpy(ptr, &optType, 4); ptr += 4;
    
    // Write lossType
    int32_t lType = (int32_t)nd->lossType;
    memcpy(ptr, &lType, 4); ptr += 4;
    
    // Write learning rate
    double lr = nd->learningRate;
    memcpy(ptr, &lr, 8); ptr += 8;
    
    // Write CNN state
    int32_t cw = nd->currentWidth;
    int32_t ch = nd->currentHeight;
    int32_t cc = nd->currentChannels;
    memcpy(ptr, &cw, 4); ptr += 4;
    memcpy(ptr, &ch, 4); ptr += 4;
    memcpy(ptr, &cc, 4); ptr += 4;
    
    // Write layer architecture (full LayerInfo)
    for (size_t i = 0; i < nd->layerInfos.size(); i++)
    {
        const LayerInfo& li = nd->layerInfos[i];
        int32_t vals[10] = {
            (int32_t)li.type,
            (int32_t)li.activation,
            li.inSize,
            li.outSize,
            li.inWidth,
            li.inHeight,
            li.inChannels,
            li.outChannels,
            li.filterWidth,
            li.filterHeight
        };
        memcpy(ptr, vals, 4 * 10); ptr += 4 * 10;
    }
    
    // Write layer params
    for (size_t i = 0; i < params.size(); i++)
    {
        int32_t paramCount = (int32_t)params[i].size();
        memcpy(ptr, &paramCount, 4); ptr += 4;
        
        if (paramCount > 0)
        {
            memcpy(ptr, params[i].data(), paramCount * sizeof(MiniDNN::Scalar));
            ptr += paramCount * sizeof(MiniDNN::Scalar);
        }
    }
    
    // Write to file using portable function
    int result = OsFileWrite(filename, buffer.data(), totalSize);
    
    if (result < 0)
    {
        vm->runtimeError("save(): Failed to write file '%s'", filename);
        return 0;
    }
    
    vm->push(vm->makeBool(true));
    return 1;
}

// Method: load(filename) -> loads network architecture and weights from file
static int network_load(Interpreter* vm, void* instance, int argCount, Value* args)
{
    NetworkData* nd = as_network(instance);
    
    if (argCount < 1 || !args[0].isString())
    {
        vm->runtimeError("load() expects a filename string");
        return 0;
    }
    
    const char* filename = args[0].asString()->chars();
    
    // Check file exists
    if (!OsFileExists(filename))
    {
        vm->runtimeError("load(): File '%s' not found", filename);
        return 0;
    }
    
    // Get file size
    int fileSize = OsFileSize(filename);
    if (fileSize < 48)  // minimum header size for v3
    {
        vm->runtimeError("load(): Invalid file format");
        return 0;
    }
    
    // Read file
    std::vector<uint8_t> buffer(fileSize);
    int bytesRead = OsFileRead(filename, buffer.data(), fileSize);
    if (bytesRead != fileSize)
    {
        vm->runtimeError("load(): Failed to read file");
        return 0;
    }
    
    uint8_t* ptr = buffer.data();
    
    // Check magic
    if (memcmp(ptr, "BUNN", 4) != 0)
    {
        vm->runtimeError("load(): Invalid file format (bad magic)");
        return 0;
    }
    ptr += 4;
    
    // Read version
    int32_t version;
    memcpy(&version, ptr, 4); ptr += 4;
    if (version != 3)
    {
        vm->runtimeError("load(): Unsupported file version %d (expected 3)", version);
        return 0;
    }
    
    // Read nlayers
    int32_t nlayers;
    memcpy(&nlayers, ptr, 4); ptr += 4;
    
    // Read inputSize
    int32_t inputSize;
    memcpy(&inputSize, ptr, 4); ptr += 4;
    
    // Read outputSize
    int32_t outputSize;
    memcpy(&outputSize, ptr, 4); ptr += 4;
    
    // Read optimizerType
    int32_t optType;
    memcpy(&optType, ptr, 4); ptr += 4;
    
    // Read lossType
    int32_t lType;
    memcpy(&lType, ptr, 4); ptr += 4;
    
    // Read learning rate
    double lr;
    memcpy(&lr, ptr, 8); ptr += 8;
    
    // Read CNN state
    int32_t cw, ch, cc;
    memcpy(&cw, ptr, 4); ptr += 4;
    memcpy(&ch, ptr, 4); ptr += 4;
    memcpy(&cc, ptr, 4); ptr += 4;
    
    // Read layer architecture and recreate layers
    std::vector<LayerInfo> layerInfos;
    layerInfos.reserve(nlayers);
    
    for (int i = 0; i < nlayers; i++)
    {
        int32_t vals[10];
        memcpy(vals, ptr, 4 * 10); ptr += 4 * 10;
        
        LayerInfo info;
        info.type = (LayerType)vals[0];
        info.activation = (ActivationType)vals[1];
        info.inSize = vals[2];
        info.outSize = vals[3];
        info.inWidth = vals[4];
        info.inHeight = vals[5];
        info.inChannels = vals[6];
        info.outChannels = vals[7];
        info.filterWidth = vals[8];
        info.filterHeight = vals[9];
        layerInfos.push_back(info);
        
        // Create the layer
        add_layer_to_network(nd, info);
    }
    
    // Store layer infos and state
    nd->layerInfos = layerInfos;
    nd->lastInputSize = inputSize;
    nd->lastOutputSize = outputSize;
    nd->optimizerType = (OptimizerType)optType;
    nd->lossType = (LossType)lType;
    nd->learningRate = lr;
    nd->currentWidth = cw;
    nd->currentHeight = ch;
    nd->currentChannels = cc;
    
    // Set output layer based on loss
    switch (nd->lossType)
    {
        case LossType::MSE:
            nd->net.set_output(new MiniDNN::RegressionMSE());
            break;
        case LossType::BINARY_CROSS_ENTROPY:
            nd->net.set_output(new MiniDNN::BinaryClassEntropy());
            break;
        case LossType::MULTI_CROSS_ENTROPY:
            nd->net.set_output(new MiniDNN::MultiClassEntropy());
            break;
    }
    
    // Initialize network
    nd->net.init(0, 0.01);
    nd->initialized = true;
    // Read layer params
    std::vector<std::vector<MiniDNN::Scalar>> params;
    params.reserve(nlayers);
    
    for (int i = 0; i < nlayers; i++)
    {
        int32_t paramCount;
        memcpy(&paramCount, ptr, 4); ptr += 4;
        
        std::vector<MiniDNN::Scalar> layerParams(paramCount);
        if (paramCount > 0)
        {
            memcpy(layerParams.data(), ptr, paramCount * sizeof(MiniDNN::Scalar));
            ptr += paramCount * sizeof(MiniDNN::Scalar);
        }
        params.push_back(layerParams);
    }
    
    // Set parameters (weights)
    try {
        nd->net.set_parameters(params);
    } catch (...) {
        vm->runtimeError("load(): Failed to set parameters");
        return 0;
    }
    
    vm->push(vm->makeBool(true));
    return 1;
}

// Register Network class
static void register_network_class(Interpreter& vm)
{
    NativeClassDef* netClass = vm.registerNativeClass("Network", network_ctor, network_dtor, 0, false);
    
    // Dense (fully connected) layers
    vm.addNativeMethod(netClass, "add", network_add);
    
    // CNN layers
    vm.addNativeMethod(netClass, "input", network_input);
    vm.addNativeMethod(netClass, "addConv2D", network_addConv2D);
    vm.addNativeMethod(netClass, "addMaxPool", network_addMaxPool);
    vm.addNativeMethod(netClass, "flatten", network_flatten);
    
    // Training/inference
    vm.addNativeMethod(netClass, "compile", network_compile);
    vm.addNativeMethod(netClass, "fit", network_fit);
    vm.addNativeMethod(netClass, "predict", network_predict);
    
    // Utilities
    vm.addNativeMethod(netClass, "layers", network_layers);
    vm.addNativeMethod(netClass, "summary", network_summary);
    vm.addNativeMethod(netClass, "save", network_save);
    vm.addNativeMethod(netClass, "load", network_load);
}

} // namespace

#endif // BU_ENABLE_MINIDNN

// ============================================
// IMAGE LOADING (PPM/PGM - Ultra lightweight)
// ============================================

// Skip whitespace and comments in PPM/PGM
static void ppm_skip_ws_comments(const char*& p, const char* end)
{
    while (p < end)
    {
        while (p < end && (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')) p++;
        if (p < end && *p == '#')
        {
            while (p < end && *p != '\n') p++;
        }
        else break;
    }
}

// Read integer from PPM/PGM
static int ppm_read_int(const char*& p, const char* end)
{
    ppm_skip_ws_comments(p, end);
    int val = 0;
    while (p < end && *p >= '0' && *p <= '9')
    {
        val = val * 10 + (*p - '0');
        p++;
    }
    return val;
}

// nn.loadImage(filename) -> {width, height, channels, data:[...]}
// Supports: PGM (P2/P5), PPM (P3/P6)
static int native_nn_loadImage(Interpreter* vm, int argCount, Value* args)
{
    if (argCount < 1 || !args[0].isString())
    {
        vm->runtimeError("loadImage() expects filename");
        return 0;
    }
    
    const char* filename = args[0].asString()->chars();
    
    // Read file
    if (!OsFileExists(filename))
    {
        vm->runtimeError("loadImage(): File '%s' not found", filename);
        return 0;
    }
    
    int fileSize = OsFileSize(filename);
    std::vector<uint8_t> buffer(fileSize + 1);
    OsFileRead(filename, buffer.data(), fileSize);
    buffer[fileSize] = 0;
    
    const char* p = (const char*)buffer.data();
    const char* end = p + fileSize;
    
    // Read magic
    if (p[0] != 'P')
    {
        vm->runtimeError("loadImage(): Not a PPM/PGM file");
        return 0;
    }
    
    char magic = p[1];
    p += 2;
    
    int channels = 1;
    bool binary = false;
    
    switch (magic)
    {
        case '2': channels = 1; binary = false; break;  // PGM ASCII
        case '3': channels = 3; binary = false; break;  // PPM ASCII
        case '5': channels = 1; binary = true; break;   // PGM Binary
        case '6': channels = 3; binary = true; break;   // PPM Binary
        default:
            vm->runtimeError("loadImage(): Unsupported format P%c", magic);
            return 0;
    }
    
    // Read dimensions
    int width = ppm_read_int(p, end);
    int height = ppm_read_int(p, end);
    int maxVal = ppm_read_int(p, end);
    
    if (width <= 0 || height <= 0 || maxVal <= 0)
    {
        vm->runtimeError("loadImage(): Invalid dimensions");
        return 0;
    }
    
    // Skip single whitespace after maxVal for binary
    if (binary && p < end) p++;
    
    // Read pixels into array
    Value dataVal = vm->makeArray();
    ArrayInstance* data = dataVal.asArray();
    
    int totalPixels = width * height * channels;
    double scale = 1.0 / maxVal;  // Normalize to [0, 1]
    
    if (binary)
    {
        // Binary format
        for (int i = 0; i < totalPixels && p < end; i++)
        {
            uint8_t val = (uint8_t)*p++;
            data->values.push(vm->makeDouble(val * scale));
        }
    }
    else
    {
        // ASCII format
        for (int i = 0; i < totalPixels; i++)
        {
            int val = ppm_read_int(p, end);
            data->values.push(vm->makeDouble(val * scale));
        }
    }
    
    // Return multiple values: width, height, channels, data
    vm->push(vm->makeDouble((double)width));
    vm->push(vm->makeDouble((double)height));
    vm->push(vm->makeDouble((double)channels));
    vm->push(dataVal);
    return 4;  // 4 return values
}

// nn.saveImage(filename, width, height, channels, data)
// Saves to PPM (RGB) or PGM (grayscale)
static int native_nn_saveImage(Interpreter* vm, int argCount, Value* args)
{
    if (argCount < 5)
    {
        vm->runtimeError("saveImage() expects (filename, width, height, channels, data)");
        return 0;
    }
    
    if (!args[0].isString() || !args[1].isNumber() || !args[2].isNumber() || 
        !args[3].isNumber() || !args[4].isArray())
    {
        vm->runtimeError("saveImage(): Invalid arguments");
        return 0;
    }
    
    const char* filename = args[0].asString()->chars();
    int width = (int)args[1].asNumber();
    int height = (int)args[2].asNumber();
    int channels = (int)args[3].asNumber();
    ArrayInstance* data = args[4].asArray();
    
    if (channels != 1 && channels != 3)
    {
        vm->runtimeError("saveImage(): channels must be 1 (grayscale) or 3 (RGB)");
        return 0;
    }
    
    // Build PPM/PGM content
    std::string content;
    content += (channels == 1) ? "P2\n" : "P3\n";
    content += std::to_string(width) + " " + std::to_string(height) + "\n";
    content += "255\n";
    
    int totalPixels = width * height * channels;
    for (int i = 0; i < totalPixels && i < (int)data->values.size(); i++)
    {
        double val = data->values[i].isNumber() ? data->values[i].asNumber() : 0;
        int pixelVal = (int)(val * 255.0 + 0.5);
        if (pixelVal < 0) pixelVal = 0;
        if (pixelVal > 255) pixelVal = 255;
        
        content += std::to_string(pixelVal);
        content += ((i + 1) % (width * channels) == 0) ? "\n" : " ";
    }
    
    // Write file
    int result = OsFileWrite(filename, content.c_str(), (int)content.size());
    
    vm->push(vm->makeBool(result >= 0));
    return 1;
}

// ============================================
// REGISTRATION
// ============================================

void Interpreter::registerNN()
{
    addModule("nn")
        // Activation functions
        .addFunction("sigmoid", native_nn_sigmoid, 1)
        .addFunction("relu", native_nn_relu, 1)
        .addFunction("leaky_relu", native_nn_leaky_relu, -1)
        .addFunction("elu", native_nn_elu, -1)
        .addFunction("swish", native_nn_swish, 1)
        .addFunction("silu", native_nn_silu, 1)
        .addFunction("gelu", native_nn_gelu, 1)
        .addFunction("softplus", native_nn_softplus, 1)
        .addFunction("mish", native_nn_mish, 1)
        .addFunction("step", native_nn_step, -1)
        
        // Derivatives (for backprop)
        .addFunction("sigmoid_d", native_nn_sigmoid_derivative, 1)
        .addFunction("relu_d", native_nn_relu_derivative, 1)
        .addFunction("tanh_d", native_nn_tanh_derivative, 1)
        
        // Loss functions
        .addFunction("mse", native_nn_mse, 2)
        .addFunction("bce", native_nn_binary_cross_entropy, 2)
        
        // Utility
        .addFunction("normalize", native_nn_normalize, 3)
        .addFunction("denormalize", native_nn_denormalize, 3)
        
        // Image loading (PPM/PGM) - returns: width, height, channels, data
        .addFunction("loadImage", native_nn_loadImage, 1)
        .addFunction("saveImage", native_nn_saveImage, 5);

#ifdef BU_ENABLE_MINIDNN
    register_network_class(*this);
#endif
}

#endif // BU_ENABLE_NN
