/**
 * @file
 * Darstellungs-Modul.
 * Das Modul kapselt die Rendering-Funktionalitaet (insbesondere der OpenGL-
 * Aufrufe) des Programms.
 *
 * Bestandteil einer Uebung im Rahmen des Moduls Praktikum Grundlagen der Computergrafik 
 * an der FH Wedel.
 *
 * @author Nicolas Hollmann, Daniel Klintworth
 */

/* ---- System Header einbinden ---- */
#ifdef WIN32
#include <windows.h>
#endif

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

/* ---- Eigene Header einbinden ---- */
#include "scene.h"
#include "renderObjects.h"
#include "logic.h"
#include "types.h"
#include "stringOutput.h"
#include "debugGL.h"

/* ---- Konstanten ---- */

/* Die Groesse einer einzelnen Kachel */
#define TILE_SIZE (2.0f / LEVELSIZE)

/* Die Farbe Weiss */
#define COLOR_WHITE { 1.0f, 1.0f, 1.0f }

/* Die Farbe Rot */
#define COLOR_RED   { 1.0f, 0.0f, 0.0f }

/* Die Farbe Gruen */
#define COLOR_GREEN { 0.0f, 1.0f, 0.0f }

/* Die Nachrichten, die bei Game Over angezeigt werden. */
const char * lose_messages[LOSE_MESSAGE_COUNT][3] = {
	{ 
		"The developers regret to inform you", 
		"that this level is impossible.", 
		"Make no attempt to solve it." 
	},
	{
		"The developers apologize for this",
		"clearly broken level.", "" 
	},
	{ 
		"Frankly, this level was a mistake.",
		"If we were you, we would quit now.", ""
	},
	{ 
		"No one will blame you for giving up.",
		"In fact, quitting at this point is a",
		"perfectly reasonable response."
	},
	{ 
		"What are you doing?", 
		"You're not even going the right way.", "" 
	},
};

/* ---- Interne Funktionen ---- */

/**
 * Zeichnet das Level. Die zu zeichnenden Objekte werden hierzu in Form 
 * einer Displayliste aus renderObjects bezogen.
 * 
 * @param gamestage der Spielzustand (In)
 */
static void drawLevel(Gamestate *gamestate)
{
	glPushMatrix();
	{
		/* Skalierung und Positionierung des Spielfelds. */
		glTranslatef(0.0f, 0.1f, 0.0f);
		glScalef(0.9f, 0.9f, 1.0f);
		for (int y = 0; y < LEVELSIZE; y++) 
		{
			for (int x = 0; x < LEVELSIZE; x++) 
			{
				glPushMatrix();
				{
					/* Kachel an Position x,y zeichnen. */
					float xOffset = -1.0 + ((float) x * (float) TILE_SIZE);
					float yOffset =  1.0 - ((float) y * (float) TILE_SIZE);
					glTranslatef(xOffset + TILE_SIZE / 2.0f, yOffset - TILE_SIZE / 2.0f, 0.0f);
					glScalef(TILE_SIZE - 0.01f, TILE_SIZE - 0.01f, 1.0f);

					wmFieldType field = gamestate->level[y][x];
					switch (field) {
						case WM_FREE:
							renderObject(RO_FREE);
							break;
						case WM_BLACK:
							// Nichts anzeigen
							break;
						case WM_WALL:
							renderObject(RO_WALL);
							break;
						case WM_GOAL:
							renderObject(RO_GOAL);
							break;
						case WM_NEWWATER: // Sollte nie gerendert werden
						case WM_WATER:
							renderObject(RO_WATER);
							break;
						case WM_SAND:
							renderObject(RO_SANDBAG);
							break;
						case WM_SUGAR:
							renderObject(RO_SUGAR);
							break;
						case WM_TOUCHED_SUGAR:
							/* Bei nassem Zucker muss zusaetzlich zum Rendern die verbleibende Lebenszeit gespeichert werden. */
							{								
								renderObject(RO_FREE);
								float sugarSize = sugar_get_lifetime(&gamestate->sugarCubes, x, y) / SUGAR_LIFETIME;
								glPushMatrix();
								{
									glScalef(sugarSize, sugarSize, 1.0f);
									renderObject(RO_SUGAR);
								}
								glPopMatrix();
							}
							break;
						case WM_START:
							INFO(("WM_START in rendering is not allowed!"));
							break;
					}

					if (x == gamestate->playerX && y == gamestate->playerY) {
						renderObject(RO_PLAYER);
					}
				}
				glPopMatrix();
			}
		}
	}
	glPopMatrix();
}

/**
 * Zeichnet die Levelnummer.
 * 
 * @param gamestage der Spielzustand (In)
 */
static void drawLevelNumber(Gamestate *gamestate)
{
	GLfloat textColor[3] = COLOR_WHITE;
	drawString(0.4, 0.95, textColor, "Level %d", gamestate->levelId);
}

/**
 * Zeichnet ein Overlay ueber dem Spielfeld.
 */
static void drawOverlay(void)
{
	glPushMatrix();
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glScalef(2.0f, 2.0f, 1.0f);
		glColor4f(0.0f, 0.0f, 0.0f, 0.85f);
		drawSquare();

		glDisable(GL_BLEND);
	}
	glPopMatrix();
}

/**
 * Zeichnet die Benachrichtigung bei Verlieren des Spiels.
 */
static void drawStageLost(Gamestate *gamestate)
{
	drawOverlay();

	GLfloat textColor[3] = COLOR_RED;

	drawString(0.4f , 0.2f , textColor, "You failed!");
	drawString(0.15f, 0.4f , textColor, lose_messages[gamestate->loseMessageId][0]);
	drawString(0.15f, 0.45f, textColor, lose_messages[gamestate->loseMessageId][1]);
	drawString(0.15f, 0.5f , textColor, lose_messages[gamestate->loseMessageId][2]);
}

/**
 * Zeichnet die Benachrichtigung bei Gewinnen des Spiels.
 */
static void drawStageWon(void)
{
	drawOverlay();

	GLfloat textColor[3] = COLOR_GREEN;
	drawString(0.25f , 0.2f , textColor, "You won! Enjoy your cake!");

	renderObject(RO_CAKE);
}

/**
 * Zeichnet die Hilfenachricht.
 */
static void drawHelp(void)
{
	drawOverlay();

	GLfloat textColor[3] = COLOR_WHITE;

	float height = 0.1f;

	#define DRAW_HELP(text) drawString(0.15f, height += 0.05f, textColor, (text))

	DRAW_HELP("Wasser marsch!");
	DRAW_HELP("==============");
	DRAW_HELP("h/H         - Diese Hilfe anzeigen /");
	DRAW_HELP("              ausblenden");
	DRAW_HELP("Pfeiltasten - Spieler bewegen");
	DRAW_HELP("Enter       - Naechstes Level /");
	DRAW_HELP("              Neu starten");
	DRAW_HELP("1/2/3/4     - Level waehlen");
	DRAW_HELP("F1          - Wireframe an/aus");
	DRAW_HELP("F2          - Fullscreen an/aus");
	DRAW_HELP("F3          - Pausieren");
	DRAW_HELP("q/Q/ESC     - Beenden");

	#undef DRAW_HELP
}

/* ---- Oeffentliche Funktionen ---- */

void drawScene(void)
{
	Gamestate *gamestate = getGamestate();

	drawLevel(gamestate);
	drawLevelNumber(gamestate);

	switch (gamestate->stage) {
		case stageRunning:
			// Nichts besonderes anzeigen
			break;
		case stageWon:
			drawStageWon();
			break;
		case stageLost:
			drawStageLost(gamestate);
			break;
	}
	
	if (gamestate->showHelp)
	{
		drawHelp();
	}

	/* DEBUG-Ausgabe */
	GLSTATE;
}

void toggleWireframeMode()
{
	static GLboolean wireframe = GL_FALSE;

	wireframe = !wireframe;

	if (wireframe)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}

int initScene(void)
{
	/* Setzen der Farbattribute */
	/* Hintergrundfarbe */
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	/* Zeichenfarbe */
	glColor3f(1.0f, 1.0f, 1.0f);

	/* Backface Culling aktivieren */
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	/* Linienbreite */
	glLineWidth(1.f);

	initDisplayList();

	/* Alles in Ordnung? */
	return (GLGETERROR == GL_NO_ERROR);
}
