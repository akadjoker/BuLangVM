#pragma once

#include "interpreter.hpp"
#include "Point.hpp"
 

namespace Vector2DBindings {
    
   
 
    void registerAll(Interpreter &vm);
    
} // namespace CounterBindings


namespace PointBindings
{

    // Constructor para Point
    static void point_ctor(Interpreter *vm, void *buffer, int argc, Value *args)
    {
        Point *p = (Point *)buffer;

        if (argc >= 2)
        {
            p->x = args[0].asNumber();
            p->y = args[1].asNumber();
        }
        else
        {
            p->x = 0.0f;
            p->y = 0.0f;
        }
    }

    // Register Point struct
    void registerPoint(Interpreter &vm);
    void registerAll(Interpreter &vm);

} // namespace PointBindings
