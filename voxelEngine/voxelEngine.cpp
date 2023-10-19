#include "voxelEngine.h"


//right now i have all the scripts in this class, which is not very clean, however the rest of the code is generally clean
class VoxelGeneration : public Script {
public:

	Shader shader;

	VoxelGeneration(render* render)
	{
		rendering = render;
		script = this;
	}

	struct Vertex {
		glm::vec3 pos;
		glm::vec3 color;
	};

	struct VoxelData {
		glm::vec3 color;
	};

	int GRID_SIZE_X = 10;
	int GRID_SIZE_Y = 10;
	int GRID_SIZE_Z = 10;

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	void generateMesh()
	{
		vertices.clear();
		indices.clear();

		int count = 0;
		for (int x = 0; x < GRID_SIZE_X; ++x) {
			for (int y = 0; y < GRID_SIZE_Y; ++y) {
				for (int z = 0; z < GRID_SIZE_Z; ++z) {
					float voxelSize = 1.0f;
					float offsetX = x * voxelSize;
					float offsetY = y * voxelSize;
					float offsetZ = z * voxelSize;

					glm::vec3 color = glm::vec3(
						static_cast<float>(x) / GRID_SIZE_X,
						static_cast<float>(y) / GRID_SIZE_Y,
						static_cast<float>(z) / GRID_SIZE_Z
					);

					// Front face
					vertices.push_back({ glm::vec3(offsetX, offsetY, offsetZ), color });
					vertices.push_back({ glm::vec3(offsetX + voxelSize, offsetY, offsetZ), color });
					vertices.push_back({ glm::vec3(offsetX + voxelSize, offsetY + voxelSize, offsetZ), color });
					vertices.push_back({ glm::vec3(offsetX, offsetY + voxelSize, offsetZ), color });

					// Back face
					vertices.push_back({ glm::vec3(offsetX, offsetY, offsetZ + voxelSize), color });
					vertices.push_back({ glm::vec3(offsetX + voxelSize, offsetY, offsetZ + voxelSize), color });
					vertices.push_back({ glm::vec3(offsetX + voxelSize, offsetY + voxelSize, offsetZ + voxelSize), color });
					vertices.push_back({ glm::vec3(offsetX, offsetY + voxelSize, offsetZ + voxelSize), color });

					// Left face
					vertices.push_back({ glm::vec3(offsetX, offsetY, offsetZ + voxelSize), color });
					vertices.push_back({ glm::vec3(offsetX, offsetY, offsetZ), color });
					vertices.push_back({ glm::vec3(offsetX, offsetY + voxelSize, offsetZ), color });
					vertices.push_back({ glm::vec3(offsetX, offsetY + voxelSize, offsetZ + voxelSize), color });

					// Right face
					vertices.push_back({ glm::vec3(offsetX + voxelSize, offsetY, offsetZ), color });
					vertices.push_back({ glm::vec3(offsetX + voxelSize, offsetY, offsetZ + voxelSize), color });
					vertices.push_back({ glm::vec3(offsetX + voxelSize, offsetY + voxelSize, offsetZ + voxelSize), color });
					vertices.push_back({ glm::vec3(offsetX + voxelSize, offsetY + voxelSize, offsetZ), color });

					// Top face
					vertices.push_back({ glm::vec3(offsetX, offsetY + voxelSize, offsetZ), color });
					vertices.push_back({ glm::vec3(offsetX + voxelSize, offsetY + voxelSize, offsetZ), color });
					vertices.push_back({ glm::vec3(offsetX + voxelSize, offsetY + voxelSize, offsetZ + voxelSize), color });
					vertices.push_back({ glm::vec3(offsetX, offsetY + voxelSize, offsetZ + voxelSize), color });

					// Bottom face
					vertices.push_back({ glm::vec3(offsetX, offsetY, offsetZ), color });
					vertices.push_back({ glm::vec3(offsetX + voxelSize, offsetY, offsetZ), color });
					vertices.push_back({ glm::vec3(offsetX + voxelSize, offsetY, offsetZ + voxelSize), color });
					vertices.push_back({ glm::vec3(offsetX, offsetY, offsetZ + voxelSize), color });

					// Indices for the cube
					unsigned int baseIndex = static_cast<unsigned int>(vertices.size()) - 24;
					for (unsigned int i = 0; i < 6; ++i) {
						indices.push_back(baseIndex + i * 4);
						indices.push_back(baseIndex + i * 4 + 1);
						indices.push_back(baseIndex + i * 4 + 2);
						indices.push_back(baseIndex + i * 4);
						indices.push_back(baseIndex + i * 4 + 2);
						indices.push_back(baseIndex + i * 4 + 3);
					}
				}
			}
		}
	}
	unsigned int VAO, VBO, EBO;


	void Update() override {

		glm::mat4 model = glm::mat4(1.0f);

		glm::mat4 projection;
		projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

		shader.setMatrix4x4("model", model);
		shader.setMatrix4x4("projection", projection);
	

		//generates our little voxel chunk

		//bind em
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
		//our position
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		//our color
		glEnableVertexAttribArray(1);
		                                                                //neat little trick to get the memory offset of a varible in a struct // cast this to a pointer
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));

		

		//drawwww
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);



	}
	void Start() override 
	{

		//generate em
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		generateMesh();

		shader.Set("shaders/vertex.txt", "shaders/fragment.txt");
		rendering->shaders.push_back(shader);
		shader.use();
	}
};

class Debug : public Script {
public:
	Debug(render* render)
	{
		rendering = render;
		script = this;
	}

	bool isDebugModeOn = false;


	int flag = 0;
	void Update() override
	{
		if (GetAsyncKeyState(VK_TAB) & 0x8000 && flag == 0)
		{
			flag = 1;
			isDebugModeOn = !isDebugModeOn;
		}
		else if (GetAsyncKeyState(VK_TAB) == 0) 
		{
			flag = 0;
		}

		if (isDebugModeOn)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
	}
	void Start() 
	{

	}
};


int main()
{

	VoxelEngine engine = VoxelEngine();

	VoxelGeneration voxels(&engine.m_rendering);
	engine.AddScript(voxels);

	Debug debug(&engine.m_rendering);
	engine.AddScript(debug);

	engine.initialize();

}

