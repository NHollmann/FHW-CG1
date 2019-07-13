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
#define SCROLL_SPEED (0.6f)

/* Der Polar Faktor */
#define POLAR_FACTOR (-0.25f)

/*  Der Azimuth Faktor */
#define AZIMUTH_FACTOR (0.25f)

/* ---- Globale Daten ---- */

/** Speichert, ob das Spiel aktuell pausiert ist */
static GLboolean g_paused = GL_FALSE;

/** Speichert, ob ein 3D Anaglyph Modus aktiviert ist */
static AnaglyphState g_anaglyphState = ANAGLYPH_OFF;

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
	/* aktueller Status der linken Maustaste */
	static int leftMouseButtonState = GLUT_UP;
	static int middleMouseButtonState = GLUT_UP;
	static int rightMouseButtonState = GLUT_UP;

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
		/* Wasser anheben */
		case GLUT_LEFT_BUTTON:
			leftMouseButtonState = buttonState;
			if (buttonState == GLUT_UP)
			{
				pick(x, y, GL_TRUE);
			}
			break;

		/* Wasser absenken */
		case GLUT_RIGHT_BUTTON:
			rightMouseButtonState = buttonState;
			if (buttonState == GLUT_UP)
			{
				pick(x, y, GL_FALSE);
			}
			break;

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
		if (leftMouseButtonState == GLUT_DOWN)
		{
			pick(x, y, GL_TRUE);
		}

		if (rightMouseButtonState == GLUT_DOWN)
		{
			pick(x, y, GL_FALSE);
		}
		
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
			/* Anaglyph 3D umschalten */
			case GLUT_KEY_F4:
				g_anaglyphState = (g_anaglyphState + 1) % ANAGLYPH_MODE_COUNT;
				break;
			/* Normalen anzeigen an/aus */
			case GLUT_KEY_F5:
				toggleNormal();
				break;
			/* Beleuchtung an/aus */
			case GLUT_KEY_F6:
				toggleLighting();
				break;
			/* Sonnenlicht an/aus */
			case GLUT_KEY_F7:
				toggleSunlight();
				break;
			/* Punktlicht an/aus */
			case GLUT_KEY_F8:
				togglePointlight();
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
				toggleHelp();
				break;
			/* Kugeln an/aus */
			case 's':
			case 'S':
				toggleSpheres();
				break;
			/* Texturen an/aus */
			case 't':
			case 'T':
				toggleTextures();
				break;
			/* Mehr Kugeln */
			case '+':
				changeWaterGrid(GL_TRUE);
				break;
			/* Weniger Kugeln */
			case '-':
				changeWaterGrid(GL_FALSE);
				break;
			/* Programm beenden */
			case 'q':
			case 'Q':
			case ESC:
				cleanup();
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

AnaglyphState getAnaglyphMode(void)
{
	return g_anaglyphState;
}
