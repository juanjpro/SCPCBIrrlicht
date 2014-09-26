#ifndef POCKETDIMENSION_ROOM_INCLUDED
#define POCKETDIMENSION_ROOM_INCLUDED

class pocketdimension : public room {
	private:
		static irr::scene::IMeshSceneNode* baseNode;
		static btBvhTriangleMeshShape* baseShape;
		pocketdimension() {};
	public:
		virtual roomTypes getType() { return roomTypes::ROOM1; }
		virtual void updateEvent() { return; }
		static void setBase(irr::scene::IMeshSceneNode* inNode,btBvhTriangleMeshShape* inShape);
		static pocketdimension* createNew(irr::core::vector3df inPosition,char inAngle);
};

#endif
