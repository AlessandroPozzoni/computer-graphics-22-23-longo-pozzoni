/*
	Aggiungere schermo che emette luce
	La palla di luce deve girare su una superficie sferica
*/


void ProductShowcase::createFloor(std::vector<VertexMesh> &vDef, std::vector<uint32_t> &vIdx) {
    vDef.push_back({{-1,0,0}, {0,1,0}, {0,0}});	// vertex 1 - Position and Normal
	vDef.push_back({{0,0,1}, {0,1,0}, {0,0}});	// vertex 2 - Position and Normal
	vDef.push_back({ {1,0,0}, {0,1,0}, {0,0} });
	vDef.push_back({ {0,0,-1}, {0,1,0}, {0,0} });

	//vIdx.push_back(0); vIdx.push_back(1); vIdx.push_back(2);
	vIdx.push_back(1); vIdx.push_back(2); vIdx.push_back(3);
	vIdx.push_back(1); vIdx.push_back(3); vIdx.push_back(0);
}

void ProductShowcase::createScreen(std::vector<VertexMesh>& vDef, std::vector<uint32_t>& vIdx, float textureHeight, float textureWidth) {
	vDef.push_back({ {-textureWidth / textureHeight / 2, 0, -textureHeight / textureHeight / 2}, {0,1,0}, {0.0f,0.01f} });	// vertex 1 - Position and Normal
	vDef.push_back({ {-textureWidth / textureHeight / 2, 0,textureHeight / textureHeight / 2}, {0,1,0}, {0,1} });	// vertex 2 - Position and Normal
	vDef.push_back({ {textureWidth / textureHeight / 2, 0,textureHeight / textureHeight / 2}, {0,1,0}, {1,1} });
	vDef.push_back({ {textureWidth / textureHeight / 2, 0, -textureHeight / textureHeight / 2}, {0,1,0}, {1.0f,0.01f} });

	//vIdx.push_back(0); vIdx.push_back(1); vIdx.push_back(2);
	vIdx.push_back(1); vIdx.push_back(2); vIdx.push_back(3);
	vIdx.push_back(1); vIdx.push_back(3); vIdx.push_back(0);
}


void drawTriangle(std::vector<uint32_t>& vIdx, int v1, int v2, int v3) {
	vIdx.push_back(v1); vIdx.push_back(v2); vIdx.push_back(v3);
}

#define M_PI 3.141595f
#define NUM_OF_STRIPS 60
#define FRAGS_PER_STRIP 180

glm::vec2 inUV(int strip, int frag) {

	float s = strip;
	float f = frag;

	float u = f / FRAGS_PER_STRIP;
	float v = s / NUM_OF_STRIPS;

	return glm::vec2(u, v);
}

void ProductShowcase::createSphereMesh(std::vector<VertexMesh> &vDef, std::vector<uint32_t> &vIdx) {
	// The primitive built here is a sphere of radius 1, centered in the origin, on which the Mars texture is applied seamless.
	
	// The procedure fills array vPos with the positions of the vertices and of the normal vectors of the mesh

	int fragsPerStrip = FRAGS_PER_STRIP;
	int numOfStrips = NUM_OF_STRIPS;
	float r = 1.0f;

	float vertAng = M_PI / numOfStrips;
	float horAng = 2 * M_PI / fragsPerStrip;

	for (int strip = 0; strip <= numOfStrips; strip++) {
		for (int frag = 0; frag <= fragsPerStrip; frag++) {
			float x = r * sin(strip * vertAng) * cos(frag * horAng);
			float y = r * cos(strip * vertAng);
			float z = r * sin(strip * vertAng) * sin(frag * horAng);

			glm::vec3 pos = glm::vec3(x, y, z);
			glm::vec3 norm = glm::normalize(pos);

			vDef.push_back({ pos, norm, inUV(strip, frag)});
		}
	}

	for (int i = 0; i < numOfStrips; i++) {
		for (int j = 0; j < fragsPerStrip; j++)	{
			drawTriangle(vIdx, i * (fragsPerStrip + 1) + j, i * (fragsPerStrip + 1) + (j + 1), (i + 1) * (fragsPerStrip + 1) + j);
			drawTriangle(vIdx, (i + 1) * (fragsPerStrip + 1) + (j + 1), (i + 1) * (fragsPerStrip + 1) + j, i * (fragsPerStrip + 1) + (j + 1));
		}
	}
}

void ProductShowcase::createBox(std::vector<VertexMesh> &vDef, std::vector<uint32_t> &vIdx, float height, float width, float thickness) {
	// The primitive built here is a box centered in the origin, with proportions respecting the texture.

	// The procedure fills array vPos with the positions of the vertices and of the normal vectors of the mesh
	/*vDef.push_back({{0,0,0}, {0,0,1}, {0,0}});	// vertex 0 - Position, Normal and uv
	vDef.push_back({{1,0,0}, {0,0,1}, {1,0}});	// vertex 1 - Position and Normal
	vDef.push_back({{0,1,0}, {0,0,1}, {0,1}});	// vertex 2 - Position and Normal
	vDef.push_back({{1,1,0}, {0,0,1}, {1,1}});// vertex 3 - Position and Normal*/

	/* 
	coordinates of the rect ? ?
	assuming that 
		dx = 91
		dz = 347
		dy = 322
		
	*/

	// float halfDx = 0.5f; -> 1.0f
	// float halfDy = 0.025f; -> 0.05f
	// float halfDz = 0.5f; -> 1.0f
	
	float halfDx = width / 2.0f;
	float halfDy = thickness / 2.0f;
	float halfDz = height / 2.0f;

	vDef.push_back({ {-halfDx, -halfDy, -halfDz}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f} });	// vertex 0.0 - Position and Normal
	vDef.push_back({ {-halfDx, -halfDy, -halfDz}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f} });	// vertex 0.1 - Position and Normal
	vDef.push_back({ {-halfDx, -halfDy, -halfDz}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f} });	// vertex 0.2 - Position and Normal

	vDef.push_back({ {halfDx, -halfDy, -halfDz}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f} });	// vertex 1.3 - Position and Normal
	vDef.push_back({ {halfDx, -halfDy, -halfDz}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f} });	// vertex 1.4 - Position and Normal
	vDef.push_back({ {halfDx, -halfDy, -halfDz}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f} });	// vertex 1.5 - Position and Normal

	vDef.push_back({ {-halfDx, -halfDy, halfDz}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f} });	// vertex 2.6 - Position and Normal
	vDef.push_back({ {-halfDx, -halfDy, halfDz}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f} });	// vertex 2.7 - Position and Normal
	vDef.push_back({ {-halfDx, -halfDy, halfDz}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f} });	// vertex 2.8 - Position and Normal

	vDef.push_back({ {halfDx, -halfDy, halfDz}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f} });	// vertex 3.9 - Position and Normal
	vDef.push_back({ {halfDx, -halfDy, halfDz}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f} });	// vertex 3.10 - Position and Normal
	vDef.push_back({ {halfDx, -halfDy, halfDz}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f} });	// vertex 3.11 - Position and Normal



	vDef.push_back({ {-halfDx, halfDy, -halfDz}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f} });	// vertex 4.12 - Position and Normal
	vDef.push_back({ {-halfDx, halfDy, -halfDz}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}});	// vertex 4.13 - Position and Normal
	vDef.push_back({ {-halfDx, halfDy, -halfDz}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f} });	// vertex 4.14 - Position and Normal

	vDef.push_back({ {halfDx, halfDy, -halfDz}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f} });	// vertex 5.15 - Position and Normal
	vDef.push_back({ {halfDx, halfDy, -halfDz}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f} });	// vertex 5.16 - Position and Normal
	vDef.push_back({ {halfDx, halfDy, -halfDz}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f} });	// vertex 5.17 - Position and Normal

	vDef.push_back({ {-halfDx, halfDy, halfDz}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f} });	// vertex 6.18 - Position and Normal
	vDef.push_back({ {-halfDx, halfDy, halfDz}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f} });	// vertex 6.19 - Position and Normal
	vDef.push_back({ {-halfDx, halfDy, halfDz}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f} });	// vertex 6.20 - Position and Normal

	vDef.push_back({ {halfDx, halfDy, halfDz}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f} });	// vertex 7.21 - Position and Normal
	vDef.push_back({ {halfDx, halfDy, halfDz}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f} });	// vertex 7.22 - Position and Normal
	vDef.push_back({ {halfDx, halfDy, halfDz}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f} });	// vertex 7.23 - Position and Normal

	// Fill the array vIdx with the indices of the vertices of the triangles
	vIdx.push_back(1); vIdx.push_back(4); vIdx.push_back(7);	// First triangle base -> bottom of the texture
	vIdx.push_back(7); vIdx.push_back(4); vIdx.push_back(10);	// Second triangle base -> bottom of the texture
	
	vIdx.push_back(16); vIdx.push_back(13); vIdx.push_back(19);	// First triangle top -> top of the texture
	vIdx.push_back(16); vIdx.push_back(19); vIdx.push_back(22);	// Second triangle top -> top of the texture
	
	vIdx.push_back(0); vIdx.push_back(6); vIdx.push_back(12);	// First triangle left -> back of the texture
	vIdx.push_back(12); vIdx.push_back(6); vIdx.push_back(18);	// Second triangle left -> back of the texture 

	vIdx.push_back(9); vIdx.push_back(3); vIdx.push_back(15);	// First triangle right -> front of the texture
	vIdx.push_back(9); vIdx.push_back(15); vIdx.push_back(21);	// Second triangle right -> front of the texture

	vIdx.push_back(5); vIdx.push_back(2); vIdx.push_back(14);	// First triangle back -> right of the texture
	vIdx.push_back(5); vIdx.push_back(14); vIdx.push_back(17);	// Second triangle back -> right of the texture
	
	vIdx.push_back(8); vIdx.push_back(11); vIdx.push_back(20);	// First triangle front -> left of the texture
	vIdx.push_back(20); vIdx.push_back(11); vIdx.push_back(23);	// Second triangle front -> left of the texture
}
