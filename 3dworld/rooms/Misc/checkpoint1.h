#ifndef CHECKPOINT1_ROOM_INCLUDED
#define CHECKPOINT1_ROOM_INCLUDED

class checkpoint1 : public room {
	private:
		static irr::scene::IMeshSceneNode* baseNode;
		static btBvhTriangleMeshShape* baseShape;
		checkpoint1() {};
	public:
		virtual roomTypes getType() { return roomTypes::ROOM2; }
		virtual void updateEvent() { return; }
		static void setBase(irr::scene::IMeshSceneNode* inNode,btBvhTriangleMeshShape* inShape);
		static checkpoint1* createNew(irr::core::vector3df inPosition,char inAngle);
};

#endif
