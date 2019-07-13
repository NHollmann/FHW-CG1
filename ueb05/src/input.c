/**
 * @file
 * Modul zur Verarbeitung der Benutzereingaben.
 *
 * Bestandteil einer Uebung im Rahmen des Moduls Praktikum Grundlagen der Computergrafik 
 * an der FH Wedel.
 *
 * @author Nicolas Hollmann, Daniel Klintworth
 */

/* ---- System Header einbinden ---- */
#include <stdlib.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

/* ---- Eigene Header einbinden ---- */
#include "input.h"
#include "logic.h"
#include "scene.h"
#include "io.h"

/* ---- Konstanten ---- */

/* Die Geschwindigkeit, mit der durch Scrollen die Kamera gezoomt wird. */
#define SCROLL_SPEED (0.1f)

/* Der Polar Faktor */
#define POLAR_FACTOR (-0.25f)

/*  Der Azimuth Faktor */
#define AZIMUTH_FACTOR (0.25f)

/* ---- Globale Daten ---- */

/** Speichert, ob das Spiel aktuell pausiert ist */
static GLboolean g_paused = GL_FALSE;

/* ---- Interne Funktionen ---- */

/**
 * Umschalten zw. Vollbild- und Fenstermodus.
 * Beim Wechsel zum Fenstermodus wird vorherige Position und Groesse
 * wiederhergestellt. HINWEIS: Fenster wird nicht korrekt auf alte Position
 * gesetzt, da GLUT_WINDOW_WIDTH/HEIGHT verfaelschte Werte liefert.
 */
static void toggleFullscreen(void)
{
	/* Flag: Fullscreen: ja/nein */
	static GLboolean fullscreen = GL_FALSE;
	/* Zwischenspeicher: Fensterposition */
	static int windowPos[2];
	/* Zwischenspeicher: Fenstergroesse */
	static int windowSize[2];

	/* Modus wechseln */
	fullscreen = !fullscreen;

	if (fullscreen)
	{
		/* Fenstereinstellungen speichern */
		windowPos[0] = glutGet(GLUT_WINDOW_X);
		windowPos[1] = glutGet(GLUT_WINDOW_Y);
		windowSize[0] = glutGet(GLUT_WINDOW_WIDTH);
		windowSize[1] = glutGet(GLUT_WINDOW_HEIGHT);
		/* In den Fullscreen-Modus wechseln */
		glutFullScreen();
	}
	else
	{
		/* alte Fenstereinstellungen wiederherstellen */
		glutReshapeWindow(windowSize[0], windowSize[1]);

		/* HINWEIS:
			Auskommentiert, da es sonst Probleme mit der Vollbildarstellung bei
			Verwendung von FreeGlut gibt */
		glutPositionWindow(windowPos[0], windowPos[1]);
	}
}

/* ---- Oeffentliche Funktionen ---- */

void handleMouseEvent (int x, int y, CGMouseEventType eventType, 
							  int button, int buttonState)
{
	/* aktueller Status der mittleren Maustaste */
	static int middleMouseButtonState = GLUT_UP;

	/* Position der Maus beim letzten Aendern der Kamera */
	static int oldMousePos[2] = { 0, 0 };

	/* Veraenderung der Kameraausrichtung */
	float radius = 0.0f;
	float polar = 0.0f;
	float azimuth = 0.0f;
	
	switch (eventType)
	{
	case mouseButton:
		switch (button)
		{
		/* Kamera Bewegen */
		case GLUT_MIDDLE_BUTTON:
			middleMouseButtonState = buttonState;
			oldMousePos[0] = x;
			oldMousePos[1] = y;
			break;

		/* Kamera zoom rein */
		case 3: /* Hoch-Scrollen */
			if (buttonState == GLUT_UP)
			{
				radius -= SCROLL_SPEED;
			}
			break;

		/* Kamera zoom raus */
		case 4: /* Runter-Scrollen */
			if (buttonState == GLUT_UP)
			{
				radius += SCROLL_SPEED;
			}
			break;
		}
		break;
		
	case mouseMotion:
		if (middleMouseButtonState == GLUT_DOWN)
		{
			int deltaX = oldMousePos[0] - x;
			int deltaY = oldMousePos[1] - y;

			polar += ((float)deltaX) * POLAR_FACTOR;
			azimuth += ((float)deltaY) * AZIMUTH_FACTOR;

			oldMousePos[0] = x;
			oldMousePos[1] = y;
		}
		break;
	default:
		break;
	}
	
	changeCameraOrientation(radius, polar, azimuth);
}

void handleKeyboardEvent(int key, int status, GLboolean isSpecialKey, int x, int y)
{
/** Keycode der ESC-Taste */
#define ESC 27
/** Keycode der Enter-Taste */
#define ENTER 13

	/* Taste gedrueckt */
	if (status == GLUT_DOWN)
	{
		/* Spezialtaste gedrueckt */
		if (isSpecialKey)
		{
			switch (key)
			{
			/* Wireframe ein-/ausschalten */
			case GLUT_KEY_F1:
				toggleWireframeMode();
				break;
			/* Vollbildmodus ein-/ausschalten */
			case GLUT_KEY_F2:
				toggleFullscreen();
				break;
			/* Spiel pausieren/fortsetzen */
			case GLUT_KEY_F3:
				g_paused = !g_paused;
				break;
			}
		}
		/* normale Taste gedrueckt */
		else
		{
			switch (key)
			{
			/* Hilfe ausgeben */
			case 'h':
			case 'H':
				drawHelp();
				break;
			/* Sinus an/aus */
			case 's':
			case 'S':
				toggleSines();
				break;
			/* Texturen an/aus */
			case 't':
			case 'T':
				toggleTextures();
				break;
			/* Gourad / Phong */
			case 'l':
			case 'L':
				togglePhong();
				break;
			/* Heightmap an/aus */
			case 'm':
			case 'M':
				toggleHeightmap();
				break;
			/* Bumpmap an/aus */
			case 'b':
			case 'B':
				toggleBumpmap();
				break;
			/* Programm beenden */
			case 'q':
			case 'Q':
			case ESC:
				exit(0);
				break;
			}
		}
	}
}

GLboolean isPaused(void)
{
	return g_paused;
}
