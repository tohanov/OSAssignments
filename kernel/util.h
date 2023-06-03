
#ifndef NULL
#define NULL (0)
#endif 

typedef char bool;
#define FALSE ((bool)0)
#define TRUE ((bool)1)

#define COLOR_NC "\e[0m"
#define COLOR_BLACK "\e[0;30m"
#define COLOR_GRAY "\e[1;30m"
#define COLOR_RED "\e[0;31m"
#define COLOR_LIGHT_RED "\e[1;31m"
#define COLOR_GREEN "\e[0;32m"
#define COLOR_LIGHT_GREEN "\e[1;32m"
#define COLOR_BROWN "\e[0;33m"
#define COLOR_YELLOW "\e[1;33m"
#define COLOR_BLUE "\e[0;34m"
#define COLOR_LIGHT_BLUE "\e[1;34m"
#define COLOR_PURPLE "\e[0;35m"
#define COLOR_LIGHT_PURPLE "\e[1;35m"
#define COLOR_CYAN "\e[0;36m"
#define COLOR_LIGHT_CYAN "\e[1;36m"
#define COLOR_LIGHT_GRAY "\e[0;37m"
#define COLOR_WHITE "\e[1;37m"
#define COLOR_ORANGE "\e[38;2;255;165;0m"


// #define DEBUG_PRINTS
// #define WARNING_PRINTS
// #define ERROR_PRINTS

#ifdef DEBUG_PRINTS
	#define debug_print(...) { printf(COLOR_YELLOW"[🤓] "); printf(__VA_ARGS__); printf("\n"COLOR_NC); }
#else
	#define debug_print(...) {}
#endif

#ifdef ERROR_PRINTS
	#define error_print(...) { printf(COLOR_RED"[👿] "); printf(__VA_ARGS__); printf("\n"COLOR_NC); }
	#define assert_print(x) {if (!(x)) { error_print(#x" doesn't hold") }}
#else
	#define error_print(...) {}
	#define assert_print(x) {}
#endif

#ifdef WARNING_PRINTS
	#define warning_print(...) { printf(COLOR_ORANGE"[🚨] "); printf(__VA_ARGS__); printf("\n"COLOR_NC); }
#else
	#define warning_print(...) {}
#endif