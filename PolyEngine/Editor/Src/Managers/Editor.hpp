#pragma once

#include <Editor/IEditor.hpp>

using namespace Poly;

class Editor : public IEditor
{
public:
	Editor() {}

	void Init();
	void Deinit();

private:
};