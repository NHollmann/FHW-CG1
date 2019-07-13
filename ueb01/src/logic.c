/**
 * @file
 * Logik-Modul.
 * Das Modul kapselt die Programmlogik. Die Programmlogik besteht wesentlich aus 
 * der Verwaltung und Berechnung der Bewegungsrichtung des Spielballs, und der
 * Kollisionsberechnung mit den Schlaegern des Spielers und des Bots. Die
 * Programmlogik ist weitgehend unabhaengig von Ein-/Ausgabe (io.h/c) und
 * Darstellung (scene.h/c).
 *
 * Bestandteil einer Uebung im Rahmen des Moduls Praktikum Grundlagen der Computergrafik 
 * an der FH Wedel.
 *
 * @author Nicolas Hollmann, Daniel Klintworth
 */

/* ---- System Header einbinden ---- */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

/* ---- Eigene Header einbinden ---- */
#include "logic.h"


/* ---- Konstanten ---- */

/** Startgeschwindigkeit des Balls */
#define BALL_START_SPEED 0.5f

/** Minimaler Startwinkel fuer den Ball */
#define BALL_START_MIN_ANGLE (-45)

/** Maximaler Startwinkel fuer den Ball */
#define BALL_START_MAX_ANGLE 45

/** Horizontale Startposition des Balls */
#define BALL_START_POS_X 0.55f

/** Die Beschleunigung des Balls bei einem Schlaegerkontakt */
#define BALL_ACCELERATION 0.1f

/** Die Rotationsgeschwindigkeit des Balls */
#define BALL_ROTATION_SPEED 60.0f

/** Die maximale Geschwindigkeit des Schlaegers */
#define PADDLE_MAX_SPEED 0.8f

/** Die maximale Rotationsgeschwindigkeit des Schlaegers */
#define PADDLE_ROTATION_SPEED 60.0f

/** Maximaler Winkel des Schlaegers */
#define PADDLE_MAX_ANGLE 35.0f

/** Vertikale Spielfeldbegrenzung fuer die Schlager */
#define PADDLE_MOVEMENT_LIMIT 0.69f

/** Geschwindigkeit (Schritte in X-Richtung pro Sekunde) */
#define X_STEPS_PS 0.35f

/** Geschwindigkeit (Schritte in Y-Richtung pro Sekunde) */
#define Y_STEPS_PS 0.5f


/* ---- Globale Daten ---- */

/** der Spielball */
static Ball g_ball = BALL_EMPTY;

/** Spielerdaten */
static Player g_human = {{-0.85f, 0.0f}, 0.0f, 3, GL_TRUE};

/** Botdaten */
static Player g_bot = {{0.85f, 0.0f}, 0.0f, 3, GL_FALSE};

/** Bewegung des Spielerschlaegers */
static GLboolean g_movement[2] = {GL_FALSE, GL_FALSE};

/** Rotation des Spielerschlaegers */
static GLboolean g_rotation[2] = {GL_FALSE, GL_FALSE};

/** Speicher fuer die Hilfslinien */
static Guidelines g_guidelines = GUIDELINES_EMPTY;

/** Wireframe Modus Renderoption */
static GLboolean g_randomShape = GL_FALSE;

/** Paddle Hilfslinien Renderoption */
static GLboolean g_paddleGuideDrawing = GL_FALSE;


/* ---- Interne Funktionen ---- */

/**
 * Prueft, ob der Spielball mit dem Rahmen kollidiert ist.
 * 
 * @return Rahmenseite, mit der kollidiert wurde
 */
static CGSide checkBorderCollision(void)
{
	CGSide res = sideNone;

	/* Ball fliegt nach rechts und
		 die rechte Seite des Balls ueberschreitet den rechten Rand */
	if (g_ball.direction[0] > 0.0f &&
		g_ball.position[0] + BALL_SIZE >= 1.0)
	{
		res = sideRight;
	}

	/* Ball fliegt nach links und
		 die linke Seite des Balls ueberschreitet den linken Rand */
	else if (g_ball.direction[0] < 0.0f &&
			 g_ball.position[0] - BALL_SIZE <= -1.0)
	{
		res = sideLeft;
	}

	/* Ball fliegt nach oben und
		 die obere Seite des Balls ueberschreitet den oberen Rand */
	else if (g_ball.direction[1] > 0.0f &&
			 g_ball.position[1] + BALL_SIZE >= BORDER_TOP - BORDER_THICKNESS / 2)
	{
		res = sideTop;
	}

	/* Ball fliegt nach unten und
		 die untere Seite des Ball ueberschreitet den unteren Rand */
	else if (g_ball.direction[1] < 0.0f &&
			 g_ball.position[1] - BALL_SIZE <= BORDER_BOTTOM + BORDER_THICKNESS / 2)
	{
		res = sideBottom;
	}

	return res;
}

/**
 * Gibt die Leben eines Spielers aus.
 * 
 * @param name Bezeichner, der vor den Leben angezeigt werden soll (In)
 * @param player der Spieler, dessen Leben angezeigt werden sollen (In)
 */
static void printLives(char *name, Player *player) 
{
	printf("%s ", name);

	if (player->lives > 0) 
	{
		for (int i = 0; i < player->lives; i++) 
		{
			printf("♥ ");
		}
	}

	printf("\n");
}

/**
 * Gibt die Leben der Spieler aus und prueft, ob 
 * einer der Spieler gewonnen hat.
 */
static void evaluatePlayerLives(void) 
{
	printLives("   Leben:", &g_human);
	printLives("KI-Leben:", &g_bot);
	printf("\n");
	fflush(stdout);

	if (g_human.lives <= 0) 
	{
		printf("Du hast leider verloren.\n");
		fflush(stdout);
		exit(0);
	} 
	else if (g_bot.lives <= 0) 
	{
		printf("Du hast gewonnen! Hurra!\n");
		fflush(stdout);
		exit(0);
	}
}

/**
 * Reagiert auf Kollisionen des Balls mit dem Rahmen.
 * 
 * @param side Rahmenseite, mit der kollidiert wurde (In)
 */
static void handleCollision(CGSide side)
{
	/* Der Spieler hat den Ball nicht abgefangen */
	if (side == sideLeft)
	{
		g_human.lives--;
		initRound(sideLeft);
	}

	/* Die KI hat den Ball nicht abgefangen */
	if (side == sideRight)
	{
		g_bot.lives--;
		initRound(sideRight);
	}

	/* Bewegung in Y-Richtung umkehren */
	if (side == sideTop || side == sideBottom)
	{
		g_ball.direction[1] *= -1;
	}
}

/**
 * Gibt dem Ball eine zufaellige Form.
 */
static void randomBallShape(void) 
{
	g_ball.shape = BALL_SHAPE_FIRST + rand() % (BALL_SHAPE_LAST + 1);
}

/**
 * Setzt die Werte eines Farb-Arrays.
 * 
 * @param color das Farb-Array, das gesetzt werden soll (Out)
 * @param r roter Farbanteil (In)
 * @param g gruener Farbanteil (In)
 * @param b blauer Farbanteil (In)
 */
static void setColor(CGColor3f color, GLfloat r, GLfloat g, GLfloat b) 
{
	color[0] = r;
	color[1] = g;
	color[2] = b;
}

/**
 * Gibt dem Ball eine zufaellige Farbe.
 */
static void randomBallColor(void) 
{
	int randColor = COLOR_FIRST + rand() % (COLOR_LAST + 1);
	switch (randColor) 
	{
		case COLOR_RED:
			setColor(g_ball.color, 1.0f, 0.0f, 0.0f);
			break;
		case COLOR_GREEN:
			setColor(g_ball.color, 0.0f, 1.0f, 0.0f);
			break;
		case COLOR_BLUE:
			setColor(g_ball.color, 0.0f, 0.0f, 1.0f);
			break;
		case COLOR_YELLOW:
			setColor(g_ball.color, 1.0f, 1.0f, 0.0f);
			break;
		case COLOR_MAGENTA:
			setColor(g_ball.color, 1.0f, 0.0f, 1.0f);
			break;
		case COLOR_CYAN:
			setColor(g_ball.color, 0.0f, 1.0f, 1.0f);
			break;
	}
}

/**
 * Reagiert auf eine Kollision des Balls mit einem Schlaeger.
 * Der Ball wird reflektiert, seine Fabre aendert sich und
 * ggf. wird seine Form angepasst.
 * 
 * @param player der Spieler, der den Ball getroffen hat (In)
 * @param intersection der Kollisionspunkt (In)
 * @param plaayerNormal die Normale des Schlaegers (In)
 */
static void handlePaddleCollision(Player *player, CGPoint2f intersection, CGVector2f playerNormal) 
{
	vector_set(g_guidelines.position, intersection);
	vector_set(g_guidelines.normal, playerNormal);

	// Einfallsvektor
	vector_set(g_guidelines.input, g_ball.direction);
	vector_mul(g_guidelines.input, -1); // Richtung des Balls wird hier gedreht

	vector_mul(g_guidelines.input, g_ball.speed);

	GLfloat normScale = vector_dot(g_guidelines.normal, g_guidelines.input);
	vector_mul(g_guidelines.normal, normScale);

	// Vektor zur Normalen
	vector_set(g_guidelines.mirrorNorm, g_guidelines.normal);
	vector_sub(g_guidelines.mirrorNorm, g_guidelines.input);

	// Ausfallsvektor durch Addition des Spiegelvektors
	vector_set(g_ball.direction, g_guidelines.normal);
	vector_add(g_ball.direction, g_guidelines.mirrorNorm);
	vector_norm(g_ball.direction);

	vector_set(g_guidelines.output, g_ball.direction);
	vector_mul(g_guidelines.output, g_ball.speed);

	// Verdoppeln fuer die Guidelines (Spiegelung)
	vector_mul(g_guidelines.mirrorNorm, 2.0f); 

	// Guidelines nur beim Spieler anzeigen, wenn ueberhaupt aktiv
	g_guidelines.enabled = player->isHuman && g_paddleGuideDrawing;

	// Bei jedem Treffer beschleunigen
	g_ball.speed += BALL_ACCELERATION;

	randomBallColor();
	
	if (g_randomShape) 
	{
		randomBallShape();
	}
}

/**
 * Berechnet, ob es eine Kollision des Balls mit dem Schlaeger eines Spielers gab.
 * Wenn ja, wird darauf direkt reagiert.
 * 
 * @param player der Spieler, fuer den die Berechnung durchgefuehrt werden soll (In)
 */
static void calcPaddleCollision(Player *player) 
{
	// Normale des Schlaegers berechnen (Vektor)
	CGVector2f playerNormal = VECTOR_ZERO;
	vector_circle(playerNormal, TO_RADIANS(player->paddleRotation));
	if (!player->isHuman) 
	{
		vector_mul(playerNormal, -1.0f);
	}

	// aktive Kante des Schlaegers berechnen (Vektor)
	CGVector2f playerRotation = VECTOR_ZERO;
	vector_circle(playerRotation, TO_RADIANS(player->paddleRotation + 90));

	// Mitte der aktiven Kante berechnen (Punkt)
	CGPoint2f playerPaddle = POINT_ZERO;
	vector_add(playerPaddle, playerNormal);
	vector_mul(playerPaddle, PADDLE_THICKNESS / 2);
	vector_add(playerPaddle, player->paddlePosition);

	// Schnittpunkt Schlaegerkante mit Schlaegernormale vom Ball aus (jeweils unendlich).
	CGPoint2f intersection = POINT_ZERO;
	vector_intersect(intersection, playerPaddle, playerRotation, g_ball.position, playerNormal);

	// Abstand Ball - Schlaegerkante
	GLfloat distBall = point_distance(g_ball.position, intersection);

	// Abstand Schnittpunkt - Schlaegerkantenmitte
	GLfloat distIntersection = point_distance(playerPaddle, intersection);

	// Kollisionspruefung
	if (distBall <= BALL_SIZE && distIntersection <= PADDLE_HEIGHT / 2) 
	{
		handlePaddleCollision(player, intersection, playerNormal);
	}
}

/**
 * Spiel-Update fuer den menschlichen Schlaeger.
 * Die Bewegung des Schlaegers wird fuer den angegebenen Zeitabschnitt berechnet.
 * 
 * @param interval die vergangene Zeit seit dem letzten Frame (In)
 */
static void calcHumanPaddle(double interval)
{
	if (g_movement[dirUp])
	{
		g_human.paddlePosition[1] += PADDLE_MAX_SPEED * (float)interval;
		if (g_human.paddlePosition[1] > PADDLE_MOVEMENT_LIMIT)
		{
			g_human.paddlePosition[1] = PADDLE_MOVEMENT_LIMIT;
		}
	}

	if (g_movement[dirDown])
	{
		g_human.paddlePosition[1] -= PADDLE_MAX_SPEED * (float)interval;
		if (g_human.paddlePosition[1] < -PADDLE_MOVEMENT_LIMIT)
		{
			g_human.paddlePosition[1] = -PADDLE_MOVEMENT_LIMIT;
		}
	}

	if (g_rotation[rotClockwise])
	{
		g_human.paddleRotation -= PADDLE_ROTATION_SPEED * (float)interval;
		if (g_human.paddleRotation < -PADDLE_MAX_ANGLE)
		{
			g_human.paddleRotation = -PADDLE_MAX_ANGLE;
		}
	}

	if (g_rotation[rotCounterClockwise])
	{
		g_human.paddleRotation += PADDLE_ROTATION_SPEED * (float)interval;
		if (g_human.paddleRotation > PADDLE_MAX_ANGLE)
		{
			g_human.paddleRotation = PADDLE_MAX_ANGLE;
		}
	}
}

/**
 * Spiel-Update fuer den KI Schlaeger.
 * Die Bewegung des Schlaegers wird fuer den angegebenen Zeitabschnitt berechnet.
 * 
 * @param interval die vergangene Zeit seit dem letzten Frame (In)
 */
static void calcBotPaddle(double interval) 
{
	GLfloat botDelta = g_ball.position[1] - g_bot.paddlePosition[1];
	if (fabsf(botDelta) > PADDLE_MAX_SPEED * (float)interval)
	{
		g_bot.paddlePosition[1] += (botDelta > 0 ? 1 : -1) * PADDLE_MAX_SPEED * (float) interval;
	} else {
		g_bot.paddlePosition[1] = g_ball.position[1];
	}

	// Hoehe beschraenken
	if (g_bot.paddlePosition[1] > PADDLE_MOVEMENT_LIMIT)
	{
		g_bot.paddlePosition[1] = PADDLE_MOVEMENT_LIMIT;
	}
	if (g_bot.paddlePosition[1] < -PADDLE_MOVEMENT_LIMIT)
	{
		g_bot.paddlePosition[1] = -PADDLE_MOVEMENT_LIMIT;
	}
}

/* ---- Oeffentliche Funktionen ---- */

void calcBall(double interval)
{
	CGSide side = sideNone;
	side = checkBorderCollision();

	if (side != sideNone)
	{
		handleCollision(side);
	}

	if (g_ball.direction[0] < 0.0f) 
	{
		calcPaddleCollision(getHumanPlayer());
	} 
	else 
	{
		calcPaddleCollision(getBotPlayer());
	}

	g_ball.position[0] += g_ball.direction[0] * g_ball.speed * (float)interval;
	g_ball.position[1] += g_ball.direction[1] * g_ball.speed * (float)interval;

	g_ball.rotation += BALL_ROTATION_SPEED * (float)interval;
}

void calcPaddles(double interval)
{
	calcHumanPaddle(interval);
	calcBotPaddle(interval);
}

void setPaddleMovement(PaddleDirection direction, GLboolean status)
{
	if (direction <= dirDown)
	{
		g_movement[direction] = status;
	}
}

void setPaddleRotation(PaddleRotation rotation, GLboolean status)
{
	if (rotation <= rotCounterClockwise)
	{
		g_rotation[rotation] = status;
	}
}

void toggleGuidelines(void) 
{
	g_paddleGuideDrawing = !g_paddleGuideDrawing;
}

void toggleRandomShape(void) 
{
	g_randomShape = !g_randomShape;

	if (g_randomShape) 
	{
		randomBallShape();
	} 
	else 
	{
		g_ball.shape = BALL_SHAPE_CIRCLE;
	}
}

Ball *getBall(void)
{
	return &g_ball;
}

Player *getHumanPlayer(void)
{
	return &g_human;
}

Player *getBotPlayer(void)
{
	return &g_bot;
}

Guidelines *getGuidelines(void) 
{
	return &g_guidelines;
}

void initRound(CGSide startPos)
{
	evaluatePlayerLives();

	randomBallColor();
	
	g_ball.speed = BALL_START_SPEED;

	GLfloat angle = (rand() % (BALL_START_MAX_ANGLE - BALL_START_MIN_ANGLE)) + BALL_START_MIN_ANGLE;
	vector_circle(g_ball.direction, TO_RADIANS(angle));

	if (startPos == sideLeft) 
	{
		g_ball.position[0] = -BALL_START_POS_X;
	} 
	else 
	{
		g_ball.position[0] = BALL_START_POS_X;
		vector_mul(g_ball.direction, -1.0f);
	}
	g_ball.position[1] = 0.0f;
}
