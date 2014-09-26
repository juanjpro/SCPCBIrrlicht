#ifndef TUNNEL_ROOM_INCLUDED
#define TUNNEL_ROOM_INCLUDED

class tunnel : public room {
	private:
		static irr::scene::IMeshSceneNode* baseNode;
		static btBvhTriangleMeshShape* baseShape;
		tunnel() {};
	public:
		virtual roomTypes getType() { return roomTypes::ROOM2; }
		virtual void updateEvent() { return; }
		static void setBase(irr::scene::IMeshSceneNode* inNode,btBvhTriangleMeshShape* inShape);
		static tunnel* createNew(irr::core::vector3df inPosition,char inAngle);
};

#endif