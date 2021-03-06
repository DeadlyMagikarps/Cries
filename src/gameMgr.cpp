/*
 * gameMgr.cpp
 *
 *  Created on: Mar 11, 2017
 *      Author: sushil
 */

#include <gameMgr.h>
#include <engine.h>
#include <OgreMeshManager.h>

GameMgr::GameMgr(Engine *engine): Mgr(engine), entitySceneNodes(){
	floor = Ogre::Plane(Ogre::Vector3::UNIT_Y, 0);
    ceiling = Ogre::Plane(-Ogre::Vector3::UNIT_Y, -300);
    gameplayTime = 0;
    entityCount = 0;
    grid = 0;
}

GameMgr::~GameMgr(){

}

void GameMgr::init(){

}

void GameMgr::loadLevel(){
	this->loadLevel("level002.txt");

}

void GameMgr::stop(){

}

void GameMgr::tick(float dt){
	//engine->gfxMgr->ogreAnimationState->addTime(dt);
	if(engine->theState == STATE::GAMEPLAY)
	{
		gameplayTime += dt;

		Ogre::Vector3 cameraPos = engine->gfxMgr->cameraNode->getPosition();

		engine->soundMgr->setSourceLocation("Camera1", cameraPos);
		engine->soundMgr->setSourceLocation("Camera2", cameraPos);
		engine->soundMgr->setSourceLocation("Camera3", cameraPos);
		engine->soundMgr->setSourceLocation("Camera4", cameraPos);

		if(!engine->soundMgr->isSourcePlaying("Camera1"))
		{
			engine->soundMgr->playAudio("Cycle", "Camera1");
		}

		if(!engine->soundMgr->isSourcePlaying("Camera2"))
		{
			engine->soundMgr->playAudio("Release", "Camera2");
		}

		// Are we moving?
		if(engine->inputMgr->isMoving and !engine->inputMgr->isCrouching)
		{
			// Are we sprinting?
			if(engine->inputMgr->isSprinting)
			{
				engine->soundMgr->stopSource("Camera3");
				if(!engine->soundMgr->isSourcePlaying("Camera4"))
				{
					engine->soundMgr->playAudio("Footsteps Fast", "Camera4");
				}
			}
			else
			{
				engine->soundMgr->stopSource("Camera4");
				if(!engine->soundMgr->isSourcePlaying("Camera3"))
				{
					engine->soundMgr->playAudio("Footsteps Slow", "Camera3");
				}
			}
		}
		else
		{
			engine->soundMgr->stopSource("Camera3");
			engine->soundMgr->stopSource("Camera4");
		}
	}
}


void GameMgr::createGround(int width, int heigth, std::string &material)
{
	// Create Plane //////////////////////////////////////////////////////////////////////////////////////////
	Ogre::MeshManager::getSingleton().createPlane(
	    "ground",
	    Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
	    floor,
		width, heigth, 20, 20,
	    true,
	    1, 5, 5,
	    Ogre::Vector3::UNIT_Z);
	//////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Create Ground Entity //////////////////////////////////////////////////////////////////////////////////
	Ogre::Entity* groundEntity = engine->gfxMgr->ogreSceneManager->createEntity("ground");
	engine->gfxMgr->ogreSceneManager->getRootSceneNode()->createChildSceneNode()->attachObject(groundEntity);
	groundEntity->setCastShadows(false);
	groundEntity->setMaterialName(material);
	//////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Create Separated Water Ground //////gridMgr = new Grid();//////////////////////////////////////////////////////////////////
	Ogre::Entity* groundEntity2 = engine->gfxMgr->ogreSceneManager->createEntity("ground");
	engine->gfxMgr->ogreSceneManager->getRootSceneNode()->createChildSceneNode()->attachObject(groundEntity2);
	groundEntity2->setCastShadows(false);
	groundEntity2->setMaterialName("Examples/WaterStream");
	//////////////////////////////////////////////////////////////////////////////////////////////////////////
}

void GameMgr::createCeiling(int width, int heigth)
{
	// Create Ceiling ///////////////////////////////////////////////////////////////////////////////////////
	Ogre::MeshManager::getSingleton().createPlane(
	    "ceiling",
	    Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		ceiling,
		width, heigth, 20, 20,
	    true,
	    1, 5, 5,
	    Ogre::Vector3::UNIT_Z);
	//////////////////////////////////////////////////////////////////////////////////////////////////////////


	// Create Ceiling Entity //////////////////////////////////////////////////////////////////////////////////
	Ogre::Entity* ceilingEntity = engine->gfxMgr->ogreSceneManager->createEntity("ceiling");
	engine->gfxMgr->ogreSceneManager->getRootSceneNode()->createChildSceneNode()->attachObject(ceilingEntity);
	ceilingEntity->setCastShadows(false);
	ceilingEntity->setMaterialName("Examples/Rockwall");
}

void GameMgr::createSky(){

	engine->gfxMgr->ogreSceneManager->setSkyBox(true, "Examples/SpaceSkyBox");

}

void GameMgr::loadLevel(std::string levelFilename)
{
	// Load the environment, objects, and characters
	this->loadEnvironment(levelFilename);
	this->setupEnvironment();
	this->setupSounds();
	this->loadObjects();
	this->loadCharacters();
}

void GameMgr::loadEnvironment(std::string levelFilename)
{
	// Variables
	std::ifstream fin;
	int gridRowSize, gridColSize;
	float x_offset, y_offset, z_offset;
	//float scale, orientation;
	std::string groundMaterial, objectMesh, characterMesh;
	std::string buffer, buff;
	std::map<std::string, readFromFile*> objects;


	// Open File
	fin.open(levelFilename);

	// Check for bad file
	if(!fin.is_open())
	{
		std::cerr << "ERROR, FILE CANNOT BE OPENED" << std::endl;
		return;
	}

	// First block is world dimensions and material name
	fin >> gridRowSize >> gridColSize;
	fin >> groundMaterial;

	// TESTING FILE READ ////////////////////////////////////////////
	std::cerr << "Ground Dimensions: " << gridRowSize << " x " << gridColSize << std::endl;
	std::cerr << "Ground Material: " << groundMaterial << std::endl;
	/////////////////////////////////////////////////////////////////


	// Create floor mesh with read in dimensions
	createGround( gridRowSize*1000, gridColSize*1000, groundMaterial);

	// Create Ceiling
	createCeiling( gridRowSize*1000, gridColSize*1000 );

	// Setup the grid
	this->grid = new Grid( engine->gfxMgr->ogreSceneManager, gridRowSize, gridColSize, engine);

	// Second block reads in location of you and enemies
	// Check for Objects line
	fin >> buffer;

	if( buffer != "Objects" )
	{
		std::cerr << "FILE NOT FORMATTED CORRECTLY" << std::endl;
	}

	// Read Objects and positions
	readFromFile* readEnt = new readFromFile();

	fin >> buff; // read single char

	fin >> readEnt->mesh;
	fin >> x_offset >> y_offset >> z_offset;
	fin >> readEnt->entityOrientation >> readEnt->entityScale;

	// Set entity info
	readEnt->positionOffset = Ogre::Vector3(x_offset, y_offset, z_offset);
	readEnt->entity = false;
	objects[buff] = readEnt;

	readEnt = new readFromFile(); // read in next if any

	// Check for Characters line /////////////////////////////////////////////////////////////////
	fin >> buffer;

	if( buffer != "Characters" )
	{
		std::cerr << "FILE NOT FORMATTED CORRECTLY" << std::endl;
	}

	// Read in Characters

	fin >> buff; // read next char

	fin >> readEnt->mesh;
	fin >> x_offset >> y_offset >> z_offset;
	fin >> readEnt->entityOrientation >> readEnt->entityScale;

	// set entity info
	readEnt->positionOffset = Ogre::Vector3(x_offset, y_offset, z_offset);
	readEnt->entity = true;
	objects[buff] = readEnt;

	// read next if any
	readEnt = new readFromFile();

	// Read the World Placement //////////////////////////////////////////////////////////////////
	char c;

	// Start to see if world is in file
	fin >> buffer;

	if( buffer != "World" )
	{
		std::cerr << "FILE NOT FORMATTED CORRECTLY" << std::endl;
	}

	// Pre Conditions for World setup
	/*
	Ogre::Vector3 wallPosition;
	wallPosition = Ogre::Vector3(-1850, 50, 1200);

	Ogre::Vector3 archPosition;
	archPosition = Ogre::Vector3(0, 0, 0);

	Ogre::Vector3 enemyPosition;
	enemyPosition = Ogre::Vector3(-1200, 0, 1000);
	*/

	std::ofstream fout;
	fout.open("GRID.txt");

	// Loop through map dimensions
	for( int row = 0; row < gridRowSize; row++ )
	{
		for( int col = 0; col < gridColSize; col++ )
		{
			fin >> c;

			fout << c; // test world read in

			Ogre::Vector3 gridPositionInWorld = this->grid->getPosition(row, col);


			if( c == 'P')
			{
				// Find the Start Position and set camera to there
				// Make camera position higher then rest
				//gridPositionInWorld.y += 20;
				gridPositionInWorld.y = gridPositionInWorld.y + 100;
				engine->gfxMgr->cameraNode->setPosition( gridPositionInWorld );
				std::cerr << gridPositionInWorld << std::endl;
			}


			// Check for walls (Not player or enemy nodes)
			if( c == 'W' )
			{

				// Grab the "objects" config stuff from the object map that Hadi built
				//readFromFile * objectEntData = objects["D"]; // Currently not used lmao

				GridParams * gridParam =  this->grid->getGrid(row, col);
				if(gridParam) gridParam->notWalkable();

				engine->entityMgr->CreateEntity(EntityType::WALL, gridPositionInWorld, 0);

				//objectEntData = NULL;
				gridParam = NULL;
			}


			// Check for Arch Facing North South
			else if( c == 'A' )
			{
				/*
//				std::cerr << "Spawning Arch" << std::endl;
//				engine->entityMgr->CreateEntity(EntityType::ARCH, archPosition, 0);
//				archPosition.x += 50;
				//readFromFile * objectEntData = objects["C"]; // Currently not used lmao
				*/

				//GridParams * gridParam =  this->grid->getGrid(row, col);
				//if(gridParam) gridParam->notWalkable();

				//engine->entityMgr->CreateEntity(EntityType::ARCH, gridPositionInWorld, 0);

				//objectEntData = NULL;
				//gridParam = NULL;

				//objectEntData = NULL;
			}

			// See no evil
			else if( c == 'C' )
			{
				engine->entityMgr->CreateEntity(EntityType::SEENO, gridPositionInWorld, 0);
			}

			else if(c == 'S')
			{
				engine->entityMgr->CreateEntity(EntityType::SPEAKNO, gridPositionInWorld, 0);
			}
			else if(c == 'H')
			{
				engine->entityMgr->CreateEntity(EntityType::HEARNO, gridPositionInWorld, 0);
			}

			// Create end arch
			else if(c == 'E')
			{
				engine->entityMgr->CreateEntity(EntityType::ENDARCH, gridPositionInWorld, 0);
				endPts.push_back(grid->getPos(gridPositionInWorld));
			}
		}

		fout << std::endl;
	}

	fout.close();
	// Create the Entities

	// Create Skybox for the hell of it
	createSky();
//	delete readEnt;

}

void GameMgr::setupEnvironment()
{
	//We know graphicsMgr is ready and initialized
	engine->gfxMgr->ogreSceneManager->setAmbientLight(Ogre::ColourValue(0.3, 0.3, 0.3));
	Ogre::Light* light = engine->gfxMgr->ogreSceneManager->createLight("MainLight");
	light->setType(Ogre::Light::LT_POINT);
	light->setPosition(100.0, 800.0, 100.0);
	light->setDiffuseColour(Ogre::ColourValue::White);
	light->setSpecularColour(Ogre::ColourValue::White);
}

/*
 *
 * Basic Setup for pre-alpha testing
 * Will be more complex in later phases
 *
 */
void GameMgr::setupSounds()
{

	Ogre::Vector3 cameraPos = engine->gfxMgr->cameraNode->getPosition();

	// Stop the background music playing (it was probably the theme)
	engine->soundMgr->stopSource("Camera1");

	// Caution. This function doesnt check if the source already exists :)
	engine->soundMgr->createSource("Camera3");
	engine->soundMgr->createSource("Camera4");
	engine->soundMgr->createSource("Camera5");

	engine->soundMgr->setSourceLocation("Camera1", cameraPos);
	engine->soundMgr->setSourceLocation("Camera2", cameraPos);
	engine->soundMgr->setSourceLocation("Camera3", cameraPos);
	engine->soundMgr->setSourceLocation("Camera4", cameraPos);
	engine->soundMgr->setSourceLocation("Camera5", cameraPos);

	// Load new background music
	if(engine->omegaOn)
	{
		engine->soundMgr->loadAudio("September", "sounds/september.wav");

	}
	else
	{
		engine->soundMgr->loadAudio("Cycle", "sounds/Cries - Cycle16.wav");
		engine->soundMgr->loadAudio("Release", "sounds/Cries - Release16.wav");
	}

	// Load SFX
	engine->soundMgr->loadAudio("Footsteps Slow", "sounds/Cries - FootstepsSlow16.wav");
	engine->soundMgr->loadAudio("Footsteps Fast", "sounds/Cries - FootstepsFast16.wav");
	engine->soundMgr->loadAudio("Breathing", "sounds/Cries - Breathing16.wav");

	if(engine->omegaOn)
	{
		engine->soundMgr->playAudio("September", "Camera1");
	}
	else
	{
		engine->soundMgr->playAudio("Cycle", "Camera1");
		engine->soundMgr->playAudio("Release", "Camera2");
	}

}

void GameMgr::loadObjects()
{
	// Create Entity
	Ogre::Entity *splash = engine->gfxMgr->ogreSceneManager->createEntity("Splash.mesh");

	// Create scene node for this entity
	engine->gfxMgr->splashNode = engine->gfxMgr->ogreSceneManager->getRootSceneNode()->createChildSceneNode();
	engine->gfxMgr->splashNode->attachObject(splash);
	splash->setMaterialName("Material");
	engine->gfxMgr->splashNode->setScale(10.0f, 10.0f, 10.0f);
	engine->gfxMgr->splashNode->setPosition( 0.0f, 400, -3500);
	engine->gfxMgr->splashNode->roll(Ogre::Degree(-360));
	engine->gfxMgr->splashNode->pitch(Ogre::Degree(90));
}

void GameMgr::loadCharacters()
{
	//Creating the entities
//	engine->entityMgr->CreateEntity(EntityType::HEARNO, Ogre::Vector3(0, 10, -1000), 0);
//	engine->entityMgr->CreateEntity(EntityType::SEENO, Ogre::Vector3(500, 0, -500), Ogre::Math::HALF_PI / 2);
//	engine->entityMgr->CreateEntity(EntityType::SPEAKNO, Ogre::Vector3(-500, 20, -500), Ogre::Math::HALF_PI / -2);
/*
	// Set Animation States
	//engine->gfxMgr->ogreAnimationState = splash->getAnimationState("Test");
	engine->gfxMgr->ogreAnimationState->setWeight(1);
	engine->gfxMgr->ogreAnimationState->setLoop(true);
	engine->gfxMgr->ogreAnimationState->setEnabled(true);
*/


}

std::string GameMgr:: getNewName()
{

	return std::string("object_" + std::to_string(entityCount++));
}

Grid* GameMgr::getGrid() const
{
	return this->grid;
}

