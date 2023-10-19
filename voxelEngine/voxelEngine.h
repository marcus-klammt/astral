#pragma once
#include "engine.h"
#include <vector>

class VoxelEngine {
public:
	VoxelEngine()
	{
	}

	render m_rendering;
	int m_scriptsLoaded;

	void AddScript(Script script)
	{
		m_scriptsLoaded++;
		script.index = m_scriptsLoaded;
		m_rendering.scripts.push_back(script);
		
	}
	void RemoveScript(Script script)
	{
		m_rendering.scripts.erase(m_rendering.scripts.begin() + script.index - 1);
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

