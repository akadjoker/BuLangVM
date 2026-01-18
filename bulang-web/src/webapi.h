#ifndef WEB_API_H
#define WEB_API_H

#include "interpreter.hpp"

// ============================================
// Web API Natives para Playground
// ============================================

#ifdef __EMSCRIPTEN__

 

// Register all web natives
void registerWebNatives(Interpreter* vm);

#endif // __EMSCRIPTEN__

#endif // WEB_API_H
