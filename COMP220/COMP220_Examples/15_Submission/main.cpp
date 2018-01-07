//main.cpp - defines the entry point of the application
//copy $(ProjectDir)\*.glsl $(OutDir)\*.glsl

#include "main.h"

int main(int argc, char* args[])
{
	initSDL();

	sceneCamera = new Camera((800 / 600),vec3(0.0f, 0.0f, -50.0f), vec3(5.0f, 5.0f, 5.0f));

	mat4 viewMatrix = sceneCamera->cameraMatrix;

	std::vector<GameObject *> gameObjectList;

	GameObject * Volleyball = new GameObject();

	Volleyball->loadMeshes("Volleyball.fbx");
	Volleyball->loadDiffuseMap("Volleyball_TEX.png");
	Volleyball->setPosition(glm::vec3(50.0f, 0.0f, 0.0f));
	Volleyball->setScale(glm::vec3(0.1f, 0.1f, 0.1f));
	
	Volleyball->update();

	Volleyball->loadShaderProgram("PosterizeVertexShader.glsl", "PosterizeFragmentShader.glsl");

	gameObjectList.push_back(Volleyball);

	Volleyball = new GameObject();

	Volleyball->loadMeshes("Volleyball.fbx");
	Volleyball->loadDiffuseMap("Volleyball_TEX.png");
	Volleyball->setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
	Volleyball->setScale(glm::vec3(0.1f, 0.1f, 0.1f));

	Volleyball->update();

	Volleyball->loadShaderProgram("lightVertexShader.glsl", "lightFragmentShader.glsl");

	gameObjectList.push_back(Volleyball);

	Volleyball = new GameObject();

	Volleyball->loadMeshes("Volleyball.fbx");
	Volleyball->loadDiffuseMap("Volleyball_TEX.png");
	Volleyball->setPosition(glm::vec3(-50.0f, 50.0f, 0.0f));
	Volleyball->setScale(glm::vec3(0.1f, 0.1f, 0.1f));

	Volleyball->update();

	Volleyball->loadShaderProgram("vertexShader.glsl", "fragmentShader.glsl");

	gameObjectList.push_back(Volleyball);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	GLuint vertexBuffer;
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

	mat4 projectionMatrix = perspective(radians(90.0f), float(4.0f / 3.0f), 0.1f, 100.0f);

	int mouseXPosition = 0.0f;
	int mouseYPosition = 0.0f;
	
	// horizontal angle : toward -Z
	float horizontalAngle = 3.14f;
	// vertical angle : 0, look at the horizon
	float verticalAngle = 0.0f;
	// Initial Field of View
	float initialFoV = 90.0f;

	float speed = 3.0f; // 3 units / second
	float mouseSpeed = 0.05f;

	///collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
	btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();

	///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
	btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);

	///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
	btBroadphaseInterface* overlappingPairCache = new btDbvtBroadphase();

	///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
	btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;

	btDiscreteDynamicsWorld* dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);

	dynamicsWorld->setGravity(btVector3(0, -100, 0));

	btCollisionShape* groundShape = new btBoxShape(btVector3(btScalar(300.), btScalar(2.), btScalar(300.)));

	btTransform groundTransform;
	groundTransform.setIdentity();
	groundTransform.setOrigin(btVector3(0, -25, 0));

	//A mass of 0 means the object is Kinematic (Static)
	btScalar mass(0.);

	btVector3 localInertia(0, 0, 0);

	//using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects (This creates a Rigidbody)
	btDefaultMotionState* myMotionState = new btDefaultMotionState(groundTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, groundShape, localInertia);
	btRigidBody* GroundRigidbody = new btRigidBody(rbInfo);

	//This dictates How much objects will bounce when they hit this object
	GroundRigidbody->setRestitution(0.75);

	//add the body to the dynamics world
	dynamicsWorld->addRigidBody(GroundRigidbody);

	btCollisionShape* BallCollisionShape = new btSphereShape(btScalar(4.));

	vec3 BallPosition = Volleyball->GetPosition();
	// Create Dynamic ball Objects
	btTransform BallTransform;
	BallTransform.setIdentity();
	BallTransform.setOrigin(btVector3(BallPosition.x, BallPosition.y, BallPosition.z));

	btScalar BallMass(2.f);

	btVector3 BallInertia(0, 0, 0);

	BallCollisionShape->calculateLocalInertia(BallMass, BallInertia);

	btDefaultMotionState* BallMotionState = new btDefaultMotionState(BallTransform);
	btRigidBody::btRigidBodyConstructionInfo BallrbInfo(BallMass, BallMotionState, BallCollisionShape, BallInertia);
	btRigidBody* BallRigidbody = new btRigidBody(BallrbInfo);

	//This dictates how bouncey the ball will be
	BallRigidbody->setRestitution(1.0);

	dynamicsWorld->addRigidBody(BallRigidbody);



	glEnable(GL_DEPTH_TEST);
	int lastTicks = SDL_GetTicks();
	int currentTicks = SDL_GetTicks();


	//Event loop, we will loop until running is set to false, usually if escape has been pressed or window is closed
	bool running = true;
	//SDL Event structure, this will be checked in the while loop
	SDL_Event ev;
	while (running)
	{

		//Poll for the events which have happened in this frame
		//https://wiki.libsdl.org/SDL_PollEvent
		while (SDL_PollEvent(&ev))
		{
			//Switch case for every message we are intereted in
			switch (ev.type)
			{
				//QUIT Message, usually called when the window has been closed
			case SDL_QUIT:
				running = false;
				break;
				//KEYDOWN Message, called when a key has been pressed down
			case SDL_KEYDOWN:
				//Check the actual key code of the key that has been pressed
				switch (ev.key.keysym.sym)
				{
					//Escape key
				case SDLK_ESCAPE:
					running = false;
					break;

				case SDLK_w:
					sceneCamera->move(0.5f);
					break;

				case SDLK_s:
					sceneCamera->move(-0.5f);
					break;

				case SDLK_a:
					sceneCamera->strafe(0.5f);
					break;

				case SDLK_d:
					sceneCamera->strafe(-0.5f);
					break;

				case SDLK_LCTRL:
					sceneCamera->lift(-0.5f);
					break;

				case SDLK_SPACE:
					sceneCamera->lift(0.5f);
					break;
				}
			}
		}

		SDL_GetRelativeMouseState(&mouseXPosition, &mouseYPosition);



		currentTicks = SDL_GetTicks();
		float deltaTime = (float)(currentTicks - lastTicks) / 1000.0f;

		dynamicsWorld->stepSimulation(1.f / 60.f, 10);

		BallTransform = BallRigidbody->getWorldTransform();

		btVector3 BallOrigin = BallTransform.getOrigin();
		btQuaternion BallRotation = BallTransform.getRotation();
		vec3 NewBallPosition = vec3(BallOrigin.getX(), BallOrigin.getY(), BallOrigin.getZ());

		Volleyball->setPosition(NewBallPosition);
		Volleyball->update();

		if (mouseXPosition != 0 && mouseYPosition != 0)
		{
			horizontalAngle += (mouseSpeed * deltaTime) * float(mouseXPosition);
			verticalAngle += (mouseSpeed * deltaTime) * float(mouseYPosition);

			sceneCamera->rotate(float(mouseXPosition), float(mouseYPosition));
		}
		//Update Screenspace
		glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
		glClearDepth(1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



		viewMatrix = sceneCamera->cameraMatrix;

		for (GameObject * pCurrentObj : gameObjectList)
		{

			vec3 lightDirection = vec3(0.0f, 0.0f, -1.0f);

			vec4 diffuseLightColour = vec4(1.0f, 1.0f, 1.0f, 1.0f);

			vec4 diffuseMaterialColour = vec4(0.8f, 0.8f, 0.8f, 1.0f);

			vec3 CameraPosition = sceneCamera->getworldPosition();

			vec4 specularLightColour = vec4(1.0f, 1.0f, 1.0f, 1.0f);

			vec4 specularMaterialColour = vec4(1.0f, 1.0f, 1.0f, 1.0f);

			float specularPower = 50.0f;

			mat4 MVPMatrix = projectionMatrix * viewMatrix * pCurrentObj->getModelMatrix();

			pCurrentObj->preRender();

			GLuint lightDirectionLocation = glGetUniformLocation(pCurrentObj->getShaderProgramID(), "lightDirection");
			GLuint diffuseLightColourLocation = glGetUniformLocation(pCurrentObj->getShaderProgramID(), "diffuseLightColour");
			GLuint diffuseMaterialColourLocation = glGetUniformLocation(pCurrentObj->getShaderProgramID(), "diffuseMaterialColour");

			GLuint cameraPositionLocation = glGetUniformLocation(pCurrentObj->getShaderProgramID(), "cameraPosition");

			GLuint specularLightColourLocation = glGetUniformLocation(pCurrentObj->getShaderProgramID(), "specularLightColour");
			GLuint specularMaterialColourLocation = glGetUniformLocation(pCurrentObj->getShaderProgramID(), "specularMaterialColour");
			GLuint specularPowerLocation = glGetUniformLocation(pCurrentObj->getShaderProgramID(), "specularPower");


			glUniform3fv(lightDirectionLocation, 1, value_ptr(lightDirection));
			glUniform4fv(diffuseLightColourLocation, 1, value_ptr(diffuseLightColour));
			glUniform4fv(diffuseMaterialColourLocation, 1, value_ptr(diffuseMaterialColour));

			glUniform3fv(cameraPositionLocation, 1, value_ptr(CameraPosition));

			glUniform4fv(diffuseLightColourLocation, 1, value_ptr(diffuseLightColour));
			glUniform4fv(diffuseMaterialColourLocation, 1, value_ptr(diffuseMaterialColour));
			glUniform1f(specularPowerLocation, specularPower);

			GLint modelMatrixLocation = glGetUniformLocation(pCurrentObj->getShaderProgramID(), "modelMatrix");

			mat4 ModelMatrix = pCurrentObj->getModelMatrix();

			glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &ModelMatrix[0][0]);

			GLint MVPMatrixLocation = glGetUniformLocation(pCurrentObj->getShaderProgramID(), "MVPMatrix");

			glUniformMatrix4fv(MVPMatrixLocation, 1, GL_FALSE, &MVPMatrix[0][0]);



			pCurrentObj->render();



		}


		SDL_GL_SwapWindow(window);

		lastTicks = currentTicks;
	}

	glDeleteVertexArrays(1, &VertexArrayID);
	glDeleteBuffers(1, &vertexBuffer);

	delete sceneCamera;
	dynamicsWorld->removeRigidBody(BallRigidbody);
	delete BallCollisionShape;
	delete BallRigidbody->getMotionState();
	delete BallRigidbody;
		
	dynamicsWorld->removeRigidBody(GroundRigidbody);
	//Delete the ground shape
	delete groundShape;

	delete GroundRigidbody->getMotionState();
	delete GroundRigidbody;

	//delete dynamics world
	delete dynamicsWorld;

	//delete solver
	delete solver;

	//delete broadphase
	delete overlappingPairCache;

	//delete dispatcher
	delete dispatcher;

	delete collisionConfiguration;

	auto iter = gameObjectList.begin();
	while (iter != gameObjectList.end())
	{
		if ((*iter))
		{

			(*iter)->destroy();
			delete (*iter);
			(*iter) = nullptr;
			iter = gameObjectList.erase(iter);
		}
	}

	deleteSDL();

	return 0;
}

int initSDL()
{
	//Initialises the SDL Library, passing in SDL_INIT_VIDEO to only initialise the video subsystems
	//https://wiki.libsdl.org/SDL_Init
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		//Display an error message box
		//https://wiki.libsdl.org/SDL_ShowSimpleMessageBox
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, SDL_GetError(), "SDL_Init failed", NULL);
		return 1;
	}

	//Create a window, note we have to free the pointer returned using the DestroyWindow Function
	//https://wiki.libsdl.org/SDL_CreateWindow
	window = SDL_CreateWindow("SDL2 Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
	//Checks to see if the window has been created, the pointer will have a value of some kind
	if (window == nullptr)
	{
		//Show error
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, SDL_GetError(), "SDL_CreateWindow failed", NULL);
		//Close the SDL Library
		//https://wiki.libsdl.org/SDL_Quit
		SDL_Quit();
		return 1;
	}

	//lets ask for a 3.2 core profile version of OpenGL
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	GL_Context = SDL_GL_CreateContext(window);
	if (GL_Context == nullptr)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, SDL_GetError(), "SDL GL Create Context failed", NULL);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return 1;
	}
	//Initialize GLEW
	glewExperimental = GL_TRUE;
	GLenum glewError = glewInit();
	if (glewError != GLEW_OK)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, (char*)glewGetErrorString(glewError), "GLEW Init Failed", NULL);
		return 1;
	}

	return 0;
}

void deleteSDL()
{
	SDL_GL_DeleteContext(GL_Context);
	//Destroy the window and quit SDL2, NB we should do this after all cleanup in this order!!!
	//https://wiki.libsdl.org/SDL_DestroyWindow
	SDL_DestroyWindow(window);
	//https://wiki.libsdl.org/SDL_Quit
	SDL_Quit();
}
