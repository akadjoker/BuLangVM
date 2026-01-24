#include "bindings.hpp"
//#include <box2d/box2d.h>

namespace Box2DBindings
{
    // =============================================================
    // WORLD
    // =============================================================

    // Value native_b2WorldCreate(Interpreter *vm, int argc, Value *args)
    // {
    //     float gravityX = argc > 0 ? args[0].asNumber() : 0.0f;
    //     float gravityY = argc > 1 ? args[1].asNumber() : -10.0f;

    //     b2WorldDef worldDef = b2DefaultWorldDef();
    //     worldDef.gravity = {gravityX, gravityY};

    //     b2WorldId worldId = b2CreateWorld(&worldDef);
    //     // Store the world ID as a pointer-sized value
    //     return vm->makeInt((int64_t)worldId.index1);
    // }

    // Value native_b2WorldDestroy(Interpreter *vm, int argc, Value *args)
    // {
    //     if (argc < 1) return vm->makeNil();

    //     b2WorldId worldId;
    //     worldId.index1 = (uint16_t)args[0].asNumber();
    //     worldId.revision = 0;

    //     b2DestroyWorld(worldId);
    //     return vm->makeNil();
    // }

    // Value native_b2WorldStep(Interpreter *vm, int argc, Value *args)
    // {
    //     if (argc < 2) return vm->makeNil();

    //     b2WorldId worldId;
    //     worldId.index1 = (uint16_t)args[0].asNumber();
    //     worldId.revision = 0;

    //     float timeStep = args[1].asNumber();
    //     int subStepCount = argc > 2 ? (int)args[2].asNumber() : 4;

    //     b2World_Step(worldId, timeStep, subStepCount);
    //     return vm->makeNil();
    // }

    // Value native_b2WorldSetGravity(Interpreter *vm, int argc, Value *args)
    // {
    //     if (argc < 3) return vm->makeNil();

    //     b2WorldId worldId;
    //     worldId.index1 = (uint16_t)args[0].asNumber();
    //     worldId.revision = 0;

    //     b2Vec2 gravity = {(float)args[1].asNumber(), (float)args[2].asNumber()};
    //     b2World_SetGravity(worldId, gravity);
    //     return vm->makeNil();
    // }

    // // =============================================================
    // // BODY
    // // =============================================================

    // Value native_b2CreateBody(Interpreter *vm, int argc, Value *args)
    // {
    //     if (argc < 4) return vm->makeNil();

    //     b2WorldId worldId;
    //     worldId.index1 = (uint16_t)args[0].asNumber();
    //     worldId.revision = 0;

    //     int bodyType = (int)args[1].asNumber();  // 0=static, 1=kinematic, 2=dynamic
    //     float x = args[2].asNumber();
    //     float y = args[3].asNumber();

    //     b2BodyDef bodyDef = b2DefaultBodyDef();
    //     bodyDef.type = (b2BodyType)bodyType;
    //     bodyDef.position = {x, y};

    //     b2BodyId bodyId = b2CreateBody(worldId, &bodyDef);

    //     // Return body ID as map with index info
    //     MapInstance* map = vm->newMap();
    //     map->set(vm->createString("index"), vm->makeInt(bodyId.index1));
    //     map->set(vm->createString("world"), vm->makeInt(bodyId.world0));
    //     map->set(vm->createString("revision"), vm->makeInt(bodyId.revision));
    //     return vm->makeMap(map);
    // }

    // Value native_b2DestroyBody(Interpreter *vm, int argc, Value *args)
    // {
    //     if (argc < 1 || !args[0].isMap()) return vm->makeNil();

    //     MapInstance* map = args[0].asMap();
    //     Value indexVal, worldVal, revVal;

    //     b2BodyId bodyId;
    //     if (map->get(vm->createString("index"), &indexVal))
    //         bodyId.index1 = (int32_t)indexVal.asNumber();
    //     if (map->get(vm->createString("world"), &worldVal))
    //         bodyId.world0 = (uint16_t)worldVal.asNumber();
    //     if (map->get(vm->createString("revision"), &revVal))
    //         bodyId.revision = (uint16_t)revVal.asNumber();

    //     b2DestroyBody(bodyId);
    //     return vm->makeNil();
    // }

    // Value native_b2BodyGetPosition(Interpreter *vm, int argc, Value *args)
    // {
    //     if (argc < 1 || !args[0].isMap()) return vm->makeNil();

    //     MapInstance* map = args[0].asMap();
    //     Value indexVal, worldVal, revVal;

    //     b2BodyId bodyId;
    //     if (map->get(vm->createString("index"), &indexVal))
    //         bodyId.index1 = (int32_t)indexVal.asNumber();
    //     if (map->get(vm->createString("world"), &worldVal))
    //         bodyId.world0 = (uint16_t)worldVal.asNumber();
    //     if (map->get(vm->createString("revision"), &revVal))
    //         bodyId.revision = (uint16_t)revVal.asNumber();

    //     b2Vec2 pos = b2Body_GetPosition(bodyId);

    //     ArrayInstance* arr = vm->newArray();
    //     arr->values.push_back(vm->makeNumber(pos.x));
    //     arr->values.push_back(vm->makeNumber(pos.y));
    //     return vm->makeArray(arr);
    // }

    // Value native_b2BodyGetAngle(Interpreter *vm, int argc, Value *args)
    // {
    //     if (argc < 1 || !args[0].isMap()) return vm->makeNil();

    //     MapInstance* map = args[0].asMap();
    //     Value indexVal, worldVal, revVal;

    //     b2BodyId bodyId;
    //     if (map->get(vm->createString("index"), &indexVal))
    //         bodyId.index1 = (int32_t)indexVal.asNumber();
    //     if (map->get(vm->createString("world"), &worldVal))
    //         bodyId.world0 = (uint16_t)worldVal.asNumber();
    //     if (map->get(vm->createString("revision"), &revVal))
    //         bodyId.revision = (uint16_t)revVal.asNumber();

    //     b2Rot rot = b2Body_GetRotation(bodyId);
    //     float angle = b2Rot_GetAngle(rot);
    //     return vm->makeNumber(angle);
    // }

    // Value native_b2BodySetTransform(Interpreter *vm, int argc, Value *args)
    // {
    //     if (argc < 4 || !args[0].isMap()) return vm->makeNil();

    //     MapInstance* map = args[0].asMap();
    //     Value indexVal, worldVal, revVal;

    //     b2BodyId bodyId;
    //     if (map->get(vm->createString("index"), &indexVal))
    //         bodyId.index1 = (int32_t)indexVal.asNumber();
    //     if (map->get(vm->createString("world"), &worldVal))
    //         bodyId.world0 = (uint16_t)worldVal.asNumber();
    //     if (map->get(vm->createString("revision"), &revVal))
    //         bodyId.revision = (uint16_t)revVal.asNumber();

    //     b2Vec2 pos = {(float)args[1].asNumber(), (float)args[2].asNumber()};
    //     float angle = args[3].asNumber();

    //     b2Body_SetTransform(bodyId, pos, b2MakeRot(angle));
    //     return vm->makeNil();
    // }

    // Value native_b2BodyApplyForce(Interpreter *vm, int argc, Value *args)
    // {
    //     if (argc < 3 || !args[0].isMap()) return vm->makeNil();

    //     MapInstance* map = args[0].asMap();
    //     Value indexVal, worldVal, revVal;

    //     b2BodyId bodyId;
    //     if (map->get(vm->createString("index"), &indexVal))
    //         bodyId.index1 = (int32_t)indexVal.asNumber();
    //     if (map->get(vm->createString("world"), &worldVal))
    //         bodyId.world0 = (uint16_t)worldVal.asNumber();
    //     if (map->get(vm->createString("revision"), &revVal))
    //         bodyId.revision = (uint16_t)revVal.asNumber();

    //     b2Vec2 force = {(float)args[1].asNumber(), (float)args[2].asNumber()};
    //     b2Vec2 point = b2Body_GetPosition(bodyId);

    //     b2Body_ApplyForce(bodyId, force, point, true);
    //     return vm->makeNil();
    // }

    // Value native_b2BodyApplyLinearImpulse(Interpreter *vm, int argc, Value *args)
    // {
    //     if (argc < 3 || !args[0].isMap()) return vm->makeNil();

    //     MapInstance* map = args[0].asMap();
    //     Value indexVal, worldVal, revVal;

    //     b2BodyId bodyId;
    //     if (map->get(vm->createString("index"), &indexVal))
    //         bodyId.index1 = (int32_t)indexVal.asNumber();
    //     if (map->get(vm->createString("world"), &worldVal))
    //         bodyId.world0 = (uint16_t)worldVal.asNumber();
    //     if (map->get(vm->createString("revision"), &revVal))
    //         bodyId.revision = (uint16_t)revVal.asNumber();

    //     b2Vec2 impulse = {(float)args[1].asNumber(), (float)args[2].asNumber()};
    //     b2Vec2 point = b2Body_GetPosition(bodyId);

    //     b2Body_ApplyLinearImpulse(bodyId, impulse, point, true);
    //     return vm->makeNil();
    // }

    // // =============================================================
    // // SHAPES
    // // =============================================================

    // Value native_b2CreateBoxShape(Interpreter *vm, int argc, Value *args)
    // {
    //     if (argc < 3 || !args[0].isMap()) return vm->makeNil();

    //     MapInstance* map = args[0].asMap();
    //     Value indexVal, worldVal, revVal;

    //     b2BodyId bodyId;
    //     if (map->get(vm->createString("index"), &indexVal))
    //         bodyId.index1 = (int32_t)indexVal.asNumber();
    //     if (map->get(vm->createString("world"), &worldVal))
    //         bodyId.world0 = (uint16_t)worldVal.asNumber();
    //     if (map->get(vm->createString("revision"), &revVal))
    //         bodyId.revision = (uint16_t)revVal.asNumber();

    //     float halfWidth = args[1].asNumber();
    //     float halfHeight = args[2].asNumber();

    //     b2ShapeDef shapeDef = b2DefaultShapeDef();
    //     shapeDef.density = argc > 3 ? args[3].asNumber() : 1.0f;
    //     shapeDef.friction = argc > 4 ? args[4].asNumber() : 0.3f;
    //     shapeDef.restitution = argc > 5 ? args[5].asNumber() : 0.0f;

    //     b2Polygon box = b2MakeBox(halfWidth, halfHeight);
    //     b2ShapeId shapeId = b2CreatePolygonShape(bodyId, &shapeDef, &box);

    //     return vm->makeInt(shapeId.index1);
    // }

    // Value native_b2CreateCircleShape(Interpreter *vm, int argc, Value *args)
    // {
    //     if (argc < 2 || !args[0].isMap()) return vm->makeNil();

    //     MapInstance* map = args[0].asMap();
    //     Value indexVal, worldVal, revVal;

    //     b2BodyId bodyId;
    //     if (map->get(vm->createString("index"), &indexVal))
    //         bodyId.index1 = (int32_t)indexVal.asNumber();
    //     if (map->get(vm->createString("world"), &worldVal))
    //         bodyId.world0 = (uint16_t)worldVal.asNumber();
    //     if (map->get(vm->createString("revision"), &revVal))
    //         bodyId.revision = (uint16_t)revVal.asNumber();

    //     float radius = args[1].asNumber();

    //     b2ShapeDef shapeDef = b2DefaultShapeDef();
    //     shapeDef.density = argc > 2 ? args[2].asNumber() : 1.0f;
    //     shapeDef.friction = argc > 3 ? args[3].asNumber() : 0.3f;
    //     shapeDef.restitution = argc > 4 ? args[4].asNumber() : 0.0f;

    //     b2Circle circle = {{0.0f, 0.0f}, radius};
    //     b2ShapeId shapeId = b2CreateCircleShape(bodyId, &shapeDef, &circle);

    //     return vm->makeInt(shapeId.index1);
    // }

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
