

#include <iostream>



#include "pch.h" 


#include "Config.hpp"
#include "Vector.hpp"
#include "Map.hpp"
#include "Mem.hpp"
#include "Stack.hpp"
#include "String.hpp"
#include "Queue.hpp"
#include "Utils.hpp"


#include "Interpreter.hpp" 




static String readFile(const String& filePath)
{

    char* text = LoadTextFile(filePath.c_str());
    if (text == NULL)
    {
        return "";
    }
    String code = text;
    FreeTextFile(text);
    return code;
}


int main()
{



  //  String code = readFile("C:\\Users\\lsant\\source\\repos\\vm\\main.bu");
  String code;

#if defined(_WIN32)
       code= readFile("../main.bu");

   #else
       code= readFile("main.bu");
   #endif
   

    if (code.length() == 0)
    {
        return 0;
    }


    Interpreter interpreter;
    if (interpreter.Load(code))
    {
       interpreter.Disassemble();
        interpreter.Run();
    }





     





    return 0;


}

