/*
	Aggiungere schermo che emette luce
	La palla di luce deve girare su una superficie sferica
*/


void A16::createFloor(std::vector<VertexMesh> &vDef, std::vector<uint32_t> &vIdx) {
    vDef.push_back({{-1,0,0}, {0,1,0}, {0,0}});	// vertex 1 - Position and Normal
	vDef.push_back({{0,0,1}, {0,1,0}, {0,0}});	// vertex 2 - Position and Normal
	vDef.push_back({ {1,0,0}, {0,1,0}, {0,0} });
	vDef.push_back({ {0,0,-1}, {0,1,0}, {0,0} });

	//vIdx.push_back(0); vIdx.push_back(1); vIdx.push_back(2);
	vIdx.push_back(1); vIdx.push_back(2); vIdx.push_back(3);
	vIdx.push_back(1); vIdx.push_back(3); vIdx.push_back(0);
}

void A16::createScreen(std::vector<VertexMesh>& vDef, std::vector<uint32_t>& vIdx, float textureHeight, float textureWidth) {
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

void A16::createSphereMesh(std::vector<VertexMesh> &vDef, std::vector<uint32_t> &vIdx) {
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

			if (frag == 0 || frag == fragsPerStrip) {
				printf("%f, %f, %f\n", x, y, z);
			}

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
