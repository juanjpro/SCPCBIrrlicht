#ifndef ROOM2OFFICES2_ROOM_INCLUDED
#define ROOM2OFFICES2_ROOM_INCLUDED

class room2offices2 : public room {
	private:
		static irr::scene::IMeshSceneNode* baseNode;
		static btBvhTriangleMeshShape* baseShape;
		room2offices2() {};
	public:
		virtual roomTypes getType() { return roomTypes::ROOM2; }
		virtual void updateEvent() { return; }
		static void setBase(irr::scene::IMeshSceneNode* inNode,btBvhTriangleMeshShape* inShape);
		static room2offices2* createNew(irr::core::vector3df inPosition,char inAngle);
};

#endif
