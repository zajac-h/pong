/*
 *Michał Chamera, 7.9.2025
*/

#include "raylib.h"
#include <stdlib.h>
#include <math.h> //only for vector2Normalize function

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define TARGET_FPS 60

#define UI_TEXT_SIZE 40

#define PLAYER_WIDTH 10
#define PLAYER_HEIGHT 100
#define PLAYER_SPEED 5

#define BALL_SIZE 10
#define BALL_SPEED 5

#define FOREGROUND_COLOR WHITE
#define BACKGROUND_COLOR BLACK

int main(void)
{
	// window setup
	InitWindow( SCREEN_WIDTH, SCREEN_HEIGHT, "my own pong clone");
	SetTargetFPS(60);

	// game setup
	struct Player
	{
		int score;
		float positionY;
	};

	struct Ball
	{
		Vector2 position;
		Vector2 direction;
	};

	// initial positions
	struct Player player1; 
	struct Player player2;

	struct Ball ball = { { SCREEN_WIDTH/2, SCREEN_HEIGHT/2 }, { 0.0f, 0.0f} };

	void resetScore(void)
	{
		player1.score = 0;
		player2.score = 0;
	}

	void resetPositions(void)
	{
		player1.positionY = SCREEN_HEIGHT/2 - PLAYER_HEIGHT/2;

		player2.positionY = SCREEN_HEIGHT/2 - PLAYER_HEIGHT/2;

		ball.position = (Vector2){ SCREEN_WIDTH/2, SCREEN_HEIGHT/2 };

		//randomize the initial ball direction
		switch (rand() % 2)
		{
		case 0:
			ball.direction = (Vector2) { 1.0f, 0.0f };
			break;

		case 1:
			ball.direction = (Vector2) { -1.0f, 0.0f};
			break;
		}
	}

	// clamp vertical movement of players
	void playerClampMovement(struct Player * player)
	{
		const int upperClamp = 0;
		const int bottomClamp = SCREEN_HEIGHT - PLAYER_HEIGHT;

		if (player->positionY <= (float)upperClamp)
		{
			player->positionY = (float)upperClamp;
		}

	    if (player->positionY >= (float)bottomClamp)
		{
			player->positionY = (float)bottomClamp;
		}
	}

	// move the ball along it's direction vector
	void moveBall()
	{
		ball.position.x = ball.position.x + ball.direction.x * BALL_SPEED;
		ball.position.y = ball.position.y + ball.direction.y * BALL_SPEED;
	}

	// for detecting collisions with walls; in case of upper and lower ones also mirrors the ball direction in y axis
	void checkWallTouch(void)
	{
		const int leftWall = 0 + BALL_SIZE/2;
		const int rightWall = SCREEN_WIDTH - BALL_SIZE/2;
		const int topWall = 0 + BALL_SIZE/2;
		const int bottomWall = SCREEN_HEIGHT - BALL_SIZE/2;

		if (ball.position.x <= (float)leftWall)
		{
			resetPositions();
			player2.score += 1;
		}

		if (ball.position.x >= (float)rightWall)
		{
			resetPositions();
			player1.score +=1;
		
		}

		if (ball.position.y <= (float)topWall || 
		    ball.position.y >= (float)bottomWall)
		{
			ball.direction = (Vector2){ ball.direction.x,
					  ball.direction.y * -1.0f };
		}
	}

	Vector2 vector2Normalize(Vector2 inputVector)
	{
		Vector2 outputVector;
		const float inputVectorLength = sqrt(powf(inputVector.x, 2) + powf(inputVector.y, 2));
		outputVector = (Vector2){ inputVector.x/inputVectorLength, inputVector.y/inputVectorLength };
		return (Vector2)outputVector;
	}

	void checkBallPlayerCollision()
	{
		const int boundaryLeft = 20 + BALL_SIZE/2;
		const int boundaryRight = SCREEN_WIDTH - 20 - BALL_SIZE/2;

		// TODO write a function for this
		if (ball.position.x <= (float)boundaryLeft && 
		    ball.position.y >= player1.positionY &&
		    ball.position.y <= player1.positionY + PLAYER_HEIGHT)
		{
			const int playerCenter = player1.positionY - PLAYER_HEIGHT/2;
			const int playerBallHeightDiff = ball.position.y - player1.positionY;
			// ^^ results negative for upper half, positive for lower

			ball.direction = (Vector2){ ball.direction.x * -1.0f,
				          ball.direction.y + (float)playerBallHeightDiff/PLAYER_HEIGHT/2};
			ball.direction = vector2Normalize(ball.direction);
			// replace multiplying .y by -1.0f by some smart number
			// based on how far is it from the center of player
			//
			// write normalization of direction vector
		}

		if (ball.position.x >= (float)boundaryRight &&
		    ball.position.y >= player2.positionY &&
		    ball.position.y <= player2.positionY + PLAYER_HEIGHT)
		{
			ball.direction = (Vector2){ ball.direction.x * -1.0f,
					  ball.direction.y * -1.0f };
		}
	}

	resetScore();
	resetPositions();

	while (!WindowShouldClose()) //reacts to window close button or ESC key press
	{
		checkWallTouch();
		moveBall();
		checkBallPlayerCollision();

		// player 1 movement
		if (IsKeyDown(KEY_W)) player1.positionY -= PLAYER_SPEED;
		if (IsKeyDown(KEY_S)) player1.positionY += PLAYER_SPEED;

		// player 2 movement
		if (IsKeyDown(KEY_UP)) player2.positionY -= PLAYER_SPEED;
		if (IsKeyDown(KEY_DOWN)) player2.positionY += PLAYER_SPEED;

		// player movement clamp mechanism
		playerClampMovement(&player1);
		playerClampMovement(&player2);

		// ball physics

		// collision detection
	
		// rendering
		BeginDrawing();

			ClearBackground(BACKGROUND_COLOR);	

			// player 1
			DrawRectangle( 10, player1.positionY, 
				       PLAYER_WIDTH, PLAYER_HEIGHT,
			       	       FOREGROUND_COLOR );

			//player 2
			DrawRectangle( SCREEN_WIDTH - 20, player2.positionY,
				       PLAYER_WIDTH, PLAYER_HEIGHT,
				       FOREGROUND_COLOR );
		
			//ball
			DrawCircle( ball.position.x, ball.position.y,
				    BALL_SIZE, FOREGROUND_COLOR );

			//score
			DrawText( TextFormat( "%i:%i", (int)player1.score, (int)player2.score ), 
				  SCREEN_WIDTH/2 - MeasureText("0:0",20)/2,
				  10, UI_TEXT_SIZE, FOREGROUND_COLOR);

		EndDrawing();
	}

	CloseWindow();

	return 0;
}
