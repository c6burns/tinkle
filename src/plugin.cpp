#include "IUnityInterface.h"
#include "IUnityProfilerCallbacks.h"


#include <string>

static IUnityProfilerCallbacks *s_UnityProfilerCallbacks = NULL;
static UnityProfilerMarkerDesc gcAllocDesc;

void UNITY_INTERFACE_API createCategoryCallback(const UnityProfilerCategoryDesc* categoryDesc, void* userData)
{
	return;
}

void UNITY_INTERFACE_API createMarkerCallback(const UnityProfilerMarkerDesc* markerDesc, void* userData)
{
	return;
}

void UNITY_INTERFACE_API markerEventCallback(const UnityProfilerMarkerDesc* markerDesc, UnityProfilerMarkerEventType eventType, uint16_t eventDataCount, const UnityProfilerMarkerData* eventData, void* userData)
{
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

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API UnityPluginLoad(IUnityInterfaces* unityInterfaces)
{
	s_UnityProfilerCallbacks = unityInterfaces->Get<IUnityProfilerCallbacks>();

	memset(&gcAllocDesc, 0, sizeof(gcAllocDesc));
	gcAllocDesc.name = "GC.Alloc";
	s_UnityProfilerCallbacks->RegisterMarkerEventCallback(&gcAllocDesc, markerEventCallback, NULL);
	s_UnityProfilerCallbacks->RegisterCreateMarkerCallback(createMarkerCallback, NULL);
}

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API UnityPluginUnload()
{
	s_UnityProfilerCallbacks->UnregisterMarkerEventCallback(&gcAllocDesc, markerEventCallback, NULL);
	s_UnityProfilerCallbacks->UnregisterCreateMarkerCallback(createMarkerCallback, NULL);
}