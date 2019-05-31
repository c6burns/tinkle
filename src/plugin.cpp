#include "IUnityInterface.h"
#include "IUnityProfilerCallbacks.h"

#include <cassert>
#include <algorithm>
#include <string>
#include <unordered_map>
#include <mutex>
#include <thread>

typedef std::unordered_map<UnityProfilerCategoryId, const UnityProfilerCategoryDesc *> CategoryMap;
CategoryMap categoryMap;

static IUnityProfilerCallbacksV2 *s_UnityProfilerCallbacks = NULL;
static const UnityProfilerMarkerDesc *gcAllocDesc = NULL;
static const UnityProfilerCategoryDesc *gcCatDesc = NULL;


struct BacktraceFrame
{
	int line;
	char func[128 - 4];
	char file[128];
};

struct Backtrace
{
	int numFrames;
	int maxFrames;
	struct BacktraceFrame frameList[128];

	Backtrace()
	{
		numFrames = 0;
		maxFrames = 128;
		memset(frameList, 0, sizeof(frameList));
	}
};


typedef int(*BacktraceCallback)(struct Backtrace *bt);
BacktraceCallback btCallback = NULL;

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API RegisterBacktraceCallback(BacktraceCallback cb)
{
	assert(cb);
	btCallback = cb;
}

void UNITY_INTERFACE_API createCategoryCallback(const UnityProfilerCategoryDesc* categoryDesc, void* userData)
{
	if (!strcmp(categoryDesc->name, "GC")) {
		gcCatDesc = categoryDesc;
	}
	categoryMap.insert(std::make_pair(categoryDesc->id, categoryDesc));
	return;
}

void UNITY_INTERFACE_API markerEventCallback(const UnityProfilerMarkerDesc* markerDesc, UnityProfilerMarkerEventType eventType, uint16_t eventDataCount, const UnityProfilerMarkerData* eventData, void* userData)
{
	CategoryMap::iterator cat_it = categoryMap.find(markerDesc->categoryId);
	if (cat_it == categoryMap.end()) return;

	int getStackFrames = 0;
	if (gcAllocDesc && gcAllocDesc->id == markerDesc->id) {
		// is GC.Alloc
		getStackFrames = 1;
	} else if (gcCatDesc && gcCatDesc->id == markerDesc->categoryId) {
		// is in the GC category
		getStackFrames = 1;
	}

	if (!getStackFrames) return;

	if (!btCallback) return;

	struct Backtrace *bt = new Backtrace();
	(*btCallback)(bt);
	delete bt;
}

void UNITY_INTERFACE_API createMarkerCallback(const UnityProfilerMarkerDesc* markerDesc, void* userData)
{
	s_UnityProfilerCallbacks->RegisterMarkerEventCallback(markerDesc, markerEventCallback, NULL);

	size_t found;
	std::string markerName(markerDesc->name);
	if (!markerName.compare("GC.Alloc")) {
		gcAllocDesc = markerDesc;
	}
}

void UNITY_INTERFACE_API frameCallback(void* userData)
{
	return;
}

void UNITY_INTERFACE_API threadCallback(const UnityProfilerThreadDesc* threadDesc, void* userData)
{
	return;
}

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API UnityPluginLoad(IUnityInterfaces *unityInterfaces)
{
	s_UnityProfilerCallbacks = unityInterfaces->Get<IUnityProfilerCallbacksV2>();
	s_UnityProfilerCallbacks->RegisterCreateMarkerCallback(createMarkerCallback, NULL);
	s_UnityProfilerCallbacks->RegisterFrameCallback(frameCallback, NULL);
	s_UnityProfilerCallbacks->RegisterCreateThreadCallback(threadCallback, NULL);
	s_UnityProfilerCallbacks->RegisterCreateCategoryCallback(createCategoryCallback, NULL);
}

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API UnityPluginUnload()
{
	s_UnityProfilerCallbacks->UnregisterFrameCallback(frameCallback, NULL);
	s_UnityProfilerCallbacks->UnregisterCreateMarkerCallback(createMarkerCallback, NULL);
	s_UnityProfilerCallbacks->UnregisterCreateThreadCallback(threadCallback, NULL);
	s_UnityProfilerCallbacks->UnregisterCreateCategoryCallback(createCategoryCallback, NULL);
}