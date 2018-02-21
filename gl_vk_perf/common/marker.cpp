#include "marker.hpp"

#ifdef _WIN32

#include <Windows.h>

#define GetThreadID GetCurrentThreadId

#elif defined(__unix__)

#include <pthread.h>

#define GetThreadID pthread_self

#endif

Marker::Marker()
{
	nameCategory(1, "SETUP");
	nameCategory(2, "FRAME");
}

void Marker::nameCurrentThread(const char *name)
{
	nvtxNameOsThreadA(GetThreadID(), "main");
}

void Marker::nameCategory(uint32_t category, const char *name)
{
	nvtxNameCategoryA(category, name);
}

nvtxRangeId_t Marker::startRange(uint32_t category, uint32_t color, const char *name)
{
	nvtxEventAttributes_t eventAttrib{};
	eventAttrib.version = NVTX_VERSION;
	eventAttrib.size = NVTX_EVENT_ATTRIB_STRUCT_SIZE;
	eventAttrib.category = category;
	eventAttrib.colorType = NVTX_COLOR_ARGB;
	eventAttrib.color = 0xFF000000 | color;
	eventAttrib.messageType = NVTX_MESSAGE_TYPE_ASCII;
	eventAttrib.message.ascii = name;

	return nvtxRangeStartEx(&eventAttrib);
}

void Marker::endRange(nvtxRangeId_t range)
{
	nvtxRangeEnd(range);
}

void Marker::event(uint32_t category, uint32_t color, const char *name)
{
	nvtxEventAttributes_t eventAttrib{};
	eventAttrib.version = NVTX_VERSION;
	eventAttrib.size = NVTX_EVENT_ATTRIB_STRUCT_SIZE;
	eventAttrib.category = category;
	eventAttrib.colorType = NVTX_COLOR_ARGB;
	eventAttrib.color = 0xFF000000 | color;
	eventAttrib.messageType = NVTX_MESSAGE_TYPE_ASCII;
	eventAttrib.message.ascii = name;

	nvtxMarkEx(&eventAttrib);
}

MarkerScoped::MarkerScoped(uint32_t category, uint32_t color, const char *name)
{
	nvtxEventAttributes_t eventAttrib{};
	eventAttrib.version = NVTX_VERSION;
	eventAttrib.size = NVTX_EVENT_ATTRIB_STRUCT_SIZE;
	eventAttrib.category = category;
	eventAttrib.colorType = NVTX_COLOR_ARGB;
	eventAttrib.color = 0xFF000000 | color;
	eventAttrib.messageType = NVTX_MESSAGE_TYPE_ASCII;
	eventAttrib.message.ascii = name;

	nvtxRangePushEx(&eventAttrib);
}

MarkerScoped::~MarkerScoped()
{
	nvtxRangePop();
}