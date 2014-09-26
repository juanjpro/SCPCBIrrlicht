#include "../room.h"
#include "start.h"

irr::scene::IMeshSceneNode* start::baseNode = nullptr;
btBvhTriangleMeshShape* start::baseShape = nullptr;

void start::setBase(irr::scene::IMeshSceneNode* inNode,btBvhTriangleMeshShape* inShape) {
	if (start::baseNode==nullptr || start::baseShape==nullptr) {
		start::baseNode = inNode; start::baseShape = inShape; start::baseNode->setVisible(false);
	}
}

start* start::createNew(irr::core::vector3df inPosition,char inAngle) {
	start* retRoom = new start;
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
	start::baseShape->calculateLocalInertia(0.0, localInertia);

	retRoom->rbody = new btRigidBody(0.0, MotionState, start::baseShape, localInertia);
	room::dynamics->sharedRegisterRBody(retRoom->node,retRoom->rbody,0.f);

	retRoom->rbody->setFriction(1.f);
	retRoom->rbody->setRollingFriction(1.f);
	return retRoom;
}