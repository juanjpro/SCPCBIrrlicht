#include "../room.h"
#include "lockroom.h"

irr::scene::IMeshSceneNode* lockroom::baseNode = nullptr;
btBvhTriangleMeshShape* lockroom::baseShape = nullptr;

void lockroom::setBase(irr::scene::IMeshSceneNode* inNode,btBvhTriangleMeshShape* inShape) {
	if (lockroom::baseNode==nullptr || lockroom::baseShape==nullptr) {
		lockroom::baseNode = inNode; lockroom::baseShape = inShape; lockroom::baseNode->setVisible(false);
	}
}

lockroom* lockroom::createNew(irr::core::vector3df inPosition,char inAngle) {
	lockroom* retRoom = new lockroom;
	retRoom->node = baseNode->clone(); retRoom->node->setVisible(true);
	retRoom->node->setPosition(inPosition);
	retRoom->node->setRotation(irr::core::vector3df(0,inAngle*90.f,0));
	retRoom->angle = inAngle;

	//Add the Bullet rigid body
	retRoom->node->updateAbsolutePosition();
	irr::core::vector3df irrPos = retRoom->node->getAbsolutePosition();
	btVector3 btPos(irrPos.X, irrPos.Y, irrPos.Z);
	btTransform Transform;
	Transform.setIdentity();
	Transform.setOrigin(btPos);
	btQuaternion btRot;
	irr::core::vector3df irrRot = retRoom->node->getRotation();
	btRot.setEulerZYX(irrRot.Z*irr::core::DEGTORAD,irrRot.Y*irr::core::DEGTORAD,irrRot.X*irr::core::DEGTORAD);
	Transform.setRotation(btRot);

	btDefaultMotionState *MotionState = new btDefaultMotionState(Transform);

	btVector3 localInertia;
	lockroom::baseShape->calculateLocalInertia(0.0, localInertia);

	retRoom->rbody = new btRigidBody(0.0, MotionState, lockroom::baseShape, localInertia);
	room::dynamics->sharedRegisterRBody(retRoom->node,retRoom->rbody,0.f);

	retRoom->rbody->setFriction(1.f);
	retRoom->rbody->setRollingFriction(1.f);
	return retRoom;
}
