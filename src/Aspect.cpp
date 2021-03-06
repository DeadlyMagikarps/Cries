/*
 * Aspect.cpp
 *
 *  Created on: Feb 22, 2017
 *      Author: sushil
 */

#include <Aspect.h>
#include <Utils.h>
#include <math.h>

Aspect::Aspect (Entity381 * ent){
	this->entity = ent;
	this->aspectType = ASPECT_TYPE::NONE;

}

Aspect::~Aspect(){

}


Renderable::Renderable(Entity381 * ent): Aspect(ent){
	this->aspectType = ASPECT_TYPE::RENDERABLE;
	return;
}

Renderable::~Renderable(){

}

void Renderable::Tick(float dt){
	entity->ogreSceneNode->setPosition(entity->pos);
	entity->ogreSceneNode->setOrientation(Ogre::Quaternion::IDENTITY);
	entity->ogreSceneNode->yaw(Ogre::Radian(-entity->heading + Ogre::Math::HALF_PI));
}

Physics::Physics(Entity381 * ent): Aspect(ent) {
	this->aspectType = ASPECT_TYPE::PHYSICS;
}

Physics::~Physics(){

}

//static float FixAngle(float radiansAngle){
//
//	while (radiansAngle > Ogre::Math::TWO_PI)
//		radiansAngle -= Ogre::Math::TWO_PI;
//	while (radiansAngle < 0)
//		radiansAngle += Ogre::Math::TWO_PI;
//
//	return radiansAngle;
//}
//static float RadToDegree = 57.2958; //180/pi

void Physics::Tick(float dt){

	if(entity->speed < entity->desiredSpeed)
		entity->speed += entity->acceleration *dt;
	if(entity->speed > entity->desiredSpeed)
		entity->speed -= entity->acceleration *dt;

	//entity->speed = std::max(entity->minSpeed, std::min(entity->speed, entity->maxSpeed)); //clamp


	if(entity->heading < entity->desiredHeading)
		entity->heading += entity->turnRate * dt;
	if(entity->heading > entity->desiredHeading)
		entity->heading -= entity->turnRate * dt;

	entity->heading = FixAngle(entity->heading); // between -pi and pi

	entity->vel = Ogre::Vector3(cos(entity->heading) * entity->speed, 0, sin(entity->heading) * entity->speed);
	entity->pos += entity->vel * dt;
	//If this movement places the entity in a wall, reverse direction
	if(!(entity->engine->gameMgr->grid->getPos(entity->pos)->isWalkable()))
	{
		entity->pos -= entity->vel *dt;
	}
	Ogre::Vector3 diff = entity->pos - entity->engine->gfxMgr->cameraNode->getPosition();
	diff.y = 0;//Don't count height differences
	if(diff.length() < 150 && (entity->entityType == EntityType::SEENO ||
			entity->entityType == EntityType::SPEAKNO || entity->entityType == EntityType::HEARNO))
	{
		std::cout << "Game over distance reached" << std::endl;
		entity->engine->theState = STATE::GAMEOVER;
		entity->engine->uiMgr->loadGameOver();
	}
	/*if(entity->entityType == EntityType::SEENO || entity->entityType == EntityType::SPEAKNO
			|| entity->entityType == EntityType::HEARNO)
	{
		entity->engine->entityMgr->fixCollisions(entity);
	}*/
	entity->pos.y = 60.0f;

}

UnitAI::UnitAI(Entity381* ent): Aspect(ent){
	commands.clear();
	this->aspectType = ASPECT_TYPE::AI;
}

UnitAI::~UnitAI(){

}

void UnitAI::AddCommand(Command *c){
	commands.push_back(c);
}

void UnitAI::SetCommand(Command* c){
	delete commands.front();
	commands.clear();
	commands.push_front(c);
}

void UnitAI::Tick(float dt){

	if(!commands.empty()){
		commands.front()->tick(dt);
		if(commands.front()->done()){
			commands.clear();
		}
	}
}

void UnitAI::clear()
{
	commands.clear();
}

