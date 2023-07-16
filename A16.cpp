// This has been adapted from the Vulkan tutorial

#include "Starter.hpp"

// The uniform buffer objects data structures
// Remember to use the correct alignas(...) value
//        float : alignas(4)
//        vec2  : alignas(8)
//        vec3  : alignas(16)
//        vec4  : alignas(16)
//        mat3  : alignas(16)
//        mat4  : alignas(16)

struct MeshUniformBlock {
	alignas(4) float amb;
	alignas(4) float gamma;
	alignas(16) glm::vec3 sColor;
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

struct GlobalUniformBlock {
	alignas(16) glm::vec3 DlightDir;
	alignas(16) glm::vec3 DlightColor;
	alignas(16) glm::vec3 AmbLightColor;
	alignas(16) glm::vec3 eyePos;
};

struct GlobalUniformBufferObjectLight {
	alignas(16) glm::vec3 lightPos;
	alignas(16) glm::vec3 lightDir;
	alignas(16) glm::vec4 lightColor;
	alignas(16) glm::vec3 AmbLightColor;
	alignas(16) glm::vec3 eyePos;
};

// The vertices data structures
struct VertexMesh {
	glm::vec3 pos;
	glm::vec3 norm;
	glm::vec2 UV;
};

struct VertexOverlay {
	glm::vec2 pos;
	glm::vec2 UV;
};

/* A16 */
/* Add the C++ datastructure for the required vertex format */



// MAIN ! 
class A16 : public BaseProject {
	protected:

	// Current aspect ratio (used by the callback that resized the window
	float Ar;

	// Descriptor Layouts ["classes" of what will be passed to the shaders]
	DescriptorSetLayout DSLGubo, DSLMesh, DSLOverlay, DSLGuboLight, DSLObj;
	/* A16 */
	/* Add the variable that will contain the required Descriptor Set Layout */


	// Vertex formats
	VertexDescriptor VMesh;
	VertexDescriptor VOverlay;
	/* A16 */
	/* Add the variable that will contain the required Vertex format definition */

	// Pipelines [Shader couples]
	Pipeline PMesh;
	Pipeline POverlay;
	Pipeline PObj;
	/* A16 */
	/* Add the variable that will contain the new pipeline */

	// Models, textures and Descriptors (values assigned to the uniforms)
	// Please note that Model objects depends on the corresponding vertex structure
	Model<VertexMesh> MBody, MHandle, MWheel, MPhone, MFront, MScreenMesh, MCamera, MFloor, MBallLight, MScreen;
	/* A16 */
	/* Add the variable that will contain the model for the room */
	Model<VertexOverlay> MKey, MSplash;
	DescriptorSet DSGubo, DSGuboLight, DSBody, DSHandle, DSWheel1, DSWheel2, DSWheel3, DSKey, DSSplash, DSFloor,
					DSBallLight, DSBallLight2, DSScreen,
					DSPhone, DSFront, DSScreenMesh, DSCamera;
	/* A16 */
	/* Add the variable that will contain the Descriptor Set for the room */	
	Texture TBody, THandle, TWheel, TKey, TSplash, TPhone, TScreenMesh, TFloor, TBallLight, TScreen;
	
	// C++ storage for uniform variables
	MeshUniformBlock uboBody, uboHandle, uboWheel1, uboWheel2, uboWheel3, uboFloor, uboBallLight, uboBallLight2;
	UniformBufferObjectOBJ uboPhone, uboScreen, uboFront, uboScreenMesh, uboCamera;
	/* A16 */
	/* Add the variable that will contain the Uniform Block in slot 0, set 1 of the room */
	GlobalUniformBlock gubo;
	OverlayUniformBlock uboKey, uboSplash;
	GlobalUniformBufferObjectLight guboL, guboL2, guboL3;

	// Other application parameters
	float CamH, CamRadius, CamPitch, CamYaw, LightAngle, LightHeight;
	int gameState;
	float HandleRot = 0.0;
	float Wheel1Rot = 0.0;
	float Wheel2Rot = 0.0;
	float Wheel3Rot = 0.0;


	int showPos;


	// Here you set the main application parameters
	void setWindowParameters() {
		// window size, titile and initial background
		windowWidth = 800;
		windowHeight = 600;
		windowTitle = "A16";
    	windowResizable = GLFW_TRUE;
		initialBackgroundColor = {0.0f, 0.005f, 0.01f, 1.0f};
		
		// Descriptor pool sizes
		/* A16 */
		/* Update the requirements for the size of the pool */
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
					// this array contains the bindings:
					// first  element : the binding number
					// second element : the type of element (buffer or texture)
					//                  using the corresponding Vulkan constant
					// third  element : the pipeline stage where it will be used
					//                  using the corresponding Vulkan constant
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
		/* A16 */
		/* Init the new Data Set Layout */
				
		DSLGubo.init(this, {
					{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS}
				});

		DSLGuboLight.init(this, {
					{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS},
					{1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS},
					{2, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS}
				});

		// Vertex descriptors
		VMesh.init(this, {
				  // this array contains the bindings
				  // first  element : the binding number
				  // second element : the stride of this binging
				  // third  element : whether this parameter change per vertex or per instance
				  //                  using the corresponding Vulkan constant
				  {0, sizeof(VertexMesh), VK_VERTEX_INPUT_RATE_VERTEX}
				}, {
				  // this array contains the location
				  // first  element : the binding number
				  // second element : the location number
				  // third  element : the offset of this element in the memory record
				  // fourth element : the data type of the element
				  //                  using the corresponding Vulkan constant
				  // fifth  elmenet : the size in byte of the element
				  // sixth  element : a constant defining the element usage
				  //                   POSITION - a vec3 with the position
				  //                   NORMAL   - a vec3 with the normal vector
				  //                   UV       - a vec2 with a UV coordinate
				  //                   COLOR    - a vec4 with a RGBA color
				  //                   TANGENT  - a vec4 with the tangent vector
				  //                   OTHER    - anything else
				  //
				  // ***************** DOUBLE CHECK ********************
				  //    That the Vertex data structure you use in the "offsetoff" and
				  //	in the "sizeof" in the previous array, refers to the correct one,
				  //	if you have more than one vertex format!
				  // ***************************************************
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
		/* A16 */
		/* Define the new Vertex Format */

		// Pipelines [Shader couples]
		// The second parameter is the pointer to the vertex definition
		// Third and fourth parameters are respectively the vertex and fragment shaders
		// The last array, is a vector of pointer to the layouts of the sets that will
		// be used in this pipeline. The first element will be set 0, and so on..
		PMesh.init(this, &VMesh, "shaders/MeshVert.spv", "shaders/MeshFrag.spv", {&DSLGuboLight, &DSLMesh});
		PObj.init(this, &VMesh, "shaders/CookTorrVert.spv", "shaders/CookTorrFrag.spv", {&DSLGuboLight, &DSLObj});
		POverlay.init(this, &VOverlay, "shaders/OverlayVert.spv", "shaders/OverlayFrag.spv", {&DSLOverlay});
		POverlay.setAdvancedFeatures(VK_COMPARE_OP_LESS_OR_EQUAL, VK_POLYGON_MODE_FILL,
 								    VK_CULL_MODE_NONE, false);
		/* A16 */
		/* Create the new pipeline, using shaders "VColorVert.spv" and "VColorFrag.spv" */

		// Models, textures and Descriptors (values assigned to the uniforms)

		// Create models
		// The second parameter is the pointer to the vertex definition for this model
		// The third parameter is the file name
		// The last is a constant specifying the file type: currently only OBJ or GLTF
		MBody.init(this,   &VMesh, "Models/SlotBody.obj", OBJ);
		MHandle.init(this, &VMesh, "Models/SlotHandle.obj", OBJ);
		MWheel.init(this,  &VMesh, "Models/SlotWheel.obj", OBJ);
		MPhone.init(this, &VMesh, "models/back.obj", OBJ);
		MFront.init(this, &VMesh, "models/front.obj", OBJ);
		MScreenMesh.init(this, &VMesh, "models/screen.obj", OBJ);
		MCamera.init(this, &VMesh, "models/camera.obj", OBJ);

		// createSphereMesh(MPhone.vertices, MPhone.indices);
		// MPhone.initMesh(this, &VMesh);
		/* A16 */
		/* load the mesh for the room, contained in OBJ file "Room.obj" */
		createFloor(MFloor.vertices, MFloor.indices);
		MFloor.initMesh(this, &VMesh);

		createSphereMesh(MBallLight.vertices, MBallLight.indices);
		MBallLight.initMesh(this, &VMesh);

		createScreen(MScreen.vertices, MScreen.indices, 1800.0f, 831.0f);
		MScreen.initMesh(this, &VMesh);

		
		// Creates a mesh with direct enumeration of vertices and indices
		MKey.vertices = {{{-0.8f, 0.6f}, {0.0f,0.0f}}, {{-0.8f, 0.95f}, {0.0f,1.0f}},
						 {{ 0.8f, 0.6f}, {1.0f,0.0f}}, {{ 0.8f, 0.95f}, {1.0f,1.0f}}};
		MKey.indices = {0, 1, 2,    1, 2, 3};
		MKey.initMesh(this, &VOverlay);
		
		// Creates a mesh with direct enumeration of vertices and indices
		MSplash.vertices = {{{-1.0f, -0.58559f}, {0.0102f, 0.0f}}, {{-1.0f, 0.58559f}, {0.0102f,0.85512f}},
						 {{ 1.0f,-0.58559f}, {1.0f,0.0f}}, {{ 1.0f, 0.58559f}, {1.0f,0.85512f}}};
		MSplash.indices = {0, 1, 2,    1, 2, 3};
		MSplash.initMesh(this, &VOverlay);
		
		// Create the textures
		// The second parameter is the file name
		TBody.init(this,   "textures/SlotBody.png");
		THandle.init(this, "textures/SlotHandle.png");
		TWheel.init(this,  "textures/SlotWheel.png");
		TKey.init(this,    "textures/PressSpace.png");
		TSplash.init(this, "textures/SplashScreen.png");

		TPhone.init(this, "textures/Solid_red.png");
		TScreenMesh.init(this, "textures/Black.png");
		TFloor.init(this, "textures/Grey.png");
		TBallLight.init(this, "textures/Grey.png");
		TScreen.init(this, "textures/iphone_screen.png");
		
		// Init local variables
		CamH = 1.0f;
		CamRadius = 3.0f;
		CamPitch = glm::radians(15.f);
		CamYaw = glm::radians(0.0f);
		LightAngle = glm::radians(0.0f);
		LightHeight = 2.0f;
		gameState = 0;


		showPos = 0;
	}
	
	// Here you create your pipelines and Descriptor Sets!
	void pipelinesAndDescriptorSetsInit() {
		// This creates a new pipeline (with the current surface), using its shaders
		PMesh.create();
		PObj.create();
		POverlay.create();
		/* A16 */
		/* Create the new pipeline */
		
		// Here you define the data set
		DSBody.init(this, &DSLMesh, {
		// the second parameter, is a pointer to the Uniform Set Layout of this set
		// the last parameter is an array, with one element per binding of the set.
		// first  elmenet : the binding number
		// second element : UNIFORM or TEXTURE (an enum) depending on the type
		// third  element : only for UNIFORMs, the size of the corresponding C++ object. For texture, just put 0
		// fourth element : only for TEXTUREs, the pointer to the corresponding texture object. For uniforms, use nullptr
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
					{1, TEXTURE, 0, &TBody}
				});
		DSHandle.init(this, &DSLMesh, {
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
					{1, TEXTURE, 0, &THandle}
				});
		DSWheel1.init(this, &DSLMesh, {
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
					{1, TEXTURE, 0, &TWheel}
				});
		DSWheel2.init(this, &DSLMesh, {
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
					{1, TEXTURE, 0, &TWheel}
				});
		DSWheel3.init(this, &DSLMesh, {
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
					{1, TEXTURE, 0, &TWheel}
				});
		/* A16 */
		/* Define the data set for the room */
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

		DSFloor.init(this, &DSLMesh, {
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
					{1, TEXTURE, 0, &TFloor}
			});

		DSBallLight.init(this, &DSLMesh, {
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
					{1, TEXTURE, 0, &TBallLight}
			});

		DSBallLight2.init(this, &DSLMesh, {
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
					{1, TEXTURE, 0, &TBallLight}
			});

		DSScreen.init(this, &DSLObj, {
					{0, UNIFORM, sizeof(UniformBufferObjectOBJ), nullptr},
					{1, TEXTURE, 0, &TScreen},
					{2, TEXTURE, 0, &TScreen}
			});

		DSKey.init(this, &DSLOverlay, {
					{0, UNIFORM, sizeof(OverlayUniformBlock), nullptr},
					{1, TEXTURE, 0, &TKey}
				});
		DSSplash.init(this, &DSLOverlay, {
					{0, UNIFORM, sizeof(OverlayUniformBlock), nullptr},
					{1, TEXTURE, 0, &TSplash}
				});
		DSGubo.init(this, &DSLGubo, {
					{0, UNIFORM, sizeof(GlobalUniformBlock), nullptr}
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
		/* A16 */
		/* cleanup the new pipeline */

		// Cleanup datasets
		DSBody.cleanup();
		DSHandle.cleanup();
		DSWheel1.cleanup();
		DSWheel2.cleanup();
		DSWheel3.cleanup();
		/* A16 */
		/* cleanup the dataset for the room */
		DSPhone.cleanup();
		DSFront.cleanup();
		DSScreenMesh.cleanup();
		DSCamera.cleanup();
		DSFloor.cleanup();
		DSBallLight.cleanup();
		DSBallLight2.cleanup();
		DSScreen.cleanup();

		DSKey.cleanup();
		DSSplash.cleanup();
		DSGubo.cleanup();
		DSGuboLight.cleanup();
	}

	// Here you destroy all the Models, Texture and Desc. Set Layouts you created!
	// All the object classes defined in Starter.hpp have a method .cleanup() for this purpose
	// You also have to destroy the pipelines: since they need to be rebuilt, they have two different
	// methods: .cleanup() recreates them, while .destroy() delete them completely
	void localCleanup() {
		// Cleanup textures
		TBody.cleanup();
		THandle.cleanup();
		TWheel.cleanup();
		TKey.cleanup();
		TSplash.cleanup();
		TPhone.cleanup();
		TScreenMesh.cleanup();
		TFloor.cleanup();
		TBallLight.cleanup();
		TScreen.cleanup();
		
		// Cleanup models
		MBody.cleanup();
		MHandle.cleanup();
		MWheel.cleanup();
		MKey.cleanup();
		MSplash.cleanup();
		/* A16 */
		/* Cleanup the mesh for the room */
		MPhone.cleanup();
		MFront.cleanup();
		MScreenMesh.cleanup();
		MCamera.cleanup();
		MFloor.cleanup();
		MBallLight.cleanup();
		MScreen.cleanup();
		
		// Cleanup descriptor set layouts
		DSLMesh.cleanup();
		DSLObj.cleanup();
		DSLOverlay.cleanup();
		/* A16 */
		/* Cleanup the new Descriptor Set Layout */

		DSLGubo.cleanup();
		DSLGuboLight.cleanup();
		
		
		// Destroies the pipelines
		PMesh.destroy();		
		PObj.destroy();
		POverlay.destroy();
		/* A16 */
		/* Destroy the new pipeline */
	}
	
	// Here it is the creation of the command buffer:
	// You send to the GPU all the objects you want to draw,
	// with their buffers and textures
	
	void populateCommandBuffer(VkCommandBuffer commandBuffer, int currentImage) {
		// sets global uniforms (see below fro parameters explanation)
		//DSGubo.bind(commandBuffer, PMesh, 0, currentImage);
		DSGuboLight.bind(commandBuffer, PMesh, 0, currentImage);
		// binds the pipeline
		PMesh.bind(commandBuffer);
		// For a pipeline object, this command binds the corresponing pipeline to the command buffer passed in its parameter

		// binds the model
		MBody.bind(commandBuffer);
		// For a Model object, this command binds the corresponing index and vertex buffer
		// to the command buffer passed in its parameter
		
		// binds the data set
		DSBody.bind(commandBuffer, PMesh, 1, currentImage);
		// For a Dataset object, this command binds the corresponing dataset
		// to the command buffer and pipeline passed in its first and second parameters.
		// The third parameter is the number of the set being bound
		// As described in the Vulkan tutorial, a different dataset is required for each image in the swap chain.
		// This is done automatically in file Starter.hpp, however the command here needs also the index
		// of the current image in the swap chain, passed in its last parameter
					
		// record the drawing command in the command buffer
		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(MBody.indices.size()), 1, 0, 0, 0);
		// the second parameter is the number of indexes to be drawn. For a Model object,
		// this can be retrieved with the .indices.size() method.

		MHandle.bind(commandBuffer);
		DSHandle.bind(commandBuffer, PMesh, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(MHandle.indices.size()), 1, 0, 0, 0);

		MWheel.bind(commandBuffer);
		DSWheel1.bind(commandBuffer, PMesh, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(MWheel.indices.size()), 1, 0, 0, 0);
		DSWheel2.bind(commandBuffer, PMesh, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(MWheel.indices.size()), 1, 0, 0, 0);
		DSWheel3.bind(commandBuffer, PMesh, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(MWheel.indices.size()), 1, 0, 0, 0);
		/* A16 */
		/* Insert the commands to draw the room */

		MFloor.bind(commandBuffer);
		DSFloor.bind(commandBuffer, PMesh, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(MFloor.indices.size()), 1, 0, 0, 0);

		MBallLight.bind(commandBuffer);
		DSBallLight.bind(commandBuffer, PMesh, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(MBallLight.indices.size()), 1, 0, 0, 0);
		DSBallLight2.bind(commandBuffer, PMesh, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(MBallLight.indices.size()), 1, 0, 0, 0);

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

		MScreen.bind(commandBuffer);
		DSScreen.bind(commandBuffer, PObj, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(MScreen.indices.size()), 1, 0, 0, 0);

		POverlay.bind(commandBuffer);
		MKey.bind(commandBuffer);
		DSKey.bind(commandBuffer, POverlay, 0, currentImage);
		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(MKey.indices.size()), 1, 0, 0, 0);

		MSplash.bind(commandBuffer);
		DSSplash.bind(commandBuffer, POverlay, 0, currentImage);
		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(MSplash.indices.size()), 1, 0, 0, 0);
	}

	// Here is where you update the uniforms.
	// Very likely this will be where you will be writing the logic of your application.
	void updateUniformBuffer(uint32_t currentImage) {
		// Standard procedure to quit when the ESC key is pressed
		if(glfwGetKey(window, GLFW_KEY_ESCAPE)) {
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		
		// Integration with the timers and the controllers
		float deltaT;
		
		static auto startTime = std::chrono::high_resolution_clock::now();
		
		auto currentTime = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period>
					(currentTime - startTime).count();
		glm::vec3 m = glm::vec3(0.0f), r = glm::vec3(0.0f);
		bool fire = false;
		getSixAxis(deltaT, m, r, fire);
		// getSixAxis() is defined in Starter.hpp in the base class.
		// It fills the float point variable passed in its first parameter with the time
		// since the last call to the procedure.
		// It fills vec3 in the second parameters, with three values in the -1,1 range corresponding
		// to motion (with left stick of the gamepad, or ASWD + RF keys on the keyboard)
		// It fills vec3 in the third parameters, with three values in the -1,1 range corresponding
		// to motion (with right stick of the gamepad, or Arrow keys + QE keys on the keyboard, or mouse)
		// If fills the last boolean variable with true if fire has been pressed:
		//          SPACE on the keyboard, A or B button on the Gamepad, Right mouse button

		// To debounce the pressing of the fire button, and start the event when the key is released
		static bool wasFire = false;
		bool handleFire = (wasFire && (!fire));
		wasFire = fire;
		
		// Parameters: wheels and handle speed and range
		const float HandleSpeed = glm::radians(90.0f);
		const float HandleRange = glm::radians(45.0f);
		const float WheelSpeed = glm::radians(180.0f);
		const float SymExtent = glm::radians(15.0f);	// size of one symbol on the wheel in angle rad.
		// static variables for current angles
		static float HandleRot = 0.0;
		static float Wheel1Rot = 0.0;
		static float Wheel2Rot = 0.0;
		static float Wheel3Rot = 0.0;
		static float TargetRot = 0.0;	// Target rotation

//std::cout << gameState << "\n";	
		switch(gameState) {		// main state machine implementation
		  case 0: // initial state - show splash screen
			if(handleFire) {
				gameState = 1;	// jump to the wait key state
			}
			break;
		  case 1: // wait key state
			if(handleFire) {
				gameState = 2;	// jump to the moving handle state
			}
			break;
		  case 2: // handle moving down state
			HandleRot += HandleSpeed * deltaT;
			Wheel1Rot += WheelSpeed * deltaT;
			Wheel2Rot += WheelSpeed * deltaT;
			Wheel3Rot += WheelSpeed * deltaT;
			if(HandleRot > HandleRange) {	// when limit is reached, jump the handle moving up state
				gameState = 3;
				HandleRot = HandleRange;
			}
			break;
		  case 3: // handle moving up state
			HandleRot -= HandleSpeed * deltaT;
			Wheel1Rot += WheelSpeed * deltaT;
			Wheel2Rot += WheelSpeed * deltaT;
			Wheel3Rot += WheelSpeed * deltaT;
			if(HandleRot < 0.0f) {	// when limit is reached, jump the 3 wheels spinning state
				gameState = 4;
				HandleRot = 0.0f;
				TargetRot = Wheel1Rot + (10 + (rand() % 11)) * SymExtent;
			}
			break;
		  case 4: // 3 wheels spinning state
			Wheel1Rot += WheelSpeed * deltaT;
			Wheel2Rot += WheelSpeed * deltaT;
			Wheel3Rot += WheelSpeed * deltaT;
//std::cout << Wheel1Rot << " --- " << TargetRot << "\n";
			if(Wheel1Rot >= TargetRot) {	// When the target rotation is reached, jump to the next state
				gameState = 5;
				Wheel1Rot = round(TargetRot / SymExtent) * SymExtent; // quantize position
				TargetRot = Wheel2Rot + (10 + (rand() % 11)) * SymExtent;
			}
			break;
		  case 5: // 2 wheels spinning state
			Wheel2Rot += WheelSpeed * deltaT;
			Wheel3Rot += WheelSpeed * deltaT;
			if(Wheel2Rot >= TargetRot) {	// When the target rotation is reached, jump to the next state
				gameState = 6;
				Wheel2Rot = round(TargetRot / SymExtent) * SymExtent; // quantize position
				TargetRot = Wheel3Rot + (10 + (rand() % 11)) * SymExtent;
			}
			break;
		  case 6: // 1 wheels spinning state
			Wheel3Rot += WheelSpeed * deltaT;
			if(Wheel3Rot >= TargetRot) {	// When the target rotation is reached, jump to the next state
				gameState = 1;
				Wheel3Rot = round(TargetRot / SymExtent) * SymExtent; // quantize position
			}
			break;
		}

		glm::mat4 translation = glm::mat4(1.0f);
		glm::mat4 scaling = glm::mat4(1.0f);
		glm::mat4 rotation = glm::mat4(1.0f);
		glm::mat4 phoneWorld = glm::mat4(1.0f);

		glm::vec3 mouse = glm::vec3(0.0f);
		glm::vec3 arrows = glm::vec3(0.0f);
		int Lpressed;
		int Cpressed;
		int Dpressed;
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

		getInteraction(mouse, arrows, Lpressed, Cpressed, Dpressed, Epressed);
		if (Lpressed && Cpressed + Dpressed + Epressed == 0) {
			showPos = 1;
		} else if (Cpressed && Lpressed + Dpressed + Epressed == 0) {
			showPos = 2;
		} else if (Dpressed && Lpressed + Cpressed + Epressed == 0) {
			showPos = 3;
		} else if (Epressed && Lpressed + Cpressed + Dpressed == 0) {
			showPos = 4;
		} else {
			showPos = 0;
		}

		switch(showPos) {
		  case 1: //lightning port
		  	tra.x = 0.0f;
			tra.y = 1.0f;
			tra.z = 1.25f;

			rot.x = glm::radians(0.0f);
			rot.y = glm::radians(0.0f);
			rot.z = glm::radians(0.0f);

			emit = 0.0f;
		  				
			break;
		  case 2: //camera
		 	tra.x = 0.20f;
			tra.y = 0.5f;
			tra.z = 1.75f;

			rot.x = glm::radians(90.0f);
			rot.y = glm::radians(0.0f);
			rot.z = glm::radians(180.0f);

			emit = 0.0f;
			
			break;
		  case 3: //display
			tra.x = 0.0f;
			tra.y = 1.0f;
			tra.z = 0.75f;


			rot.x = glm::radians(90.0f);
			rot.y = glm::radians(0.0f);
			rot.z = glm::radians(0.0f);

			emit = 1.0f;
			
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
		  default:
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
		const float rotSpeed = glm::radians(180.0f);
		const float movSpeed = 1.0f;
		
		CamH += m.z * movSpeed * deltaT;
		CamRadius -= m.x * movSpeed * deltaT;
		//CamPitch -= r.x * rotSpeed * deltaT;
		//CamYaw += r.y * rotSpeed * deltaT;
		LightAngle += arrows.x * rotSpeed * deltaT;
		LightHeight += arrows.y * rotSpeed * deltaT;
		
		glm::mat4 Prj = glm::perspective(FOVy, Ar, nearPlane, farPlane);
		Prj[1][1] *= -1;
		glm::vec3 camTarget = glm::vec3(0,CamH,0);
		/*glm::vec3 camPos = camTarget +
							  CamRadius * glm::vec3(cos(CamPitch) * sin(CamYaw),
													sin(CamPitch),
													cos(CamPitch) * cos(CamYaw));*/

		glm::vec3 camPos = glm::vec3(0.0f, 1.0f, 2.5f);
		glm::mat4 View = glm::lookAt(camPos, camTarget, glm::vec3(0,1,0));

		/*
		gubo.DlightDir = glm::normalize(glm::vec3(1, 2, 3));
		gubo.DlightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		gubo.AmbLightColor = glm::vec3(0.1f);
		gubo.eyePos = camPos;

		// Writes value to the GPU
		DSGubo.map(currentImage, &gubo, sizeof(gubo), 0);
		// the .map() method of a DataSet object, requires the current image of the swap chain as first parameter
		// the second parameter is the pointer to the C++ data structure to transfer to the GPU
		// the third parameter is its size
		// the fourth parameter is the location inside the descriptor set of this uniform block
		*/

		float angle = 0.0f;
		angle = LightAngle;
		float lightDist = 2.8f;

		float offset = 0;
		static float currOffset = 1.0f;


		guboL.lightPos = glm::vec3(lightDist * cos(angle), LightHeight, lightDist * sin(angle));
		//guboL.lightDir = -glm::normalize(glm::vec3(-cos(angle), LightHeight, -sin(angle)));
		guboL.lightDir = -glm::normalize(currTra - guboL.lightPos + currOffset);
		//printf("TARGET: %f, HEIGHT: %f\t\t\tx: %f, y: %f, z: %f\n", 2.0f - LightHeight, LightHeight, guboL.lightDir.x, guboL.lightDir.y, guboL.lightDir.z);
		guboL.lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		guboL.AmbLightColor = glm::vec3(0.05f);
		guboL.eyePos = camPos;

		guboL2.lightPos = glm::vec3(lightDist * cos(glm::radians(120.0f)  + LightAngle), LightHeight, lightDist * sin(glm::radians(120.0f) + LightAngle));
		//guboL.lightDir = -glm::normalize(glm::vec3(-cos(angle), LightHeight, -sin(angle)));
		guboL2.lightDir = -glm::normalize(currTra - guboL2.lightPos + currOffset);
		//printf("TARGET: %f, HEIGHT: %f\t\t\tx: %f, y: %f, z: %f\n", 2.0f - LightHeight, LightHeight, guboL.lightDir.x, guboL.lightDir.y, guboL.lightDir.z);
		guboL2.lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		guboL2.AmbLightColor = glm::vec3(0.05f);
		guboL2.eyePos = camPos;

		guboL3.lightPos = glm::vec3(lightDist * cos(glm::radians(240.0f)  + LightAngle), LightHeight, lightDist * sin(glm::radians(240.0f) + LightAngle));
		//guboL.lightDir = -glm::normalize(glm::vec3(-cos(angle), LightHeight, -sin(angle)));
		guboL3.lightDir = -glm::normalize(currTra - guboL3.lightPos + currOffset);
		//printf("TARGET: %f, HEIGHT: %f\t\t\tx: %f, y: %f, z: %f\n", 2.0f - LightHeight, LightHeight, guboL.lightDir.x, guboL.lightDir.y, guboL.lightDir.z);
		guboL3.lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		guboL3.AmbLightColor = glm::vec3(0.05f);
		guboL3.eyePos = camPos;

		DSGuboLight.map(currentImage, &guboL, sizeof(guboL), 0);
		DSGuboLight.map(currentImage, &guboL2, sizeof(guboL2), 1);
		DSGuboLight.map(currentImage, &guboL3, sizeof(guboL3), 2);

		currOffset = 0.98 * currOffset + 0.02 * offset;

		glm::mat4 World = glm::translate(glm::mat4(1.0f), glm::vec3(10.0f, 0.5f, 10.0f));
		uboBody.amb = 1.0f; uboBody.gamma = 180.0f; uboBody.sColor = glm::vec3(1.0f);
		uboBody.mvpMat = Prj * View * World;
		uboBody.mMat = World;
		uboBody.nMat = glm::inverse(glm::transpose(World));
		DSBody.map(currentImage, &uboBody, sizeof(uboBody), 0);
	
		World = glm::rotate(glm::translate(glm::mat4(1.0f), glm::vec3(10.0f,0.5f,10.0f)),
							HandleRot, glm::vec3(1,0,0));
		uboHandle.amb = 1.0f; uboHandle.gamma = 180.0f; uboHandle.sColor = glm::vec3(1.0f);
		uboHandle.mvpMat = Prj * View * World;
		uboHandle.mMat = World;
		uboHandle.nMat = glm::inverse(glm::transpose(World));
		DSHandle.map(currentImage, &uboHandle, sizeof(uboHandle), 0);
	
		World = glm::rotate(glm::translate(glm::mat4(1.0f), glm::vec3(10.0f,0.93f,10.0f)),
							Wheel1Rot, glm::vec3(1,0,0));
		uboWheel1.amb = 1.0f; uboWheel1.gamma = 180.0f; uboWheel1.sColor = glm::vec3(1.0f);
		uboWheel1.mvpMat = Prj * View * World;
		uboWheel1.mMat = World;
		uboWheel1.nMat = glm::inverse(glm::transpose(World));
		DSWheel1.map(currentImage, &uboWheel1, sizeof(uboWheel1), 0);
	
		World = glm::rotate(glm::translate(glm::mat4(1.0f), glm::vec3(10.0f,0.93f,10.0f)),
							Wheel2Rot, glm::vec3(1,0,0));
		uboWheel2.amb = 1.0f; uboWheel2.gamma = 180.0f; uboWheel2.sColor = glm::vec3(1.0f);
		uboWheel2.mvpMat = Prj * View * World;
		uboWheel2.mMat = World;
		uboWheel2.nMat = glm::inverse(glm::transpose(World));
		DSWheel2.map(currentImage, &uboWheel2, sizeof(uboWheel2), 0);
	
		World = glm::rotate(glm::translate(glm::mat4(1.0f), glm::vec3(10.0f,0.93f,10.0f)),
							Wheel3Rot, glm::vec3(1,0,0));
		uboWheel3.amb = 1.0f; uboWheel3.gamma = 180.0f; uboWheel3.sColor = glm::vec3(1.0f);
		uboWheel3.mvpMat = Prj * View * World;
		uboWheel3.mMat = World;
		uboWheel3.nMat = glm::inverse(glm::transpose(World));
		DSWheel3.map(currentImage, &uboWheel3, sizeof(uboWheel3), 0);
		/* A16 */
		/* fill the uniform block for the room. Identical to the one of the body of the slot machine */
		/*World = glm::rotate(
			glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)), glm::vec3(0.01f, 0.01f, 0.01f)),
			time * glm::radians(30.0f),
			glm::vec3(0.0f, 1.0f, 0.0f));*/
/*
		glm::mat4 staticRotation = glm::rotate(glm::mat4(1.0f), glm::radians(-20.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		glm::mat4 continuousRotation = glm::rotate(glm::mat4(1.0f), time * glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 combinedRotation = continuousRotation * staticRotation;

		glm::mat4 translateToCenter = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.5f, 0.0f));
		glm::mat4 floating = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.5f + 0.2f * sin(time), 0.0f));

		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.01f));*/

		World = glm::translate(phoneWorld, glm::vec3(0.0f, currInterSpace * (-0.2f), 0.0f));
		//World = glm::scale(glm::mat4(1.0f), glm::vec3(0.01f));

		uboPhone.amb = 1.0f; uboPhone.rho = 0.05f; uboPhone.K = 0.05f; uboPhone.F0 = 0.3f; uboPhone.g = 1.5f; uboPhone.beta = 2.0f; uboPhone.emit = 0.0f;
		uboPhone.sColor = glm::vec3(1.0f);

		uboPhone.mvpMat = Prj * View * World;
		uboPhone.mMat = World;
		uboPhone.nMat = glm::inverse(glm::transpose(World));
		DSPhone.map(currentImage, &uboPhone, sizeof(uboPhone), 0);

		
		World = glm::translate(phoneWorld, glm::vec3(0.0f, currInterSpace * 0.4f, 0.0f));

		uboFront.amb = 1.0f; uboFront.rho = 0.05f; uboFront.K = 0.05f; uboFront.F0 = 0.3f; uboFront.g = 1.5f; uboFront.beta = 2.0f; uboFront.emit = 0.0f;
		uboFront.sColor = glm::vec3(1.0f);

		uboFront.mvpMat = Prj * View * World;
		uboFront.mMat = World;
		uboFront.nMat = glm::inverse(glm::transpose(World));
		DSFront.map(currentImage, &uboFront, sizeof(uboFront), 0);

		
		World = glm::translate(phoneWorld, glm::vec3(0.0f, currInterSpace * 0.2f, 0.0f));

		uboScreenMesh.amb = 1.0f; uboScreenMesh.rho = 0.05f; uboScreenMesh.K = 0.05f; uboScreenMesh.F0 = 0.3f; uboScreenMesh.g = 1.5f; uboScreenMesh.beta = 2.0f; uboScreenMesh.emit = 0.0f;
		uboScreenMesh.sColor = glm::vec3(1.0f);

		uboScreenMesh.mvpMat = Prj * View * World;
		uboScreenMesh.mMat = World;
		uboScreenMesh.nMat = glm::inverse(glm::transpose(World));
		DSScreenMesh.map(currentImage, &uboScreenMesh, sizeof(uboScreenMesh), 0);

		
		World = glm::translate(phoneWorld, glm::vec3(0.0f, currInterSpace * -0.4f, 0.0f));

		uboCamera.amb = 1.0f; uboCamera.rho = 0.05f; uboCamera.K = 0.05f; uboCamera.F0 = 0.3f; uboCamera.g = 1.5f; uboCamera.beta = 2.0f; uboCamera.emit = 0.0f;
		uboCamera.sColor = glm::vec3(1.0f);

		uboCamera.mvpMat = Prj * View * World;
		uboCamera.mMat = World;
		uboCamera.nMat = glm::inverse(glm::transpose(World));
		DSCamera.map(currentImage, &uboCamera, sizeof(uboCamera), 0);



		World = glm::scale(glm::translate(phoneWorld, glm::vec3(0.0f, currInterSpace * 0.2f + 4.0f, 0.0f)), glm::vec3(132.7f));
		//World = glm::mat4(1.0f);

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

		World = glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(lightDist * cos(LightAngle), LightHeight, lightDist * sin(LightAngle))), glm::vec3(0.05f));

		uboBallLight.amb = 0.0f; uboBallLight.gamma = 180.0f; uboBallLight.sColor = glm::vec3(1.0f);
		uboBallLight.mvpMat = Prj * View * World;
		uboBallLight.mMat = World;
		uboBallLight.nMat = glm::inverse(glm::transpose(World));
		DSBallLight.map(currentImage, &uboBallLight, sizeof(uboBallLight), 0);
		
		World = glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(lightDist * cos(glm::radians(120.0f) + LightAngle), LightHeight, lightDist * sin(glm::radians(120.0f) + LightAngle))), glm::vec3(0.05f));

		uboBallLight2.amb = 0.0f; uboBallLight2.gamma = 180.0f; uboBallLight2.sColor = glm::vec3(1.0f);
		uboBallLight2.mvpMat = Prj * View * World;
		uboBallLight2.mMat = World;
		uboBallLight2.nMat = glm::inverse(glm::transpose(World));
		DSBallLight2.map(currentImage, &uboBallLight2, sizeof(uboBallLight2), 0);

		/* map the uniform data block to the GPU */


		//uboKey.visible = (gameState == 1) ? 1.0f : 0.0f;
		uboKey.visible = 0.0f;
		DSKey.map(currentImage, &uboKey, sizeof(uboKey), 0);

		//uboSplash.visible = (gameState == 0) ? 1.0f : 0.0f;
		uboSplash.visible = 0.0f;
		DSSplash.map(currentImage, &uboSplash, sizeof(uboSplash), 0);
	}	

	void createFloor(std::vector<VertexMesh>& vDef, std::vector<uint32_t>& vIdx);
	void createScreen(std::vector<VertexMesh>& vDef, std::vector<uint32_t>& vIdx, float textureHeight, float textureWidth);
	void createSphereMesh(std::vector<VertexMesh> &vDef, std::vector<uint32_t> &vIdx);
};

#include "objectGenerator.hpp"

// This is the main: probably you do not need to touch this!
int main() {
    A16 app;

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}