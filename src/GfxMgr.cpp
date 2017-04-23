/*
 * GfxMgr.cpp
 *
 *  Created on: Mar 9, 2017
 *      Author: sushil
 */

#include <GfxMgr.h>
#include <engine.h>
#include <unistd.h>

GfxMgr::GfxMgr(Engine *eng): Mgr(eng) {

	resources = "resources.cfg";
	plugins   = "plugins.cfg";

	ogreRoot = new Ogre::Root(plugins);

	cf.load(resources);
	loadConfig(cf);
	configureRenderSystem();
	ogreRenderWindow = ogreRoot->initialise(true, "381 Game Engine");
	createSceneManager();
	createCamera();
	createViewport();

}

void GfxMgr::loadConfig(Ogre::ConfigFile cf){
	Ogre::String name, locType;
	Ogre::ConfigFile::SectionIterator secIt = cf.getSectionIterator();
	while(secIt.hasMoreElements()){
		Ogre::ConfigFile::SettingsMultiMap* settings = secIt.getNext();
		Ogre::ConfigFile::SettingsMultiMap::iterator it;
		for(it = settings->begin(); it != settings->end(); ++it){
			locType = it->first;
			name = it->second;
			Ogre::ResourceGroupManager::getSingleton().addResourceLocation(name, locType);
		}
	}
}

void GfxMgr::configureRenderSystem(){
	if(!(ogreRoot->restoreConfig() || ogreRoot->showConfigDialog()))
		engine->stop();
}

void GfxMgr::initResources(){
	Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);
	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}

void GfxMgr::createSceneManager(){
	ogreSceneManager = ogreRoot->createSceneManager(Ogre::ST_GENERIC);
}

void GfxMgr::createCamera(){

	// Set up for first-person camera follows

	// Create main camera
	ogreCamera = ogreSceneManager->createCamera("MainCam");
	ogreCamera->setPosition(0, 5, 0);
	ogreCamera->setNearClipDistance(5);
	cameraNode = ogreSceneManager->getRootSceneNode()->createChildSceneNode();

	// Create yaw node
	yawNode = cameraNode->createChildSceneNode();

	// Create pitch node
	pitchNode = yawNode->createChildSceneNode();
	pitchNode->attachObject(ogreCamera);


	//cameraNode->setPosition(0, 100, 500);
	//ogreCamera->lookAt(0, 0, 0);

}

void GfxMgr::createViewport(){
	ogreViewport = ogreRenderWindow->addViewport(ogreCamera);
	ogreViewport->setBackgroundColour(Ogre::ColourValue(0, 0, 0));
	ogreCamera->setAspectRatio(Ogre::Real(ogreViewport->getActualWidth()) /
											Ogre::Real(ogreViewport->getActualHeight()));
}

void GfxMgr::testScene(){
	Ogre::Entity *ent = ogreSceneManager->createEntity("ogrehead.mesh");
	Ogre::SceneNode* node = ogreSceneManager->getRootSceneNode()->createChildSceneNode();
	node->attachObject(ent);
	ogreSceneManager->setAmbientLight(Ogre::ColourValue(0.5, 0.5, 0.5));
	Ogre::Light* light = ogreSceneManager->createLight("MainLight");
	light->setPosition(20, 80, 50);
	std::cout << "Test scene done" << std::endl;
}




GfxMgr::~GfxMgr() {
	delete ogreRoot; //after inputMgr destructor
}

void GfxMgr::init(){
	std::cout << "Initialize gfx" << std::endl;
	initResources();

	//testScene();
}

void GfxMgr::loadLevel(){

}


void GfxMgr::tick(float dt){
	Ogre::WindowEventUtilities::messagePump();
	//if(ogreRenderWindow->isClosed()) engine->stop();
	if(!ogreRoot->renderOneFrame()) engine->stop();
	return;
}

void GfxMgr::stop(){
	std::cout << "stopping engine and ogre" << std::endl;
	ogreRoot->shutdown();
	return;
}
