void drawTriangle(std::vector<uint32_t>& vIdx, int v1, int v2, int v3) {
	vIdx.push_back(v1); vIdx.push_back(v2); vIdx.push_back(v3);
}


void ProductShowcase::createFloor(std::vector<VertexMesh> &vDef, std::vector<uint32_t> &vIdx) {
    float theta = 1.0f;
	int approxNum = 360.f / theta;
	float r = 1.0f;

	vDef.push_back({ {0.0f, -1.0f, 0.0f}, {0.0f, -1.0f, 0.0f}, {0.0f,0.0f} }); // 0
	vDef.push_back({ {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f,0.0f}});  // 1


	for (int j = 0; j < 2; j++) {
		for (int i = 0; i < approxNum; i++) {

			glm::vec3 pos = glm::vec3(r * cos(glm::radians(i * theta)), -1.0f + j * 2.0f, r * sin(glm::radians(i * theta)));

			glm::vec3 normV = glm::vec3(0.0f, -1.0f + j * (2.0f), 0.0f);
			glm::vec3 normH = glm::vec3(r * cos(glm::radians(i * theta)), 0.0f, r * sin(glm::radians(i * theta)));
			normH = normH / glm::length(normH);

			vDef.push_back({ pos, normV, {0.0f,0.0f} });
			vDef.push_back({ pos, normH, {0.0f,0.0f} });
		}
	}
	

	for (int j = 0; j < 2; j++) {
		for (int i = 0; i < approxNum; i++) {

			drawTriangle(vIdx, 2 * j * approxNum + (2 * i) + 2, j, 2 * j * approxNum + (2 * ((i + 1) % approxNum)) + 2);

			if (j == 0) {
				drawTriangle(vIdx, 2 * ((i + 1) % approxNum) + 3, 2 * i + 3, 2 * approxNum + 2 * i + 3);
			}
			if (j == 1) {
				drawTriangle(vIdx, 2 * ((i + 1) % approxNum) + 3, 2 * approxNum + 2 * i + 3, 2 * approxNum + 2 * ((i + 1) % approxNum) + 3);
			}
		}
	}
}

void ProductShowcase::createScreen(std::vector<VertexMesh>& vDef, std::vector<uint32_t>& vIdx, float textureHeight, float textureWidth) {
	vDef.push_back({ {-textureWidth / textureHeight / 2, 0, -textureHeight / textureHeight / 2}, {0,1,0}, {0.0f,0.01f} });	// vertex 1 - Position and Normal
	vDef.push_back({ {-textureWidth / textureHeight / 2, 0,textureHeight / textureHeight / 2}, {0,1,0}, {0,1} });	// vertex 2 - Position and Normal
	vDef.push_back({ {textureWidth / textureHeight / 2, 0,textureHeight / textureHeight / 2}, {0,1,0}, {1,1} });
	vDef.push_back({ {textureWidth / textureHeight / 2, 0, -textureHeight / textureHeight / 2}, {0,1,0}, {1.0f,0.01f} });

	vIdx.push_back(1); vIdx.push_back(2); vIdx.push_back(3);
	vIdx.push_back(1); vIdx.push_back(3); vIdx.push_back(0);
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
	
	float halfDx = width / 2.0f;
	float halfDy = thickness / 2.0f;
	float halfDz = height / 2.0f;

	vDef.push_back({ {-halfDx, -halfDy, -halfDz}, {-1.0f, 0.0f, 0.0f}, {0.1f, 0.1f} });	// vertex 0.0 - Position and Normal
	vDef.push_back({ {-halfDx, -halfDy, -halfDz}, {0.0f, -1.0f, 0.0f}, {0.1f, 0.1f} });	// vertex 0.1 - Position and Normal
	vDef.push_back({ {-halfDx, -halfDy, -halfDz}, {0.0f, 0.0f, -1.0f}, {0.1f, 0.1f} });	// vertex 0.2 - Position and Normal

	vDef.push_back({ {halfDx, -halfDy, -halfDz}, {1.0f, 0.0f, 0.0f}, {0.1f, 0.1f} });	// vertex 1.3 - Position and Normal
	vDef.push_back({ {halfDx, -halfDy, -halfDz}, {0.0f, -1.0f, 0.0f}, {0.1f, 0.1f} });	// vertex 1.4 - Position and Normal
	vDef.push_back({ {halfDx, -halfDy, -halfDz}, {0.0f, 0.0f, -1.0f}, {0.1f, 0.1f} });	// vertex 1.5 - Position and Normal

	vDef.push_back({ {-halfDx, -halfDy, halfDz}, {-1.0f, 0.0f, 0.0f}, {0.1f, 0.1f} });	// vertex 2.6 - Position and Normal
	vDef.push_back({ {-halfDx, -halfDy, halfDz}, {0.0f, -1.0f, 0.0f}, {0.1f, 0.1f} });	// vertex 2.7 - Position and Normal
	vDef.push_back({ {-halfDx, -halfDy, halfDz}, {0.0f, 0.0f, 1.0f}, {0.1f, 0.1f} });	// vertex 2.8 - Position and Normal

	vDef.push_back({ {halfDx, -halfDy, halfDz}, {1.0f, 0.0f, 0.0f}, {0.1f, 0.1f} });	// vertex 3.9 - Position and Normal
	vDef.push_back({ {halfDx, -halfDy, halfDz}, {0.0f, -1.0f, 0.0f}, {0.1f, 0.1f} });	// vertex 3.10 - Position and Normal
	vDef.push_back({ {halfDx, -halfDy, halfDz}, {0.0f, 0.0f, 1.0f}, {0.1f, 0.1f} });	// vertex 3.11 - Position and Normal



	vDef.push_back({ {-halfDx, halfDy, -halfDz}, {-1.0f, 0.0f, 0.0f}, {0.1f, 0.1f} });	// vertex 4.12 - Position and Normal
	vDef.push_back({ {-halfDx, halfDy, -halfDz}, {0.0f, 1.0f, 0.0f}, {0.05f, 0.05f}});	// vertex 4.13 - Position and Normal
	vDef.push_back({ {-halfDx, halfDy, -halfDz}, {0.0f, 0.0f, -1.0f}, {0.1f, 0.1f} });	// vertex 4.14 - Position and Normal

	vDef.push_back({ {halfDx, halfDy, -halfDz}, {1.0f, 0.0f, 0.0f}, {0.1f, 0.1f} });	// vertex 5.15 - Position and Normal
	vDef.push_back({ {halfDx, halfDy, -halfDz}, {0.0f, 1.0f, 0.0f}, {0.95f, 0.05f} });	// vertex 5.16 - Position and Normal
	vDef.push_back({ {halfDx, halfDy, -halfDz}, {0.0f, 0.0f, -1.0f}, {0.1f, 0.1f} });	// vertex 5.17 - Position and Normal

	vDef.push_back({ {-halfDx, halfDy, halfDz}, {-1.0f, 0.0f, 0.0f}, {0.1f, 0.1f} });	// vertex 6.18 - Position and Normal
	vDef.push_back({ {-halfDx, halfDy, halfDz}, {0.0f, 1.0f, 0.0f}, {0.05f, 0.95f} });	// vertex 6.19 - Position and Normal
	vDef.push_back({ {-halfDx, halfDy, halfDz}, {0.0f, 0.0f, 1.0f}, {0.1f, 0.1f} });	// vertex 6.20 - Position and Normal

	vDef.push_back({ {halfDx, halfDy, halfDz}, {1.0f, 0.0f, 0.0f}, {0.1f, 0.1f} });	// vertex 7.21 - Position and Normal
	vDef.push_back({ {halfDx, halfDy, halfDz}, {0.0f, 1.0f, 0.0f}, {0.95f, 0.95f} });	// vertex 7.22 - Position and Normal
	vDef.push_back({ {halfDx, halfDy, halfDz}, {0.0f, 0.0f, 1.0f}, {0.1f, 0.1f} });	// vertex 7.23 - Position and Normal

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
