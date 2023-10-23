#include "voxelEngine.h"
#include "FastNoiseLight.h"
#include <thread>

//right now i have all the scripts in this class, which is not very clean, however the rest of the code is generally clean
class VoxelGeneration : public Script {
public:
	struct Vertex {
		glm::vec3 pos;
		glm::vec3 color;
	};

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	Shader shader;

	glm::vec3 lightDirection = glm::vec3(1,1,4);

	//our chunk size, set to 64 max height so chunks are super slow
	int GRID_SIZE_X = 16;
	int GRID_SIZE_Y = 64;
	int GRID_SIZE_Z = 16;

	//seed
	int seedNum = 123;

	//trying to cache our vector size function
	int gridSizeX = 0;
	int gridSizeY = 0;
	int gridSizeZ = 0;

	unsigned int VAO, VBO, EBO;

	VoxelGeneration(render* render)
	{
		rendering = render;
		script = this;
	}

	//clamp not working lol??
	template <typename T>
	T clip(const T& n, const T& lower, const T& upper) {
		return std::max(lower, std::min(n, upper));
	}

	using VoxelGrid = std::vector<std::vector<std::vector<int>>>;

	VoxelGrid createEmptyVoxelGrid(int sizeX, int sizeY, int sizeZ) {
		return VoxelGrid(sizeX, std::vector<std::vector<int>>(sizeY, std::vector<int>(sizeZ, 0)));
	}

	VoxelGrid generateVoxelGrid(int seed)
	{
		VoxelGrid voxelGrid = createEmptyVoxelGrid(GRID_SIZE_X, GRID_SIZE_Y, GRID_SIZE_Z);

		FastNoiseLite noiseGen = (seed);
		//open to change settings etc.
		noiseGen.SetFractalOctaves(4);
		noiseGen.SetFractalLacunarity(1.75);
		noiseGen.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
		noiseGen.SetFractalType(FastNoiseLite::FractalType_FBm);

		//loop through our set chunk size
		//we do two loops to make sure culling of inside faces work, i need to fix this as we end up calling our noise function two times more then we need
		for (int x = 0; x < GRID_SIZE_X; ++x)
		{
			for (int z = 0; z < GRID_SIZE_Z; ++z)
			{
				float noiseValue = noiseGen.GetNoise(static_cast<float>(x), static_cast<float>(z) / 2.0f + .5f);

				int32_t height = 10 + static_cast<int32_t>(noiseValue * 45);
				//clamp
				height = clip(height, 0, 64);
				//set the bottom of our voxel grid
				voxelGrid[x][0][z] = 1;

				for (int32_t y = 0; y < height; y++)
				{
					//set voxels for however high the noise gets us
					voxelGrid[x][y][z] = 1;
				}
			}
		}

		//caching our 3d vector sizes to try and sqeeze performance
		gridSizeX = voxelGrid.size();
		gridSizeY = voxelGrid[0].size();
		gridSizeZ = voxelGrid[0][0].size();

		//same idea just actually placing the blocks now
		for (int x = 0; x < GRID_SIZE_X; ++x)
		{
			for (int z = 0; z < GRID_SIZE_Z; ++z)
			{
				float noiseValue = noiseGen.GetNoise(static_cast<float>(x), static_cast<float>(z) / 2.0f + .5f);

				int32_t height = 10 + static_cast<int32_t>(noiseValue * 45);

				height = clip(height, 0, 64);
				placeBlock(voxelGrid, x, 0, z);

				for (int32_t y = 0; y < height; y++)
				{
					placeBlock(voxelGrid, x, y, z);

				}
			}
		}
		return voxelGrid;
	}
	//voxel check
	bool hasVoxelAt(VoxelGrid grid, int x, int y, int z)
	{
		if (x >= 0 && x < gridSizeX && y >= 0 && y < gridSizeY && z >= 0 && z < gridSizeZ) {
			return grid[x][y][z] == 1;
		}
		return false;
	}
	//placing blocks, features face culling
	void placeBlock(VoxelGrid voxelGrid, int x, int y, int z)
	{
		//random color for each block
		glm::vec3 color = glm::vec3(
			static_cast<float>((double)rand() / (RAND_MAX + 1.0)),
			static_cast<float>((double)rand() / (RAND_MAX + 1.0)),
			static_cast<float>((double)rand() / (RAND_MAX + 1.0))
		);

		if (x == GRID_SIZE_X - 1 || !hasVoxelAt(voxelGrid, x + 1, y, z))
		{
			vertices.push_back({ glm::vec3(x + 1, y, z), color });
			vertices.push_back({ glm::vec3(x + 1, y + 1, z), color });
			vertices.push_back({ glm::vec3(x + 1, y + 1, z + 1), color });
			vertices.push_back({ glm::vec3(x + 1, y, z + 1), color });

			unsigned int baseIndex = static_cast<unsigned int>(vertices.size()) - 4;
			indices.push_back(baseIndex);
			indices.push_back(baseIndex + 1);
			indices.push_back(baseIndex + 2);
			indices.push_back(baseIndex);
			indices.push_back(baseIndex + 2);
			indices.push_back(baseIndex + 3);
		}

		// Check neighbors in the negative X direction
		if (x == 0 || !hasVoxelAt(voxelGrid, x - 1, y, z)) {
			// Add faces for the negative X direction
			vertices.push_back({ glm::vec3(x, y, z), color });
			vertices.push_back({ glm::vec3(x, y + 1, z), color });
			vertices.push_back({ glm::vec3(x, y + 1, z + 1), color });
			vertices.push_back({ glm::vec3(x, y, z + 1), color });

			unsigned int baseIndex = static_cast<unsigned int>(vertices.size()) - 4;
			indices.push_back(baseIndex);
			indices.push_back(baseIndex + 1);
			indices.push_back(baseIndex + 2);
			indices.push_back(baseIndex);
			indices.push_back(baseIndex + 2);
			indices.push_back(baseIndex + 3);
		}

		// Check neighbors in the positive Y direction
		if (y == GRID_SIZE_Y - 1 || !hasVoxelAt(voxelGrid, x, y + 1, z)) {
			// Add faces for the positive Y direction
			vertices.push_back({ glm::vec3(x, y + 1, z), color });
			vertices.push_back({ glm::vec3(x + 1, y + 1, z), color });
			vertices.push_back({ glm::vec3(x + 1, y + 1, z + 1), color });
			vertices.push_back({ glm::vec3(x, y + 1, z + 1), color });

			unsigned int baseIndex = static_cast<unsigned int>(vertices.size()) - 4;
			indices.push_back(baseIndex);
			indices.push_back(baseIndex + 1);
			indices.push_back(baseIndex + 2);
			indices.push_back(baseIndex);
			indices.push_back(baseIndex + 2);
			indices.push_back(baseIndex + 3);
		}

		// Check neighbors in the negative Y direction
		if (y == 0 || !hasVoxelAt(voxelGrid, x, y - 1, z)) {
			// Add faces for the negative Y direction
			vertices.push_back({ glm::vec3(x, y, z), color });
			vertices.push_back({ glm::vec3(x + 1, y, z), color });
			vertices.push_back({ glm::vec3(x + 1, y, z + 1), color });
			vertices.push_back({ glm::vec3(x, y, z + 1), color });

			unsigned int baseIndex = static_cast<unsigned int>(vertices.size()) - 4;
			indices.push_back(baseIndex);
			indices.push_back(baseIndex + 1);
			indices.push_back(baseIndex + 2);
			indices.push_back(baseIndex);
			indices.push_back(baseIndex + 2);
			indices.push_back(baseIndex + 3);
		}

		// Check neighbors in the positive Z direction
		if (z == GRID_SIZE_Z - 1 || !hasVoxelAt(voxelGrid, x, y, z + 1)) {
			// Add faces for the positive Z direction
			vertices.push_back({ glm::vec3(x, y, z + 1), color });
			vertices.push_back({ glm::vec3(x + 1, y, z + 1), color });
			vertices.push_back({ glm::vec3(x + 1, y + 1, z + 1), color });
			vertices.push_back({ glm::vec3(x, y + 1, z + 1), color });

			unsigned int baseIndex = static_cast<unsigned int>(vertices.size()) - 4;
			indices.push_back(baseIndex);
			indices.push_back(baseIndex + 1);
			indices.push_back(baseIndex + 2);
			indices.push_back(baseIndex);
			indices.push_back(baseIndex + 2);
			indices.push_back(baseIndex + 3);
		}

		// Check neighbors in the negative Z direction
		if (z == 0 || !hasVoxelAt(voxelGrid, x, y, z - 1)) {
			// Add faces for the negative Z direction
			vertices.push_back({ glm::vec3(x, y, z), color });
			vertices.push_back({ glm::vec3(x + 1, y, z), color });
			vertices.push_back({ glm::vec3(x + 1, y + 1, z), color });
			vertices.push_back({ glm::vec3(x, y + 1, z), color });

			unsigned int baseIndex = static_cast<unsigned int>(vertices.size()) - 4;
			indices.push_back(baseIndex);
			indices.push_back(baseIndex + 1);
			indices.push_back(baseIndex + 2);
			indices.push_back(baseIndex);
			indices.push_back(baseIndex + 2);
			indices.push_back(baseIndex + 3);
		}
	}

	void generateMesh()
	{
		//call this for a new chunk
		vertices.clear();
		indices.clear();
		VoxelGrid voxelGrid = generateVoxelGrid(seedNum);
	}

	//called every frame
	void Update() override {
		glm::mat4 model = glm::mat4(1.0f);
		shader.setMatrix4x4("model", model);
		shader.setVector3("lightDirection", glm::normalize(lightDirection));
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
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		//multithread this chunk
		std::thread t(&VoxelGeneration::generateMesh, this);
		t.detach();

		shader.Set("shaders/vertex.txt", "shaders/fragment.txt");
		rendering->shaders.push_back(shader);
		shader.use();
	}


	//also called every frame
	void OnGui() override
	{
		ImGui::Begin("Voxel Rendering");
		ImGui::Separator();

		char buf[30];
		sprintf_s(buf, "Vertices in chunk: %d", vertices.size());
		ImGui::Text(buf);

		ImGui::InputInt("Chunk Seed", &seedNum);

		if (ImGui::Button("Regenerate Chunk"))
		{
			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &VBO);
			glGenBuffers(1, &EBO);

			//multithread this chunk
			std::thread t(&VoxelGeneration::generateMesh, this);
			t.detach();
		}

		ImGui::InputFloat("Light Direction X", &lightDirection.x);
		ImGui::InputFloat("Light Direction Y", &lightDirection.y);
		ImGui::InputFloat("Light Direction Z", &lightDirection.z);

		ImGui::End();
	}

	void OnClose() override
	{
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
	}
};

//seperate script for showing wireframe and other debugging things for future
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
};


int main()
{
	VoxelEngine engine = VoxelEngine(900, 700);

	VoxelGeneration voxels(&engine.m_rendering);
	engine.AddScript(voxels);

	Debug debug(&engine.m_rendering);
	engine.AddScript(debug);

	engine.initialize();
}

