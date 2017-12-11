#pragma once
#include <OutputStream.hpp>
#include "LoggerWidget.hpp"

class LoggerWidget;

class EditorOutputStream : public Poly::FileOutputStream
{
public:
	EditorOutputStream(const char* name, LoggerWidget* widget);
	void Append(const char* data) override;
private:
	LoggerWidget* Logger = nullptr;
};