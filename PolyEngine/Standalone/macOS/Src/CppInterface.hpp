#pragma once

void PolyEngineLoad(void* window, unsigned width, unsigned height);
void PolyEngineUpdate();
void PolyEngineRelease();
void PolyEngineResize(unsigned width, unsigned height);
void PolyEngineKeyDown(unsigned keyCode);
void PolyEngineKeyUp(unsigned keyCode);
void PolyEngineUpdateMousePos(unsigned x, unsigned y);
void PolyEngineUpdateWheelPos(unsigned x, unsigned y);