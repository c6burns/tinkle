#include "IUnityInterface.h"
#include "IUnityProfilerCallbacks.h"

#include <algorithm>
#include <string>

static IUnityProfilerCallbacks *s_UnityProfilerCallbacks = NULL;
static UnityProfilerMarkerDesc gcAllocDesc;
static std::string giantString;

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API tinkletytinkerz(char *out_str, size_t out_len)
{
	const size_t cpysz = std::min<size_t>(giantString.length(), out_len);
	giantString.copy(out_str, cpysz, 0);
	return;
}

void UNITY_INTERFACE_API createCategoryCallback(const UnityProfilerCategoryDesc* categoryDesc, void* userData)
{
	return;
}

void UNITY_INTERFACE_API markerEventCallback(const UnityProfilerMarkerDesc* markerDesc, UnityProfilerMarkerEventType eventType, uint16_t eventDataCount, const UnityProfilerMarkerData* eventData, void* userData)
{
	return;
}

void UNITY_INTERFACE_API createMarkerCallback(const UnityProfilerMarkerDesc* markerDesc, void* userData)
{
	giantString.append(markerDesc->name);
	giantString.push_back('\n');
	if (markerDesc->name == "GC.Alloc") {
		memcpy(&gcAllocDesc, markerDesc, sizeof(gcAllocDesc));
		s_UnityProfilerCallbacks->RegisterMarkerEventCallback(markerDesc, markerEventCallback, NULL);
	}
	return;
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
	s_UnityProfilerCallbacks = unityInterfaces->Get<IUnityProfilerCallbacks>();

	memset(&gcAllocDesc, 0, sizeof(gcAllocDesc));
	gcAllocDesc.name = "GC.Alloc";
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