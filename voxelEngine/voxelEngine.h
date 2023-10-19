#pragma once
#include "engine.h"
#include <vector>

class VoxelEngine {
public:

	render m_rendering;

	int m_scriptsLoaded;
	VoxelEngine()
	{
		m_rendering = render();
	}

	

	void AddScript(Script script)
	{
		m_scriptsLoaded++;
		script.index = m_scriptsLoaded;
		m_rendering.scripts.push_back(script);
		
	}
	void RemoveScript(Script script)
	{
	}
	
	void initialize()
	{
		m_rendering.initalize();
	}

	bool isKeyDown(int key)
	{
		return glfwGetKey(m_rendering.window, key);
	}
};

