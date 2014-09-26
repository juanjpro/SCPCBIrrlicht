#ifndef ROOM2OFFICES_ROOM_INCLUDED
#define ROOM2OFFICES_ROOM_INCLUDED

class room2offices : public room {
	private:
		static irr::scene::IMeshSceneNode* baseNode;
		static btBvhTriangleMeshShape* baseShape;
		room2offices() {};
	public:
		virtual roomTypes getType() { return roomTypes::ROOM2; }
		virtual void updateEvent() { return; }
		static void setBase(irr::scene::IMeshSceneNode* inNode,btBvhTriangleMeshShape* inShape);
		static room2offices* createNew(irr::core::vector3df inPosition,char inAngle);
};

#endif
