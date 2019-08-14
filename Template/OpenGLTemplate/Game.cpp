/* 
OpenGL Template for INM376 / IN3005
City University London, School of Mathematics, Computer Science and Engineering
Source code drawn from a number of sources and examples, including contributions from
 - Ben Humphrey (gametutorials.com), Michal Bubner (mbsoftworks.sk), Christophe Riccio (glm.g-truc.net)
 - Christy Quinn, Sam Kellett and others

 For educational use by Department of Computer Science, City University London UK.

 This template contains a skybox, simple terrain, camera, lighting, shaders, texturing

 Potential ways to modify the code:  Add new geometry types, shaders, change the terrain, load new meshes, change the lighting, 
 different camera controls, different shaders, etc.
 
 Template version 5.0a 29/01/2017
 Dr Greg Slabaugh (gregory.slabaugh.1@city.ac.uk) 
*/


#include "game.h"
#include <math.h>

// Setup includes
#include "HighResolutionTimer.h"
#include "GameWindow.h"

// Game includes
#include "Camera.h"
#include "Skybox.h"
#include "Plane.h"
#include "Shaders.h"
#include "FreeTypeFont.h"
#include "Sphere.h"
#include "MatrixStack.h"
#include "OpenAssetImportMesh.h"
#include "Audio.h"
#include "CatmullRom.h"
#include "Player.h"
#include "obstacle.h"
#include "pyramid.h"
#include "Cube.h"

// Constructor
Game::Game()
{
	m_pSkybox = NULL;
	m_pCamera = NULL;
	m_pShaderPrograms = NULL;
	m_pPlanarTerrain = NULL;
	m_pFtFont = NULL;
	m_pBarrelMesh = NULL;
	m_pHorseMesh = NULL;
	m_pCarMesh = NULL;
	m_pSphere = NULL;
	m_pHighResolutionTimer = NULL;
	m_pAudio = NULL;
	m_pCatmullRom = NULL;
	m_pConeMesh = NULL;
	m_pCube = NULL;
	m_dt = 0.0;
	m_framesPerSecond = 0;
	m_frameCount = 0;
	m_elapsedTime = 0.0f;
}

// Destructor
Game::~Game() 
{ 
	//game objects
	delete m_pCamera;
	delete m_pSkybox;
	delete m_pPlanarTerrain;
	delete m_pFtFont;
	delete m_pBarrelMesh;
	delete m_pHorseMesh;
	delete m_pSphere;
	delete m_pAudio;
	delete m_pCatmullRom;
	delete m_Player;
	delete m_pCube;

	if (m_pShaderPrograms != NULL) {
		for (unsigned int i = 0; i < m_pShaderPrograms->size(); i++)
			delete (*m_pShaderPrograms)[i];
	}
	delete m_pShaderPrograms;

	//setup objects
	delete m_pHighResolutionTimer;
}

// Initialisation:  This method only runs once at startup
void Game::Initialise() 
{
	
	// Set the clear colour and depth
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClearDepth(1.0f);

	amLight = 1;

	/// Create objects
	m_pCamera = new CCamera;
	m_pSkybox = new CSkybox;
	m_pShaderPrograms = new vector <CShaderProgram *>;
	m_pPlanarTerrain = new CPlane;
	m_pFtFont = new CFreeTypeFont;
	m_pBarrelMesh = new COpenAssetImportMesh;
	m_pHorseMesh = new COpenAssetImportMesh;
	m_pCarMesh = new COpenAssetImportMesh;
	m_pConeMesh = new COpenAssetImportMesh;
	m_pWellMesh = new COpenAssetImportMesh;
	m_pStonehengeMesh = new COpenAssetImportMesh;
	m_pSphere = new CSphere;
	m_pAudio = new CAudio;
	m_pCatmullRom = new CCatmullRom;
	m_Player = new Player(glm::vec3(0,0,0));
	
	m_pPyramid = new pyramid;
	m_pCube = new CCube;
	

	RECT dimensions = m_gameWindow.GetDimensions();

	int width = dimensions.right - dimensions.left;
	int height = dimensions.bottom - dimensions.top;

	// Set the orthographic and perspective projection matrices based on the image size
	m_pCamera->SetOrthographicProjectionMatrix(width, height); 
	m_pCamera->SetPerspectiveProjectionMatrix(45.0f, (float) width / (float) height, 0.5f, 5000.0f);

	// Load shaders
	vector<CShader> shShaders;
	vector<string> sShaderFileNames;
	sShaderFileNames.push_back("mainShader.vert");
	sShaderFileNames.push_back("mainShader.frag");
	sShaderFileNames.push_back("textShader.vert");
	sShaderFileNames.push_back("textShader.frag");

	for (int i = 0; i < (int) sShaderFileNames.size(); i++) {
		string sExt = sShaderFileNames[i].substr((int) sShaderFileNames[i].size()-4, 4);
		int iShaderType;
		if (sExt == "vert") iShaderType = GL_VERTEX_SHADER;
		else if (sExt == "frag") iShaderType = GL_FRAGMENT_SHADER;
		else if (sExt == "geom") iShaderType = GL_GEOMETRY_SHADER;
		else if (sExt == "tcnl") iShaderType = GL_TESS_CONTROL_SHADER;
		else iShaderType = GL_TESS_EVALUATION_SHADER;
		CShader shader;
		shader.LoadShader("resources\\shaders\\"+sShaderFileNames[i], iShaderType);
		shShaders.push_back(shader);
	}

	// Create the main shader program
	CShaderProgram *pMainProgram = new CShaderProgram;
	pMainProgram->CreateProgram();
	pMainProgram->AddShaderToProgram(&shShaders[0]);
	pMainProgram->AddShaderToProgram(&shShaders[1]);
	pMainProgram->LinkProgram();
	m_pShaderPrograms->push_back(pMainProgram);

	// Create a shader program for fonts
	CShaderProgram *pFontProgram = new CShaderProgram;
	pFontProgram->CreateProgram();
	pFontProgram->AddShaderToProgram(&shShaders[2]);
	pFontProgram->AddShaderToProgram(&shShaders[3]);
	pFontProgram->LinkProgram();
	m_pShaderPrograms->push_back(pFontProgram);

	// You can follow this pattern to load additional shaders

	// Create the skybox
	// Skybox downloaded from http://www.akimbo.in/forum/viewtopic.php?f=10&t=9
	m_pSkybox->Create(2500.0f);
	
	//create the cube
	m_pCube->Create("resources\\textures\\dirtpile01.jpg");
	//m_pPyramid->Create("resources\\textures\\dirtpile01.jpg");

	// Create the planar terrain
	m_pPlanarTerrain->Create("resources\\textures\\", "grassfloor01.jpg", 2000.0f, 2000.0f, 50.0f); // Texture downloaded from http://www.psionicgames.com/?page_id=26 on 24 Jan 2013

	m_pFtFont->LoadSystemFont("arial.ttf", 32);
	m_pFtFont->SetShaderProgram(pFontProgram);

	// Load some meshes in OBJ format
	m_pBarrelMesh->Load("resources\\models\\Barrel\\Barrel02.obj");  // Downloaded from http://www.psionicgames.com/?page_id=24 on 24 Jan 2013
	m_pHorseMesh->Load("resources\\models\\Horse\\Horse2.obj");  // Downloaded from http://opengameart.org/content/horse-lowpoly on 24 Jan 2013
	m_pCarMesh->Load("resources\\models\\car1\\car\\car.obj");
	m_pConeMesh->Load("resources\\models\\low\\low.obj");
	m_Player->getMesh()->Load("resources\\models\\Horse\\Horse2.obj");
	m_pWellMesh->Load("resources\\models\\well\\well.obj");
	m_pStonehengeMesh->Load("resources\\models\\Stonehenge\\Stonehengebed.obj");
	

	// Create a sphere
	m_pSphere->Create("resources\\textures\\", "dirtpile01.jpg", 25, 25);  // Texture downloaded from http://www.psionicgames.com/?page_id=26 on 24 Jan 2013
	glEnable(GL_CULL_FACE);

	// Initialise audio and play background music
	m_pAudio->Initialise();
	m_pAudio->LoadEventSound("Resources\\Audio\\Boing.wav");					// Royalty free sound from freesound.org
	m_pAudio->LoadMusicStream("Resources\\Audio\\DST-Garote.mp3");	// Royalty free music from http://www.nosoapradio.us/
	//m_pAudio->PlayMusicStream();

	glm::vec3 p0 = glm::vec3(-500, 10, -200);
	glm::vec3 p1 = glm::vec3(0, 10, -200);
	glm::vec3 p2 = glm::vec3(0, 10, 200);
	glm::vec3 p3 = glm::vec3(-500, 10, 200);

	m_pCatmullRom->CreateCentreline();
	m_pCatmullRom->CreateOffsetCurves();
	m_pCatmullRom->CreateTrack();
	//m_pCatmullRom->CreateBorders(4);
	m_currentDistance = 0.0f;//
	m_cameraSpeed = 0.7f;

	m_Player->SetCamera(m_pCamera);

	//create the barrels
	//obstacles.push_back()
	vector<glm::vec3*> tmp;

	
	//obstacles = tmp;
}

// Render method runs repeatedly in a loop
void Game::Render() 
{
	


	// Clear the buffers and enable depth testing (z-buffering)
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	// Set up a matrix stack
	glutil::MatrixStack modelViewMatrixStack;
	modelViewMatrixStack.SetIdentity();

	// Use the main shader program 
	CShaderProgram *pMainProgram = (*m_pShaderPrograms)[0];
	pMainProgram->UseProgram();
	pMainProgram->SetUniform("bUseTexture", true);
	pMainProgram->SetUniform("sampler0", 0);
	// Note: cubemap and non-cubemap textures should not be mixed in the same texture unit.  Setting unit 10 to be a cubemap texture.
	int cubeMapTextureUnit = 10; 
	pMainProgram->SetUniform("CubeMapTex", cubeMapTextureUnit);
	

	

	// Set the projection matrix
	pMainProgram->SetUniform("matrices.projMatrix", m_pCamera->GetPerspectiveProjectionMatrix());

	// Call LookAt to create the view matrix and put this on the modelViewMatrix stack. 
	// Store the view matrix and the normal matrix associated with the view matrix for later (they're useful for lighting -- since lighting is done in eye coordinates)
	modelViewMatrixStack.LookAt(m_pCamera->GetPosition(), m_pCamera->GetView(), m_pCamera->GetUpVector());
	glm::mat4 viewMatrix = modelViewMatrixStack.Top();
	glm::mat3 viewNormalMatrix = m_pCamera->ComputeNormalMatrix(viewMatrix);


	
	
	// Set light and materials in main shader program
	// Set light and materials in sphere programme
	

	//set the ambient lighting colour
	//pMainProgram->SetUniform("La", glm::vec3(1.0f, 1.0f, 1.0f));

	glm::vec4 lightPosition1(100, 100, 0, 1); // Position of light source *in world coordinates*
	pMainProgram->SetUniform("light1.position", viewMatrix*lightPosition1); // Light position in eye coordinates
	pMainProgram->SetUniform("light1.La", glm::vec3(1.0f, 1.0f, 1.0f));
	pMainProgram->SetUniform("light1.Ld", glm::vec3(1.0f, 1.0f, 1.0f));
	pMainProgram->SetUniform("light1.Ls", glm::vec3(1.0f, 1.0f, 1.0f));
	pMainProgram->SetUniform("light1.direction",
		glm::normalize(viewNormalMatrix*glm::vec3(0, -1, 0)));
	pMainProgram->SetUniform("light1.exponent", 20.0f);
	pMainProgram->SetUniform("light1.cutoff", 30.0f);
	// set light 2
	glm::vec4 lightPosition2(-71, 50, -70, 1); // Position of light source *in world coordinates*
	pMainProgram->SetUniform("light2.position", viewMatrix*lightPosition2); // Light position in eye coordinates
	pMainProgram->SetUniform("light2.La", glm::vec3(0.0f, 0.0f, 1.0f));
	pMainProgram->SetUniform("light2.Ld", glm::vec3(0.0f, 0.0f, 1.0f));
	pMainProgram->SetUniform("light2.Ls", glm::vec3(0.0f, 0.0f, 1.0f));
	pMainProgram->SetUniform("light2.direction",
		glm::normalize(viewNormalMatrix*glm::vec3(0, -1, 0)));
	pMainProgram->SetUniform("light2.exponent", 20.0f);
	pMainProgram->SetUniform("light2.cutoff", 30.0f);

	// set light 3
	glm::vec4 lightPosition3(-100, 110, -100, 1); // Position of light source *in world coordinates*
	pMainProgram->SetUniform("light3.position", viewMatrix*lightPosition2); // Light position in eye coordinates
	pMainProgram->SetUniform("light3.La", glm::vec3(1.0f, 1.0f, 1.0f));
	pMainProgram->SetUniform("light3.Ld", glm::vec3(1.0f, 1.0f, 1.0f));
	pMainProgram->SetUniform("light3.Ls", glm::vec3(1.0f, 1.0f, 1.0f));
	

	pMainProgram->SetUniform("material1.Ma", glm::vec3(1.0f));	// Ambient material reflectance
	pMainProgram->SetUniform("material1.Md", glm::vec3(1.0f));	// Diffuse material reflectance
	pMainProgram->SetUniform("material1.Ms", glm::vec3(1.0f));	// Specular material reflectance
	pMainProgram->SetUniform("material1.shininess", 15.0f);		// Shininess material property
		
	pMainProgram->SetUniform("ambientBrightness", amLight);	//abmient brightness property

	// Render the skybox and terrain with full ambient reflectance 
	modelViewMatrixStack.Push();
		pMainProgram->SetUniform("renderSkybox", true);
		// Translate the modelview matrix to the camera eye point so skybox stays centred around camera
		glm::vec3 vEye = m_pCamera->GetPosition();
		modelViewMatrixStack.Translate(vEye);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pSkybox->Render(cubeMapTextureUnit);
		pMainProgram->SetUniform("renderSkybox", false);
	modelViewMatrixStack.Pop();

	

	// Render the planar terrain
	modelViewMatrixStack.Push();
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pPlanarTerrain->Render();
	modelViewMatrixStack.Pop();

// Turn on diffuse + specular materials
	pMainProgram->SetUniform("material1.Ma", glm::vec3(0.40f));	// Ambient material reflectance
	pMainProgram->SetUniform("material1.Md", glm::vec3(0.40f));	// Diffuse material reflectance
	pMainProgram->SetUniform("material1.Ms", glm::vec3(0.40f));	// Specular material reflectance
		


	// Render the horse 
	modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(glm::vec3(0.0f, 0.0f, 0.0f));
		modelViewMatrixStack.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), 180.0f);
		modelViewMatrixStack.Scale(2.5f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pHorseMesh->Render();
	modelViewMatrixStack.Pop();

	// Render the player 
	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(m_Player->getPosition()+glm::vec3(0,2,0));
	glm::vec2 pRot(m_Player->getRotation().x, m_Player->getRotation().z);
	modelViewMatrixStack.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), 90+ glm::atan(pRot.x, pRot.y) * 180 / 3.1415);
	modelViewMatrixStack.Scale(5.0f);
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	if (m_pCamera->getMode() != 0)
		
		m_pCarMesh->Render();
	modelViewMatrixStack.Pop();

	
	// Render the barrel 
	modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(glm::vec3(100.0f, 0.0f, 0.0f));
		modelViewMatrixStack.Scale(5.0f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pBarrelMesh->Render();
	modelViewMatrixStack.Pop();

	// Render the cube
	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(glm::vec3(10.0f, 5.0f, 0.0f));
	modelViewMatrixStack.Scale(5.0f);
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_pCube->Render();
	modelViewMatrixStack.Pop();

	// render the pyramid
	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(glm::vec3(10.0f, 5.0f, 0.0f));
	modelViewMatrixStack.Scale(5.0f);
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_pPyramid->render();
	modelViewMatrixStack.Pop();

	// Render stonehenge
	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(glm::vec3(170, 5.0f, 0.0f));
	modelViewMatrixStack.Scale(20.0f);
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_pStonehengeMesh->Render();
	modelViewMatrixStack.Pop();
	
	// Render the well
	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(glm::vec3(60, 5.0f, 0.0f));
	modelViewMatrixStack.Scale(2.0f);
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_pWellMesh->Render();
	modelViewMatrixStack.Pop();

	// Render the edge of the track
	for (auto a : m_pCatmullRom->conePositions)
	{
		modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(a);
		modelViewMatrixStack.Scale(5.0f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pConeMesh->Render();
		modelViewMatrixStack.Pop();
	}
	
	
	// Render the obstacles 
	modelViewMatrixStack.Push();
	for (int i = 0; i<1; i++)
	{
		modelViewMatrixStack.Translate(m_pCatmullRom->getObstacle(i));
		modelViewMatrixStack.Scale(5.0f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		m_pBarrelMesh->Render();
		//m_pConeMesh->Render();
		modelViewMatrixStack.Pop();
	}

	/*
	//render spline
	modelViewMatrixStack.Push();
	pMainProgram->SetUniform("bUseTexture", false); // turn off texturing
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix",
		m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	// Render your object here
	m_pCatmullRom->RenderCentreline();
	
	modelViewMatrixStack.Pop();
	

	//render offset Curves
	modelViewMatrixStack.Push();
	pMainProgram->SetUniform("bUseTexture", false); // turn off texturing
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix",
		m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	// Render your object here
	m_pCatmullRom->RenderOffsetCurves();

	modelViewMatrixStack.Pop();
*/

	// render the track
	modelViewMatrixStack.Push();
	pMainProgram->SetUniform("bUseTexture", true); // turn off texturing
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix",
		m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	// Render your object here
	
	m_pCatmullRom->RenderTrack();
	modelViewMatrixStack.Pop();


	// Render the sphere
	modelViewMatrixStack.Push();
		modelViewMatrixStack.Translate(glm::vec3(72.0f, 8.0f, 72.0f));
		modelViewMatrixStack.Scale(2.0f);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		// To turn off texture mapping and use the sphere colour only (currently white material), uncomment the next line
		pMainProgram->SetUniform("bUseTexture", true);
		m_pSphere->Render();
	modelViewMatrixStack.Pop();

	/*
	//render the pyramid
	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(glm::vec3(0.0f, 20.0f, 0.0f));
	modelViewMatrixStack.Scale(2.0f);
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	// To turn off texture mapping and use the sphere colour only (currently white material), uncomment the next line
	pMainProgram->SetUniform("bUseTexture", true);
	m_pPyramid->render();
	modelViewMatrixStack.Pop();
	*/	

	// Draw the 2D graphics after the 3D graphics
	DisplayFrameRate();

	// Swap buffers to show the rendered image
	SwapBuffers(m_gameWindow.Hdc());		

	
}

// Update method runs repeatedly with the Render method
void Game::Update() 
{
	
	
	// Update the camera using the amount of time that has elapsed to avoid framerate dependent motion
	m_pCamera->Update(m_dt);
	static float t = 0.0f;
	t += 0.0005f * (float)m_dt;

	/*
	if (m_currentDistance > 500.0f)
		t = t;
	*/

	m_currentDistance =+ 120*t;
	
	glm::vec3 pNext;
	m_pCatmullRom->Sample(m_currentDistance + 1, pNext);
	glm::vec3 tangent;
	glm::vec3 p;
	m_pCatmullRom->Sample(m_currentDistance, p);

	tangent = pNext-p;
	glm::normalize(tangent);

	glm::vec3 normal = glm::cross(tangent, glm::vec3(0, 1, 0));
	glm::normalize(normal);
	glm::vec3 o(0, 0, 0);
	glm::vec3 laneOffset(m_pCamera->getXOffset()*normal);
	//m_pCamera->Set(p+glm::vec3(0,1,0)+ laneOffset, p + laneOffset + 1000.0f*tangent, glm::vec3(0, 1, 0));
	m_Player->setPosition((p +  laneOffset));
	m_Player->setRotation(p + laneOffset + 1000.0f*tangent);
	glm::vec3 temp = m_Player->getPosition();
	glm::vec3 lookAtPlayer = m_Player->getPosition();
	glm::vec3 playerHeight(0, 4, 0);

	
	switch (m_pCamera->getMode())
	{
	case 0:
		m_pCamera->Set(m_Player->getPosition() + playerHeight, m_Player->getRotation(), glm::vec3(0, 1, 0));
		break;
	case 1:
		m_pCamera->Set(m_Player->getPosition() + playerHeight - tangent*10.0f + glm::vec3(0, 4, 0), m_Player->getPosition(), glm::vec3(0, 1, 0));
		break;
	case 2:
		m_pCamera->Set(m_Player->getPosition() + glm::vec3(0, 40, 0),  m_Player->getPosition(), glm::vec3(0, 0, 1));
		break;
	case 3:
		m_pCamera->Set(m_Player->getPosition()+ playerHeight + normal*-10.0f, m_Player->getPosition(), glm::vec3(0, 1, 0));
		break;
	}

	//m_pCamera->Set(m_Player->getPosition()-tangent*10.0f + glm::vec3(0, 4, 0) , m_Player->getRotation(), glm::vec3(0, 1, 0));
	for (int i = 0; i < 1; i++) {
		if (glm::distance(m_pCatmullRom->getObstacle(i), m_Player->getPosition())<7)//a->getPosition()-m_Player->getPosition()))
		{ 
			t = 0;
			m_currentDistance = 0;
			
		}
		
	}
	
		amLight = m_pCamera->amLight;

	m_pAudio->Update();
}



void Game::DisplayFrameRate()
{


	CShaderProgram *fontProgram = (*m_pShaderPrograms)[1];

	RECT dimensions = m_gameWindow.GetDimensions();
	int height = dimensions.bottom - dimensions.top;

	// Increase the elapsed time and frame counter
	m_elapsedTime += m_dt;
	m_frameCount++;

	// Now we want to subtract the current time by the last time that was stored
	// to see if the time elapsed has been over a second, which means we found our FPS.
	if (m_elapsedTime > 1000)
    {
		m_elapsedTime = 0;
		m_framesPerSecond = m_frameCount;

		// Reset the frames per second
		m_frameCount = 0;
    }

	if (m_framesPerSecond > 0) {
		// Use the font shader program and render the text
		fontProgram->UseProgram();
		glDisable(GL_DEPTH_TEST);
		fontProgram->SetUniform("matrices.modelViewMatrix", glm::mat4(1));
		fontProgram->SetUniform("matrices.projMatrix", m_pCamera->GetOrthographicProjectionMatrix());
		fontProgram->SetUniform("vColour", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
		m_pFtFont->Render(20, height - 20, 20, "FPS: %d", m_framesPerSecond);
	}
}

// The game loop runs repeatedly until game over
void Game::GameLoop()
{
	/*
	// Fixed timer
	dDt = pHighResolutionTimer->Elapsed();
	if (dDt > 1000.0 / (double) Game::FPS) {
		pHighResolutionTimer->Start();
		Update();
		Render();
	}
	*/
	
	
	// Variable timer
	m_pHighResolutionTimer->Start();
	Update();
	Render();
	m_dt = m_pHighResolutionTimer->Elapsed();
	

}


WPARAM Game::Execute() 
{
	m_pHighResolutionTimer = new CHighResolutionTimer;
	m_gameWindow.Init(m_hInstance);

	if(!m_gameWindow.Hdc()) {
		return 1;
	}

	Initialise();

	m_pHighResolutionTimer->Start();

	
	MSG msg;

	while(1) {													
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) { 
			if(msg.message == WM_QUIT) {
				break;
			}

			TranslateMessage(&msg);	
			DispatchMessage(&msg);
		} else if (m_appActive) {
			GameLoop();
		} 
		else Sleep(200); // Do not consume processor power if application isn't active
	}

	m_gameWindow.Deinit();

	return(msg.wParam);
}

LRESULT Game::ProcessEvents(HWND window,UINT message, WPARAM w_param, LPARAM l_param) 
{
	LRESULT result = 0;

	switch (message) {


	case WM_ACTIVATE:
	{
		switch(LOWORD(w_param))
		{
			case WA_ACTIVE:
			case WA_CLICKACTIVE:
				m_appActive = true;
				m_pHighResolutionTimer->Start();
				break;
			case WA_INACTIVE:
				m_appActive = false;
				break;
		}
		break;
		}

	case WM_SIZE:
			RECT dimensions;
			GetClientRect(window, &dimensions);
			m_gameWindow.SetDimensions(dimensions);
		break;

	case WM_PAINT:
		PAINTSTRUCT ps;
		BeginPaint(window, &ps);
		EndPaint(window, &ps);
		break;

	case WM_KEYDOWN:
		switch(w_param) {
		case VK_ESCAPE:
			PostQuitMessage(0);
			break;
		case '1':
			m_pAudio->PlayEventSound();
			break;
		case VK_F1:
			m_pAudio->PlayEventSound();
			break;
		}
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		result = DefWindowProc(window, message, w_param, l_param);
		break;
	}

	return result;
}

Game& Game::GetInstance() 
{
	static Game instance;

	return instance;
}

void Game::SetHinstance(HINSTANCE hinstance) 
{
	m_hInstance = hinstance;
}

LRESULT CALLBACK WinProc(HWND window, UINT message, WPARAM w_param, LPARAM l_param)
{
	return Game::GetInstance().ProcessEvents(window, message, w_param, l_param);
}

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE, PSTR, int) 
{
	Game &game = Game::GetInstance();
	game.SetHinstance(hinstance);

	return game.Execute();
}
