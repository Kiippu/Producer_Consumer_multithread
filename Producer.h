#pragma once
#include "iBoundBufferObjects.h"
class Producer :
	public iBoundBufferObjects
{
public:
	Producer();
	~Producer();

	/// Pure Virtual methods
	bool get() { return true; };
	bool process() { return true; };
	bool busy() { return true; };
private:



};

