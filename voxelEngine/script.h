#pragma once

#include "engine.h"
class Script {


public:

	Script* script;
	render* rendering;

	Script(render* render)
	{
		rendering = render;
	}


	virtual void Update() {};
	virtual void Start() {}
	int index = 0;

};

