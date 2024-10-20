//THREE
#include <stdio.h>
#include "raylib.h"
#include <stdint.h>

typedef uint8_t u8;
typedef uint16_t u16;

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

Camera3D camera = { 0 };
Vector3 player_position = (Vector3){0.0f,0.0f,1.0f}, cam_pos = { 0 }, cam_rot = { 0 };
Color BELK={0,0,0,120}, REED={255,0,0,120};
u8 move_index = 5;
const Vector3 V3_UP = {0.0, 0.0, 1.0};
const Vector3 V3_DOWN = {0.0, 0.0, -1.0};
const Vector3 V3_LEFT = {-1.0, 0.0, 0.0};
const Vector3 V3_RIGHT = {1.0, 0.0, 1.0};
const Vector3 V3_FORWARD = {0.0, 1.0, 0.0};
const Vector3 V3_BACK = {0.0, -1.0, 0.0};
const double PLAYER_SPEED = 0.05;
const double DIAGONAL_MULTIPLIER = 0.7;
const u16 BOARD_HEIGHT = 8, BOARD_WIDTH  =8;

static void UpdateDrawFrame(void);
bool MovePlayerFromMoveIndex(Vector3 *player_positionP, u8 move_indexIN);
void V3Lerp(Vector3 *victimP, Vector3 target_value, float strength);

int main()
{
    const int screenWidth = 1920;
    const int screenHeight = 1080;
	InitWindow(screenWidth, screenHeight, "raylib goofballin'");
    camera.up = (Vector3){ 0.0f, 0.0f, 1.0f };
    camera.position = (Vector3){0.0f,-6.0f,1.0f};
    //camera.position.y -= 2;
    //camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
    camera.fovy = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;
	//cam_rot.y = -15;

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 60, 1);
#else
    SetTargetFPS(144);
	
    //Main game loop
    while (!WindowShouldClose()) //Detect window close button or ESC key
    {
		move_index = 5 + IsKeyDown(KEY_W)*3 - IsKeyDown(KEY_S)*3 + IsKeyDown(KEY_A) - IsKeyDown(KEY_D);

		if(move_index != 5) MovePlayerFromMoveIndex(&player_position, move_index);

		//camera.position = player_position;
		//camera.position.y -= 6;
		//camera.position.y += 6;

		camera.target = player_position;
		V3Lerp(&camera.position,
			   (Vector3){player_position.x, player_position.y-3.0, 2.0f},
			   0.05);
		
		//camera.position.x += 0.01;
		//camera.position.y += 0.01;
		//camera.position.z += 0.01;
		//UpdateCameraPro(&camera, cam_pos, cam_rot, 0);
		//camera.rotation.x += 10;

        UpdateDrawFrame();
    }
#endif
    CloseWindow(); //Close window and OpenGL context
    return 0;
}

// Update and draw game frame
static void UpdateDrawFrame(void)
{
    //UpdateCamera(&camera, CAMERA_ORBITAL);
	
    BeginDrawing();
	ClearBackground(DARKGREEN);
	BeginMode3D(camera);
	
	for(int y=0 ; y<BOARD_HEIGHT ; y++)
	{
		for(int x=0 ; x<BOARD_WIDTH ; x++)
		{
			bool is_red = (x%2 + y%2)%2;
			float x_pos = x - (BOARD_WIDTH/2) + 0.5f;
			float y_pos = y - (BOARD_HEIGHT/2) + 0.5f;
			DrawCube((Vector3){x_pos, y_pos, 0.0f},
					 1.0f, 1.0f, 1.0f, is_red? REED:BELK);
			DrawCubeWires((Vector3){x_pos, y_pos, 0.0f},
						  1.0f, 1.0f, 1.0f, RAYWHITE);
		}
	}

	//Board frame
	DrawCube((Vector3){0.0f, +4.5f, 0.0f},
			 10.0f, 1.0f, 1.0f, BLACK);
	DrawCubeWires((Vector3){0.0f, +4.5f, 0.0f},
				  10.0f, 1.0f, 1.0f, RAYWHITE);
	DrawCube((Vector3){0.0f, -4.5f, 0.0f},
			 10.0f, 1.0f, 1.0f, RAYWHITE);
	DrawCubeWires((Vector3){0.0f, -4.5f, 0.0f},
				  10.0f, 1.0f, 1.0f, BLACK);
	DrawCube((Vector3){-4.5f, 0.0, 0.0f},
			 1.0f, 8.0f, 1.0f, BROWN);
	DrawCubeWires((Vector3){-4.5f, 0.0, 0.0f},
				  1.0f, 8.0f, 1.0f, WHITE);
	DrawCube((Vector3){4.5f, 0.0, 0.0f},
			 1.0f, 8.0f, 1.0f, BROWN);
	DrawCubeWires((Vector3){4.5f, 0.0, 0.0f},
				  1.0f, 8.0f, 1.0f, WHITE);
	
	//DrawPlane((Vector3){ 0.0f, 0.0f, 0.0f }, (Vector2){ 16.0f, 16.0f }, LIGHTGRAY);
	DrawCube(player_position, 1.0f, 1.0f, 1.0f, GREEN);
	DrawCubeWires(player_position, 1.0f, 1.0f, 1.0f, RED);

	DrawGrid(10, 1.0f);
	EndMode3D();
	DrawText("Transparent cubes, neato.", 20, 20, 50, RAYWHITE);
	DrawFPS(0, 0);
    EndDrawing();
}

bool MovePlayerFromMoveIndex(Vector3 *player_positionP, u8 move_indexIN)
{
	//Returns true if the player moves this frame.
	//789
	//456
	//123
	switch(move_indexIN)
	{
	case 7:
		player_positionP->x += PLAYER_SPEED * DIAGONAL_MULTIPLIER;
		player_positionP->y += PLAYER_SPEED * DIAGONAL_MULTIPLIER;
		return true;
	case 8:
		player_positionP->y += PLAYER_SPEED;
		return true;
	case 9:
		player_positionP->x -= PLAYER_SPEED * DIAGONAL_MULTIPLIER;
		player_positionP->y += PLAYER_SPEED * DIAGONAL_MULTIPLIER;
		return true;
	case 4:
		player_positionP->x += PLAYER_SPEED;
		return true;
	case 5:
		return false;
	case 6:
		player_positionP->x -= PLAYER_SPEED;
		return true;
	case 1:
		player_positionP->y -= PLAYER_SPEED * DIAGONAL_MULTIPLIER;
		player_positionP->x += PLAYER_SPEED * DIAGONAL_MULTIPLIER;
		return true;
	case 2:
		player_positionP->y -= PLAYER_SPEED;
		return true;
	case 3:
		player_positionP->y -= PLAYER_SPEED * DIAGONAL_MULTIPLIER;
		player_positionP->x -= PLAYER_SPEED * DIAGONAL_MULTIPLIER;
		return true;
	default:
		for(int i=0;++i<100;)printf("OOPS!");
		return false;
	}
}

void V3Lerp(Vector3 *victimP, Vector3 target_value, float strength)
{
	float difference_x = target_value.x - victimP->x;
	float difference_y = target_value.y - victimP->y;
	float difference_z = target_value.z - victimP->z;
	victimP->x = victimP->x + (difference_x*strength);
	victimP->y = victimP->y + (difference_y*strength);
	victimP->z = victimP->z + (difference_z*strength);
}
