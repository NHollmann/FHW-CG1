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
#include "texture.h"
#include "logic.h"
#include "scene.h"
#include "hud.h"
#include "debugGL.h"

/* ---- Konstanten ---- */

/** Anzahl der Aufrufe der Timer-Funktion pro Sekunde */
#define TIMER_CALLS_PS 60

/** Anzahl der Aufrufe der Logik-Funktion pro Sekunde */
#define LOGIC_CALLS_PS 80

/* ---- Interne Funktionen ---- */

/**
 * Berechnung der Frames pro Sekunde.
 * 
 * @return aktuelle Framerate.
 */
static GLfloat frameRate(void)
{
	/* Zeitpunkt der letzten Neuberechnung */
	static int timebase = 0;
	/* Anzahl der Aufrufe seit letzter Neuberechnung */
	static int frameCount = 0;
	/* Zuletzt berechneter FPS-Wert */
	static float fps = 0.0;
	/* aktuelle "Zeit" */
	static int time = 0;

	/* Diesen Aufruf hinzuzaehlen */
	frameCount++;

	/* seit dem Start von GLUT vergangene Zeit ermitteln */
	time = glutGet (GLUT_ELAPSED_TIME);

	/* Eine Sekunde ist vergangen */
	if (time - timebase > 1000)
	{
		/* FPS-Wert neu berechnen */
		fps = frameCount * 1000.0f / (time - timebase);

		/* Zureuecksetzen */
		timebase = time;
		frameCount = 0;
	}

	/* Aktuellen FPS-Wert zurueckgeben */
	return fps;
}

/**
 * Verarbeitung der Picking-Ergebnisse.
 * Findet den Treffer, der dem Betrachter am naechsten liegt und gibt diesen
 * an die Logik weiter.
 * 
 * @param numHits Anzahl der getroffenen Objekte (In).
 * @param buffer Buffer, in dem die Treffer gespeichert sind (In).
 * @param leftButton true, wenn das Picking mit der linken Maustaste getaetigt wurde (In).
 */
static void processHits (GLint numHits, GLuint buffer[], GLboolean leftButton)
{
	/* Anzahl der Namen des Treffers, der am naechsten zum Betrachter ist */
	GLint numOfClosestNames = 0;
	/* Anzahl der Namen des Treffers, der am naechsten zum Betrachter ist */
	GLuint *ptrClosestNames = NULL;

	/* Laufvariable - Nummer des akt. verarbeiteten Treffers */
	GLint i;
	/* Laufvariable - Zeiger auf den Beginn des Hit-Records */
	GLuint *ptr = (GLuint *) buffer;

	/* DepthBuffer - Wert */
	GLuint minZ = 0xffffffff;

	if (numHits > 0)
	{
		/* Alle Treffer verarbeiten */
		for (i = 0; i < numHits; i++)
		{
			/* Ist der Treffer naeher dran, als der bisher naechste? */
			if (*(ptr + 1) < minZ)
			{
				/* Anzahl der Namen merken */
				numOfClosestNames = *ptr;
				/* Zeiger auf den ersten Namen merken */
				ptrClosestNames = ptr + 3;
				/* als neuen minimalen Tiefenwert merken */
				minZ = *(ptr + 1);
			}
			/* zum Beginn des naechsten Hit-Records springen */
			/* Schrittweite: Anzahl-Feld + minZ-Feld + maxZ-Feld + Namen-Felder */
			ptr += 3 + *ptr;
		}

		/* Alle Namen des Treffers an die Logik weiterreichen, der am naechsten zum Betrachter liegt */
		for (i = 0; i < numOfClosestNames; i++, ptrClosestNames++)
		{
			handleMousePick(*ptrClosestNames, leftButton);
		}
	}
}

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
		gluOrtho2D(-1, 1,				  /* left, right */
				-1 / aspect, 1 / aspect); /* bottom, top */
	}
	else
	{
		gluOrtho2D(-1 * aspect, 1 * aspect, /* left, right */
				-1, 1);					    /* bottom, top */
	}

	/* Folge Operationen beeinflussen die Modelviewmatrix */
	glMatrixMode(GL_MODELVIEW);

	/* Einheitsmatrix laden */
	glLoadIdentity();
}

/**
 * Mouse-Button-Callback.
 * 
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
 * 
 * @param x X-Position des Mauszeigers.
 * @param y Y-Position des Mauszeigers.
 */
static void cbMouseMotion (int x, int y)
{
	handleMouseEvent(x, y, mouseMotion, 0, 0);
}

/**
 * Mouse-Passive-Motion-Callback.
 * 
 * @param x X-Position des Mauszeigers.
 * @param y Y-Position des Mauszeigers.
 */
static void cbMousePassiveMotion (int x, int y)
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
 * Initiiert Neuzeichnen und setzt sich selbst erneut als Timer-Callback.
 * 
 * @param lastCallTime Zeitpunkt, zu dem die Funktion als Timer-Funktion
 *   registriert wurde (In)
 */
static void cbTimer(int lastCallTime)
{
	/* Seit dem Programmstart vergangene Zeit in Millisekunden */
	int thisCallTime = glutGet(GLUT_ELAPSED_TIME);

	/* Wieder als Timer-Funktion registrieren */
	glutTimerFunc(1000 / TIMER_CALLS_PS, cbTimer, thisCallTime);

	/* Neuzeichnen anstossen */
	glutPostRedisplay();
}

/**
 * Idle-Callback.
 * Initiiert Berechnung der aktuellen Position und Farben.
 * Dient als getrennte Update-Schleife.
 */
static void cbIdle()
{
	static int lastCallTime = 0;
	static double accumulator = 0.0;

	/* Seit dem Programmstart vergangene Zeit in Millisekunden */
	int thisCallTime = glutGet(GLUT_ELAPSED_TIME);

	/* Seit dem letzten Funktionsaufruf vergangene Zeit in Sekunden */
	double interval = (double)(thisCallTime - lastCallTime) / 1000.0f;

	accumulator += interval;
	
	while (accumulator >= 1.0 / LOGIC_CALLS_PS)
	{
		
		if (!isPaused())
		{
			/* Spiellogik updaten */
			updateLogic(1.0 / LOGIC_CALLS_PS);
		}

		accumulator -= 1.0 / LOGIC_CALLS_PS;
	}

	lastCallTime = thisCallTime;
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
		drawScene(EYE_CENTER);
	}
	else
	{
		glColorMask(GL_TRUE, GL_FALSE, GL_FALSE, GL_TRUE);
		drawScene(EYE_LEFT);

		glClear(GL_DEPTH_BUFFER_BIT);
		glColorMask(GL_FALSE, getAnaglyphMode() == ANAGLYPH_COLOR, GL_TRUE, GL_TRUE);
		drawScene(EYE_RIGHT);

		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	}

	/* HUD */
	set2DViewport(0, 0, width, height);
	drawHUD();

	/* Objekte anzeigen */
	glutSwapBuffers();

	/* Framerate bestimmen */
	getGamestate()->fps = frameRate();
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
	
	/* Idle-Callback fuer die Update-Schleife */
	glutIdleFunc(cbIdle);

	/* Display-Callback - wird an mehreren Stellen imlizit (z.B. im Anschluss an
	 * Reshape-Callback) oder explizit (durch glutPostRedisplay) angestossen */
	glutDisplayFunc(cbDisplay);
}

/* ---- Oeffentliche Funktionen ---- */

void pick(int x, int y, GLboolean leftButton)
{
	/** Groesse des Buffers fuer Picking Ergebnisse */
	#define SELECTBUFSIZE 512

	/* Fensterdimensionen auslesen */
	int width = glutGet(GLUT_WINDOW_WIDTH);
	int height = glutGet(GLUT_WINDOW_HEIGHT);

	/* Buffer zuruecksetzen */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/* 3D Ansicht */
	set3DViewport(0, 0, width, height);

	/* Viewport (Darstellungsbereich des Fensters) */
	GLint viewport[4] = {0, 0, width, height};

	/* Puffer fuer Picking-Ergebnis */
	GLuint buffer[SELECTBUFSIZE];

	/* Anzahl der getroffenen Namen beim Picking */
	GLint numHits;

	/* Puffer festlegen, Name-Stack leeren und Rendermode wechseln */
	glSelectBuffer(SELECTBUFSIZE, buffer);
	glInitNames();
	glRenderMode(GL_SELECT);

	/* Seitenverhaeltnis bestimmen */
	double aspect = (double)width / height;

	/* Folge Operationen beeinflussen die Projektionsmatrix */
	glMatrixMode(GL_PROJECTION);

	/* Einheitsmatrix laden */
	glLoadIdentity();

	/* Viewport-Position und -Ausdehnung bestimmen */
	glViewport(x, y, width, height);

	gluPickMatrix(x, height - y, 10, 10, viewport);

	/* Perspektivische Darstellung */
	gluPerspective(70, 		/* Oeffnungswinkel */
				   aspect,  /* Seitenverhaeltnis */
				   0.05,	/* nahe Clipping-Ebene */
				   100);	/* ferne Clipping-Ebene */

	/* Folge Operationen beeinflussen die Modelviewmatrix */
	glMatrixMode(GL_MODELVIEW);

	/* Einheitsmatrix laden */
	glLoadIdentity();

	drawPickingScene();

	/* Zeichnen beenden und auswerten */
	glFlush();

	numHits = glRenderMode(GL_RENDER);

	processHits(numHits, buffer, leftButton);
}

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
			INFO (("Lade und initialisiere Texturen...\n"));

			if (initTextures())
			{
				INFO (("...fertig.\n\n"));
				INFO(("Initialisiere Logik...\n"));

				initLogic();

				INFO(("...fertig.\n\n"));
				INFO(("Registriere Callbacks...\n"));

				registerCallbacks();

				INFO(("...fertig.\n\n"));
				INFO(("Trete in Schleife der Ereignisbehandlung ein...\n"));

				glutMainLoop();
			}
			else
			{
				INFO (("...fehlgeschlagen.\n\n"));

				glutDestroyWindow (windowID);
				windowID = 0;
			}
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
