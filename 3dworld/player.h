#ifndef PLAYER_H_INCLUDED
#define PLAYER_H_INCLUDED

const unsigned int inventory_size = 10;

class player {
    private:
        world* owner;
        irr::scene::ISceneManager* irrSmgr;
        irrDynamics* dynamics;
        MainEventReceiver* irrReceiver;
        btVector3 prevLinearVelocity;
        float walkingSpeed;
        irr::scene::ICameraSceneNode* Camera;
        btRigidBody* Capsule;
        irr::core::vector3df selfRotation;

        item* inventory[inventory_size];

		float height,radius;

        float crouchState = 0.f;
        bool crouched = false;

        bool dead = false;

        float sprintTimer = 0;

        sound* breathSound[5][2];
        unsigned char currBreathSound = 4;
        sound* stepSound[4][2][4]; //(normal/metal/pocket dimension/forest, walk/run, id)
        float shakeFactor = 0.f;

        float shake = 0.f;

        btVector3 dynSpeed = btVector3(0.f,0.f,0.f);
        float dir = 0;
    public:
        player(world* own,irr::scene::ISceneManager* smgr,irrDynamics* dyn,MainEventReceiver* receiver,float height=26.0f,float radius=4.0f,float mass=5.0f);
        //mass should stay low if you want the player to be able the climb up stairs
        ~player();
        void update(); void resetSpeeds(); void updateHead();
        float yaw = 0.f,pitch = 0.f,roll = 0.f;

        float BlinkTimer=100.0,Stamina=100.0;

        void addToInventory(item* it);
        void takeFromInventory(unsigned char slot);

        void teleport(irr::core::vector3df position);

        bool seesMeshNode(irr::scene::IMeshSceneNode* node);

        irr::scene::IMeshSceneNode* testNode;

        irr::core::vector3df getPosition() {
            return irr::core::vector3df(Capsule->getCenterOfMassPosition()[0],Capsule->getCenterOfMassPosition()[1],Capsule->getCenterOfMassPosition()[2]);
        }
};

#endif // PLAYER_H_INCLUDED
