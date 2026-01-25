#include "bindings.hpp"
//#include <box2d/box2d.h>

namespace Box2DBindings
{
    // =============================================================
    // WORLD
    // =============================================================

   

    // =============================================================
    // REGISTRATION
    // =============================================================

    void registerAll(Interpreter &vm)
    {
        ModuleBuilder module = vm.addModule("Box2D");

        // module
        //     // World
        //     .addFunction("b2WorldCreate", native_b2WorldCreate, 2)
        //     .addFunction("b2WorldDestroy", native_b2WorldDestroy, 1)
        //     .addFunction("b2WorldStep", native_b2WorldStep, 3)
        //     .addFunction("b2WorldSetGravity", native_b2WorldSetGravity, 3)

        //     // Body
        //     .addFunction("b2CreateBody", native_b2CreateBody, 4)
        //     .addFunction("b2DestroyBody", native_b2DestroyBody, 1)
        //     .addFunction("b2BodyGetPosition", native_b2BodyGetPosition, 1)
        //     .addFunction("b2BodyGetAngle", native_b2BodyGetAngle, 1)
        //     .addFunction("b2BodySetTransform", native_b2BodySetTransform, 4)
        //     .addFunction("b2BodyApplyForce", native_b2BodyApplyForce, 3)
        //     .addFunction("b2BodyApplyLinearImpulse", native_b2BodyApplyLinearImpulse, 3)

        //     // Shapes
        //     .addFunction("b2CreateBoxShape", native_b2CreateBoxShape, 6)
        //     .addFunction("b2CreateCircleShape", native_b2CreateCircleShape, 5)

        //     // Body Types
        //     .addInt("b2_staticBody", b2_staticBody)
        //     .addInt("b2_kinematicBody", b2_kinematicBody)
        //     .addInt("b2_dynamicBody", b2_dynamicBody);
    }
}
