#ifdef DEBUG
#define debug_printf(format, ...) printf("[DEBUG] " format, ##__VA_ARGS__)
#else
#define debug_printf(format, ...)
#endif