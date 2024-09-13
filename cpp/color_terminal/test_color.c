#include <stdio.h>

#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_GRAY "\x1b[37m"
#define ANSI_COLOR_DEFAULT "\x1b[39m"
#define ANSI_COLOR_RESET "\x1b[0m"

#define FONT_COLOR_RED "\x1b[41m"
#define FONT_COLOR_GREEN "\x1b[42m"
#define FONT_COLOR_YELLOW "\x1b[43m"
#define FONT_COLOR_BLUE "\x1b[44m"
#define FONT_COLOR_MAGENTA "\x1b[45m"
#define FONT_COLOR_CYAN "\x1b[46m"
#define FONT_COLOR_GRAY "\x1b[47m"
#define FONT_COLOR_DEFAULT "\x1b[49m"

int main()
{
	printf(ANSI_COLOR_RED "\n\tWE CAN PRINT%s COLOR FONTS %sAND ALSO%s COLOR BACKGROUND\n", ANSI_COLOR_GREEN, FONT_COLOR_MAGENTA, ANSI_COLOR_RESET);

	printf("\t%s\tRED    %s%s\tRED    %s\n", ANSI_COLOR_RED,     ANSI_COLOR_RESET, FONT_COLOR_RED,     ANSI_COLOR_RESET);
	printf("\t%s\tGREEN  %s%s\tGREEN  %s\n", ANSI_COLOR_GREEN,   ANSI_COLOR_RESET, FONT_COLOR_GREEN,   ANSI_COLOR_RESET);
	printf("\t%s\tYELLOW %s%s\tYELLOW %s\n", ANSI_COLOR_YELLOW,  ANSI_COLOR_RESET, FONT_COLOR_YELLOW,  ANSI_COLOR_RESET);
	printf("\t%s\tBLUE   %s%s\tBLUE   %s\n", ANSI_COLOR_BLUE,    ANSI_COLOR_RESET, FONT_COLOR_BLUE,    ANSI_COLOR_RESET);
	printf("\t%s\tMAGENTA%s%s\tMAGENTA%s\n", ANSI_COLOR_MAGENTA, ANSI_COLOR_RESET, FONT_COLOR_MAGENTA, ANSI_COLOR_RESET);
	printf("\t%s\tCYAN   %s%s\tCYAN   %s\n", ANSI_COLOR_CYAN,    ANSI_COLOR_RESET, FONT_COLOR_CYAN,    ANSI_COLOR_RESET);
	printf("\t%s\tGRAY   %s%s\tGRAY   %s\n", ANSI_COLOR_GRAY,    ANSI_COLOR_RESET, FONT_COLOR_GRAY,    ANSI_COLOR_RESET);
	printf("\t%s\tDEFAULT%s%s\tDEFAULT%s\n", ANSI_COLOR_DEFAULT, ANSI_COLOR_RESET, FONT_COLOR_DEFAULT, ANSI_COLOR_RESET);
	printf("\n\n\t");

	for (int row = 1; row < 10; row++) {
		for (int col = 1; col < 10; col++) {
			char buf1[64];
			char buf2[64];
			snprintf(buf1, sizeof(buf1), "\x1b[%d;%dm", 30 + row, 40 + col);
			snprintf(buf2, sizeof(buf2), "\\x1b[%d;%dm", 30 + row, 40 + col);
			printf("%s%s%s ", buf1, buf2, ANSI_COLOR_RESET);
		}
		printf("\n\t");
	}
	printf("\n\n\t");

	for (int row = 1; row < 10; row++) {
		for (int col = 1; col < 10; col++) {
			char buf1[64];
			char buf2[64];
			snprintf(buf1, sizeof(buf1), "\x1b[%d;%d;7m", 30 + row, 40 + col);
			snprintf(buf2, sizeof(buf2), "\\x1b[%d;%d;7m", 30 + row, 40 + col);
			printf("%s%s%s ", buf1, buf2, ANSI_COLOR_RESET);
		}
		printf("\n\t");
	}

	return 0;
}
