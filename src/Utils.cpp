
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

   time_t rawTime;
    struct tm* timeInfo;
    char timeBuffer[80];

    time(&rawTime);
    timeInfo = localtime(&rawTime);

    strftime(timeBuffer, sizeof(timeBuffer), "[%H:%M:%S]", timeInfo);

    char consoleFormat[1024];
    snprintf(consoleFormat, sizeof(consoleFormat), "%s%s %s%s%s: %s\n", CONSOLE_COLOR_CYAN,
             timeBuffer, color, type, CONSOLE_COLOR_RESET, fmt);

    char fileFormat[1024];
    snprintf(fileFormat, sizeof(fileFormat), "%s %s: %s", timeBuffer, type, fmt);

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
