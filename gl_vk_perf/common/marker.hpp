#ifndef __H_MARKER__
#define __H_MARKER__

#include <nvToolsExt.h>

class MarkerScoped
{
	public:
		MarkerScoped(uint32_t category, uint32_t color, const char *name);
		~MarkerScoped();
};

class Marker
{
	public:
		static const uint32_t CAT_SETUP = 1;
		static const uint32_t CAT_FRAME = 2;
		static const uint32_t CAT_LOADING = 3;

		static const uint32_t COL_RED = 0xFF0000;
		static const uint32_t COL_GREEN = 0x00FF00;
		static const uint32_t COL_BLUE = 0x0000FF;
		static const uint32_t COL_YELLOW = 0xFFFF00;
		static const uint32_t COL_MAGENTA = 0xFF00FF;
		static const uint32_t COL_CYAN = 0x00FFFF;

		Marker();
		Marker(Marker const&) = delete;
		void operator=(Marker const&) = delete;

		static Marker& getInstance()
		{
			static Marker marker;

			return marker;
		}

	public:
		static void nameCurrentThread(const char *name);
		static void nameCategory(uint32_t category, const char *name);

		static nvtxRangeId_t startRange(uint32_t category, uint32_t color, const char *name);
		static void endRange(nvtxRangeId_t range);

		static void event(uint32_t category, uint32_t color, const char *name);
};

#endif