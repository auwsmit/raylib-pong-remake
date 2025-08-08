// EXPLANATION:
// All the game's logic, including how/when to draw to screen.

#ifndef TEST_PONG_HEADER_GUARD
#define TEST_PONG_HEADER_GUARD

// Macros
// --------------------------------------------------------------------------------
#define WIN_SCORE 5 // Score needed to win

#define PADDLE_LENGTH 120 // Initial settings for paddles
#define PADDLE_WIDTH 20
#define PADDLE_SPEED 500  // Paddle's default speed in pixels per second

#define BALL_SIZE 20      // Initial settings for ball
#define BALL_SPEED 500

#define SCORE_FONT_SIZE 180     // Also used for pause font size
#define DIFFICULTY_FONT_SIZE 50 // For text that shows difficulty at bottom of screen
#define WIN_FONT_SIZE 100

// Ball physics tweaks for game feel
#define BOUNCE_MULTIPLIER 1.1f       // How much faster the ball gets after hitting a paddle
#define PADDLE_HIT_MAX_ANGLE 45.0f   // How much the ball's angle is affected by where it hits the paddle (0 to 90 degrees)
                                     // This is the angle the ball will deflect at if it hits the top or bottom of the paddle
#define MINIMUM_VERTICAL_ANGLE 25.0f // The minimum vertical angle the ball can move (1 degree minimum)
#define RETURN_POSITION_VARIATION 50 // How much the ball's vertical position can change after scoring
#define RETURN_ANGLE_VARIATION 500   // How much the ball's angle can change after scoring

#define SCORE_PAUSE_TIME 1.0f  // Time to pause after a score
#define WIN_PAUSE_TIME 10.0f   // Time to pause after a win

// Types and Structures
// --------------------------------------------------------------------------------

typedef enum GameTurn { TURN_RIGHT_SIDE, TURN_LEFT_SIDE } GameTurn;

typedef enum ScreenState
{
    SCREEN_LOGO, SCREEN_TITLE, SCREEN_GAMEPLAY, SCREEN_ENDING
} ScreenState;

typedef enum GameMode
{
    MODE_ONEPLAYER, MODE_TWOPLAYER, MODE_DEMO
} GameMode;

typedef enum Difficulty // Multiplier for CPU paddle speed
{
    DIFFICULTY_EASY, DIFFICULTY_MEDIUM, DIFFICULTY_HARD
} Difficulty;

typedef struct Paddle
{
    Vector2 position;
    float nextHitPos; // Only used for Computer paddle
                      // Determines the random spot on the paddle will aim to get the ball to hit
    float speed;
    int length;
    int width;
} Paddle;

typedef struct Ball
{
    Vector2 position;
    Vector2 direction;
    float speed; // the ball is always set to this speed
    int size;
} Ball;

typedef struct GameState
{
    Ball ball;
    Paddle paddleL;
    Paddle paddleR;
    GameMode currentMode;
    GameTurn currentTurn; // keeps track of whose turn it currently is
    ScreenState currentScreen;
    Difficulty difficulty; // unused for MODE_TWOPLAYER
    int scoreL;
    int scoreR;
    bool playerWon; // set after a player wins
    bool isPaused;
    float textFade; // tracks time for the fade animation
    float textFadeTimeElapsed; // tracks time for the fade animation
    float winTimer; // countdown after player wins
    float scoreTimer; // countdown after a score
    bool gameShouldExit; // flag to tell the game window to close
} GameState;

// Prototypes
// --------------------------------------------------------------------------------

// Initialization
GameState InitGameState(void); // Initialize game objects and data for the game loop

// Collision
bool CheckCollisionBallPaddle(Ball ball, Paddle paddle); // Check if ball and paddle are colliding
void EdgeCollisionPaddle(Paddle *paddle); // Paddles collide with screen edges
void BounceBallEdge(GameState *pong); // Ball bounces off screen edges and updates the score
void BounceBallPaddle(Ball *ball,     // Ball bounces off paddle
                      Paddle *paddle, // TODO: just pass *pong instead?
                      GameTurn *currentTurn);


// Update game
void UpdatePaddlePlayer1(Paddle *paddle); // Paddle updates based on player input (W/S with Left Shift)
void UpdatePaddlePlayer2(Paddle *paddle); // Paddle updates based on player input (O/L and Up/Down with Right Shift)
void UpdatePaddleComputer(Paddle *paddle, // Paddle updates based on Computer AI
                          Ball *ball, GameTurn currentTurn, Difficulty difficulty); // TODO: pass *pong instead?
void UpdateBall(Ball *ball); // Moves the ball based on its direction, and normalizes its speed
void UpdatePongFrame(GameState *pong); // Updates all the game's data and objects for the current frame

// Draw game
void DrawDottedLine(bool isPaused, bool isDemoMode);
void DrawScores(GameState *pong);
void DrawWinnerMessage(int scoreL, int scoreR, Color fadeColor);
void DrawPongFrame(GameState *pong); // Draws all the game's objects for the current frame

// Game functions
void ResetBall(Ball *ball); // Reset the ball's horizontal position and modify its vertical position and angle

#endif // TEST_PONG_HEADER_GUARD
