#include "../../libbu/include/interpreter.hpp"
#include <stdio.h>

// Struct nativo simples
struct Point {
    float x;
    float y;
};

static void point_ctor(Interpreter* vm, void* buffer, int argc, Value* args) {
    (void)vm;
    Point* p = static_cast<Point*>(buffer);
    p->x = (argc > 0) ? args[0].asNumber() : 0.0f;
    p->y = (argc > 1) ? args[1].asNumber() : 0.0f;
}

int main() {
    Interpreter vm;
    
    // Registrar struct nativo
    NativeStructDef* point = vm.registerNativeStruct(
        "Point",
        sizeof(Point),
        point_ctor,
        nullptr,
        false
    );
    
    vm.addStructField(point, "x", offsetof(Point, x), FieldType::FLOAT);
    vm.addStructField(point, "y", offsetof(Point, y), FieldType::FLOAT);
    
    printf("Native struct 'Point' registered successfully!\n");
    
    // Código BuLang para testar
    const char* code = R"(
        print("Testing native Point struct...");
        var p = Point(10.0, 20.0);
        print("p.x = " + str(p.x));
        print("p.y = " + str(p.y));
        
        p.x = 100.0;
        p.y = 200.0;
        print("After modification:");
        print("p.x = " + str(p.x));
        print("p.y = " + str(p.y));
        
        print("SUCCESS: Native bindings work!");
    )";
    
    printf("Compiling and running...\n");
    
    // Compilar e executar
    if (!vm.run(code)) {
        printf("FAILED!\n");
        return 1;
    }
    
    printf("\nNative bindings test PASSED!\n");
    return 0;
}
