#include "3dworld.h"
//#include "rooms/rmesh.h"
#include "rooms/room.h"

#include <iostream>
#include <cmath>
#include <cstdlib>

world::world(unsigned int width,unsigned int height,bool fullscreen) {
    mainWidth = width; mainHeight = height;

    irrDriverType = irr::video::EDT_OPENGL;
    irrReceiver = new MainEventReceiver;
    irrDevice = irr::createDevice(irrDriverType,irr::core::dimension2d<irr::u32>(width,height),32,fullscreen,false,true,irrReceiver);
    irrDriver = irrDevice->getVideoDriver();
	irrSmgr = irrDevice->getSceneManager();

	//irrSmgr->setAmbientLight(irr::video::SColor(255,20,20,20));

    irrDriver->setTextureCreationFlag(irr::video::ETCF_ALWAYS_32_BIT, true);

    irrEnv = irrDevice->getGUIEnvironment();

    irrFileSystem = irrDevice->getFileSystem();

    irrTimer = irrDevice->getTimer();

	font1 = irr::gui::CGUITTFont::createTTFont(irrEnv, "test/arial.ttf", 16, true, true);

    srand(irrTimer->getRealTime());

    irr::video::IGPUProgrammingServices* irrGpu = irrDriver->getGPUProgrammingServices();
    RoomShader = irr::video::EMT_LIGHTMAP; // Fallback material type
    RoomCallback = new RoomShaderCallBack;
    RoomShader = irrGpu->addHighLevelShaderMaterialFromFiles("RoomVertShader.txt", "vertMain", irr::video::EVST_VS_1_1,"RoomFragShader.txt", "fragMain", irr::video::EPST_PS_1_1,RoomCallback, irr::video::EMT_LIGHTMAP);

    NormalsShader = irr::video::EMT_SOLID; // Fallback material type
    NormalsShaderCallBack* NormalsCallback= new NormalsShaderCallBack;
    NormalsShader = irrGpu->addHighLevelShaderMaterialFromFiles("NewNormalVert.txt", "main", irr::video::EVST_VS_1_1,"NewNormalFrag.txt", "main", irr::video::EPST_PS_1_1,NormalsCallback, irr::video::EMT_SOLID);
    NormalsCallback->fvAmbient = irr::video::SColor(255,20,20,20);

    blurImage = irrDriver->addRenderTargetTexture(irr::core::dimension2d<irr::u32>(width,height),"",irr::video::ECF_A8R8G8B8);
    blurImage2 = irrDriver->addRenderTargetTexture(irr::core::dimension2d<irr::u32>(width,height),"",irr::video::ECF_A8R8G8B8);
    BlinkMeterIMG = irrDriver->getTexture("test/BlinkMeter.jpg");
    StaminaMeterIMG = irrDriver->getTexture("test/StaminaMeter.jpg");

    /*defItemParams.getNode = &irr::scene::ISceneManager::addMeshSceneNode;
    defItemParams.registerRBody = &irrDynamics::registerNewRBody;
    defItemParams.unregisterRBody = &irrDynamics::unregisterRBody;*/

    // Add camera
	/*irr::scene::ICameraSceneNode *Camera = irrSmgr->addCameraSceneNodeFPS(0, 50, 0.02);
	Camera->setPosition(irr::core::vector3df(0, 5, -5));
	Camera->setTarget(irr::core::vector3df(0, 0, 0));*/

	//Add test model

    dynamics = new irrDynamics();//irrDynamics::getInstance();
    dynamics->setGravity(-100*RoomScale);

	irr::scene::IMeshSceneNode* node = nullptr;
    dynRegister* itemDyn = new dynRegister(dynamics);

    node = irrSmgr->addMeshSceneNode(irrSmgr->getMesh("test/eyedrops.b3d"));
    node->setMaterialFlag(irr::video::EMF_NORMALIZE_NORMALS, true);

	item::setDynamics(itemDyn);

    node->setScale(irr::core::vector3df(0.06*RoomScale,0.06*RoomScale,0.06*RoomScale));
    itemEyedrops::setMeshNode(node);

    node = irrSmgr->addMeshSceneNode(irrSmgr->getMesh("test/gasmask.b3d"));
    node->setMaterialFlag(irr::video::EMF_NORMALIZE_NORMALS, true);

    node->setScale(irr::core::vector3df(0.6*RoomScale,0.6*RoomScale,0.6*RoomScale));
    itemGasMask::setMeshNode(node);

    for (irr::u32 ui=0;ui<10000;ui++) {
        item* it = itemEyedrops::createItemEyedrops();
        itemList.push_back(it);

        it = itemGasMask::createItemGasMask();
        itemList.push_back(it);
    }

	btRigidBody* rbody;

	/*btTransform Transform;
	Transform.setIdentity();
	btDefaultMotionState *MotionState = new btDefaultMotionState(Transform);
	btVector3 localInertia(0,0,0);
	btRigidBody* rbody = new btRigidBody(0.f, MotionState, rme->shape,localInertia);
	//rbody->setActivationState(DISABLE_DEACTIVATION);
	dynamics->registerNewRBody(node,rbody,0);
	//all rigid bodies must have a friction value for friction to work
	rbody->setFriction(1.f);
	rbody->setRollingFriction(1.f);*/

	//node->setScale(irr::core::vector3df(0.1f*RoomScale));

	room::setDynamics(itemDyn);

	RMesh* rme;
	//TODO: give the RMesh object to the room so it can clean it up after it's not used anymore
	//LCZ
	/*lockroom*/rme = loadRMesh(std::string("test/lockroom_opt.rmesh"),irrFileSystem,irrDriver); lockroom::setBase(irrSmgr->addMeshSceneNode(rme->mesh),rme->shape);
	/*start*/rme = loadRMesh(std::string("test/173_opt.rmesh"),irrFileSystem,irrDriver); start::setBase(irrSmgr->addMeshSceneNode(rme->mesh),rme->shape);
	/*room2storage*/rme = loadRMesh(std::string("test/room2storage_opt.rmesh"),irrFileSystem,irrDriver); room2storage::setBase(irrSmgr->addMeshSceneNode(rme->mesh),rme->shape);
	/*room3storage*/rme = loadRMesh(std::string("test/room3storage_opt.rmesh"),irrFileSystem,irrDriver); room3storage::setBase(irrSmgr->addMeshSceneNode(rme->mesh),rme->shape);
	/*endroom*/rme = loadRMesh(std::string("test/endroom_opt.rmesh"),irrFileSystem,irrDriver); endroom::setBase(irrSmgr->addMeshSceneNode(rme->mesh),rme->shape);
	/*room012*/rme = loadRMesh(std::string("test/room012_opt.rmesh"),irrFileSystem,irrDriver); room012::setBase(irrSmgr->addMeshSceneNode(rme->mesh),rme->shape);
	/*room2*/rme = loadRMesh(std::string("test/room2_opt.rmesh"),irrFileSystem,irrDriver); room2::setBase(irrSmgr->addMeshSceneNode(rme->mesh),rme->shape);
	/*room2_2*/rme = loadRMesh(std::string("test/room2_2_opt.rmesh"),irrFileSystem,irrDriver); room2_2::setBase(irrSmgr->addMeshSceneNode(rme->mesh),rme->shape);
	/*room2c*/rme = loadRMesh(std::string("test/room2C_opt.rmesh"),irrFileSystem,irrDriver); room2c::setBase(irrSmgr->addMeshSceneNode(rme->mesh),rme->shape);
	/*room2closets*/rme = loadRMesh(std::string("test/room2closets_opt.rmesh"),irrFileSystem,irrDriver); room2closets::setBase(irrSmgr->addMeshSceneNode(rme->mesh),rme->shape);
	/*room2elevator*/rme = loadRMesh(std::string("test/room2elevator_opt.rmesh"),irrFileSystem,irrDriver); room2elevator::setBase(irrSmgr->addMeshSceneNode(rme->mesh),rme->shape);
	/*room2doors*/rme = loadRMesh(std::string("test/room2doors_opt.rmesh"),irrFileSystem,irrDriver); room2doors::setBase(irrSmgr->addMeshSceneNode(rme->mesh),rme->shape);
	/*room2scps*/rme = loadRMesh(std::string("test/room2scps_opt.rmesh"),irrFileSystem,irrDriver); room2scps::setBase(irrSmgr->addMeshSceneNode(rme->mesh),rme->shape);
	/*room3storage*/rme = loadRMesh(std::string("test/room3storage_opt.rmesh"),irrFileSystem,irrDriver); room3storage::setBase(irrSmgr->addMeshSceneNode(rme->mesh),rme->shape);
	/*room2testroom2*/rme = loadRMesh(std::string("test/room2testroom2_opt.rmesh"),irrFileSystem,irrDriver); room2testroom2::setBase(irrSmgr->addMeshSceneNode(rme->mesh),rme->shape);
	/*room3*/rme = loadRMesh(std::string("test/room3_opt.rmesh"),irrFileSystem,irrDriver); room3::setBase(irrSmgr->addMeshSceneNode(rme->mesh),rme->shape);
	/*room3_2*/rme = loadRMesh(std::string("test/room3_2_opt.rmesh"),irrFileSystem,irrDriver); room3_2::setBase(irrSmgr->addMeshSceneNode(rme->mesh),rme->shape);
	/*room4*/rme = loadRMesh(std::string("test/room4_opt.rmesh"),irrFileSystem,irrDriver); room4::setBase(irrSmgr->addMeshSceneNode(rme->mesh),rme->shape);
	/*roompj*/rme = loadRMesh(std::string("test/roompj_opt.rmesh"),irrFileSystem,irrDriver); roompj::setBase(irrSmgr->addMeshSceneNode(rme->mesh),rme->shape);
	/*r_914*/rme = loadRMesh(std::string("test/machineroom_opt.rmesh"),irrFileSystem,irrDriver); r_914::setBase(irrSmgr->addMeshSceneNode(rme->mesh),rme->shape);
	//HCZ
	/*r_008*/rme = loadRMesh(std::string("test/008_opt.rmesh"),irrFileSystem,irrDriver); r_008::setBase(irrSmgr->addMeshSceneNode(rme->mesh),rme->shape);
	/*coffin*/rme = loadRMesh(std::string("test/coffin_opt.rmesh"),irrFileSystem,irrDriver); coffin::setBase(irrSmgr->addMeshSceneNode(rme->mesh),rme->shape);
	/*endroom2*/rme = loadRMesh(std::string("test/endroom2_opt.rmesh"),irrFileSystem,irrDriver); endroom2::setBase(irrSmgr->addMeshSceneNode(rme->mesh),rme->shape);
	/*testroom*/rme = loadRMesh(std::string("test/testroom_opt.rmesh"),irrFileSystem,irrDriver); testroom::setBase(irrSmgr->addMeshSceneNode(rme->mesh),rme->shape);
	/*tunnel*/rme = loadRMesh(std::string("test/tunnel_opt.rmesh"),irrFileSystem,irrDriver); tunnel::setBase(irrSmgr->addMeshSceneNode(rme->mesh),rme->shape);
	/*tunnel2*/rme = loadRMesh(std::string("test/tunnel2_opt.rmesh"),irrFileSystem,irrDriver); tunnel2::setBase(irrSmgr->addMeshSceneNode(rme->mesh),rme->shape);
	/*room035*/rme = loadRMesh(std::string("test/room035_opt.rmesh"),irrFileSystem,irrDriver); room035::setBase(irrSmgr->addMeshSceneNode(rme->mesh),rme->shape);
	/*room049*/rme = loadRMesh(std::string("test/room049_opt.rmesh"),irrFileSystem,irrDriver); room049::setBase(irrSmgr->addMeshSceneNode(rme->mesh),rme->shape);
	/*room106*/rme = loadRMesh(std::string("test/room106_opt.rmesh"),irrFileSystem,irrDriver); room106::setBase(irrSmgr->addMeshSceneNode(rme->mesh),rme->shape);
	/*room2ctunnel*/rme = loadRMesh(std::string("test/room2Ctunnel_opt.rmesh"),irrFileSystem,irrDriver); room2ctunnel::setBase(irrSmgr->addMeshSceneNode(rme->mesh),rme->shape);
	/*room2nuke*/rme = loadRMesh(std::string("test/room2nuke_opt.rmesh"),irrFileSystem,irrDriver); room2nuke::setBase(irrSmgr->addMeshSceneNode(rme->mesh),rme->shape);
	/*room2pipes*/rme = loadRMesh(std::string("test/room2pipes_opt.rmesh"),irrFileSystem,irrDriver); room2pipes::setBase(irrSmgr->addMeshSceneNode(rme->mesh),rme->shape);
	/*room2pit*/rme = loadRMesh(std::string("test/room2pit_opt.rmesh"),irrFileSystem,irrDriver); room2pit::setBase(irrSmgr->addMeshSceneNode(rme->mesh),rme->shape);
	/*room3pit*/rme = loadRMesh(std::string("test/room3pit_opt.rmesh"),irrFileSystem,irrDriver); room3pit::setBase(irrSmgr->addMeshSceneNode(rme->mesh),rme->shape);
	/*room2servers*/rme = loadRMesh(std::string("test/room2servers_opt.rmesh"),irrFileSystem,irrDriver); room2servers::setBase(irrSmgr->addMeshSceneNode(rme->mesh),rme->shape);
	/*room2tunnel*/rme = loadRMesh(std::string("test/room2tunnel_opt.rmesh"),irrFileSystem,irrDriver); room2tunnel::setBase(irrSmgr->addMeshSceneNode(rme->mesh),rme->shape);
	/*room3tunnel*/rme = loadRMesh(std::string("test/room3tunnel_opt.rmesh"),irrFileSystem,irrDriver); room3tunnel::setBase(irrSmgr->addMeshSceneNode(rme->mesh),rme->shape);
	/*room4tunnels*/rme = loadRMesh(std::string("test/4tunnels_opt.rmesh"),irrFileSystem,irrDriver); room4tunnels::setBase(irrSmgr->addMeshSceneNode(rme->mesh),rme->shape);
	/*room513*/rme = loadRMesh(std::string("test/room513_opt.rmesh"),irrFileSystem,irrDriver); room513::setBase(irrSmgr->addMeshSceneNode(rme->mesh),rme->shape);
	//EZ
	/*room860*/rme = loadRMesh(std::string("test/room860_opt.rmesh"),irrFileSystem,irrDriver); room860::setBase(irrSmgr->addMeshSceneNode(rme->mesh),rme->shape);
	/*exit1*/rme = loadRMesh(std::string("test/exit1_opt.rmesh"),irrFileSystem,irrDriver); exit1::setBase(irrSmgr->addMeshSceneNode(rme->mesh),rme->shape);
	/*gateaentrance*/rme = loadRMesh(std::string("test/gateaentrance_opt.rmesh"),irrFileSystem,irrDriver); gateaentrance::setBase(irrSmgr->addMeshSceneNode(rme->mesh),rme->shape);
	/*lockroom2*/rme = loadRMesh(std::string("test/lockroom2_opt.rmesh"),irrFileSystem,irrDriver); lockroom2::setBase(irrSmgr->addMeshSceneNode(rme->mesh),rme->shape);
	/*room079*/rme = loadRMesh(std::string("test/room079_opt.rmesh"),irrFileSystem,irrDriver); room079::setBase(irrSmgr->addMeshSceneNode(rme->mesh),rme->shape);
	/*room2z3*/rme = loadRMesh(std::string("test/room2z3_opt.rmesh"),irrFileSystem,irrDriver); room2z3::setBase(irrSmgr->addMeshSceneNode(rme->mesh),rme->shape);
	/*room2cafeteria*/rme = loadRMesh(std::string("test/room2cafeteria_opt.rmesh"),irrFileSystem,irrDriver); room2cafeteria::setBase(irrSmgr->addMeshSceneNode(rme->mesh),rme->shape);
	/*room2cz3*/rme = loadRMesh(std::string("test/room2Cz3_opt.rmesh"),irrFileSystem,irrDriver); room2cz3::setBase(irrSmgr->addMeshSceneNode(rme->mesh),rme->shape);
	/*room2ccont*/rme = loadRMesh(std::string("test/room2ccont_opt.rmesh"),irrFileSystem,irrDriver); room2ccont::setBase(irrSmgr->addMeshSceneNode(rme->mesh),rme->shape);
	/*room2offices*/rme = loadRMesh(std::string("test/room2offices_opt.rmesh"),irrFileSystem,irrDriver); room2offices::setBase(irrSmgr->addMeshSceneNode(rme->mesh),rme->shape);
	/*room2offices2*/rme = loadRMesh(std::string("test/room2offices2_opt.rmesh"),irrFileSystem,irrDriver); room2offices2::setBase(irrSmgr->addMeshSceneNode(rme->mesh),rme->shape);
	/*room2offices3*/rme = loadRMesh(std::string("test/room2offices3_opt.rmesh"),irrFileSystem,irrDriver); room2offices3::setBase(irrSmgr->addMeshSceneNode(rme->mesh),rme->shape);
	/*room2poffices*/rme = loadRMesh(std::string("test/room2poffices_opt.rmesh"),irrFileSystem,irrDriver); room2poffices::setBase(irrSmgr->addMeshSceneNode(rme->mesh),rme->shape);
	/*room2poffices2*/rme = loadRMesh(std::string("test/room2poffices2_opt.rmesh"),irrFileSystem,irrDriver); room2poffices2::setBase(irrSmgr->addMeshSceneNode(rme->mesh),rme->shape);
	/*room2sroom*/rme = loadRMesh(std::string("test/room2sroom_opt.rmesh"),irrFileSystem,irrDriver); room2sroom::setBase(irrSmgr->addMeshSceneNode(rme->mesh),rme->shape);
	/*room2toilets*/rme = loadRMesh(std::string("test/room2toilets_opt.rmesh"),irrFileSystem,irrDriver); room2toilets::setBase(irrSmgr->addMeshSceneNode(rme->mesh),rme->shape);
	/*room2tesla*/rme = loadRMesh(std::string("test/room2tesla_opt.rmesh"),irrFileSystem,irrDriver); room2tesla::setBase(irrSmgr->addMeshSceneNode(rme->mesh),rme->shape);
	/*room3servers*/rme = loadRMesh(std::string("test/room3servers_opt.rmesh"),irrFileSystem,irrDriver); room3servers::setBase(irrSmgr->addMeshSceneNode(rme->mesh),rme->shape);
	/*room3servers2*/rme = loadRMesh(std::string("test/room3servers2_opt.rmesh"),irrFileSystem,irrDriver); room3servers2::setBase(irrSmgr->addMeshSceneNode(rme->mesh),rme->shape);
	/*room3z3*/rme = loadRMesh(std::string("test/room3z3_opt.rmesh"),irrFileSystem,irrDriver); room3z3::setBase(irrSmgr->addMeshSceneNode(rme->mesh),rme->shape);
	/*room4z3*/rme = loadRMesh(std::string("test/room4z3_opt.rmesh"),irrFileSystem,irrDriver); room4z3::setBase(irrSmgr->addMeshSceneNode(rme->mesh),rme->shape);
	//Misc
	/*r_173*/rme = loadRMesh(std::string("test/173bright_opt.rmesh"),irrFileSystem,irrDriver); r_173::setBase(irrSmgr->addMeshSceneNode(rme->mesh),rme->shape);
	/*checkpoint1*/rme = loadRMesh(std::string("test/checkpoint1_opt.rmesh"),irrFileSystem,irrDriver); checkpoint1::setBase(irrSmgr->addMeshSceneNode(rme->mesh),rme->shape);
	/*checkpoint2*/rme = loadRMesh(std::string("test/checkpoint2_opt.rmesh"),irrFileSystem,irrDriver); checkpoint2::setBase(irrSmgr->addMeshSceneNode(rme->mesh),rme->shape);
	/*gatea*/rme = loadRMesh(std::string("test/gatea_opt.rmesh"),irrFileSystem,irrDriver); gatea::setBase(irrSmgr->addMeshSceneNode(rme->mesh),rme->shape);
	/*pocketdimension*/rme = loadRMesh(std::string("test/pocketdimension1_opt.rmesh"),irrFileSystem,irrDriver); pocketdimension::setBase(irrSmgr->addMeshSceneNode(rme->mesh),rme->shape);

	createMap();
	/*room2::createNew(irr::core::vector3df(0,0,0),0);
	room2c::createNew(irr::core::vector3df(0,0,204.8f*RoomScale),1);
	room3::createNew(irr::core::vector3df(0,0,-204.8f*RoomScale),2);
	room4::createNew(irr::core::vector3df(-204.8f*RoomScale,0,-204.8f*RoomScale),2);
	room1::createNew(irr::core::vector3df(204.8f*RoomScale,0,-204.8f*RoomScale),1);*/

    //node->setPosition(irr::core::vector3df(0,0*RoomScale,0));

    //dynamics->addTriMesh_static(node);

	for (int i = 0;i<1;i++) {
        irr::scene::IMesh* mesh1 = irrSmgr->getMesh("test/scp-066.b3d");

        node = irrSmgr->addMeshSceneNode(mesh1);


        node->setScale(irr::core::vector3df(2.1*RoomScale));
        node->setPosition(irr::core::vector3df(-0*RoomScale,10*RoomScale,0*RoomScale));

        rbody = dynamics->addTriMesh_moving(node,5.0f,5,1,1);
        rbody->setFriction(10.0f);
        rbody->setDamping(0,0);
        node->setMaterialFlag(irr::video::EMF_NORMALIZE_NORMALS, true);

        node->getMaterial(0).setTexture(1, irrDriver->getTexture("test/scp-066_normal.png"));
        node->getMaterial(0).setTexture(2, irrDriver->getTexture("test/scp-066_specular.png"));
        node->getMaterial(1).MaterialType = (irr::video::E_MATERIAL_TYPE)NormalsShader;
        node->getMaterial(1).setTexture(1, irrDriver->getTexture("test/scp-066_normal.png"));
        node->getMaterial(1).setTexture(2, irrDriver->getTexture("test/scp-066_specular.png"));
        node->getMaterial(0).MaterialType = (irr::video::E_MATERIAL_TYPE)NormalsShader;

        node->getMaterial(0).SpecularColor.set(0,0,0,0);
        node->getMaterial(0).Shininess = 0.f;
        node->getMaterial(1).SpecularColor.set(0,0,0,0);
        node->getMaterial(1).Shininess = 0.0f;
	}

	mainPlayer = new player(this,irrSmgr,dynamics,irrReceiver);

    //test node
    irr::scene::IMesh* mesh1 = irrSmgr->getMesh("test/173_2.b3d");

    node = irrSmgr->addMeshSceneNode(mesh1);

    node->setScale(irr::core::vector3df(1.3*RoomScale));
    node->setPosition(irr::core::vector3df(0*RoomScale,10*RoomScale,-10*RoomScale));
    node->setMaterialFlag(irr::video::EMF_LIGHTING, true);
    node->setMaterialFlag(irr::video::EMF_NORMALIZE_NORMALS, true);

    node->getMaterial(0).Lighting = true;
    node->getMaterial(0).MaterialType = (irr::video::E_MATERIAL_TYPE)NormalsShader;

    rbody = dynamics->addTriMesh_moving(node,16000.f,20,1,1);
    rbody->setAngularFactor(btVector3(0,1.0,0));
    //rbody->setLinearFactor(btVector3(0.1,0.1,0.1));

    node->getMaterial(0).setTexture(1, irrDriver->getTexture("test/173_norm.jpg"));
    node->getMaterial(0).setTexture(2, irrDriver->getTexture("test/173_Spec.jpg"));

    node->getMaterial(0).EmissiveColor = irr::video::SColor(100,100,100,100);

    mainPlayer->testNode = node;
    //------------

	irrDevice->getCursorControl()->setVisible(false);
}

world::~world() {
    //fileSystem->drop();
    //env->drop();
    //smgr->drop();
    //driver->drop();
    irrDevice->drop();
}

void world::createMap() {
	auto getZone = [] (short y) { return std::min(std::floor((float)(20-y)/20.f*3.f),2.f); };

	short x,y,temp;
	short x2,y2;
	short width,height;

	struct tempRoom {
		roomTypes type;
		char angle;
	};

	tempRoom roomTemp[20][20];
	for (x=0;x<20;x++) {
		for (y=0;y<20;y++) {
			roomArray[x][y] = nullptr;
			roomTemp[x][y].type = roomTypes::ROOM1;
			roomTemp[x][y].angle = -1;
		}
	}

	x = 10;
	y = 18;

	for (int i = y;i<20;i++) {
		roomTemp[x][i].angle = 0;
	}

	while (y>=2) {
		width = (rand() % 6) + 10;

		if (x>12) {
			width = -width;
		} else if (x>8) {
			x = x-10;
		}

		//make sure the hallway doesn't go outside the array
		if (x+width > 17) {
			width=17-x;
		} else if (x+width < 2) {
			width=-x+2;
		}


		x = std::min(x,short(x + width));
		width = std::abs(width);
		for (int i = x;i<=x+width;i++) {
			roomTemp[std::min(i,19)][y].angle = 0;
		}

		height = (rand() % 2) + 3;
		if (y - height < 1) height = y;
		//if (y + height > 18) height = y;

		int yhallways = (rand() % 2) + 4;

		if (getZone(y-height)!=getZone(y-height+1)) height--;

		for (int i=1;i<=yhallways;i++) {
			x2 = std::max(std::min((rand() % (width-1)) + x,18),2);
			while (roomTemp[x2][y-1].angle>=0 || roomTemp[x2-1][y-1].angle>=0 || roomTemp[x2+1][y-1].angle>=0) {
				x2++;
			}

			if (x2<x+width) {
				short tempheight;
				if (i==1) {
					tempheight = height;
					if (rand()%2 == 0) x2 = x; else x2 = x+width;
				} else {
					tempheight = (rand()%height) + 1;
				}

				for (y2 = y - tempheight;y2<=y;y2++) {
					if (getZone(y2)!=getZone(y2+1)) { //a room leading from zone to another
						roomTemp[x2][y2].angle = 127;
					} else {
						roomTemp[x2][y2].angle = 0;
					}
				}

				if (tempheight == height) temp = x2;
			}
		}

		x = temp;
		y -= height;
	}

	for (x=0;x<20;x++) {
		for (y=0;y<20;y++) {
			std::cout<<(roomTemp[y][x].angle>-1);
		}
		std::cout<<"\n";
	}

	for (x=0;x<20;x++) {
		for (y=0;y<20;y++) {
			bool hasNorth,hasSouth,hasEast,hasWest;
			hasNorth = hasSouth = hasEast = hasWest = false;
			if (roomTemp[x][y].angle==0) { //this is not a checkpoint room
				if (x>0) {
					hasWest = (roomTemp[x-1][y].angle>-1);
				}
				if (x<19) {
					hasEast = (roomTemp[x+1][y].angle>-1);
				}
				if (y>0) {
					hasNorth = (roomTemp[x][y-1].angle>-1);
				}
				if (y<19) {
					hasSouth = (roomTemp[x][y+1].angle>-1);
				}
				if (hasNorth && hasSouth) {
					if (hasEast && hasWest) { //room4
						roomTemp[x][y].type = roomTypes::ROOM4;
						roomTemp[x][y].angle = rand()%4;
					} else if (hasEast && !hasWest) { //room3, pointing east
						roomTemp[x][y].type = roomTypes::ROOM3;
						roomTemp[x][y].angle = 3;
					} else if (!hasEast && hasWest) { //room3, pointing west
						roomTemp[x][y].type = roomTypes::ROOM3;
						roomTemp[x][y].angle = 1;
					} else { //vertical room2
						roomTemp[x][y].type = roomTypes::ROOM2;
						roomTemp[x][y].angle = (rand()%2)*2;
					}
				} else if (hasEast && hasWest) {
					if (hasNorth && !hasSouth) { //room3, pointing north
						roomTemp[x][y].type = roomTypes::ROOM3;
						roomTemp[x][y].angle = 0;
					} else if (!hasNorth && hasSouth) { //room3, pointing south
						roomTemp[x][y].type = roomTypes::ROOM3;
						roomTemp[x][y].angle = 2;
					} else { //horizontal room2
						roomTemp[x][y].type = roomTypes::ROOM2;
						roomTemp[x][y].angle = ((rand()%2)*2)+1;
					}
				} else if (hasNorth) {
					if (hasEast) { //room2c, north-east
						roomTemp[x][y].type = roomTypes::ROOM2C;
						roomTemp[x][y].angle = 0;
					} else if (hasWest) { //room2c, north-west
						roomTemp[x][y].type = roomTypes::ROOM2C;
						roomTemp[x][y].angle = 1;
					} else { //room1, north
						roomTemp[x][y].type = roomTypes::ROOM1;
						roomTemp[x][y].angle = 0;
					}
				} else if (hasSouth) {
					if (hasEast) { //room2c, south-east
						roomTemp[x][y].type = roomTypes::ROOM2C;
						roomTemp[x][y].angle = 3;
					} else if (hasWest) { //room2c, south-west
						roomTemp[x][y].type = roomTypes::ROOM2C;
						roomTemp[x][y].angle = 2;
					} else { //room1, south
						roomTemp[x][y].type = roomTypes::ROOM1;
						roomTemp[x][y].angle = 2;
					}
				} else if (hasEast) { //room1, east
					roomTemp[x][y].type = roomTypes::ROOM1;
					roomTemp[x][y].angle = 3;
				} else { //room1, west
					roomTemp[x][y].type = roomTypes::ROOM1;
					roomTemp[x][y].angle = 1;
				}
			}
		}
	}
	for (y=19;y>=0;y--) {
		for (x=19;x>=0;x--) {
			if (roomTemp[x][y].angle>-1 && roomTemp[x][y].angle<127) {
				irr::core::vector3df pos(204.8*RoomScale*x,0,204.8*RoomScale*y);
				int choice = 0;
				switch (roomTemp[x][y].type) {
					case roomTypes::ROOM1:
						switch ((int)getZone(y)) {
							case 0: //LCZ
								choice = rand()%100;
								if (choice>=0 && choice<100) {
									endroom::createNew(irr::core::vector3df(x*204.8f*RoomScale,0,y*204.8f*RoomScale),roomTemp[x][y].angle);
								}
							break;
							case 1: //HCZ
								choice = rand()%100;
								if (choice>=0 && choice<100) {
									endroom2::createNew(irr::core::vector3df(x*204.8f*RoomScale,0,y*204.8f*RoomScale),roomTemp[x][y].angle);
								}
							break;
							case 2: //EZ
								choice = rand()%100;
								if (choice>=0 && choice<100) {
									endroom::createNew(irr::core::vector3df(x*204.8f*RoomScale,0,y*204.8f*RoomScale),roomTemp[x][y].angle);
								}
							break;
						}
					break;
					case roomTypes::ROOM2:
						switch ((int)getZone(y)) {
							case 0: //LCZ
								choice = rand()%150;
								if (choice>=0 && choice<50) {
									room2::createNew(irr::core::vector3df(x*204.8f*RoomScale,0,y*204.8f*RoomScale),roomTemp[x][y].angle);
								}
								if (choice>=50 && choice<100) {
									room2_2::createNew(irr::core::vector3df(x*204.8f*RoomScale,0,y*204.8f*RoomScale),roomTemp[x][y].angle);
								}
								if (choice>=100 && choice<120) {
									room2elevator::createNew(irr::core::vector3df(x*204.8f*RoomScale,0,y*204.8f*RoomScale),roomTemp[x][y].angle);
								}
								if (choice>=120 && choice<150) {
									room2doors::createNew(irr::core::vector3df(x*204.8f*RoomScale,0,y*204.8f*RoomScale),roomTemp[x][y].angle);
								}
							break;
							case 1: //HCZ
								choice = rand()%295;
								if (choice>=0 && choice<100) {
									tunnel::createNew(irr::core::vector3df(x*204.8f*RoomScale,0,y*204.8f*RoomScale),roomTemp[x][y].angle);
								}
								if (choice>=100 && choice<170) {
									tunnel2::createNew(irr::core::vector3df(x*204.8f*RoomScale,0,y*204.8f*RoomScale),roomTemp[x][y].angle);
								}
								if (choice>=170 && choice<220) {
									room2pipes::createNew(irr::core::vector3df(x*204.8f*RoomScale,0,y*204.8f*RoomScale),roomTemp[x][y].angle);
								}
								if (choice>=220 && choice<295) {
									room2pit::createNew(irr::core::vector3df(x*204.8f*RoomScale,0,y*204.8f*RoomScale),roomTemp[x][y].angle);
								}
							break;
							case 2: //EZ
								choice = rand()%300;
								if (choice>=0 && choice<100) {
									room2z3::createNew(irr::core::vector3df(x*204.8f*RoomScale,0,y*204.8f*RoomScale),roomTemp[x][y].angle);
								}
								if (choice>=100 && choice<130) {
									room2offices::createNew(irr::core::vector3df(x*204.8f*RoomScale,0,y*204.8f*RoomScale),roomTemp[x][y].angle);
								}
								if (choice>=130 && choice<150) {
									room2offices2::createNew(irr::core::vector3df(x*204.8f*RoomScale,0,y*204.8f*RoomScale),roomTemp[x][y].angle);
								}
								if (choice>=150 && choice<170) {
									room2offices3::createNew(irr::core::vector3df(x*204.8f*RoomScale,0,y*204.8f*RoomScale),roomTemp[x][y].angle);
								}
								if (choice>=170 && choice<200) {
									room2toilets::createNew(irr::core::vector3df(x*204.8f*RoomScale,0,y*204.8f*RoomScale),roomTemp[x][y].angle);
								}
								if (choice>=200 && choice<300) {
									room2tesla::createNew(irr::core::vector3df(x*204.8f*RoomScale,0,y*204.8f*RoomScale),roomTemp[x][y].angle);
								}
							break;
						}
					break;
					case roomTypes::ROOM2C:
						switch ((int)getZone(y)) {
							case 0: //LCZ
								choice = rand()%70;
								if (choice>=0 && choice<30) {
									lockroom::createNew(irr::core::vector3df(x*204.8f*RoomScale,0,y*204.8f*RoomScale),roomTemp[x][y].angle);
								}
								if (choice>=30 && choice<70) {
									room2c::createNew(irr::core::vector3df(x*204.8f*RoomScale,0,y*204.8f*RoomScale),roomTemp[x][y].angle);
								}
							break;
							case 1: //HCZ
								choice = rand()%40;
								if (choice>=0 && choice<40) {
									room2ctunnel::createNew(irr::core::vector3df(x*204.8f*RoomScale,0,y*204.8f*RoomScale),roomTemp[x][y].angle);
								}
							break;
							case 2: //EZ
								choice = rand()%100;
								if (choice>=0 && choice<100) {
									room2cz3::createNew(irr::core::vector3df(x*204.8f*RoomScale,0,y*204.8f*RoomScale),roomTemp[x][y].angle);
								}
							break;
						}
					break;
					case roomTypes::ROOM3:
						switch ((int)getZone(y)) {
							case 0: //LCZ
								choice = rand()%200;
								if (choice>=0 && choice<100) {
									room3::createNew(irr::core::vector3df(x*204.8f*RoomScale,0,y*204.8f*RoomScale),roomTemp[x][y].angle);
								}
								if (choice>=100 && choice<200) {
									room3_2::createNew(irr::core::vector3df(x*204.8f*RoomScale,0,y*204.8f*RoomScale),roomTemp[x][y].angle);
								}
							break;
							case 1: //HCZ
								choice = rand()%200;
								if (choice>=0 && choice<100) {
									room3pit::createNew(irr::core::vector3df(x*204.8f*RoomScale,0,y*204.8f*RoomScale),roomTemp[x][y].angle);
								}
								if (choice>=100 && choice<200) {
									room3tunnel::createNew(irr::core::vector3df(x*204.8f*RoomScale,0,y*204.8f*RoomScale),roomTemp[x][y].angle);
								}
							break;
							case 2: //EZ
								choice = rand()%100;
								if (choice>=0 && choice<100) {
									room3z3::createNew(irr::core::vector3df(x*204.8f*RoomScale,0,y*204.8f*RoomScale),roomTemp[x][y].angle);
								}
							break;
						}
					break;
					case roomTypes::ROOM4:
						switch ((int)getZone(y)) {
							case 0: //LCZ
								choice = rand()%100;
								if (choice>=0 && choice<100) {
									room4::createNew(irr::core::vector3df(x*204.8f*RoomScale,0,y*204.8f*RoomScale),roomTemp[x][y].angle);
								}
							break;
							case 1: //HCZ
								choice = rand()%100;
								if (choice>=0 && choice<100) {
									room4tunnels::createNew(irr::core::vector3df(x*204.8f*RoomScale,0,y*204.8f*RoomScale),roomTemp[x][y].angle);
								}
							break;
							case 2: //EZ
								choice = rand()%100;
								if (choice>=0 && choice<100) {
									room4z3::createNew(irr::core::vector3df(x*204.8f*RoomScale,0,y*204.8f*RoomScale),roomTemp[x][y].angle);
								}
							break;
						}
					break;
				}
			} else if (roomTemp[x][y].angle==127) {
				if (getZone(y)==1) {
					checkpoint1::createNew(irr::core::vector3df(x*204.8f*RoomScale,0,y*204.8f*RoomScale),0);
				} else {
					checkpoint2::createNew(irr::core::vector3df(x*204.8f*RoomScale,0,y*204.8f*RoomScale),0);
				}
			}
		}
	}
}

bool world::run() {

    if (prevTime==0) { FPSfactor = 1.0; } else {
        FPSfactor = (irrTimer->getRealTime()-prevTime)/(1000.0/70.0);
    }
    prevTime = irrTimer->getRealTime();

    float prec = 0.75f;

    dynamics->simStep(irrTimer->getRealTime(),60.f * prec);

    mainPlayer->update();
    irrDriver->beginScene(true, true, irr::video::SColor(255, 255, 0, 255));
    irrDriver->setRenderTarget(blurImage2); //copy the old render
    irrDriver->draw2DImage(blurImage,irr::core::position2d<irr::s32>(0,0),irr::core::rect<irr::s32>(0,0,mainWidth,mainHeight), 0,irr::video::SColor(255,255,255,255), false);
    irrDriver->setRenderTarget(blurImage); //create a new render, using the old one to add a blur effect
    irrSmgr->drawAll();
    float BlinkTimer = mainPlayer->BlinkTimer;
    if (BlinkTimer<0) {
        double darkA;
        if (mainPlayer->BlinkTimer>-0.44625) {
            darkA = std::max(darkA,std::sin(-BlinkTimer*201.68));
        } else if (mainPlayer->BlinkTimer>-1.33875) {
            darkA = std::max(darkA,std::abs(std::sin(BlinkTimer*201.68)));
        }
        irrDriver->draw2DRectangle(irr::video::SColor(std::min(255.0,darkA*255.0),0,0,0),irr::core::rect<irr::s32>(0,0,mainWidth,mainHeight));
    }
    irrDriver->draw2DImage(blurImage2,irr::core::position2d<irr::s32>(0,0),irr::core::rect<irr::s32>(0,0,mainWidth,mainHeight), 0,irr::video::SColor(std::min(blurAlpha/FPSfactor,200.0f),255,255,255), false);
    irrDriver->setRenderTarget(0); //draw to screen
    irrDriver->clearZBuffer();
    irrDriver->draw2DImage(blurImage,irr::core::position2d<irr::s32>(0,0),irr::core::rect<irr::s32>(0,0,mainWidth,mainHeight), 0,irr::video::SColor(255,255,255,255), false);
	font1->draw("This is some random text",irr::core::rect<irr::s32>(mainWidth/2+2,mainHeight+2,mainWidth/2+2,mainHeight/2+102),irr::video::SColor(150,0,0,0),true,true);
	font1->draw("This is some random text",irr::core::rect<irr::s32>(mainWidth/2,mainHeight,mainWidth/2,mainHeight/2+100),irr::video::SColor(255,255,255,255),true,true);

    //Blink meter
    irrDriver->draw2DLine(irr::core::position2d<irr::s32>(80,mainHeight-95),irr::core::position2d<irr::s32>(284,mainHeight-95),irr::video::SColor(255,255,255,255));
    irrDriver->draw2DLine(irr::core::position2d<irr::s32>(80,mainHeight-75),irr::core::position2d<irr::s32>(284,mainHeight-75),irr::video::SColor(255,255,255,255));
    irrDriver->draw2DLine(irr::core::position2d<irr::s32>(80,mainHeight-95),irr::core::position2d<irr::s32>(80,mainHeight-75),irr::video::SColor(255,255,255,255));
    irrDriver->draw2DLine(irr::core::position2d<irr::s32>(284,mainHeight-95),irr::core::position2d<irr::s32>(284,mainHeight-75),irr::video::SColor(255,255,255,255));

    for (int i=0;i<mainPlayer->BlinkTimer/100*20;i++) {
        irrDriver->draw2DImage(BlinkMeterIMG,irr::core::position2d<irr::s32>(83+i*10,mainHeight-92),irr::core::rect<irr::s32>(0,0,8,14), 0,irr::video::SColor(255,255,255,255), false);
    }

    //Stamina meter
    irrDriver->draw2DLine(irr::core::position2d<irr::s32>(80,mainHeight-55),irr::core::position2d<irr::s32>(284,mainHeight-55),irr::video::SColor(255,255,255,255));
    irrDriver->draw2DLine(irr::core::position2d<irr::s32>(80,mainHeight-35),irr::core::position2d<irr::s32>(284,mainHeight-35),irr::video::SColor(255,255,255,255));
    irrDriver->draw2DLine(irr::core::position2d<irr::s32>(80,mainHeight-55),irr::core::position2d<irr::s32>(80,mainHeight-35),irr::video::SColor(255,255,255,255));
    irrDriver->draw2DLine(irr::core::position2d<irr::s32>(284,mainHeight-55),irr::core::position2d<irr::s32>(284,mainHeight-35),irr::video::SColor(255,255,255,255));

    for (int i=0;i<mainPlayer->Stamina/100*20;i++) {
        irrDriver->draw2DImage(StaminaMeterIMG,irr::core::position2d<irr::s32>(83+i*10,mainHeight-52),irr::core::rect<irr::s32>(0,0,8,14), 0,irr::video::SColor(255,255,255,255), false);
    }

    if (irrReceiver->IsMouseDown(0)) {
        for (unsigned int i=0;i<itemList.size();i++) {
            if (itemList[i]->getPicked()) {
                itemList[i]->Unpick(mainPlayer->getPosition());
                break;
            }
        }
    }

    if (irrReceiver->IsMouseDown(1)) {
        for (unsigned int i=0;i<itemList.size();i++) {
            if (!itemList[i]->getPicked()) {
                itemList[i]->Pick();
                break;
            }
        }
    }

    blurAlpha = 200;

    irrDriver->endScene();

    irr::core::position2di mousePos = irrReceiver->getMousePos();
    if (mousePos != irr::core::position2di(mainWidth/2,mainHeight/2)) {
        mainPlayer->yaw += ((int)mousePos.X-(int)(mainWidth/2))*0.1;
        mainPlayer->pitch += ((int)mousePos.Y-(int)(mainHeight/2))*0.1;
        irrDevice->getCursorControl()->setPosition((irr::s32)mainWidth/2,(irr::s32)mainHeight/2);
    }

    return irrDevice->run();
}

float world::getFPSfactor() {
    return FPSfactor;
}

player::player(world* own,irr::scene::ISceneManager* smgr,irrDynamics* dyn,MainEventReceiver* receiver,float iheight,float iradius,float mass) {
    owner = own;
    irrSmgr = smgr;
    dynamics = dyn;
    irrReceiver = receiver;

	height = iheight*RoomScale;
	radius = iradius*RoomScale;

    selfRotation.X = selfRotation.Y = selfRotation.Z = 0;

    // Add camera
    Camera = irrSmgr->addCameraSceneNode(0,irr::core::vector3df(0,0,0),irr::core::vector3df(0,0,-1));
    Camera->setPosition(irr::core::vector3df(15*204.8f*RoomScale, 10*RoomScale, 15*204.8f*RoomScale));
    Camera->setTarget(irr::core::vector3df(0, 0, 0));
    Camera->setNearValue(5.0*RoomScale);
    Camera->setFarValue(256.0*RoomScale);

    irrSmgr->setActiveCamera(Camera);

    //Add capsule
    Capsule = dynamics->addPlayerColliderObject(Camera,height,radius,mass,1,1);
    Capsule->setAngularFactor(btVector3(0,0,0)); //don't let the capsule rotate until the player dies
    Capsule->setSleepingThresholds (0.0, 0.0);
    Capsule->setGravity(btVector3(0.f,-300.0f*RoomScale,0.f));
	Capsule->setActivationState(DISABLE_DEACTIVATION);

    //clear inventory
    for (irr::u32 i=0;i<inventory_size;i++) {
        inventory[i]=nullptr;
    }
}

void player::update() {

    Camera->updateAbsolutePosition();

    if (irrReceiver->IsKeyDown(irr::KEY_SPACE)) {
        if (BlinkTimer>0) { BlinkTimer = 0.0; }
        if (BlinkTimer<-0.8925) { BlinkTimer = -0.8925; }
    }

    irr::core::vector3df dir(0,1,0);
    irr::core::matrix4 rotMatrix;

    BlinkTimer-=owner->getFPSfactor()*0.09;

    if (BlinkTimer<=-1.785) {
        BlinkTimer = 100;
    }

    while (yaw>180.0) { yaw-=360.0; }
    while (yaw<-180.0) { yaw+=360.0; }

    while (pitch>180.0) { pitch-=360.0; }
    while (pitch<-180.0) { pitch+=360.0; }

    if (pitch>70.0) pitch = 70.0;
    if (pitch<-70.0) pitch = -70.0;

    selfRotation.X = pitch;
    selfRotation.Y = yaw;

    rotMatrix.setRotationDegrees(irr::core::vector3df(0,0,5));
    rotMatrix.transformVect(dir);

    rotMatrix.setRotationDegrees(selfRotation);
    rotMatrix.transformVect(dir);

    rotMatrix.setRotationDegrees(Camera->getRotation());
    rotMatrix.transformVect(dir);
    Camera->setUpVector(dir);
    dir.X = 0;
    dir.Y = 0;
    dir.Z = 1;

    rotMatrix.setRotationDegrees(selfRotation);
    rotMatrix.transformVect(dir);

    rotMatrix.setRotationDegrees(Camera->getRotation());
    rotMatrix.transformVect(dir);
    Camera->updateAbsolutePosition();
    Camera->setTarget(Camera->getAbsolutePosition()+dir);

    btVector3 speed = Capsule->getLinearVelocity();

    /*float addVSpeed = 0.0f;//(speed.y()>=0.0)*0.75;
    if (irrReceiver->IsKeyDown(irr::KEY_KEY_W)) {
        btVector3 newSpeed(sin(irr::core::degToRad(yaw))*walkSpeed*RoomScale,speed.y()-addVSpeed,cos(irr::core::degToRad(yaw))*walkSpeed*RoomScale);
        Capsule->setLinearVelocity(newSpeed);
    } else if (irrReceiver->IsKeyDown(irr::KEY_KEY_S)) {
        btVector3 newSpeed(sin(irr::core::degToRad(yaw))*-walkSpeed*RoomScale,speed.y()-addVSpeed,cos(irr::core::degToRad(yaw))*-walkSpeed*RoomScale);
        Capsule->setLinearVelocity(newSpeed);
    } else if (irrReceiver->IsKeyDown(irr::KEY_KEY_A)) {
        btVector3 newSpeed(sin(irr::core::degToRad(yaw-90.0))*walkSpeed*RoomScale,speed.y()-addVSpeed,cos(irr::core::degToRad(yaw-90.0))*walkSpeed*RoomScale);
        Capsule->setLinearVelocity(newSpeed);
    } else if (irrReceiver->IsKeyDown(irr::KEY_KEY_D)) {
        btVector3 newSpeed(sin(irr::core::degToRad(yaw+90.0))*walkSpeed*RoomScale,speed.y()-addVSpeed,cos(irr::core::degToRad(yaw+90.0))*walkSpeed*RoomScale);
        Capsule->setLinearVelocity(newSpeed);
    } else {
        Capsule->setLinearVelocity(btVector3(0,speed.y()-addVSpeed,0));
    }*/
    if (!dead) {
		if ((irrReceiver->IsKeyDown(irr::KEY_KEY_W)
			|| irrReceiver->IsKeyDown(irr::KEY_KEY_S)
			|| irrReceiver->IsKeyDown(irr::KEY_KEY_A)
			|| irrReceiver->IsKeyDown(irr::KEY_KEY_D)
			) && std::abs(speed[1]) < 20.f) {

			float walkSpeed = 40.0;
			if (crouchState<0.015f) {
				if (irrReceiver->IsKeyDown(irr::KEY_LSHIFT)) {
					if (Stamina>0) walkSpeed=80.0;
					Stamina-=0.2*owner->getFPSfactor();
					if (Stamina<0) Stamina = -10.0;
				} else {
					Stamina=std::min(Stamina+0.15*owner->getFPSfactor(),100.0);
				}
			} else {
				if (crouchState>0.5f) walkSpeed = 10.f; else walkSpeed = 20.f;
				Stamina=std::min(Stamina+0.15*owner->getFPSfactor(),100.0);
			}
			float dir = 0;
			if (irrReceiver->IsKeyDown(irr::KEY_KEY_W)) {
				dir = 0;
				if (irrReceiver->IsKeyDown(irr::KEY_KEY_A)) {
					dir -= 45;
				} else if (irrReceiver->IsKeyDown(irr::KEY_KEY_D)) {
					dir += 45;
				}
			} else if (irrReceiver->IsKeyDown(irr::KEY_KEY_S)) {
				dir = 180;
				if (irrReceiver->IsKeyDown(irr::KEY_KEY_A)) {
					dir += 45;
				} else if (irrReceiver->IsKeyDown(irr::KEY_KEY_D)) {
					dir -= 45;
				}
			} else {
				if (irrReceiver->IsKeyDown(irr::KEY_KEY_A)) {
					dir = -90;
				} else if (irrReceiver->IsKeyDown(irr::KEY_KEY_D)) {
					dir = 90;
				}
			}
			dir-=90;
			dir *= irr::core::DEGTORAD;
			Capsule->setFriction(0.15f);
			float xs,zs,nxs,nzs,d,dd;
			xs = speed[0];
			zs = speed[2];
			d = irr::core::squareroot(xs*xs+zs*zs);
			if (irr::core::squareroot(xs*xs+zs*zs)<walkSpeed*0.75f*RoomScale) {
				Capsule->applyCentralImpulse(btVector3(std::cos(dir+yaw*irr::core::DEGTORAD)*walkSpeed*2.f*RoomScale,0.f,-std::sin(dir+yaw*irr::core::DEGTORAD)*walkSpeed*2.f*RoomScale));
			} else {
				nxs = std::cos(dir+yaw*irr::core::DEGTORAD);
				nzs = -std::sin(dir+yaw*irr::core::DEGTORAD);
				nxs = std::atan2(nxs,nzs);
				nzs = std::atan2(xs,zs);
				while (nxs<-irr::core::PI) nxs+=irr::core::PI*2.f;
				while (nxs>irr::core::PI) nxs-=irr::core::PI*2.f;
				while (nzs<-irr::core::PI) nzs+=irr::core::PI*2.f;
				while (nzs>irr::core::PI) nzs-=irr::core::PI*2.f;
				dd = nxs - nzs;
				while (dd<-irr::core::PI) dd+=irr::core::PI*2.f;
				while (dd>irr::core::PI) dd-=irr::core::PI*2.f;
				nzs = nzs+dd * 0.08f;
				nzs -= irr::core::PI/2.f;

				Capsule->setLinearVelocity(btVector3(std::cos(nzs)*d,speed[1],-std::sin(nzs)*d));
			}
		} else {
			if (std::abs(speed[1])<=4.f) Capsule->setFriction(3.0f); else Capsule->setFriction(1.0f);
			Stamina=std::min(Stamina+0.15*owner->getFPSfactor(),100.0);
		}
		if (irrReceiver->IsKeyDown(irr::KEY_LCONTROL)) {
			crouched = true;
		} else {
			btVector3 start = Capsule->getCenterOfMassPosition();
			btVector3 finish = start + btVector3(0,height*0.85f,0);
			start += btVector3(0,(height * 0.25f),0);
			if (!dynamics->rayTest(start,finish)) {
				if (!dynamics->rayTest(start+btVector3(radius*1.15f,0,radius*1.15f),finish+btVector3(radius*1.15f,0,radius*1.15f))) {
					if (!dynamics->rayTest(start+btVector3(-radius*1.15f,0,radius*1.15f),finish+btVector3(-radius*1.15f,0,radius*1.15f))) {
						if (!dynamics->rayTest(start+btVector3(radius*1.15f,0,-radius*1.15f),finish+btVector3(radius*1.15f,0,-radius*1.15f))) {
							if (!dynamics->rayTest(start+btVector3(-radius*1.15f,0,-radius*1.15f),finish+btVector3(-radius*1.15f,0,-radius*1.15f))) {
								crouched = false; //don't stand up if there's something above your head
							}
						}
					}
				}
			}
		}
	} else {
		Capsule->setFriction(3.0f);
		Capsule->setRollingFriction(25.0f);
		btVector3 start = Capsule->getCenterOfMassPosition();
		btVector3 finish = start + btVector3(0,height*0.85f,0);
		start += btVector3(0,(height * 0.25f),0);
		if (!dynamics->rayTest(start,finish)) {
			if (!dynamics->rayTest(start+btVector3(radius*1.15f,0,radius*1.15f),finish+btVector3(radius*1.15f,0,radius*1.15f))) {
				if (!dynamics->rayTest(start+btVector3(-radius*1.15f,0,radius*1.15f),finish+btVector3(-radius*1.15f,0,radius*1.15f))) {
					if (!dynamics->rayTest(start+btVector3(radius*1.15f,0,-radius*1.15f),finish+btVector3(radius*1.15f,0,-radius*1.15f))) {
						if (!dynamics->rayTest(start+btVector3(-radius*1.15f,0,-radius*1.15f),finish+btVector3(-radius*1.15f,0,-radius*1.15f))) {
							crouched = false; //don't stand up if there's something above your head
						}
					}
				}
			}
		}
	}

    if (irrReceiver->IsKeyDown(irr::KEY_KEY_Q)) {
		Capsule->setAngularFactor(btVector3(1,1,1)); dead = true;
	}
    if (irrReceiver->IsKeyDown(irr::KEY_KEY_E)) {
		dead = false;
		Capsule->setAngularFactor(btVector3(0,0,0));
		const btQuaternion identity(0,0,0,1);
		btTransform Transform = Capsule->getCenterOfMassTransform();
		Transform.setRotation(identity);
		Capsule->setCenterOfMassTransform(Transform);
		Capsule->setAngularVelocity(btVector3(0,0,0));
	}

	bool changed = false;
	if (crouched) {
		if (crouchState<0.5f) changed = true;

		crouchState=std::min(0.5f,crouchState+(0.5f-crouchState)*owner->getFPSfactor()*0.2f);
		if (crouchState>0.4998f) crouchState = 0.5f;

		if (changed) { //must unregister body to perform changes to its shape
			dynamics->unregisterRBody(Capsule);
			Capsule->getCollisionShape()->setLocalScaling(btVector3(1.f,1.f-crouchState,1.f));
			dynamics->registerNewRBody(Camera,Capsule,-1,1,1,irr::core::vector3df(0,(-(height/2.f)*(1.f-crouchState))+(radius/1.5f),0));
			Capsule->setGravity(btVector3(0.f,-300.0f*RoomScale,0.f));
		}
	} else {
		if (crouchState>0.0f) changed = true;

		crouchState=std::max(0.f,crouchState+(-crouchState)*owner->getFPSfactor()*0.1f);
		if (crouchState<0.002f) crouchState = 0.f;

		if (changed) { //must unregister body to perform changes to its shape
			dynamics->unregisterRBody(Capsule);
			Capsule->getCollisionShape()->setLocalScaling(btVector3(1.f,1.f-crouchState,1.f));
			dynamics->registerNewRBody(Camera,Capsule,-1,1,1,irr::core::vector3df(0,(-(height/2.f)*(1.f-crouchState))+(radius/1.5f),0));
			Capsule->setGravity(btVector3(0.f,-300.0f*RoomScale,0.f));
		}
	}
	//std::cout<<Capsule->getCollisionShape()->getLocalScaling()[1]<<" "<<crouchState<<"\n";

    //std::cout<<"seesNode: "<<seesMeshNode(testNode)<<"\n";
}

void player::addToInventory(item* it) {
    for (irr::u32 i=0;i<inventory_size;i++) {
        if (inventory[i]==nullptr) {
            inventory[i]=it;
            it->Pick();
            break;
        }
    }
}
void player::takeFromInventory(unsigned char slot) {

}

bool player::seesMeshNode(irr::scene::IMeshSceneNode* node) {
    irr::scene::SViewFrustum frust = *Camera->getViewFrustum();

    //transform the frustum to the node's current absolute transformation
    irr::core::matrix4 invTrans(node->getAbsoluteTransformation(), irr::core::matrix4::EM4CONST_INVERSE);
    //invTrans.makeInverse();
    frust.transform(invTrans);

    irr::core::vector3df edges[8];
    node->getBoundingBox().getEdges(edges);

    bool result = false;

    for (irr::s32 i=0; i<irr::scene::SViewFrustum::VF_PLANE_COUNT; ++i) {
        bool boxInFrustum=false;
        for (irr::u32 j=0; j<8; ++j)
        {
            if (frust.planes[i].classifyPointRelation(edges[j]) != irr::core::ISREL3D_FRONT)
            {
                boxInFrustum=true;
                break;
            }
        }

        if (!boxInFrustum)
        {
            result = true;
            break;
        }
    }
    return !result;
}

bool getNodeTriangleTextureName //taken from here: http://irrlicht.sourceforge.net/forum/viewtopic.php?f=9&t=45212
(
		irr::scene::ISceneNode* node,
		const irr::core::triangle3df& tri,
		std::string& texname
) //this function can be slow, use it carefully
{
		irr::scene::IMesh* mesh = 0;

		irr::scene::ESCENE_NODE_TYPE type = node->getType();
		if ((type == irr::scene::ESNT_MESH) || (type == irr::scene::ESNT_OCTREE))
		{
				mesh = static_cast<irr::scene::IMeshSceneNode*>(node)->getMesh();
		}
		else if (type == irr::scene::ESNT_ANIMATED_MESH)
		{
				mesh = static_cast<irr::scene::IAnimatedMeshSceneNode*>(node)->getMesh()->getMesh(0);
		}
		else
		{
				return false;
		}

		if (!mesh)
				return false;


		irr::core::vector3df ptA = tri.pointA;
		irr::core::vector3df ptB = tri.pointB;
		irr::core::vector3df ptC = tri.pointC;

		irr::core::matrix4 matrix = node->getAbsoluteTransformation();
		irr::core::matrix4 inverse;
		irr::core::vector3df p0, p1, p2;

		if (matrix.getInverse(inverse))
		{
				inverse.transformVect(p0, ptA);
				inverse.transformVect(p1, ptB);
				inverse.transformVect(p2, ptC);
		}
		else { p0 = ptA; p1 = ptB; p2 = ptC; }

		for (irr::u32 i=0; i<mesh->getMeshBufferCount(); ++i)
		{
				bool p0Found = false;
				bool p1Found = false;
				bool p2Found = false;

				//printf("mesh->getMeshBuffer(%i): vertexCount=%i\n", i, mesh->getMeshBuffer(i)->getVertexCount());

				irr::scene::IMeshBuffer* buf = mesh->getMeshBuffer(i);
				for (irr::u32 j=0; j<buf->getVertexCount(); ++j)
				{
						irr::core::vector3df pos = buf->getPosition(j);

						//if (pos.equals(p0) || pos.equals(p1) || pos.equals(p2))
								//printf("vertice found!\n");

						if ((!p0Found) && (pos.equals(p0)))
						//if (pos.equals(p0))
						{
								p0Found = true;
						}

						if ((!p1Found) && (pos.equals(p1)))
						//if (pos.equals(p1))
						{
								p1Found = true;
						}

						if ((!p2Found) && (pos.equals(p2)))
						//if (pos.equals(p2))
						{
								p2Found = true;
						}
				}

				if (p0Found && p1Found && p2Found)
				{
						irr::video::ITexture* tex = buf->getMaterial().getTexture(0);
						if (!tex)
								return false;

						texname = std::string(tex->getName().getPath().c_str());
						return true;
				}
		}

		return false;
}
