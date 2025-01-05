#include <stdio.h>
#include "raylib.h"
#include <stdint.h>
#include <math.h>
//#define PI M_PI (PI is defined in Raylib)
typedef uint8_t u8;
typedef uint16_t u16;

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

Camera3D camera = { 0 };
Vector3 player_position = (Vector3){0.0f,0.0f,1.0f}, cam_pos = { 0 }, cam_rot = { 0 };
Color BELK={0,0,0,120}, REED={255,0,0,120}, GREN={0,255,0,120};
u8 player_move_index = 5;
const float DIAGONAL_MULTIPLIER = 0.7;
const float PLAYER_DEFAULT_SPEED = 0.05;
float player_speed = PLAYER_DEFAULT_SPEED;
float camera_arm_len = 3.0f;
float cam_height = 2.0f;
float cam_arm_radians = 0;
float cam_arm_rot_rate = 0.015; //Radians per frame
float cam_lerp_rate = 0.05f;

const u16 BOARD_HEIGHT = 16, BOARD_WIDTH = 16;

static void UpdateDrawFrame(void);
bool MovePlayerFromMoveIndex(Vector3 *player_positionP, u8 player_move_indexIN);
void V3LerpVictim(Vector3 *victimP, Vector3 target_value, float strength);
void V3SumPsVictim(Vector3 *aP, Vector3 *bP, Vector3 *victimP);
void V3SumVictim(Vector3 aP, Vector3 bP, Vector3 *victimP);

int main()
{
    const int screenWidth = 1920;
    const int screenHeight = 1080;
	InitWindow(screenWidth, screenHeight, "raylib goofballin'");
    camera.up = (Vector3){ 0.0f, 0.0f, 1.0f };
    camera.position = (Vector3){0.0f,-6.0f,1.0f};
    camera.fovy = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;
	//cam_rot.y = -15;

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 60, 1);
#else
    SetTargetFPS(144);
    //MAIN GAME LOOP
    //MAIN GAME LOOP
    //MAIN GAME LOOP
    while (!WindowShouldClose()) //Detect window close button or ESC key
    {
		//Player
		player_speed = IsKeyDown(KEY_LEFT_SHIFT) ?
			(PLAYER_DEFAULT_SPEED*0.5)
			:
			PLAYER_DEFAULT_SPEED;
		player_move_index =
			5
			+IsKeyDown(KEY_W)*3 //789
			+IsKeyDown(KEY_A)   //456
			-IsKeyDown(KEY_S)*3 //123
			-IsKeyDown(KEY_D);
		if(player_move_index != 5)
			MovePlayerFromMoveIndex(&player_position, player_move_index);
		//Camera
		cam_arm_radians += cam_arm_rot_rate * IsKeyDown(KEY_Q);
		cam_arm_radians -= cam_arm_rot_rate * IsKeyDown(KEY_E);
		float cam_x_goal = player_position.x + (camera_arm_len * sin(cam_arm_radians));
		float cam_y_goal = player_position.y - (camera_arm_len * cos(cam_arm_radians));
		Vector3 camera_position_goal = {cam_x_goal, cam_y_goal, cam_height};
		V3LerpVictim(&camera.position, camera_position_goal, cam_lerp_rate);
		camera.target = player_position;
		UpdateCameraPro(&camera, (Vector3){0,0,0}, (Vector3){0,0,0}, 0);
		//Other
        UpdateDrawFrame();
    }
#endif
    CloseWindow(); //Close window and OpenGL context
    return 0;
}



// Update and draw game frame
static void UpdateDrawFrame(void)
{
    BeginDrawing();
	ClearBackground(DARKGREEN);
	BeginMode3D(camera);
	//Chess spaces
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
	//Chess board frame
	DrawCube((Vector3){0.0f, (BOARD_HEIGHT/2) + 0.5f, 0.0f},
			 BOARD_WIDTH+2.0f, 1.0f, 1.0f,
			 BLACK);
	DrawCubeWires((Vector3){0.0f, (BOARD_HEIGHT/2) + 0.5f, 0.0f},
				  BOARD_WIDTH+2.0f, 1.0f, 1.0f, RAYWHITE);
	DrawCube((Vector3){0.0f, -(BOARD_HEIGHT/2) - 0.5f, 0.0f},
			 BOARD_WIDTH+2.0f, 1.0f, 1.0f,
			 RAYWHITE);
	DrawCubeWires((Vector3){0.0f, -(BOARD_HEIGHT/2) - 0.5f, 0.0f},
				  BOARD_WIDTH+2.0f, 1.0f, 1.0f, BLACK);
	DrawCube((Vector3){-(BOARD_WIDTH/2) - 0.5f, 0.0, 0.0f},
			 1.0f, BOARD_HEIGHT, 1.0f,
			 BROWN);
	DrawCubeWires((Vector3){-(BOARD_WIDTH/2) - 0.5f, 0.0, 0.0f},
				  1.0f, BOARD_HEIGHT, 1.0f, WHITE);
	DrawCube((Vector3){(BOARD_WIDTH/2) + 0.5f, 0.0, 0.0f},
			 1.0f, BOARD_HEIGHT, 1.0f,
			 BROWN);
	DrawCubeWires((Vector3){(BOARD_WIDTH/2) + 0.5f, 0.0, 0.0f},
				  1.0f, BOARD_HEIGHT, 1.0f, WHITE);
	//Player cube
	DrawCube(player_position, 1.0f, 1.0f, 1.0f, GREN);
	DrawCubeWires(player_position, 1.0f, 1.0f, 1.0f, RED);
	//Other
	EndMode3D();
	DrawText("Camera controls, neato! (Q/E)", 20, 20, 50, RAYWHITE);
	DrawFPS(0, 0);
    EndDrawing();
}

bool MovePlayerFromMoveIndex(Vector3 *player_positionP, u8 player_move_indexIN)
{
	//789 Returns true if the player moves this frame.
	//456
	//123
	switch(player_move_indexIN)
	{
	case 7:
		player_positionP->x += player_speed * DIAGONAL_MULTIPLIER;
		player_positionP->y += player_speed * DIAGONAL_MULTIPLIER;
		return true;
	case 8:
		player_positionP->y += player_speed;
		return true;
	case 9:
		player_positionP->x -= player_speed * DIAGONAL_MULTIPLIER;
		player_positionP->y += player_speed * DIAGONAL_MULTIPLIER;
		return true;
	case 4:
		player_positionP->x += player_speed;
		return true;
	case 5:
		return false;
	case 6:
		player_positionP->x -= player_speed;
		return true;
	case 1:
		player_positionP->y -= player_speed * DIAGONAL_MULTIPLIER;
		player_positionP->x += player_speed * DIAGONAL_MULTIPLIER;
		return true;
	case 2:
		player_positionP->y -= player_speed;
		return true;
	case 3:
		player_positionP->y -= player_speed * DIAGONAL_MULTIPLIER;
		player_positionP->x -= player_speed * DIAGONAL_MULTIPLIER;
		return true;
	default:
		for(int i=0;++i<100;)printf("OOPS!");
		return false;
	}
}
void V3LerpVictim(Vector3 *victimP, Vector3 target_value, float strength)
{
	float difference_x = target_value.x - victimP->x;
	float difference_y = target_value.y - victimP->y;
	float difference_z = target_value.z - victimP->z;
	victimP->x = victimP->x + (difference_x*strength);
	victimP->y = victimP->y + (difference_y*strength);
	victimP->z = victimP->z + (difference_z*strength);
}
void V3SumPsVictim(Vector3 *aP, Vector3 *bP, Vector3 *victimP)
{
	//Adds first two pointed V3s and assigns to victim
	victimP->x = aP->x + bP->x;
	victimP->y = aP->y + bP->y;
	victimP->z = aP->z + bP->z;
}
void V3SumVictim(Vector3 aP, Vector3 bP, Vector3 *victimP)
{
	//Adds first two V3s and assigns to victim
	victimP->x = aP.x + bP.x;
	victimP->y = aP.y + bP.y;
	victimP->z = aP.z + bP.z;
}
