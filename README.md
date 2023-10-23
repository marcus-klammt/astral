# astral
Voxel engine written in C++/OpenGL

Currently features noise based chunk generation, imgui implementation, and face culling.

# Chunks
Chunks are 16x64x16 blocks of terrain which can be generated using a seed. Chunks are multithreaded but still not optimized up to standards.

![2023-10-22 21-39-25 (2)](https://github.com/marcus-klammt/astral/assets/55520137/0f504b89-7360-4648-9d08-e12e36df98d4)

# Culling
Chunks are only rendered if the player can see the terrain, meaning there is no vertices that are rendered underground. This allows for many chunks to be loaded at once with little strain on the gpu.

![2023-10-22 21-40-32](https://github.com/marcus-klammt/astral/assets/55520137/89b06c23-fc93-410c-9c51-18be077c2cff)

Public Roadmap: https://trello.com/b/Z0PYRrRu

