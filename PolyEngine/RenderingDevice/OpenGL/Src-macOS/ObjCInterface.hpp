#pragma once

// returns view instance
void* CreateDeviceImpl(void* window, unsigned width, unsigned height);
void DestroyDeviceImpl(void* window);
void EndFrameImpl(void* window);