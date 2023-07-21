#include "Starter.hpp"

struct MeshUniformBlock {
	alignas(4) float amb;
	alignas(4) float gamma;
	alignas(16) glm::vec3 sColor;
	alignas(16) glm::mat4 mvpMat;
	alignas(16) glm::mat4 mMat;
	alignas(16) glm::mat4 nMat;
};

struct UniformBufferObject {
	alignas(16) glm::mat4 mvpMat;
	alignas(16) glm::mat4 mMat;
	alignas(16) glm::mat4 nMat;
};

struct UniformBufferObjectOBJ {
	alignas(4) float amb;
	alignas(4) float rho;
	alignas(4) float K;
	alignas(4) float F0;
	alignas(4) float g;
	alignas(4) float beta;
	alignas(4) float emit;

	alignas(16) glm::vec3 sColor;
	alignas(16) glm::mat4 mvpMat;
	alignas(16) glm::mat4 mMat;
	alignas(16) glm::mat4 nMat;
};

struct OverlayUniformBlock {
	alignas(4) float visible;
};

struct GlobalUniformBufferObjectLight {
	alignas(16) glm::vec3 lightPos;
	alignas(16) glm::vec3 lightDir;
	alignas(16) glm::vec4 lightColor;
	alignas(16) glm::vec3 AmbLightColor;
	alignas(16) glm::vec3 eyePos;
};



struct VertexMesh {
	glm::vec3 pos;
	glm::vec3 norm;
	glm::vec2 UV;
};

struct VertexOverlay {
	glm::vec2 pos;
	glm::vec2 UV;
};



class ProductShowcase : public BaseProject {
	protected:

	float Ar;

	// Descriptor Set Layout
	DescriptorSetLayout DSLMesh, DSLOverlay, DSLGuboLight, DSLObj, DSLSkyBox;

	// Vertex Formats
	VertexDescriptor VMesh;
	VertexDescriptor VOverlay;

	// Pipelines
	Pipeline PMesh;
	Pipeline POverlay;
	Pipeline PObj;
	Pipeline PSkyBox;

	// Models, textures and Descriptors
	Model<VertexMesh> MSkyBox, MPhone, MFront, MScreenMesh, MCamera, MChip, MFloor, MBallLight, MSpotlight, MScreen;

	DescriptorSet DSGuboLight, DSFloor, DSSkyBox,
					DSBallLight, DSBallLight2, DSBallLight3, DSSpotlight,
					DSScreen, DSPhone, DSFront, DSScreenMesh, DSCamera, DSChip;

	Texture TPhone, TScreenMesh, TFloor, TSkyBox, TBallLight, TScreen, TChip;
	

	MeshUniformBlock uboFloor, uboBallLight, uboBallLight2, uboBallLight3, uboSpotlight;
	UniformBufferObjectOBJ uboPhone, uboScreen, uboFront, uboScreenMesh, uboCamera, uboChip;

	GlobalUniformBufferObjectLight guboL, guboL2, guboL3;

	UniformBufferObject skyBubo;

	// Other application parameters
	float CamH, CamDist, CamYaw, LightHorAngle, LightVertAngle;

	int showPos;

	// Here you set the main application parameters
	void setWindowParameters() {
		// window size, titile and initial background
		windowWidth = 800;
		windowHeight = 600;
		windowTitle = "New IPhone mega power plasss";
    	windowResizable = GLFW_TRUE;
		initialBackgroundColor = {0.0f, 0.000f, 0.00f, 1.0f};
		
		// Descriptor pool sizes
		uniformBlocksInPool = 100;
		texturesInPool = 100;
		setsInPool = 100;
		
		Ar = (float)windowWidth / (float)windowHeight;
	}
	
	// What to do when the window changes size
	void onWindowResize(int w, int h) {
		Ar = (float)w / (float)h;
	}
	
	// Here you load and setup all your Vulkan Models and Texutures.
	// Here you also create your Descriptor set layouts and load the shaders for the pipelines
	void localInit() {

		// Descriptor Layouts [what will be passed to the shaders]
		DSLMesh.init(this, {
					{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS},
					{1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT}
				});

		DSLObj.init(this, {
					{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS},
					{1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT},
					{2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT}
				});
				
		DSLOverlay.init(this, {
					{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS},
					{1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT}
				});

		DSLGuboLight.init(this, {
					{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS},
					{1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS},
					{2, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS}
				});

		DSLSkyBox.init(this, {
					{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT},
					{1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT}
				  });

		// Vertex descriptors
		VMesh.init(this, {
				  // this array contains the bindings
				  {0, sizeof(VertexMesh), VK_VERTEX_INPUT_RATE_VERTEX}
				}, {
				  // this array contains the location
				  {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexMesh, pos),
				         sizeof(glm::vec3), POSITION},
				  {0, 1, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexMesh, norm),
				         sizeof(glm::vec3), NORMAL},
				  {0, 2, VK_FORMAT_R32G32_SFLOAT, offsetof(VertexMesh, UV),
				         sizeof(glm::vec2), UV}
				});

		VOverlay.init(this, {
				  {0, sizeof(VertexOverlay), VK_VERTEX_INPUT_RATE_VERTEX}
				}, {
				  {0, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(VertexOverlay, pos),
				         sizeof(glm::vec2), OTHER},
				  {0, 1, VK_FORMAT_R32G32_SFLOAT, offsetof(VertexOverlay, UV),
				         sizeof(glm::vec2), UV}
				});


		PMesh.init(this, &VMesh, "shaders/MeshVert.spv", "shaders/MeshFrag.spv", {&DSLGuboLight, &DSLMesh});
		PObj.init(this, &VMesh, "shaders/CookTorrVert.spv", "shaders/CookTorrFrag.spv", {&DSLGuboLight, &DSLObj});
		POverlay.init(this, &VOverlay, "shaders/OverlayVert.spv", "shaders/OverlayFrag.spv", {&DSLOverlay});
		POverlay.setAdvancedFeatures(VK_COMPARE_OP_LESS_OR_EQUAL, VK_POLYGON_MODE_FILL,
 								    VK_CULL_MODE_NONE, false);

		PSkyBox.init(this, &VMesh, "shaders/SkyBoxVert.spv", "shaders/SkyBoxFrag.spv", {&DSLSkyBox});
		PSkyBox.setAdvancedFeatures(VK_COMPARE_OP_LESS_OR_EQUAL, VK_POLYGON_MODE_FILL,
 								    VK_CULL_MODE_BACK_BIT, false);

		// Models, textures and Descriptors (values assigned to the uniforms)

		// Create models
		MPhone.init(this, &VMesh, "models/back.obj", OBJ);
		MFront.init(this, &VMesh, "models/front.obj", OBJ);
		MScreenMesh.init(this, &VMesh, "models/screen.obj", OBJ);
		MCamera.init(this, &VMesh, "models/camera.obj", OBJ);

		createBox(MChip.vertices, MChip.indices, 1.0f, 1.0f, 0.05f);
		MChip.initMesh(this, &VMesh);
		
		MSkyBox.init(this, &VMesh, "models/SkyBoxCube.obj", OBJ);
		const char *T2fn[] = {"textures/skybox/Skybox_Right.png", "textures/skybox/Skybox_Left.png",
							  "textures/skybox/Skybox_Top.png",   "textures/skybox/Skybox_Bottom.png",
							  "textures/skybox/Skybox_Front.png", "textures/skybox/Skybox_Back.png"};
		TSkyBox.initCubic(this, T2fn);

		createFloor(MFloor.vertices, MFloor.indices);
		MFloor.initMesh(this, &VMesh);

		createSphereMesh(MBallLight.vertices, MBallLight.indices);
		MBallLight.initMesh(this, &VMesh);

		MSpotlight.init(this, &VMesh, "models/spotlight.obj", OBJ);

		createScreen(MScreen.vertices, MScreen.indices, 1800.0f, 831.0f);
		MScreen.initMesh(this, &VMesh);
		
		// Create the textures
		// The second parameter is the file name
		TPhone.init(this, "textures/Solid_red.png");
		TScreenMesh.init(this, "textures/Black.png");
		TFloor.init(this, "textures/Grey.png");
		TBallLight.init(this, "textures/Grey.png");
		TScreen.init(this, "textures/iphone_screen.png");
		TChip.init(this, "textures/Chip.png");
		
		// Init local variables
		CamH = 1.0f;
		CamDist = 2.5f;
		CamYaw = glm::radians(90.0f);
		LightHorAngle = glm::radians(90.0f);
		LightVertAngle = glm::radians(45.0f);

		showPos = 0;
	}
	
	// Here you create your pipelines and Descriptor Sets!
	void pipelinesAndDescriptorSetsInit() {
		// This creates a new pipeline (with the current surface), using its shaders
		PMesh.create();
		PObj.create();
		POverlay.create();
		PSkyBox.create();
		
		// Here you define the data set
		DSPhone.init(this, &DSLObj, {
					{0, UNIFORM, sizeof(UniformBufferObjectOBJ), nullptr},
					{1, TEXTURE, 0, &TPhone},
					{2, TEXTURE, 0, &TScreen}
			});

		DSFront.init(this, &DSLObj, {
					{0, UNIFORM, sizeof(UniformBufferObjectOBJ), nullptr},
					{1, TEXTURE, 0, &TPhone},
					{2, TEXTURE, 0, &TScreen}
			});

		DSScreenMesh.init(this, &DSLObj, {
					{0, UNIFORM, sizeof(UniformBufferObjectOBJ), nullptr},
					{1, TEXTURE, 0, &TScreenMesh},
					{2, TEXTURE, 0, &TScreen}
			});
		
		DSCamera.init(this, &DSLObj, {
					{0, UNIFORM, sizeof(UniformBufferObjectOBJ), nullptr},
					{1, TEXTURE, 0, &TPhone},
					{2, TEXTURE, 0, &TScreen}
			});

		DSChip.init(this, &DSLObj, {
					{0, UNIFORM, sizeof(UniformBufferObjectOBJ), nullptr},
					{1, TEXTURE, 0, &TChip},
					{2, TEXTURE, 0, &TScreen}
			});

		DSFloor.init(this, &DSLMesh, {
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
					{1, TEXTURE, 0, &TFloor}
			});

		DSSkyBox.init(this, &DSLSkyBox, {
					{0, UNIFORM, sizeof(UniformBufferObject), nullptr},
					{1, TEXTURE, 0, &TSkyBox}
				});

		DSBallLight.init(this, &DSLMesh, {
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
					{1, TEXTURE, 0, &TBallLight}
			});

		DSBallLight2.init(this, &DSLMesh, {
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
					{1, TEXTURE, 0, &TBallLight}
			});

		DSBallLight3.init(this, &DSLMesh, {
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
					{1, TEXTURE, 0, &TBallLight}
			});

		DSSpotlight.init(this, &DSLMesh, {
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
					{1, TEXTURE, 0, &TBallLight}
			});

		DSScreen.init(this, &DSLObj, {
					{0, UNIFORM, sizeof(UniformBufferObjectOBJ), nullptr},
					{1, TEXTURE, 0, &TScreen},
					{2, TEXTURE, 0, &TScreen}
			});

		

		DSGuboLight.init(this, &DSLGuboLight, {
					{0, UNIFORM, sizeof(GlobalUniformBufferObjectLight), nullptr},
					{1, UNIFORM, sizeof(GlobalUniformBufferObjectLight), nullptr},
					{2, UNIFORM, sizeof(GlobalUniformBufferObjectLight), nullptr}
				});
	}

	// Here you destroy your pipelines and Descriptor Sets!
	// All the object classes defined in Starter.hpp have a method .cleanup() for this purpose
	void pipelinesAndDescriptorSetsCleanup() {
		// Cleanup pipelines
		PMesh.cleanup();
		PObj.cleanup();
		POverlay.cleanup();
		PSkyBox.cleanup();

		// Cleanup datasets
		DSPhone.cleanup();
		DSFront.cleanup();
		DSScreenMesh.cleanup();
		DSCamera.cleanup();
		DSChip.cleanup();
		DSFloor.cleanup();
		DSSkyBox.cleanup();
		DSBallLight.cleanup();
		DSBallLight2.cleanup();
		DSBallLight3.cleanup();
		DSSpotlight.cleanup();
		DSScreen.cleanup();



		DSGuboLight.cleanup();
	}

	// Here you destroy all the Models, Texture and Desc. Set Layouts you created!
	// All the object classes defined in Starter.hpp have a method .cleanup() for this purpose
	// You also have to destroy the pipelines: since they need to be rebuilt, they have two different
	// methods: .cleanup() recreates them, while .destroy() delete them completely
	void localCleanup() {
		// Cleanup textures
		TPhone.cleanup();
		TScreenMesh.cleanup();
		TFloor.cleanup();
		TSkyBox.cleanup();
		TBallLight.cleanup();
		TScreen.cleanup();
		TChip.cleanup();
		
		// Cleanup models
		MPhone.cleanup();
		MFront.cleanup();
		MScreenMesh.cleanup();
		MCamera.cleanup();
		MChip.cleanup();
		MFloor.cleanup();
		MSkyBox.cleanup();
		MBallLight.cleanup();
		MSpotlight.cleanup();
		MScreen.cleanup();
		
		// Cleanup descriptor set layouts
		DSLMesh.cleanup();
		DSLObj.cleanup();
		DSLOverlay.cleanup();
		DSLSkyBox.cleanup();



		DSLGuboLight.cleanup();
		
		
		// Destroies the pipelines
		PMesh.destroy();		
		PObj.destroy();
		POverlay.destroy();
		PSkyBox.cleanup();
	}
	
	// Here it is the creation of the command buffer:
	// You send to the GPU all the objects you want to draw,
	// with their buffers and textures
	
	void populateCommandBuffer(VkCommandBuffer commandBuffer, int currentImage) {

		PSkyBox.bind(commandBuffer);
		MSkyBox.bind(commandBuffer);
		DSSkyBox.bind(commandBuffer, PSkyBox, 0, currentImage);
		vkCmdDrawIndexed(commandBuffer,
					static_cast<uint32_t>(MSkyBox.indices.size()), 1, 0, 0, 0);

		// sets global uniforms
		DSGuboLight.bind(commandBuffer, PMesh, 0, currentImage);
		// binds the pipeline
		PMesh.bind(commandBuffer);
		// binds the model
		MFloor.bind(commandBuffer);
		// binds the dataset
		DSFloor.bind(commandBuffer, PMesh, 1, currentImage);
		// record the drawing command in the command buffer
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(MFloor.indices.size()), 1, 0, 0, 0);


		MBallLight.bind(commandBuffer);
		DSBallLight.bind(commandBuffer, PMesh, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(MBallLight.indices.size()), 1, 0, 0, 0);

		DSBallLight2.bind(commandBuffer, PMesh, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(MBallLight.indices.size()), 1, 0, 0, 0);

		DSBallLight3.bind(commandBuffer, PMesh, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(MBallLight.indices.size()), 1, 0, 0, 0);


		MSpotlight.bind(commandBuffer);
		DSSpotlight.bind(commandBuffer, PMesh, 1, currentImage);
		

		PObj.bind(commandBuffer);
		MPhone.bind(commandBuffer);
		DSPhone.bind(commandBuffer, PObj, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(MPhone.indices.size()), 1, 0, 0, 0);

		MFront.bind(commandBuffer);
		DSFront.bind(commandBuffer, PObj, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(MFront.indices.size()), 1, 0, 0, 0);

		MScreenMesh.bind(commandBuffer);
		DSScreenMesh.bind(commandBuffer, PObj, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(MScreenMesh.indices.size()), 1, 0, 0, 0);

		MCamera.bind(commandBuffer);
		DSCamera.bind(commandBuffer, PObj, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(MCamera.indices.size()), 1, 0, 0, 0);

		MChip.bind(commandBuffer);
		DSChip.bind(commandBuffer, PObj, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(MChip.indices.size()), 1, 0, 0, 0);

		MScreen.bind(commandBuffer);
		DSScreen.bind(commandBuffer, PObj, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(MScreen.indices.size()), 1, 0, 0, 0);

	}

	// Here is where you update the uniforms.
	// Very likely this will be where you will be writing the logic of your application.
	void updateUniformBuffer(uint32_t currentImage) {
		// Standard procedure to quit when the ESC key is pressed
		if(glfwGetKey(window, GLFW_KEY_ESCAPE)) {
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		
		// Integration with the timers and the controllers
		
		static auto startTime = std::chrono::high_resolution_clock::now();
		static float lastTime = 0.0f;
		
		auto currentTime = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period>
					(currentTime - startTime).count();
		
		float deltaT = time - lastTime;
		lastTime = time;

		glm::mat4 translation = glm::mat4(1.0f);
		glm::mat4 scaling = glm::mat4(1.0f);
		glm::mat4 rotation = glm::mat4(1.0f);
		glm::mat4 phoneWorld = glm::mat4(1.0f);

		glm::vec3 mouse = glm::vec3(0.0f);
		glm::vec3 arrows = glm::vec3(0.0f);
		glm::vec3 wasd = glm::vec3(0.0f);
		int Lpressed;
		int Cpressed;
		int Hpressed;
		int Epressed;
		glm::vec3 tra;
		glm::vec3 rot;
		static glm::vec3 currTra = glm::vec3(0.0f);
		static glm::vec3 currRot = glm::vec3(0.0f);
		float contRotSpeed;

		float emit;
		static float currEmit = 0.0f;

		float interSpace = 0;
		static float currInterSpace = 0.0f;

		static float currCamYaw = glm::radians(90.0f);
		static float currLightHorAngle = glm::radians(90.0f);
		static float currLightVertAngle = glm::radians(50.0f);

		getInteraction(mouse, arrows, wasd, Lpressed, Cpressed, Hpressed, Epressed);
		if (Lpressed && Cpressed + Hpressed + Epressed == 0) {
			showPos = 1;
		} else if (Cpressed && Lpressed + Hpressed + Epressed == 0) {
			showPos = 2;
		} else if (Hpressed && Lpressed + Cpressed + Epressed == 0) {
			showPos = 3;
		} else if (Epressed && Lpressed + Cpressed + Hpressed == 0) {
			showPos = 4;
		} else {
			showPos = 0;
		}

		/*
		L: 90 35
		C: 75 30
		H: 90 60
		*/
		switch(showPos) {
		  case 1: //lightning port
		  	tra.x = 0.0f;
			tra.y = 1.0f;
			tra.z = 1.25f;

			rot.x = glm::radians(0.0f);
			rot.y = glm::radians(0.0f);
			rot.z = glm::radians(0.0f);

			emit = 0.0f;

			LightHorAngle = glm::radians(90.0f);
			LightVertAngle = glm::radians(35.0f);

			break;
		  case 2: //camera
		 	tra.x = 0.20f;
			tra.y = 0.5f;
			tra.z = 1.75f;

			rot.x = glm::radians(90.0f);
			rot.y = glm::radians(0.0f);
			rot.z = glm::radians(180.0f);

			emit = 0.0f;
			
			LightHorAngle = glm::radians(75.0f);
			LightVertAngle = glm::radians(30.0f);

			break;
		  case 3: //display
			tra.x = 0.0f;
			tra.y = 1.0f;
			tra.z = 0.75f;


			rot.x = glm::radians(90.0f);
			rot.y = glm::radians(0.0f);
			rot.z = glm::radians(0.0f);

			emit = 1.0f;

			LightHorAngle = glm::radians(90.0f);
			LightVertAngle = glm::radians(60.0f);
			
			break;
		  case 4: //explosion
			tra.x = 0.0f;
			tra.y = 1.0f;
			tra.z = 0.0f;

			rot.x = glm::radians(90.0f);
			rot.y = glm::radians(00.0f);
			rot.z = glm::radians(-315.0f);

			emit = 1.0f;

			interSpace = 200.0f;
		  	
			break;
		  default: // floating
		  	tra.x = 0.0f;
			tra.y = 1.0f + 0.2f * sin(time);
			tra.z = 0.0f;

			rot.x = glm::radians(70.0f);
			rot.y = glm::radians(0.0f);
			rot.z = glm::radians(0.0f);

			emit = 0.0f;


		}

		float speedAnim = 0.1;

		if (mouse.z != 0) {
			currRot.z += mouse.y * glm::radians(180.0f) * deltaT;
			currRot.x -= mouse.x * glm::radians(180.0f) * deltaT;
		}
		else {
			currRot = (1.0f - speedAnim) * currRot + speedAnim * rot;
		}


		currTra = (1.0f - speedAnim) * currTra + speedAnim * tra;

		currEmit = (1.0f - speedAnim) * currEmit + speedAnim * emit;

		currInterSpace = (1.0f - speedAnim + 0.06) * currInterSpace + (speedAnim - 0.06) * interSpace;

		glm::mat4 staticRotation = 	glm::rotate(glm::mat4(1.0f), currRot.x, glm::vec3(1.0f, 0.0f, 0.0f)) *
									glm::rotate(glm::mat4(1.0f), currRot.y, glm::vec3(0.0f, 1.0f, 0.0f)) *
									glm::rotate(glm::mat4(1.0f), currRot.z, glm::vec3(0.0f, 0.0f, 1.0f));

		glm::mat4 translateToCenter = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));

		rotation = glm::inverse(translateToCenter) * staticRotation * translateToCenter;

		translation = glm::translate(glm::mat4(1.0f), currTra);


		scaling = glm::scale(glm::mat4(1.0f), glm::vec3(0.01f));

		phoneWorld = translation * rotation * scaling;

		/*glm::mat4 staticRotation = glm::rotate(glm::mat4(1.0f), glm::radians(-20.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		glm::mat4 continuousRotation = glm::rotate(glm::mat4(1.0f), time * glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 combinedRotation = continuousRotation * staticRotation;

		glm::mat4 translateToCenter = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.5f, 0.0f));
		glm::mat4 floating = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.5f + 0.2f * sin(time), 0.0f));

		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.01f));

		World = floating * glm::inverse(translateToCenter) * combinedRotation * scale * translateToCenter;*/
		
		// Parameters
		// Camera FOV-y, Near Plane and Far Plane
		const float FOVy = glm::radians(45.0f);
		const float nearPlane = 0.1f;
		const float farPlane = 100.0f;
		const float rotSpeed = glm::radians(90.0f);
		const float movSpeed = 1.0f;
		
		LightHorAngle += arrows.x * rotSpeed * deltaT;
		if(LightVertAngle + arrows.y * rotSpeed * deltaT <= glm::radians(80.0f) &&
			LightVertAngle + arrows.y * rotSpeed * deltaT >= glm::radians(5.0f)) {
			LightVertAngle += arrows.y * rotSpeed * deltaT;
		}

		currLightHorAngle = 0.9 * currLightHorAngle + 0.1 * LightHorAngle;
		currLightVertAngle = 0.9 * currLightVertAngle + 0.1 * LightVertAngle;
		
		glm::mat4 Prj = glm::perspective(FOVy, Ar, nearPlane, farPlane);
		Prj[1][1] *= -1;
		glm::vec3 camTarget = glm::vec3(0,CamH,0);

		CamH += wasd.y * movSpeed * deltaT;

		if (showPos == 4 || showPos == 0) {
			if (wasd.x != 0) {
				CamYaw -= wasd.x * rotSpeed * deltaT;
			}
			else {
				CamYaw -= rotSpeed / 5 * deltaT;
			}
		}
		else {
			CamYaw = glm::round(CamYaw / glm::radians(360.0f)) * glm::radians(360.0f) + glm::radians(90.0f);
		}
		

		currCamYaw = 0.9 * currCamYaw + 0.1 * CamYaw;

		glm::vec3 camPos = camTarget +
				CamDist * glm::vec3(cos(currCamYaw),
										0.0f,
										sin(currCamYaw));
		glm::mat4 View = glm::lookAt(camPos, camTarget, glm::vec3(0,1,0));

		float lightDist = 2.8f;

		float offset = 0;
		static float currOffset = 1.0f;

		// Three spotlights

		guboL.lightPos = glm::vec3(lightDist * cos(currLightHorAngle) * cos(currLightVertAngle), lightDist * sin(currLightVertAngle), lightDist * sin(currLightHorAngle) * cos(currLightVertAngle));
		guboL.lightDir = -glm::normalize(currTra - guboL.lightPos);
		guboL.lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		guboL.AmbLightColor = glm::vec3(0.05f);
		guboL.eyePos = camPos;

		guboL2.lightPos = glm::vec3(lightDist * cos(currLightHorAngle + glm::radians(120.0f)) * cos(currLightVertAngle), lightDist * sin(currLightVertAngle), lightDist * sin(currLightHorAngle + glm::radians(120.0f)) * cos(currLightVertAngle));
		guboL2.lightDir = -glm::normalize(currTra - guboL2.lightPos);
		guboL2.lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		guboL2.AmbLightColor = glm::vec3(0.05f);
		guboL2.eyePos = camPos;

		guboL3.lightPos = glm::vec3(lightDist * cos(currLightHorAngle + glm::radians(240.0f)) * cos(currLightVertAngle), lightDist * sin(currLightVertAngle), lightDist * sin(currLightHorAngle + glm::radians(240.0f)) * cos(currLightVertAngle));
		guboL3.lightDir = -glm::normalize(currTra - guboL3.lightPos);
		guboL3.lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		guboL3.AmbLightColor = glm::vec3(0.05f);
		guboL3.eyePos = camPos;

		// Writes value to the GPU
		DSGuboLight.map(currentImage, &guboL, sizeof(guboL), 0);
		DSGuboLight.map(currentImage, &guboL2, sizeof(guboL2), 1);
		DSGuboLight.map(currentImage, &guboL3, sizeof(guboL3), 2);

		currOffset = 0.98 * currOffset + 0.02 * offset;

		glm::mat4 World = glm::mat4(1.0f);

		World = glm::translate(phoneWorld, glm::vec3(0.0f, currInterSpace * (-0.2f), 0.0f));

		uboPhone.amb = 0.5f; uboPhone.rho = 0.05f; uboPhone.K = 0.05f; uboPhone.F0 = 0.3f; uboPhone.g = 1.5f; uboPhone.beta = 2.0f; uboPhone.emit = 0.0f;
		uboPhone.sColor = glm::vec3(1.0f);

		uboPhone.mvpMat = Prj * View * World;
		uboPhone.mMat = World;
		uboPhone.nMat = glm::inverse(glm::transpose(World));
		DSPhone.map(currentImage, &uboPhone, sizeof(uboPhone), 0);

		
		World = glm::translate(phoneWorld, glm::vec3(0.0f, currInterSpace * 0.4f, 0.0f));

		uboFront.amb = 0.5f; uboFront.rho = 0.05f; uboFront.K = 0.05f; uboFront.F0 = 0.3f; uboFront.g = 1.5f; uboFront.beta = 2.0f; uboFront.emit = 0.0f;
		uboFront.sColor = glm::vec3(1.0f);

		uboFront.mvpMat = Prj * View * World;
		uboFront.mMat = World;
		uboFront.nMat = glm::inverse(glm::transpose(World));
		DSFront.map(currentImage, &uboFront, sizeof(uboFront), 0);

		
		World = glm::translate(phoneWorld, glm::vec3(0.0f, currInterSpace * 0.2f, 0.0f));

		uboScreenMesh.amb = 0.5f; uboScreenMesh.rho = 0.05f; uboScreenMesh.K = 0.05f; uboScreenMesh.F0 = 0.3f; uboScreenMesh.g = 1.5f; uboScreenMesh.beta = 2.0f; uboScreenMesh.emit = 0.0f;
		uboScreenMesh.sColor = glm::vec3(1.0f);

		uboScreenMesh.mvpMat = Prj * View * World;
		uboScreenMesh.mMat = World;
		uboScreenMesh.nMat = glm::inverse(glm::transpose(World));
		DSScreenMesh.map(currentImage, &uboScreenMesh, sizeof(uboScreenMesh), 0);

		
		World = glm::translate(phoneWorld, glm::vec3(0.0f, currInterSpace * -0.4f, 0.0f));

		uboCamera.amb = 0.5f; uboCamera.rho = 0.05f; uboCamera.K = 0.05f; uboCamera.F0 = 0.3f; uboCamera.g = 1.5f; uboCamera.beta = 2.0f; uboCamera.emit = 0.0f;
		uboCamera.sColor = glm::vec3(1.0f);

		uboCamera.mvpMat = Prj * View * World;
		uboCamera.mMat = World;
		uboCamera.nMat = glm::inverse(glm::transpose(World));
		DSCamera.map(currentImage, &uboCamera, sizeof(uboCamera), 0);

		World = glm::scale(phoneWorld, glm::vec3(30.0f));

		uboChip.amb = 0.5f; uboChip.rho = 0.2f; uboChip.K = 0.3f; uboChip.F0 = 0.3f; uboChip.g = 1.5f; uboChip.beta = 2.0f; uboChip.emit = 0.0f;
		uboChip.sColor = glm::vec3(1.0f);

		uboChip.mvpMat = Prj * View * World;
		uboChip.mMat = World;
		uboChip.nMat = glm::inverse(glm::transpose(World));
		DSChip.map(currentImage, &uboChip, sizeof(uboChip), 0);

		World = glm::scale(glm::translate(phoneWorld, glm::vec3(0.0f, currInterSpace * 0.2f + 4.0f, 0.0f)), glm::vec3(132.7f));
		
		uboScreen.amb = 0.0f; uboScreen.rho = 0.05f; uboScreen.K = 0.0f; uboScreen.F0 = 0.3f; uboScreen.g = 1.5f; uboScreen.beta = 2.0f; uboScreen.emit = currEmit;
		uboScreen.sColor = glm::vec3(1.0f);

		uboScreen.mvpMat = Prj * View * World;
		uboScreen.mMat = World;
		uboScreen.nMat = glm::inverse(glm::transpose(World));
		DSScreen.map(currentImage, &uboScreen, sizeof(uboScreen), 0);

		World = glm::scale(glm::mat4(1.0f), glm::vec3(100.0f));

		uboFloor.amb = 1.0f; uboFloor.gamma = 180.0f; uboFloor.sColor = glm::vec3(1.0f);
		uboFloor.mvpMat = Prj * View * World;
		uboFloor.mMat = World;
		uboFloor.nMat = glm::inverse(glm::transpose(World));
		DSFloor.map(currentImage, &uboFloor, sizeof(uboFloor), 0);

		skyBubo.mvpMat = Prj * View * World;
		skyBubo.mMat = glm::mat4(1.0f);
		skyBubo.nMat = glm::mat4(1.0f);
		DSSkyBox.map(currentImage, &skyBubo, sizeof(skyBubo), 0);

		World = glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(lightDist * cos(currLightHorAngle) * cos(currLightVertAngle), lightDist * sin(currLightVertAngle), lightDist * sin(currLightHorAngle) * cos(currLightVertAngle))), glm::vec3(0.05f));

		uboBallLight.amb = 0.0f; uboBallLight.gamma = 180.0f; uboBallLight.sColor = glm::vec3(1.0f);
		uboBallLight.mvpMat = Prj * View * World;
		uboBallLight.mMat = World;
		uboBallLight.nMat = glm::inverse(glm::transpose(World));
		DSBallLight.map(currentImage, &uboBallLight, sizeof(uboBallLight), 0);
		
		World = glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(lightDist * cos(glm::radians(120.0f) + currLightHorAngle) * cos(currLightVertAngle), lightDist * sin(currLightVertAngle), lightDist * sin(glm::radians(120.0f) + currLightHorAngle) * cos(currLightVertAngle))), glm::vec3(0.05f));

		uboBallLight2.amb = 0.0f; uboBallLight2.gamma = 180.0f; uboBallLight2.sColor = glm::vec3(1.0f);
		uboBallLight2.mvpMat = Prj * View * World;
		uboBallLight2.mMat = World;
		uboBallLight2.nMat = glm::inverse(glm::transpose(World));
		DSBallLight2.map(currentImage, &uboBallLight2, sizeof(uboBallLight2), 0);

		World = glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(lightDist * cos(glm::radians(240.0f) + currLightHorAngle) * cos(currLightVertAngle), lightDist * sin(currLightVertAngle), lightDist * sin(glm::radians(240.0f) + currLightHorAngle) * cos(currLightVertAngle))), glm::vec3(0.05f));

		uboBallLight3.amb = 0.0f; uboBallLight3.gamma = 180.0f; uboBallLight3.sColor = glm::vec3(1.0f);
		uboBallLight3.mvpMat = Prj * View * World;
		uboBallLight3.mMat = World;
		uboBallLight3.nMat = glm::inverse(glm::transpose(World));
		DSBallLight3.map(currentImage, &uboBallLight3, sizeof(uboBallLight3), 0);
	}	

	void createFloor(std::vector<VertexMesh>& vDef, std::vector<uint32_t>& vIdx);
	void createScreen(std::vector<VertexMesh>& vDef, std::vector<uint32_t>& vIdx, float textureHeight, float textureWidth);
	void createSphereMesh(std::vector<VertexMesh> &vDef, std::vector<uint32_t> &vIdx);
	void createBox(std::vector<VertexMesh> &vDef, std::vector<uint32_t> &vIdx, float height, float width, float thickness);
};

#include "objectGenerator.hpp"

// This is the main: probably you do not need to touch this!
int main() {
    ProductShowcase app;

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}