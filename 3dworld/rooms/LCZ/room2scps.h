#ifndef ROOM2SCPS_ROOM_INCLUDED
#define ROOM2SCPS_ROOM_INCLUDED

class room2scps : public room {
	private:
		static irr::scene::IMeshSceneNode* baseNode;
		static btBvhTriangleMeshShape* baseShape;
		room2scps() {};
	public:
		virtual roomTypes getType() { return roomTypes::ROOM2; }
		virtual void updateEvent() { return; }
		static void setBase(irr::scene::IMeshSceneNode* inNode,btBvhTriangleMeshShape* inShape);
		static room2scps* createNew(irr::core::vector3df inPosition,char inAngle);
};

#endif
