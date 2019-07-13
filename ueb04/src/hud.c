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

/**
 * Zeichnet die FPS Anzeige.
 * 
 * @param gamestage der Spielzustand (In)
 */
static void drawFPS(Gamestate *gamestate)
{
	(void)gamestate;
	GLfloat textColor[3] = COLOR_WHITE;
	drawString(0.01, 0.05, textColor, "FPS: %.2f", gamestate->fps);
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
 * Zeichnet die Hilfenachricht.
 */
static void drawHelp(void)
{
	drawOverlay();

	GLfloat textColor[3] = COLOR_WHITE;

	float height = 0.05f;

	#define DRAW_HELP(text) drawString(0.15f, height += 0.04f, textColor, (text))

	DRAW_HELP("Wassersimulation");
	DRAW_HELP("================");
	DRAW_HELP("h/H         - Diese Hilfe anzeigen /");
	DRAW_HELP("              ausblenden");
	DRAW_HELP("Scrollen    - Zoomen");
	DRAW_HELP("MMB         - Kamera drehen");
	DRAW_HELP("F1          - Wireframe an/aus");
	DRAW_HELP("F2          - Fullscreen an/aus");
	DRAW_HELP("F3          - Pausieren");
	DRAW_HELP("F4          - Anaglyph grau/bunt/aus");
	DRAW_HELP("F5          - Normalen an/aus");
	DRAW_HELP("F6          - Beleuchtung an/aus");
	DRAW_HELP("F7          - Sonnenlicht an/aus");
	DRAW_HELP("F8          - Punktlicht an/aus");
	DRAW_HELP("S           - Kugeln an/aus");
	DRAW_HELP("T           - Texturen an/aus");
	DRAW_HELP("+           - Mehr Kugeln");
	DRAW_HELP("-           - Weniger Kugeln");
	DRAW_HELP("LMB         - Kugel hoch klicken");
	DRAW_HELP("RMB         - Kugel runter klicken");
	DRAW_HELP("q/Q/ESC     - Beenden");

	#undef DRAW_HELP
}


/* ---- Oeffentliche Funktionen ---- */

void drawHUD(void)
{
	Gamestate *gamestate = getGamestate();

    glDisable(GL_DEPTH_TEST);

	drawFPS(gamestate);
	
	if (gamestate->showHelp)
	{
		drawHelp();
	}

	/* DEBUG-Ausgabe */
	GLSTATE;
}
