/**
 * @file
 * Modul zur Verwaltung des HUD.
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
#include "hud.h"
#include "logic.h"
#include "renderObjects.h"
#include "types.h"
#include "stringOutput.h"
#include "debugGL.h"

/* ---- Konstanten ---- */

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

		glScalef(5.0f, 5.0f, 1.0f);
		glColor4f(0.0f, 0.0f, 0.0f, 0.85f);
		drawSquare(0);

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

	float height = 0.05f;

	#define DRAW_HELP(text) drawString(0.15f, height += 0.04f, textColor, (text))

	DRAW_HELP("Wasser marsch!");
	DRAW_HELP("==============");
	DRAW_HELP("h/H         - Diese Hilfe anzeigen /");
	DRAW_HELP("              ausblenden");
	DRAW_HELP("Scrollen    - Zoomen");
	DRAW_HELP("LMB + Beweg.- Kamera drehen");
	DRAW_HELP("Pfeiltasten - Spieler bewegen");
	DRAW_HELP("Enter       - Naechstes Level /");
	DRAW_HELP("              Neu starten");
	DRAW_HELP("1/2/3/4     - Level waehlen");
	DRAW_HELP("F1          - Wireframe an/aus");
	DRAW_HELP("F2          - Fullscreen an/aus");
	DRAW_HELP("F3          - Pausieren");
	DRAW_HELP("F4          - Anaglyph grau/bunt/aus");
	DRAW_HELP("F5          - Normalen an/aus");
	DRAW_HELP("F6          - Beleuchtung an/aus");
	DRAW_HELP("F7          - Weltlicht an/aus");
	DRAW_HELP("F8          - Taschenlampe an/aus");
	DRAW_HELP("a/A         - Animation");
	DRAW_HELP("c/C         - Kamera");
	DRAW_HELP("q/Q/ESC     - Beenden");

	#undef DRAW_HELP
}

/* ---- Oeffentliche Funktionen ---- */

void drawHUD(void)
{
	Gamestate *gamestate = getGamestate();

    glDisable(GL_DEPTH_TEST);

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
