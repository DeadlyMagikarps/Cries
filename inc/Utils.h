/*
 * Utils.h
 *
 *  Created on: Feb 22, 2017
 *      Author: sushil
 */

#ifndef UTILS_H_
#define UTILS_H_

#include <OgreVector3.h>


inline float clamp(float value, float lower, float upper){
	if (value > upper)
		return upper;
	if(value < lower)
		return lower;
	return value;
}

inline float makeAnglePosNeg(float angle)
{
    //printf("makeAnglePosNeg:%f ", angle);
    while (angle > Ogre::Math::PI)
        angle -= Ogre::Math::TWO_PI;
    while (angle < -Ogre::Math::PI)
        angle += Ogre::Math::TWO_PI;
    //printf("->%f ", angle);
    return angle;
}

inline float FixAngle(float radiansAngle){

	while (radiansAngle > Ogre::Math::PI)
		radiansAngle -= Ogre::Math::TWO_PI;
	while (radiansAngle < -1 * Ogre::Math::TWO_PI)
		radiansAngle += Ogre::Math::TWO_PI;

	return radiansAngle;
}
inline float differenceBetweenAngles(float angle1, float angle2){
    return makeAnglePosNeg(angle1 - angle2);
}

inline void setMagnitude(Ogre::Vector3 &vec, float mag)
{
	float multiplier = mag / vec.length();
	vec = vec * multiplier;
}


#endif /* UTILS_H_ */
