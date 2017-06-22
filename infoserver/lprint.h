#ifndef LPRINT_H
#define LPRINT_H

class lprint {
	public:
		static bool good(const char *str);
		static bool bad(const char *str);
		static bool notice(const char *str);

		static void disable_print();
		static void enable_print();
};

#endif
