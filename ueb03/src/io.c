/**
 * @file
 * Ein-/Ausgabe-Modul.
 * Das Modul kapselt die Ein- und Ausgabe-Funktionalitaet (insbesondere die GLUT-
 * Callbacks) des Programms.
 *
 * Bestandteil einer Uebung im Rahmen des Moduls Praktikum Grundlagen der Computergrafik 
 * an der FH Wedel.
 *
 * @author Nicolas Hollmann, Daniel Klintworth
 */

/* ---- System Header einbinden ---- */
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

/* ---- Eigene Header einbinden ---- */
#include "io.h"
#include "types.h"
#include "input.h"
#include "logic.h"
#include "scene.h"
#include "hud.h"
#include "debugGL.h"

/* ---- Konstanten ---- */

/** Anzahl der Aufrufe der Timer-Funktion pro Sekunde */
#define TIMER_CALLS_PS 60

/* ---- Interne Funktionen ---- */

/**
 * Setzt einen Viewport fuer 3-dimensionale Darstellung
 * mit perspektivischer Projektion und legt eine Kamera fest.
 * 
 * @param x, y Position des Viewports im Fenster - (0, 0) ist die untere linke Ecke
 * @param width, height Breite und Hoehe des Viewports
 */
static void set3DViewport(GLint x, GLint y, GLint width, GLint height)
{
	/* Seitenverhaeltnis bestimmen */
	double aspect = (double)width / height;

	/* Folge Operationen beeinflussen die Projektionsmatrix */
	glMatrixMode(GL_PROJECTION);

	/* Einheitsmatrix laden */
	glLoadIdentity();

	/* Viewport-Position und -Ausdehnung bestimmen */
	glViewport(x, y, width, height);

	/* Perspektivische Darstellung */
	gluPerspective(70, 		/* Oeffnungswinkel */
				   aspect,  /* Seitenverhaeltnis */
				   0.05,	/* nahe Clipping-Ebene */
				   100);	/* ferne Clipping-Ebene */

	/* Folge Operationen beeinflussen die Modelviewmatrix */
	glMatrixMode(GL_MODELVIEW);

	/* Einheitsmatrix laden */
	glLoadIdentity();
}

/**
 * Setzt einen Viewport fuer 2-dimensionale Darstellung.
 * 
 * @param x, y Position des Viewports im Fenster - (0, 0) ist die untere linke Ecke
 * @param width, height Breite und Hoehe des Viewports
 */
static void set2DViewport(GLint x, GLint y, GLint width, GLint height)
{
	/* Seitenverhaeltnis bestimmen */
	double aspect = (double)width / height;

	/* Folge Operationen beeinflussen die Projektionsmatrix */
	glMatrixMode(GL_PROJECTION);

	/* Einheitsmatrix laden */
	glLoadIdentity();

	/* Viewport-Position und -Ausdehnung bestimmen */
	glViewport(x, y, width, height);

	/* Das Koordinatensystem bleibt immer quadratisch */
	if (aspect <= 1)
	{
		gluOrtho2D(-1, 1,					 /* left, right */
				-1 / aspect, 1 / aspect); /* bottom, top */
	}
	else
	{
		gluOrtho2D(-1 * aspect, 1 * aspect, /* left, right */
				-1, 1);					/* bottom, top */
	}

	/* Folge Operationen beeinflussen die Modelviewmatrix */
	glMatrixMode(GL_MODELVIEW);

	/* Einheitsmatrix laden */
	glLoadIdentity();
}

/**
 * Mouse-Button-Callback.
 * @param button Taste, die den Callback ausgeloest hat.
 * @param state Status der Taste, die den Callback ausgeloest hat.
 * @param x X-Position des Mauszeigers beim Ausloesen des Callbacks.
 * @param y Y-Position des Mauszeigers beim Ausloesen des Callbacks.
 */
static void cbMouseButton (int button, int state, int x, int y)
{
	handleMouseEvent(x, y, mouseButton, button, state);
}

/**
 * Mouse-Motion-Callback.
 * @param x X-Position des Mauszeigers.
 * @param y Y-Position des Mauszeigers.
 */
static void cbMouseMotion (int x, int y)
{
	handleMouseEvent(x, y, mouseMotion, 0, 0);
}

/**
 * Mouse-Passive-Motion-Callback.
 * @param x X-Position des Mauszeigers.
 * @param y Y-Position des Mauszeigers.
 */
static void
cbMousePassiveMotion (int x, int y)
{
	handleMouseEvent(x, y, mousePassiveMotion, 0, 0);
}

/**
 * Callback fuer Tastendruck.
 * Ruft Ereignisbehandlung fuer Tastaturereignis auf.
 * 
 * @param key betroffene Taste (In)
 * @param x x-Position der Maus zur Zeit des Tastendrucks (In)
 * @param y y-Position der Maus zur Zeit des Tastendrucks (In)
 */
static void cbKeyboard(unsigned char key, int x, int y)
{
	handleKeyboardEvent(key, GLUT_DOWN, GL_FALSE, x, y);
}

/**
 * Callback fuer Tastenloslassen.
 * Ruft Ereignisbehandlung fuer Tastaturereignis auf.
 * 
 * @param key betroffene Taste (In)
 * @param x x-Position der Maus zur Zeit des Loslassens (In)
 * @param y y-Position der Maus zur Zeit des Loslassens (In)
 */
static void cbKeyboardUp(unsigned char key, int x, int y)
{
	handleKeyboardEvent(key, GLUT_UP, GL_FALSE, x, y);
}

/**
 * Callback fuer Druck auf Spezialtasten.
 * Ruft Ereignisbehandlung fuer Tastaturereignis auf.
 * 
 * @param key betroffene Taste (In)
 * @param x x-Position der Maus zur Zeit des Tastendrucks (In)
 * @param y y-Position der Maus zur Zeit des Tastendrucks (In)
 */
static void cbSpecial(int key, int x, int y)
{
	handleKeyboardEvent(key, GLUT_DOWN, GL_TRUE, x, y);
}

/**
 * Callback fuer Loslassen von Spezialtasten.
 * Ruft Ereignisbehandlung fuer Tastaturereignis auf.
 * 
 * @param key betroffene Taste (In)
 * @param x x-Position der Maus zur Zeit des Loslassens (In)
 * @param y y-Position der Maus zur Zeit des Loslassens (In)
 */
static void cbSpecialUp(int key, int x, int y)
{
	handleKeyboardEvent(key, GLUT_UP, GL_TRUE, x, y);
}

/**
 * Timer-Callback.
 * Initiiert Berechnung der aktuellen Position und Farben und anschliessendes
 * Neuzeichnen, setzt sich selbst erneut als Timer-Callback.
 * 
 * @param lastCallTime Zeitpunkt, zu dem die Funktion als Timer-Funktion
 *   registriert wurde (In)
 */
static void cbTimer(int lastCallTime)
{
	/* Seit dem Programmstart vergangene Zeit in Millisekunden */
	int thisCallTime = glutGet(GLUT_ELAPSED_TIME);

	/* Seit dem letzten Funktionsaufruf vergangene Zeit in Sekunden */
	double interval = (double)(thisCallTime - lastCallTime) / 1000.0f;

	if (isPaused())
	{
		interval = 0.0;
	}

	/* Spiellogik updaten */
	updateLogic(interval);

	/* Wieder als Timer-Funktion registrieren */
	glutTimerFunc(1000 / TIMER_CALLS_PS, cbTimer, thisCallTime);

	/* Neuzeichnen anstossen */
	glutPostRedisplay();
}

/**
 * Zeichen-Callback.
 * Loescht die Buffer, ruft das Zeichnen der Szene auf und tauscht den Front-
 * und Backbuffer.
 */
static void cbDisplay(void)
{
	/* Fensterdimensionen auslesen */
	int width = glutGet(GLUT_WINDOW_WIDTH);
	int height = glutGet(GLUT_WINDOW_HEIGHT);

	/* Buffer zuruecksetzen */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/* 3D Ansicht */
	set3DViewport(0, 0, width, height);
	if (getAnaglyphMode() == ANAGLYPH_OFF)
	{
		drawScene3D(EYE_CENTER);
	}
	else
	{
		glColorMask(GL_TRUE, GL_FALSE, GL_FALSE, GL_TRUE);
		drawScene3D(EYE_LEFT);

		glClear(GL_DEPTH_BUFFER_BIT);
		glColorMask(GL_FALSE, getAnaglyphMode() == ANAGLYPH_COLOR, GL_TRUE, GL_TRUE);
		drawScene3D(EYE_RIGHT);

		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	}

	/* 2D Minimap */
	set2DViewport(width / 3 * 2, height / 3 * 2, width / 3, height / 3);
	drawScene2D();

	/* HUD */
	set2DViewport(0, 0, width, height);
	drawHUD();

	/* Objekt anzeigen */
	glutSwapBuffers();
}

/**
 * Registrierung der GLUT-Callback-Routinen.
 */
static void registerCallbacks(void)
{
	/* Mouse-Button-Callback (wird ausgefuehrt, wenn eine Maustaste
	 * gedrueckt oder losgelassen wird) */
	glutMouseFunc (cbMouseButton);

	/* Mouse-Motion-Callback (wird ausgefuehrt, wenn die Maus bewegt wird,
	 * waehrend eine Maustaste gedrueckt wird) */
	glutMotionFunc (cbMouseMotion);

	/* Mouse-Motion-Callback (wird ausgefuehrt, wenn die Maus bewegt wird,
	 * waehrend keine Maustaste gedrueckt wird) */
	glutPassiveMotionFunc (cbMousePassiveMotion);

	/* Tasten-Druck-Callback - wird ausgefuehrt, wenn eine Taste gedrueckt wird */
	glutKeyboardFunc(cbKeyboard);

	/* Tasten-Loslass-Callback - wird ausgefuehrt, wenn eine Taste losgelassen
	 * wird */
	glutKeyboardUpFunc(cbKeyboardUp);

	/* Spezialtasten-Druck-Callback - wird ausgefuehrt, wenn Spezialtaste
	 * (F1 - F12, Links, Rechts, Oben, Unten, Bild-Auf, Bild-Ab, Pos1, Ende oder
	 * Einfuegen) gedrueckt wird */
	glutSpecialFunc(cbSpecial);

	/* Spezialtasten-Loslass-Callback - wird ausgefuehrt, wenn eine Spezialtaste
	 * losgelassen wird */
	glutSpecialUpFunc(cbSpecialUp);

	/* Automat. Tastendruckwiederholung ignorieren */
	glutIgnoreKeyRepeat(1);

	/* Timer-Callback - wird einmalig nach msescs Millisekunden ausgefuehrt */
	glutTimerFunc(1000 / TIMER_CALLS_PS,	   /* msecs - bis Aufruf von func */
				  cbTimer,					   /* func  - wird aufgerufen    */
				  glutGet(GLUT_ELAPSED_TIME)); /* value - Parameter, mit dem
													func aufgerufen wird */

	/* Display-Callback - wird an mehreren Stellen imlizit (z.B. im Anschluss an
	 * Reshape-Callback) oder explizit (durch glutPostRedisplay) angestossen */
	glutDisplayFunc(cbDisplay);
}

/* ---- Oeffentliche Funktionen ---- */

/**
 * Initialisiert das Programm (inkl. I/O und OpenGL) und startet die
 * Ereignisbehandlung.
 * 
 * @param title Beschriftung des Fensters (In)
 * @param width Breite des Fensters (In)
 * @param height Hoehe des Fensters (In)
 * @return ID des erzeugten Fensters, 0 im Fehlerfall
 */
int initAndStartIO(char *title, int width, int height)
{
	int windowID = 0;

	/* Kommandozeile imitieren */
	int argc = 1;
	char *argv = "cmd";

	/* Glut initialisieren */
	glutInit(&argc, &argv);

	INFO(("Erzeuge Fenster...\n"));

	/* Initialisieren des Fensters */
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); /* fuer DoubleBuffering */
	glutInitWindowSize(width, height);

	glutInitWindowPosition(
		(glutGet(GLUT_SCREEN_WIDTH) - width) / 2,
		(glutGet(GLUT_SCREEN_HEIGHT) - height) / 2);

	/* Fenster erzeugen */
	windowID = glutCreateWindow(title);

	if (windowID)
	{

		INFO(("...fertig.\n\n"));
		INFO(("Initialisiere Zufallsgenerator...\n"));

		srand(time(0));

		INFO(("...fertig.\n\n"));
		INFO(("Initialisiere Szene...\n"));

		if (initScene())
		{
			INFO(("...fertig.\n\n"));
			INFO(("Initialisiere Logik...\n"));

			initLevel(LEVEL_1);

			INFO(("...fertig.\n\n"));
			INFO(("Registriere Callbacks...\n"));

			registerCallbacks();

			INFO(("...fertig.\n\n"));
			INFO(("Trete in Schleife der Ereignisbehandlung ein...\n"));

			glutMainLoop();
		}
		else
		{
			INFO(("...fehlgeschlagen.\n\n"));

			glutDestroyWindow(windowID);
			windowID = 0;
		}
	}
	else
	{
		INFO(("...fehlgeschlagen.\n\n"));
	}

	return windowID;
}
