#pragma once

class IDialog
{
public:
	// Resets everything so single dialog instance can be used multiple times.
	virtual void Reset() = 0;

	// If operation was canceled by user this function will return true.
	virtual void OperationCanceled() = 0;
};