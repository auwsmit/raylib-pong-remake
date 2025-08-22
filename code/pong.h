// EXPLANATION:
// All the game's logic, including how/when to draw to screen

#ifndef PONG_GAME_HEADER_GUARD
#define PONG_GAME_HEADER_GUARD

#include "raylib.h"
#include "states.h"

// Macros
// --------------------------------------------------------------------------------
#define WIN_SCORE 5 // Score needed to win

// Game object properties
#define PADDLE_LENGTH 120 // Initial settings for paddles
#define PADDLE_WIDTH 20
#define PADDLE_SPEED 500  // Paddle's default speed in pixels per second
#define BALL_SIZE 20      // Initial settings for ball
#define BALL_SPEED 500

// Ball physics tweaks for game feel
#define BOUNCE_MULTIPLIER 1.1f       // How much faster the ball gets after hitting a paddle
#define PADDLE_HIT_MAX_ANGLE 40.0f   // How much the ball's angle is affected by where it hits the paddle (0 to 90 degrees)
                                     // This is the angle the ball will deflect at if it hits the top or bottom of the paddle
#define MINIMUM_VERTICAL_ANGLE 25.0f // The minimum vertical angle the ball can move (1 degree minimum)
#define RETURN_POSITION_VARIATION 50 // How much the ball's vertical position can change after scoring
#define RETURN_ANGLE_VARIATION 500   // How much the ball's angle can change after scoring

// UI element size
#define FIELD_LINE_WIDTH  15 // Width of the field lines (top, bottom, dotted center-line)
#define SCORE_FONT_SIZE 180     // Also used for pause font size
#define DIFFICULTY_FONT_SIZE 50 // For text that shows difficulty at bottom of screen
#define WIN_FONT_SIZE 100

#define SCORE_PAUSE_TIME 1.0f  // Time to pause after a score
#define WIN_PAUSE_TIME 10.0f   // Time to pause after a win
#define BEEP_FREQUENCY_EDGE   500 // Frequency for beep when the ball hits the screen edge
#define BEEP_FREQUENCY_PADDLE 450 // Frequency for beep when the ball hits a paddle

// Prototypes
// --------------------------------------------------------------------------------

// Initialization
GameState InitGameState(void); // Initialize game objects and data for the game loop

// Collision
bool CheckCollisionBallPaddle(Ball ball, Paddle paddle); // Check if ball and paddle are colliding
void EdgeCollisionPaddle(Paddle *paddle); // Paddles collide with screen edges
void BounceBallEdge(GameState *pong); // Ball bounces off screen edges and updates the score
void BounceBallPaddle(Ball *ball,     // Ball bounces off paddle
                      Paddle *paddle,
                      GameTurn *currentTurn);

// Update game
void UpdatePongFrame(GameState *pong, UiState *titleMenu); // Updates all the game's data and objects for the current frame
void UpdatePaddleMouseInput(Paddle *paddle); // Updates paddle's position based on the mouse
void UpdatePaddlePlayer1(Paddle *paddle); // Paddle speed updates based on player input (W/S with Left Shift)
void UpdatePaddlePlayer2(Paddle *paddle); // Paddle speed updates based on player input (O/L and Up/Down with Right Shift)
void UpdatePaddleComputer(Paddle *paddle, GameState *pong); // Paddle speed updates based on Computer AI
void UpdateBall(Ball *ball); // Moves the ball based on its direction, and normalizes its speed

// Draw game
void DrawPongFrame(GameState *pong, UiState *ui); // Draws all the game's objects for the current frame
void DrawFieldLines(bool isPaused, bool isDemoMode);
void DrawScores(GameState *pong);
void DrawWinnerMessage(int scoreL, int scoreR, Color fadeColor);

// Game functions
void ResetBall(Ball *ball); // Reset the ball's horizontal position and modify its vertical position and angle

#endif // PONG_GAME_HEADER_GUARD
