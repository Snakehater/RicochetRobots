#include "main.hpp"

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// TileShape
enum Tile_Shape {
	SHAPE_DEFAULT,
	SHAPE_CIRCLE,
	SHAPE_CROSS,
	SHAPE_TRIANGLE,
	SHAPE_SQUARE
};

// prototype macros
void framebuffer_size_callback(GLFWwindow* window, int width, int height);  
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
int  get_mesh_offset(int* mesh_offsets, int target);
Animation choose_shape(Mesh* mesh, enum Tile_Shape shape, float speed);
void draw_mesh(Mesh* our_mesh, Shader* ourShader, int ignoreMatrix = 0);
void character_callback(GLFWwindow* window, unsigned int codepoint);

// camera
Camera camera(glm::vec3(7.5f, 22.0f, 7.5f), glm::vec3(0.0f, 2.0f, 0.0f), YAW, -89.9f);
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

// world space positions of our cubes
const int board_map_size = 16;
float board_map[board_map_size][board_map_size] = {
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{1, 1, 1, 1, 1, 1, 5, 1, 1, 3, 1, 1, 1, 1, 1, 1},
	{1, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 5, 1},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1},
	{1, 1, 1, 1, 1, 1, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 4, 1, 1, 1},
	{1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1},
	{1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1},
	{1, 1, 1, 1, 5, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{1, 1, 1, 1, 1, 1, 4, 1, 5, 1, 1, 1, 1, 1, 1, 1},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4, 1, 1},
	{1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1},
	{1, 2, 1, 1, 1, 1, 1, 1, 1, 3, 1, 1, 1, 1, 1, 1},
	{1, 1, 1, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
};
std::vector<glm::vec2> walls;


// create robot(s)
std::vector<Robot*> robots;

// Animations
AnimationSeq animationSeq;

// Create commandprompt
CommandBuffer* commandBuffer;

// Ray Caster
RayCaster rayCaster;

int main() {
	// instantiate the GLFW window
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	
	int major, minor, rev;
	glfwGetVersion(&major, &minor, &rev);
	std::cout << "GLFW v" << major << '.' << minor << '.' << rev << std::endl;
	std::cout << glfwGetVersionString() << std::endl;

	// create a window object
	GLFWwindow* window = glfwCreateWindow(800, 600, "RicochetRobots", NULL, NULL);
	if (window == NULL) {
	    std::cout << "Failed to create GLFW window" << std::endl;
	    glfwTerminate();
	    return -1;
	}
	glfwMakeContextCurrent(window); // assign it to main thread

	// GLAD manages function pointers for OpenGL so we want to initialize GLAD before we call any OpenGL function:
	// We pass GLAD the function to load the address of the OpenGL function pointers which is OS-specific. GLFW gives us glfwGetProcAddress that defines the correct function based on which OS we're compiling for.
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
	    std::cout << "Failed to initialize GLAD" << std::endl;
	    return -1;
	}

	glViewport(0, 0, 800, 600);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);  
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetInputMode(window,GLFW_CURSOR,GLFW_CURSOR_DISABLED);
	
	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST); // this removes fragments that are behind other fragments
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// build and compile our shader program
	// ------------------------------------
	// we first try to load shaders from files
	Shader ourShader( "shaders/vertex_core.glsl", "shaders/fragment_core.glsl" );
	
	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	// fill wall buffer 


	walls.push_back(glm::vec2( 4.5f,  0.0f));
	walls.push_back(glm::vec2(10.5f,  0.0f));
	walls.push_back(glm::vec2( 6.5f,  1.0f));
	walls.push_back(glm::vec2( 8.5f,  1.0f));
	walls.push_back(glm::vec2( 1.0f,  1.5f));
	walls.push_back(glm::vec2( 6.0f,  1.5f));
	walls.push_back(glm::vec2( 9.0f,  1.5f));
	walls.push_back(glm::vec2(14.0f,  1.5f));
	walls.push_back(glm::vec2( 0.5f,  2.0f));
	walls.push_back(glm::vec2(14.5f,  2.0f));
	walls.push_back(glm::vec2(10.5f,  4.0f));
	walls.push_back(glm::vec2(10.0f,  4.5f));
	walls.push_back(glm::vec2( 6.0f,  4.5f));
	walls.push_back(glm::vec2( 0.0f,  5.5f));
	walls.push_back(glm::vec2( 6.5f,  5.0f));
	walls.push_back(glm::vec2(12.0f,  5.5f));
	walls.push_back(glm::vec2( 2.5f,  6.0f));
	walls.push_back(glm::vec2( 3.0f,  6.5f));
	walls.push_back(glm::vec2( 7.0f,  6.5f));
	walls.push_back(glm::vec2( 8.0f,  6.5f));
	walls.push_back(glm::vec2(12.5f,  6.0f));
	walls.push_back(glm::vec2( 6.5f,  7.0f));
	walls.push_back(glm::vec2( 8.5f,  7.0f));
	walls.push_back(glm::vec2( 6.5f,  8.0f));
	walls.push_back(glm::vec2( 8.5f,  8.0f));
	walls.push_back(glm::vec2( 7.0f,  8.5f));
	walls.push_back(glm::vec2( 8.0f,  8.5f));
	walls.push_back(glm::vec2( 3.5f,  9.0f));
	walls.push_back(glm::vec2( 4.0f,  9.5f));
	walls.push_back(glm::vec2( 6.0f,  9.5f));
	walls.push_back(glm::vec2(15.0f,  9.5f));
	walls.push_back(glm::vec2( 0.0f, 10.5f));
	walls.push_back(glm::vec2( 5.5f, 10.0f));
	walls.push_back(glm::vec2( 8.0f, 10.5f));
	walls.push_back(glm::vec2( 8.5f, 10.0f));
	walls.push_back(glm::vec2(13.0f, 10.5f));
	walls.push_back(glm::vec2(12.5f, 11.0f));
	walls.push_back(glm::vec2( 7.0f, 11.5f));
	walls.push_back(glm::vec2( 7.5f, 12.0f));
	walls.push_back(glm::vec2( 1.0f, 12.5f));
	walls.push_back(glm::vec2( 1.5f, 13.0f));
	walls.push_back(glm::vec2( 8.5f, 13.0f));
	walls.push_back(glm::vec2( 9.0f, 13.5f));
	walls.push_back(glm::vec2(14.0f, 13.5f));
	walls.push_back(glm::vec2( 3.5f, 14.0f));
	walls.push_back(glm::vec2( 3.0f, 14.5f));
	walls.push_back(glm::vec2(14.5f, 14.0f));
	walls.push_back(glm::vec2( 4.5f, 15.0f));
	walls.push_back(glm::vec2(11.5f, 15.0f));


	int vertices_size = 0;
	int stride_offset_counter = 0;
	int arr_offset_cnt = 0;

	// meshes
	Mesh nullCube(NULL);
	Mesh regular_cube("res/objects/cube.obj", 0.5f, &vertices_size, &stride_offset_counter, &arr_offset_cnt);
	Mesh redCube("res/objects/red_cube.obj", 0.5f, &vertices_size, &stride_offset_counter, &arr_offset_cnt);
	Mesh greenCube("res/objects/green_cube.obj", 0.5f, &vertices_size, &stride_offset_counter, &arr_offset_cnt);
	Mesh blueCube("res/objects/blue_cube.obj", 0.5f, &vertices_size, &stride_offset_counter, &arr_offset_cnt);
	Mesh yellowCube("res/objects/yellow_cube.obj", 0.5f, &vertices_size, &stride_offset_counter, &arr_offset_cnt); 
	Mesh wall("res/objects/wall.obj", 0.5f, &vertices_size, &stride_offset_counter, &arr_offset_cnt);
	delete commandBuffer;
	commandBuffer = new CommandBuffer(new Mesh("res/objects/command_prompt.obj", 1.0f, &vertices_size, &stride_offset_counter, &arr_offset_cnt), new Game(), &camera);


	// parameters for command buffer
	commandBuffer->mesh->vPos = glm::vec3(0.0f, 3.0f, 0.0f);

	// robot stuff
	robots.push_back(new Robot(new Mesh("res/objects/robot.obj", 0.5f, &vertices_size, &stride_offset_counter, &arr_offset_cnt)));
	robots.push_back(new Robot(new Mesh("res/objects/robot.obj", 0.5f, &vertices_size, &stride_offset_counter, &arr_offset_cnt)));
	robots.push_back(new Robot(new Mesh("res/objects/robot.obj", 0.5f, &vertices_size, &stride_offset_counter, &arr_offset_cnt)));
	robots.push_back(new Robot(new Mesh("res/objects/robot.obj", 0.5f, &vertices_size, &stride_offset_counter, &arr_offset_cnt)));
	
	robots[0]->mesh->set_position(11.0f, 1.0f, 7.0f);
	robots[1]->mesh->set_position(3.0f, 1.0f, 5.0f);
	robots[2]->mesh->set_position(14.0f, 1.0f, 6.0f);
	robots[3]->mesh->set_position(0.0f, 1.0f, 4.0f);

	// create mesh types for board map

	std::vector<Mesh*> mesh_types;
	mesh_types.push_back(&nullCube);
	mesh_types.push_back(&regular_cube);
	mesh_types.push_back(&redCube);
	mesh_types.push_back(&greenCube);
	mesh_types.push_back(&blueCube);
	mesh_types.push_back(&yellowCube);

	Mesh map_cubes[board_map_size*board_map_size]; // this will hold all cubes
	{
		int cnt = 0;
		for (int i = 0; i < board_map_size; i++) {
			for (int j = 0; j < board_map_size; j++) {
				Mesh mesh = *mesh_types[board_map[i][j]];
				mesh.set_position((float)j, 0.0f, (float)i);
				mesh.set_vRot(0.0f, 1.0f, 0.0f);
				map_cubes[cnt] = mesh;
				cnt++;
			}
		}
	}

	// fill array
	
	float vertices[vertices_size] = { };

	regular_cube.fill_arr(&vertices[0]);
	redCube.fill_arr(&vertices[0]);
	greenCube.fill_arr(&vertices[0]);
	blueCube.fill_arr(&vertices[0]);
	yellowCube.fill_arr(&vertices[0]);
	for (long unsigned int i = 0; i < robots.size(); i++)
		robots[i]->mesh->fill_arr(&vertices[0]);
	wall.fill_arr(&vertices[0]);
	commandBuffer->mesh->fill_arr(&vertices[0]);
	
	//////////////////// opengl magic /////////////////////

	// vertex buffer objects (VBO) 
	// vertex array object (VAO)
	// element buffer object (EBO)
	unsigned int VBO, VAO; //, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	
	// ..:: Initialization code :: ..
	// 1. bind Vertex Array Object	
	glBindVertexArray(VAO);

	// 2. copy our vertices array in a vertex buffer for OpenGL to use
	// bind buffer and store vertices on graphics card
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*vertices_size, vertices, GL_STATIC_DRAW);
	// (removed) 3. copy our index array in a element buffer for OpenGL to use
	
	// 4. then set the vertex attributes pointers
	
	// on the attribute pointers: 5 is the number of indexes that points to an index. 3pos + 2texcoord
	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// load and create a texture
	// -------------------------
	unsigned int texture1;
	glGenTextures( 1, &texture1 );
	glBindTexture( GL_TEXTURE_2D, texture1 );
	// set the texture wrapping parameters
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	// set texture filtering parameters
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	// load image, create texture and generate mipmap
	//stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
	int width, height, nrChannels;
	unsigned char *data = stbi_load( "res/textures/textureatlas.jpg", &width, &height, &nrChannels, 0);

	if ( data ) {
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data );
		glGenerateMipmap( GL_TEXTURE_2D );
	} else
		std::cout << "ERROR::LOAD::TEXTURE" << std::endl;

	stbi_image_free( data );
	
	// tell opengl for each sample to which texture unit it belongs to ( only once )
	// -----------------------------------------------------------------------------
	ourShader.use();
	glUniform1i( glGetUniformLocation( ourShader.ID, "texture1" ), 0 ); // 0 is our texture id, for another texture, use another id


	// uncomment this call to draw in wireframe polygons.
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// render loop
	// -----------

	// start our renderloop
	while(!glfwWindowShouldClose(window)) {
		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		
		// input
		// -----
		processInput(window);
		
		// render
		// ------
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		// bind textures on corresponding texture units
		glActiveTexture( GL_TEXTURE0 );
		glBindTexture( GL_TEXTURE_2D, texture1 );
		
		// Going 3d with our object
		// ------------------------
		// for our 3d scene to work, we need to perform following calc and get last scene before the viewport transform:
		// Vclip = Mprojection ⋅ Mview ⋅ Mmodel ⋅ Vlocal
		// where each variable is a matrix generated with glm for simplicity and optimatiations.	
		
		// activate shader
		ourShader.use();


		// prep projection matrix since we don't need to set a new one every time
		//					      FOV		aspect ratio		 near far clipping distance
		glm::mat4 projection = glm::perspective( glm::radians( camera.zoom ), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f );
		ourShader.setMat4( "projection", projection );
		

		// create tranformation matrices for view/camera and pass it to the shader
		glm::mat4 view = camera.GetViewMatrix(); 
		ourShader.setMat4( "view", view );
		
		animationSeq.tick();

		// render boxes
		glBindVertexArray( VAO );
		for (int i = 0; i < board_map_size*board_map_size; i++) {
			Mesh* our_mesh = &map_cubes[i];
			if (our_mesh->is_null())
				continue;
	//		std::cout << "id " << board_map[i][j];
	//		std::cout << " offset " << our_mesh->offset();
	//		std::cout << " size " << our_mesh->size() << std::endl;
			// calculate the model matrix for each object and pass it to shader before drawing
			draw_mesh(our_mesh, &ourShader);
		}

		for (long unsigned int i = 0; i < walls.size(); i++) {
			wall.vPos.x = walls[i].x;
			wall.vPos.y = 1.0f;
			wall.vPos.z = walls[i].y;

			if (walls[i].x == (int)walls[i].x) {
				wall.vRot = glm::vec3(0.0f, 1.0f, 0.0f);
				wall.rotation_degree = 90.0f;
			} else {
				wall.vRot = glm::vec3(0.0f, 1.0f, 0.0f);
				wall.rotation_degree = 0.0f;
			}

			draw_mesh(&wall, &ourShader);
		}
		for (long unsigned int i = 0; i < robots.size(); i++)		
			draw_mesh(robots[i]->mesh, &ourShader);
		
		if(commandBuffer->enabled){
			draw_mesh(commandBuffer->mesh, &ourShader, true);
		}

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window); 
		commandBuffer->update(window); // this needs to come before polling events
		glfwPollEvents();
		usleep( 1 );
	}
	glfwTerminate(); // delete all of GLFW's resources that were allocated
	return 0;
}

void draw_mesh(Mesh* our_mesh, Shader* ourShader, int ignoreMatrix){
	glm::mat4 model = glm::mat4( 1.0f );
	model = glm::translate(model, our_mesh->get_position());
	//float angle = ( 20.0f * i ) + glfwGetTime();
	model = glm::rotate( model, glm::radians(our_mesh->rotation_degree), our_mesh->get_vRot() );
	ourShader->setMat4("model", model);
	ourShader->setInt("ignoreMatrix", ignoreMatrix);
	ourShader->setInt("specialColorEn", (our_mesh->specialColorEn ? 1 : 0));
	ourShader->setVec4("vCol", our_mesh->vCol);
	glDrawArrays( GL_TRIANGLES, our_mesh->stride_offset(), our_mesh->vert_num());
}

Animation choose_shape(Mesh* mesh, enum Tile_Shape shape, float speed) {
	float fRot = 0.0f;
	glm::vec3 vRot = glm::vec3(0.0f, 0.0f, 0.0f);
	switch(shape) {
		case SHAPE_DEFAULT:
			fRot = 0.0f;
			break;
		case SHAPE_CIRCLE:
			fRot = 90.0f;
			vRot = glm::vec3(1.0f, 0.0f, 0.0f);
			break;
		case SHAPE_CROSS:
			fRot = -90.0f;
			vRot = glm::vec3(1.0f, 0.0f, 0.0f);
			break;
		case SHAPE_TRIANGLE:
			fRot = 90.0f;
			vRot = glm::vec3(0.0f, 0.0f, 1.0f);
			break;
		case SHAPE_SQUARE:
			fRot = -90.0f;
			vRot = glm::vec3(0.0f, 0.0f, 1.0f);
			break;
	}
	return Animation(mesh, vRot, fRot, false);
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
bool debounce = false;
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		camera.ProcessKeyboard(UP, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		camera.ProcessKeyboard(DOWN, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
		commandBuffer->enable();
	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) {
		if (!debounce)
			std::cout << rayCaster.cast(0, 0, &camera, &robots) << std::endl;
		debounce = true;
	} else
		debounce = false;
	

	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		if (robots[0]->is_available())
			robots[0]->move(&animationSeq, glm::vec3(-1.0f, 0.0f, 0.0f), &walls, board_map_size, &robots);
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		if (robots[0]->is_available())
			robots[0]->move(&animationSeq, glm::vec3(0.0f, 0.0f, 1.0f), &walls, board_map_size, &robots);
	}
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		if (robots[0]->is_available())
			robots[0]->move(&animationSeq, glm::vec3(0.0f, 0.0f, -1.0f), &walls, board_map_size, &robots);
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		if (robots[0]->is_available())
			robots[0]->move(&animationSeq, glm::vec3(1.0f, 0.0f, 0.0f), &walls, board_map_size, &robots);
	}
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}
