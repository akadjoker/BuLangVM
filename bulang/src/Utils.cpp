#include "pch.h"
#include "Utils.hpp"




void Log(int severity, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	Log(severity, fmt, args);
	va_end(args);
}

void Log(int severity, const char *fmt, va_list args)
{
	const char *type;
	const char *color;
	switch (severity)
	{
	case 0:
		type = "info";
		color = CONSOLE_COLOR_GREEN;
		break;
	case 1:
		type = "warning";
		color = CONSOLE_COLOR_PURPLE;
		break;
	case 2:
		type = "error";
		color = CONSOLE_COLOR_RED;
		break;
	case 3:
		type = "PRINT";
		color = CONSOLE_COLOR_YELLOW;
		break;
	default:
		type = "unknown";
		color = CONSOLE_COLOR_RESET;
		break;
	}


    char consoleFormat[1024];
    snprintf(consoleFormat, sizeof(consoleFormat), "%s%s%s%s:%s\n", CONSOLE_COLOR_CYAN,
              color, type, CONSOLE_COLOR_RESET, fmt);

  
    va_list argsCopy;
    va_copy(argsCopy, args);

    char consoleMessage[4096];
    vsnprintf(consoleMessage, sizeof(consoleMessage), consoleFormat, args);
    printf("%s", consoleMessage);


    va_end(argsCopy);
}


char *LoadTextFile(const char *fileName)
{
	char *text = NULL;

	if (fileName != NULL)
	{
		FILE *file = fopen(fileName, "rt");

		if (file != NULL)
		{
			fseek(file, 0, SEEK_END);
			unsigned int size = (unsigned int)ftell(file);
			fseek(file, 0, SEEK_SET);

			if (size > 0)
			{
				text = (char *)std::malloc((size + 1) * sizeof(char));

				if (text != NULL)
				{
					unsigned int count = (unsigned int)fread(text, sizeof(char), size, file);
					if (count < size)
						text = (char *)std::realloc(text, count + 1);
					text[count] = '\0';
				}
				else
					Log(1, "Failed to allocated memory for %s reading", fileName);
			}
			else
				Log(1, "Failed to read text from %s", fileName);

			fclose(file);
		}
	}
	return text;
}

void FreeTextFile(char *text)
{
	if (text != NULL)
		std::free(text);
}

const char *FormatText(const char *text, ...)
{
    static char buffers[4][512] = { 0 };
    static int index = 0;

    char *currentBuffer = buffers[index];
    memset(currentBuffer, 0, 512);  

    va_list args;
    va_start(args, text);
    int requiredByteCount = vsnprintf(currentBuffer, 512, text, args);
    va_end(args);
    if (requiredByteCount >= 512)
    {

        char *truncBuffer = buffers[index] + 512 - 4; 
        sprintf(truncBuffer, "...");
    }

    index += 1;    
    if (index >= 4) index = 0;

    return currentBuffer;
}


  bool matchString(const char *str1, const char *str2,size_t bLen)
{
    size_t aLen = strlen(str1);
    
    if (aLen != bLen) return false;
    
    return (memcmp(str1, str2, aLen) == 0);
    
}

double time_now()
{
    return (clock() / (double)CLOCKS_PER_SEC);
}

u32  GROW_CAPACITY(u32 capacity)
{
    return ((capacity) < 8 ? 8 : (capacity) * 2);
}

