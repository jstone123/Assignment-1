// Assignment1.cpp: A program using the TL-Engine

#include <TL-Engine.h>	// TL-Engine include file and namespace
using namespace tle;

void main()
{//JStone3 Josh Stone.
	// Create a 3D engine (using TLX engine here) and open a window for it
	I3DEngine* myEngine = New3DEngine(kTLX);
	myEngine->StartWindowed();

	//Add media folder
	myEngine->AddMediaFolder(".\\Media");

	/**** Set up your scene here ****/
	//Array to store starting settings of camera.
	float cameraPerspective[4] = { -20.f, 120.f, -200.f, 20.f };
	//An array is used to store the skin codes. The game will move through the array to change the skins of each sphere.
	const int numSkins = 3;
	string skins[numSkins] = { "Blue.png", "Indigo.png", "Violet.png" };
	int sphereSkinCounter1 = 0;
	int sphereSkinCounter2 = 1;
	int sphereSkinCounter3 = 2;
	//Set camera to suitable start position to watch over game cycle using values from array.
	ICamera* myCamera;
	myCamera = myEngine->CreateCamera(kFPS, cameraPerspective[0], cameraPerspective[1], cameraPerspective[2]);
	myCamera->RotateX(cameraPerspective[3]);
	//Floor 
	IMesh* floorMesh;
	IModel* floor;
	floorMesh = myEngine->LoadMesh("Floor.x");
	floor = floorMesh->CreateModel();
	floor->SetSkin("wood2.jpg");
	//Create sphere1 and assign starting position.
	const int numSpheres = 3;
	IMesh* SphereMesh;
	IModel* sphere[numSpheres];//Spheres are created using an array.
	SphereMesh = myEngine->LoadMesh("Sphere.x");
	sphere[0] = SphereMesh->CreateModel(-50.f, 10.f, 0.f);
	sphere[0]->SetSkin(skins[0]);
	//Create sphere2 and assign starting position.
	sphere[1] = SphereMesh->CreateModel(50.f, 10.f, 50.f);
	sphere[1]->SetSkin(skins[1]);
	//Create sphere3 and assign starting position.
	sphere[2] = SphereMesh->CreateModel(25.f, 10.f, 0.f);
	sphere[2]->SetSkin(skins[2]);
	const float kGameSpeed = (0.005f);//Base speed of all the spheres.
	const float sphereRotation = (3.141);//Use Pi to calculate the sphere rotation, to make it seem realistic.
	const float sphereRotationPause = (0.f);//Used to prevent spheres rotating during pause state of game.
	//Cap max and min speed of sphere.
	const int sphereMaxSpeedLimit = (3);//Sphere max total speed limit.
	const int sphereMinSpeedLimit = (0);//Sphere min total speed limit. This prevents sphere from reversing its direction.
	//Used in calculating overall speed of sphere.
	const float sphereSpeedChange = (0.1f);//Modifiable by user.
	//Values used to set the boundaries of movement of the spheres.
	const float minXLimit = (-50.f);//Lower bound of X axis movement.
	const float maxXlimit = (50.f);//Upper bound of X axis movement.
	const float minYLimit = (10.f);//Lower bound of Y axis movement.
	const float maxYLimit = (20.f);//Upper bound of Y axis movement.
	const float minZLimit = (0.f);//Lower bound of Z axis movement.
	const float maxZLimit = (100.f);//Upper bound of Z axis movement.
	//Values used to allow the user to alter speed and height of spheres.
	float sphereSpeed = (1.f);
	const float sphereHeight = (0.005f);
	/*Control state of game
	and state of spheres.*/
	bool pause = false;
	bool reverse = false;
	bool spheresOnGround = true;
	//Allow game to check if keys are pressed/held.
	bool pauseKeyPressed; //P key, pause game.
	bool reverseKeyPressed;//R key, reverse sphere direction.
	bool speedIncreaseKeyPressed;//W key, speed up spheres	.										
	bool speedDecreaseKeyPressed;//S key, slow down spheres.
	bool raiseSphereKeyHeld;//Q key, move spheres up.
	bool lowerSphereKeyHeld; //A key, move spheres down.
	bool sphereSkinKeyPressed;//E key, swap sphere skins.

	// The main game loop, repeat until engine is stopped
	while (myEngine->IsRunning())
	{
		// Draw the scene
		myEngine->DrawScene();

		/**** Update your scene each frame here ****/
		//Allow game to be quit without using alt+f4.
		if (myEngine->KeyHit(Key_Escape))
		{
			myEngine->Stop();
		}
		/*Variables used to monitor when keys are hit.
		Change bool values to enable/disable function of the keys.*/
		pauseKeyPressed = (myEngine->KeyHit(Key_P));
		reverseKeyPressed = (myEngine->KeyHit(Key_R));
		speedIncreaseKeyPressed = (myEngine->KeyHit(Key_W));
		speedDecreaseKeyPressed = (myEngine->KeyHit(Key_S));
		raiseSphereKeyHeld = (myEngine->KeyHeld(Key_Q));//KeyHeld because this key is used for changing height of sphere.
		lowerSphereKeyHeld = (myEngine->KeyHeld(Key_A));//KeyHeld because this key is used for changing height of sphere.
		sphereSkinKeyPressed = (myEngine->KeyHit(Key_E));
		//check if player presses pause key, invert pause variable to make game pause.
		if (pauseKeyPressed)
		{
			pause = !pause;
		}
		//check if player presses reverse key, invert reverse variable to make game reverse.
		if (reverseKeyPressed)
		{
			reverse = !reverse;
		}
		/*Variables used for changing and limiting movement of spheres.
		Used to find co-ordinates of spheres.
		This data is used to move spheres around circuit */
		//sphere1
		float xPosSphere1 = sphere[0]->GetX();
		float yPosSphere1 = sphere[0]->GetY();
		float zPosSphere1 = sphere[0]->GetZ();
		//sphere2
		float xPosSphere2 = sphere[1]->GetX();
		float yPosSphere2 = sphere[1]->GetY();
		float zPosSphere2 = sphere[1]->GetZ();
		//sphere3
		float xPosSphere3 = sphere[2]->GetX();
		float yPosSphere3 = sphere[2]->GetY();
		float zPosSphere3 = sphere[2]->GetZ();
		//Spheres Y Values must be checked to ensure they are on the ground.
		//Spheres will only rotate if they are on the ground.
		if (yPosSphere1 == minYLimit && yPosSphere2 == minYLimit && yPosSphere3 == minYLimit)
		{
			spheresOnGround = true;
		}
		if (yPosSphere1 != minYLimit || yPosSphere2 != minYLimit || yPosSphere3 != minYLimit)
		{
			spheresOnGround = false;
		}
		//Movement of spheres around circuit when game is unpaused and not in reverse mode. This allows this code to be toggled. 
		if (!pause && !reverse)
		{
			//The colours of the spheres use an array in a cyclic function.
			//Each time the skin needs to be change the skin counter values are incremented by one, as long as there new value doesn't exceed the number of skins.
			if (sphereSkinKeyPressed)
			{
				if (sphereSkinCounter1 + 1 < numSkins)
				{
					sphereSkinCounter1++;
				}
				else if (sphereSkinCounter1 + 1 == numSkins)
				{
					sphereSkinCounter1 = 0;
				}
				if (sphereSkinCounter2 + 1 < numSkins)
				{
					sphereSkinCounter2++;
				}
				else if (sphereSkinCounter2 + 1 == numSkins)
				{
					sphereSkinCounter2 = 0;
				}
				if (sphereSkinCounter3 + 1 < numSkins)
				{
					sphereSkinCounter3++;
				}
				else if (sphereSkinCounter3 + 1 == numSkins)
				{
					sphereSkinCounter3 = 0;
				}
				//Once the skin counter values have been incremented/changed the skins of the spheres are changed.
				sphere[0]->SetSkin(skins[sphereSkinCounter1]);
				sphere[1]->SetSkin(skins[sphereSkinCounter2]);
				sphere[2]->SetSkin(skins[sphereSkinCounter3]);
			}
			//SPHERE 1 MOVEMENTS
			//sphere1 move in positive Z direction.
			if (xPosSphere1 == minXLimit && zPosSphere1 < maxZLimit)
			{
				sphere[0]->MoveZ(kGameSpeed*sphereSpeed);
				if (spheresOnGround)
				{
					sphere[0]->RotateX(sphereRotation*(kGameSpeed*sphereSpeed));//This line is used to scale the rotation speed of sphere, so the sphere rotations look realistic at any speed.
				}
			}
			//sphere 1 move in positive X direction.
			if (zPosSphere1 == maxZLimit && xPosSphere1 < maxXlimit)
			{
				sphere[0]->MoveX(kGameSpeed*sphereSpeed);
				if (spheresOnGround)
				{
					sphere[0]->RotateZ(-sphereRotation*(kGameSpeed*sphereSpeed));//This line is used to scale the rotation speed of sphere, so the sphere rotations look realistic at any speed.
				}
			}
			//sphere1 move in negative Z direction.
			if (xPosSphere1 == maxXlimit && zPosSphere1 > minZLimit)
			{
				sphere[0]->MoveZ(-kGameSpeed*sphereSpeed);
				if (spheresOnGround)
				{
					sphere[0]->RotateX(-sphereRotation*(kGameSpeed*sphereSpeed));//This line is used to scale the rotation speed of sphere, so the sphere rotations look realistic at any speed.
				}
			}
			//sphere1 move in negative X direction.
			if (zPosSphere1 == minZLimit && xPosSphere1 > minXLimit)
			{
				sphere[0]->MoveX(-kGameSpeed*sphereSpeed);
				if (spheresOnGround)
				{
					sphere[0]->RotateZ(sphereRotation*(kGameSpeed*sphereSpeed));//This line is used to scale the rotation speed of sphere, so the sphere rotations look realistic at any speed.
				}
			}
			//SPHERE 2 MOVEMENTS
			//sphere2 move in positive Z direction.
			if (xPosSphere2 == minXLimit && zPosSphere2 < maxZLimit)
			{
				sphere[1]->MoveZ(kGameSpeed*sphereSpeed);
				if (spheresOnGround)
				{
					sphere[1]->RotateX(sphereRotation*(kGameSpeed*sphereSpeed));//This line is used to scale the rotation speed of sphere, so the sphere rotations look realistic at any speed.
				}
			}
			//sphere2 move in positive X direction.
			if (zPosSphere2 == maxZLimit && xPosSphere2 < maxXlimit)
			{
				sphere[1]->MoveX(kGameSpeed*sphereSpeed);
				if (spheresOnGround)
				{
					sphere[1]->RotateZ(-sphereRotation*(kGameSpeed*sphereSpeed));//This line is used to scale the rotation speed of sphere, so the sphere rotations look realistic at any speed.
				}
			}
			//sphere2 move in negative Z direction.
			if (xPosSphere2 == maxXlimit && zPosSphere2 > minZLimit)
			{
				sphere[1]->MoveZ(-kGameSpeed*sphereSpeed);
				if (spheresOnGround)
				{
					sphere[1]->RotateX(-sphereRotation*(kGameSpeed*sphereSpeed));//This line is used to scale the rotation speed of sphere, so the sphere rotations look realistic at any speed.
				}
			}
			//sphere2 move in negative X direction.
			if (zPosSphere2 == minZLimit && xPosSphere2 > minXLimit)
			{
				sphere[1]->MoveX(-kGameSpeed*sphereSpeed);
				if (spheresOnGround)
				{
					sphere[1]->RotateZ(sphereRotation*(kGameSpeed*sphereSpeed));//This line is used to scale the rotation speed of sphere, so the sphere rotations look realistic at any speed.
				}
			}
			///SPHERE 3 MOVEMENTS
			//sphere3 move in positive Z direction.
			if (xPosSphere3 == minXLimit && zPosSphere3 < maxZLimit)
			{
				sphere[2]->MoveZ(kGameSpeed*sphereSpeed);
				if (spheresOnGround)
				{
					sphere[2]->RotateX(sphereRotation*(kGameSpeed*sphereSpeed));//This line is used to scale the rotation speed of sphere, so the sphere rotations look realistic at any speed.
				}
			}
			//sphere3 move in positive X direction.
			if (zPosSphere3 == maxZLimit && xPosSphere3 < maxXlimit)
			{
				sphere[2]->MoveX(kGameSpeed*sphereSpeed);
				if (spheresOnGround)
				{
					sphere[2]->RotateZ(-sphereRotation*(kGameSpeed*sphereSpeed));//This line is used to scale the rotation speed of sphere, so the sphere rotations look realistic at any speed.
				}
			}
			//sphere3 move in negative Z direction.
			if (xPosSphere3 == maxXlimit && zPosSphere3 > minZLimit)
			{
				sphere[2]->MoveZ(-kGameSpeed*sphereSpeed);
				if (spheresOnGround)
				{
					sphere[2]->RotateX(-sphereRotation*(kGameSpeed*sphereSpeed));//This line is used to scale the rotation speed of sphere, so the sphere rotations look realistic at any speed.
				}
			}
			//sphere3 move in negative X direction.
			if (zPosSphere3 == minZLimit && xPosSphere3 > minXLimit)
			{
				sphere[2]->MoveX(-kGameSpeed*sphereSpeed);
				if (spheresOnGround)
				{
					sphere[2]->RotateZ(sphereRotation*(kGameSpeed*sphereSpeed));//This line is used to scale the rotation speed of sphere, so the sphere rotations look realistic at any speed.
				}
			}
		}
		/*This is used to set the X and Z co-ordinates of spheres to the max/min distances they can move.
		Since the spheres don't move at values that are factors of the X and Z limits, they will sometimes move past the movement boundaries, this prevents this.*/
		//Limit sphere1 movements
		if (xPosSphere1 < minXLimit)
		{
			sphere[0]->SetX(minXLimit);
		}
		if (xPosSphere1 > maxXlimit)
		{
			sphere[0]->SetX(maxXlimit);
		}
		if (zPosSphere1 < minZLimit)
		{
			sphere[0]->SetZ(minZLimit);
		}
		if (zPosSphere1 > maxZLimit)
		{
			sphere[0]->SetZ(maxZLimit);
		}
		//Limit sphere2 movements
		if (xPosSphere2 < minXLimit)
		{
			sphere[1]->SetX(minXLimit);
		}
		if (xPosSphere2 > maxXlimit)
		{
			sphere[1]->SetX(maxXlimit);
		}
		if (zPosSphere2 < minZLimit)
		{
			sphere[1]->SetZ(minZLimit);
		}
		if (zPosSphere2 > maxZLimit)
		{
			sphere[1]->SetZ(maxZLimit);
		}
		//Limit sphere3 movements
		if (xPosSphere3 < minXLimit)
		{
			sphere[2]->SetX(minXLimit);
		}
		if (xPosSphere3 > maxXlimit)
		{
			sphere[2]->SetX(maxXlimit);
		}
		if (zPosSphere3 < minZLimit)
		{
			sphere[2]->SetZ(minZLimit);
		}
		if (zPosSphere3 > maxZLimit)
		{
			sphere[2]->SetZ(maxZLimit);
		}
		//When the game is paused, this prevents the speed of spheres being changed and stops sphere rotating.
		if (pause)//Only occurs if pause variable is true.
		{
			speedIncreaseKeyPressed = false;
			speedDecreaseKeyPressed = false;
		}
		//Change value of sphere speed, which multiplies kGameSpeedSphere of each sphere to speed up or slow down spheres.// 		
		if (speedIncreaseKeyPressed)
		{
			sphereSpeed += (sphereSpeedChange);
		}
		if (speedDecreaseKeyPressed)
		{
			sphereSpeed -= (sphereSpeedChange);
		}
		//Capped at 0.
		if (sphereSpeed < sphereMinSpeedLimit)
		{
			sphereSpeed = sphereMinSpeedLimit;
		}
		//Capped at 3.
		if (sphereSpeed > sphereMaxSpeedLimit)
		{
			sphereSpeed = sphereMaxSpeedLimit;
		}
		/*Increase height of spheres up to 20 units max.
		Disabled at Y = 20.*/
		if (raiseSphereKeyHeld)
		{
			if (yPosSphere1 < maxYLimit)
			{
				sphere[0]->MoveY(sphereHeight);
				sphere[1]->MoveY(sphereHeight);
				sphere[2]->MoveY(sphereHeight);
			}
		}
		/*Decrese height of spheres down to 10 units.
		Disabled at Y = 10.*/
		if (lowerSphereKeyHeld)
		{
			if (yPosSphere1 > minYLimit)
			{
				sphere[0]->MoveY(-sphereHeight);
				sphere[1]->MoveY(-sphereHeight);
				sphere[2]->MoveY(-sphereHeight);
			}
		}
		//Code for sphere movement in reverse. Only occurs if reverse is true and game is unpaused. This allows this section code to be toggled using the reverse key and pause key.
		if (reverse && !pause)
		{
			//The colours of the spheres use an array in a cyclic function.
			//Each time the skin needs to be change the skin counter values are incremented by one, as long as there new value doesn't exceed the number of skins.
			if (sphereSkinKeyPressed)
			{
				if (sphereSkinCounter1 - 1 > -1)
				{
					sphereSkinCounter1--;
				}
				else if (sphereSkinCounter1 - 1 == -1)
				{
					sphereSkinCounter1 = 2;
				}
				if (sphereSkinCounter2 - 1 > -1)
				{
					sphereSkinCounter2--;
				}
				else if (sphereSkinCounter2 - 1 == -1)
				{
					sphereSkinCounter2 = 2;
				}
				if (sphereSkinCounter3 - 1 > -1)
				{
					sphereSkinCounter3--;
				}
				else if (sphereSkinCounter3 - 1 == -1)
				{
					sphereSkinCounter3 = 2;
				}
				//Once the skin counter values have been incremented/changed the skins of the spheres are changed.
				sphere[0]->SetSkin(skins[sphereSkinCounter1]);
				sphere[1]->SetSkin(skins[sphereSkinCounter2]);
				sphere[2]->SetSkin(skins[sphereSkinCounter3]);
			}
			//SPHERE 1 MOVEMENTS
			//sphere1 move in positive X direction.
			if (xPosSphere1 <= maxXlimit && zPosSphere1 == minZLimit)
			{
				sphere[0]->MoveX(kGameSpeed*sphereSpeed);
				if (spheresOnGround)//Spheres only rotate if they are on the ground.
				{
					sphere[0]->RotateZ(-sphereRotation*(kGameSpeed*sphereSpeed));//This line is used to scale the rotation speed of sphere, so the sphere rotations look realistic at any speed.
				}
			}
			//sphere1 move in positive Z direction.
			if (xPosSphere1 == maxXlimit && zPosSphere1 < maxZLimit)
			{
				sphere[0]->MoveZ(kGameSpeed*sphereSpeed);
				if (spheresOnGround)//Spheres only rotate if they are on the ground.
				{
					sphere[0]->RotateX(sphereRotation*(kGameSpeed*sphereSpeed));//This line is used to scale the rotation speed of sphere, so the sphere rotations look realistic at any speed.
				}
			}
			//sphere1 move in negative X direction.
			if (xPosSphere1 > minXLimit && zPosSphere1 == maxZLimit)
			{
				sphere[0]->MoveX(-kGameSpeed*sphereSpeed);
				if (spheresOnGround)//Spheres only rotate if they are on the ground.
				{
					sphere[0]->RotateZ(sphereRotation*(kGameSpeed*sphereSpeed));//This line is used to scale the rotation speed of sphere, so the sphere rotations look realistic at any speed.
				}
			}
			//sphere1 move in negative Z direction.
			if (xPosSphere1 == minXLimit && zPosSphere1 > minZLimit)
			{
				sphere[0]->MoveZ(-kGameSpeed*sphereSpeed);
				if (spheresOnGround)//Spheres only rotate if they are on the ground.
				{
					sphere[0]->RotateX(-sphereRotation*(kGameSpeed*sphereSpeed));//This line is used to scale the rotation speed of sphere, so the sphere rotations look realistic at any speed.
				}
			}
			//SPHERE 2 MOVEMENTS
			//sphere2 move in positive X direction.
			if (xPosSphere2 < maxXlimit && zPosSphere2 == minZLimit)
			{
				sphere[1]->MoveX(kGameSpeed*sphereSpeed);
				if (spheresOnGround)//Spheres only rotate if they are on the ground.
				{
					sphere[1]->RotateZ(-sphereRotation*(kGameSpeed*sphereSpeed));//This line is used to scale the rotation speed of sphere, so the sphere rotations look realistic at any speed.
				}
			}
			//sphere2 move in positive Z direction.
			if (xPosSphere2 == maxXlimit && zPosSphere2 < maxZLimit)
			{
				sphere[1]->MoveZ(kGameSpeed*sphereSpeed);
				if (spheresOnGround)//Spheres only rotate if they are on the ground.
				{
					sphere[1]->RotateX(sphereRotation*(kGameSpeed*sphereSpeed));//This line is used to scale the rotation speed of sphere, so the sphere rotations look realistic at any speed.
				}
			}
			//sphere2 move in negative X direction.
			if (xPosSphere2 > minXLimit && zPosSphere2 == maxZLimit)
			{
				sphere[1]->MoveX(-kGameSpeed*sphereSpeed);
				if (spheresOnGround)//Spheres only rotate if they are on the ground.
				{
					sphere[1]->RotateZ(sphereRotation*(kGameSpeed*sphereSpeed));//This line is used to scale the rotation speed of sphere, so the sphere rotations look realistic at any speed.
				}
			}
			//sphere2 move in negative Z direction.
			if (xPosSphere2 == minXLimit && zPosSphere2 > minZLimit)
			{
				sphere[1]->MoveZ(-kGameSpeed*sphereSpeed);
				if (spheresOnGround)//Spheres only rotate if they are on the ground.
				{
					sphere[1]->RotateX(-sphereRotation*(kGameSpeed*sphereSpeed));//This line is used to scale the rotation speed of sphere, so the sphere rotations look realistic at any speed.
				}
			}
			//SPHERE 3 MOVEMENTS
			//sphere3 move in positive X direction.
			if (xPosSphere3 <= maxXlimit && zPosSphere3 == minZLimit)
			{
				sphere[2]->MoveX(kGameSpeed*sphereSpeed);
				if (spheresOnGround)//Spheres only rotate if they are on the ground.
				{
					sphere[2]->RotateZ(-sphereRotation*(kGameSpeed*sphereSpeed));//This line is used to scale the rotation speed of sphere, so the sphere rotations look realistic at any speed.
				}
			}
			//sphere3 move in positive Z direction.
			if (xPosSphere3 == maxXlimit && zPosSphere3 < maxZLimit)
			{
				sphere[2]->MoveZ(kGameSpeed*sphereSpeed);
				if (spheresOnGround)//Spheres only rotate if they are on the ground.
				{
					sphere[2]->RotateX(sphereRotation*(kGameSpeed*sphereSpeed));//This line is used to scale the rotation speed of sphere, so the sphere rotations look realistic at any speed.
				}
			}
			//sphere3 move in negative X direction.
			if (xPosSphere3 > minXLimit && zPosSphere3 == maxZLimit)
			{
				sphere[2]->MoveX(-kGameSpeed*sphereSpeed);
				if (spheresOnGround)//Spheres only rotate if they are on the ground.
				{
					sphere[2]->RotateZ(sphereRotation*(kGameSpeed*sphereSpeed));//This line is used to scale the rotation speed of sphere, so the sphere rotations look realistic at any speed.
				}
			}
			//sphere3 move in negative Z direction.
			if (xPosSphere3 == minXLimit && zPosSphere3 > minZLimit)
			{
				sphere[2]->MoveZ(-kGameSpeed*sphereSpeed);
				if (spheresOnGround)//Spheres only rotate if they are on the ground.
				{
					sphere[2]->RotateX(-sphereRotation*(kGameSpeed*sphereSpeed));//This line is used to scale the rotation speed of sphere, so the sphere rotations look realistic at any speed.
				}
			}

		}
	}

	// Delete the 3D engine now we are finished with it
	myEngine->Delete();
}
