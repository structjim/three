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
Vector3 cube_position = { 0 }, cam_pos = { 0 }, cam_rot = { 0 };
Color BELK = {0,0,0,90};
u8 move_index = 5;
const double PLAYER_SPEED = 0.2;
const double DIAGONAL_MULTIPLIER = 0.7;
const Vector3 V3_UP = {0.0, 0.0, 1.0};
const Vector3 V3_DOWN = {0.0, 0.0, -1.0};
const Vector3 V3_LEFT = {-1.0, 0.0, 0.0};
const Vector3 V3_RIGHT = {1.0, 0.0, 1.0};
const Vector3 V3_FORWARD = {0.0, 1.0, 0.0};
const Vector3 V3_BACK = {0.0, -1.0, 0.0};

static void UpdateDrawFrame(void);          // Update and draw one frame
bool MovePlayerWithMoveIndex(Vector3 *player_positionP, u8 move_indexIN);

int main()
{
    const int screenWidth = 1920;
    const int screenHeight = 1080;
	InitWindow(screenWidth, screenHeight, "raylib goofballin'");
    camera.up = (Vector3){ 0.0f, 0.0f, 1.0f };
    camera.position = (Vector3){0,-2,1};
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

		if(move_index != 5) MovePlayerWithMoveIndex(&cube_position, move_index);

		//camera.position = cube_position;
		//camera.position.y -= 6;
		//camera.position.y += 6;

		camera.target = (Vector3){0.0, 0.0, 1.0};
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
	
	//DrawPlane((Vector3){ 0.0f, 0.0f, 0.0f }, (Vector2){ 16.0f, 16.0f }, LIGHTGRAY);
	DrawCube(cube_position, 2.0f, 2.0f, 2.0f, BELK);
	DrawCubeWires(cube_position, 2.0f, 2.0f, 2.0f, RED);

	DrawGrid(10, 1.0f);
	EndMode3D();
	DrawText("Transparent cubes, neato.", 20, 20, 50, RAYWHITE);
	DrawFPS(0, 0);
    EndDrawing();
}

bool MovePlayerWithMoveIndex(Vector3 *player_positionP, u8 move_indexIN)
{
	//Returns true if the player moved this frame.
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
