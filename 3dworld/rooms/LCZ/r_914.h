#ifndef R_914_ROOM_INCLUDED
#define R_914_ROOM_INCLUDED

class r_914 : public room {
	private:
		static irr::scene::IMeshSceneNode* baseNode;
		static btBvhTriangleMeshShape* baseShape;
		r_914() {};
	public:
		virtual roomTypes getType() { return roomTypes::ROOM1; }
		virtual void updateEvent() { return; }
		static void setBase(irr::scene::IMeshSceneNode* inNode,btBvhTriangleMeshShape* inShape);
		static r_914* createNew(irr::core::vector3df inPosition,char inAngle);
};

#endif
