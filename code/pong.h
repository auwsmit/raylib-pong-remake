#ifndef TEST_GAME_HEADER_GUARD
#define TEST_GAME_HEADER_GUARD

#include "config.h"

// Macros
// ----------------------------------------------------------------------------------
#define WIN_SCORE 5 // Score needed to win

#define PADDLE_LENGTH 150 // Initial settings for paddles
#define PADDLE_WIDTH 25
#define PADDLE_SPEED 600  // Paddle's default speed in pixels per second

#define BALL_SIZE 25      // Initial settings for ball
#define BALL_SPEED 600

// Ball physics tweaks for game feel
#define BOUNCE_MULTIPLIER 1.1f       // How much faster the ball gets after hitting a paddle
#define PADDLE_HIT_MAX_ANGLE 40.0f   // How much the ball's angle is affected by where it hits the paddle (0 to 90 degrees)
#define PADDLE_SPEED_INFLUENCE 10    // How much the paddle speed affects the ball on hit (recommend values <20)
#define MINIMUM_VERTICAL_ANGLE 25.0f // The minimum vertical angle the ball can move (1 degree minimum)
#define RETURN_POS_VARIATION 50      // How much the ball's vertical position can change after scoring
#define RETURN_ANGLE_VARIATION 500   // How much the ball's angle can change after scoring

#define SCORE_PAUSE_TIME 1.0f // Time to pause after a score
#define WIN_PAUSE_TIME 10.0f   // Time to pause after a win

// Types and Structures
// ----------------------------------------------------------------------------------
typedef struct Paddle
{
    Vector2 position;
    float speed;
    int length;
    int width;
} Paddle;

typedef struct Ball
{
    Vector2 position;
    Vector2 direction; // this is only used for its direction
    float speed; // the ball is always set to this speed
    int size;
} Ball;

typedef struct GameState
{
    int scoreL;
    int scoreR;
    bool playerWon; // set after a player wins
    float winTimer; // countdown after player wins
    float scoreTimer; // countdown after a score
    Paddle paddleL;
    Paddle paddleR;
    Ball ball;
} GameState;

// Prototypes
// ----------------------------------------------------------------------------------

// Initialization
GameState InitGameState(void); // Initialize game objects and data for the game loop.

// Game Flow
// Collision
bool CheckCollisionBallPaddle(Ball ball, Paddle paddle); // Check if ball and paddle are colliding.
void EdgeCollisionPaddle(Paddle *paddle); // Paddles collide with screen edges.

void BounceBallEdge(GameState *pong); // Ball bounces off screen edges and updates the score.
void BounceBallPaddle(Ball *ball, Paddle *paddle); // Ball bounces off paddle.

// Update game
void UpdatePaddlePlayer1(Paddle *paddle); // Paddle updates based on player input. (W/S with Left Shift)
void UpdatePaddlePlayer2(Paddle *paddle); // Paddle updates based on player input. (O/L and Up/Down with Right Shift)
void UpdatePaddleComputer(Paddle *paddle, Ball *ball); // Paddle updates based on Computer AI.
void UpdateBall(Ball *ball); // Moves the ball based on its direction, and normalizes its speed.
void UpdatePong(GameState *pong); // Updates all the game's data and objects for the current frame.

// Draw game
void DrawDottedLine(void);
void DrawScores(GameState *pong);
void DrawWinnerMessage(int scoreL, int scoreR);
void DrawGame(GameState *pong); // Draws all the game's objects for the current frame.

// Game functions
void ResetBall(Ball *ball); // Reset the ball's horizontal position and modifies its vertical position and angle.

#endif // TEST_GAME_HEADER_GUARD
