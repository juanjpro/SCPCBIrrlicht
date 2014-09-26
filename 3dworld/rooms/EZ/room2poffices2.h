#ifndef ROOM2POFFICES2_ROOM_INCLUDED
#define ROOM2POFFICES2_ROOM_INCLUDED

class room2poffices2 : public room {
	private:
		static irr::scene::IMeshSceneNode* baseNode;
		static btBvhTriangleMeshShape* baseShape;
		room2poffices2() {};
	public:
		virtual roomTypes getType() { return roomTypes::ROOM2; }
		virtual void updateEvent() { return; }
		static void setBase(irr::scene::IMeshSceneNode* inNode,btBvhTriangleMeshShape* inShape);
		static room2poffices2* createNew(irr::core::vector3df inPosition,char inAngle);
};

#endif