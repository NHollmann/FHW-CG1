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

/* Stellt moderne OpenGL-Funktionalität bereit. */
#include <GL/glew.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#define WINDOW_PROFILE (GLUT_3_2_CORE_PROFILE)
#else
/* Anstelle der "glut.h" wird die "freeglut.h" eingebunden, da nur diese
 * das explizite Einstellen einer OpenGL-Version erlaubt. */
#include <GL/freeglut.h>
#define WINDOW_PROFILE (0)
#endif

/* ---- Eigene Header einbinden ---- */
#include "io.h"
#include "types.h"
#include "input.h"
#include "texture.h"
#include "logic.h"
#include "scene.h"
#include "debugGL.h"

/* ---- Konstanten ---- */

/** Anzahl der Aufrufe der Timer-Funktion pro Sekunde */
#define TIMER_CALLS_PS 60


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

	/* Seitenverhaeltnis bestimmen */
	double aspect = (double)width / height;

	/* Zeit abfragen */
	float time = (float) glutGet(GLUT_ELAPSED_TIME) / 1000;

	/* Buffer zuruecksetzen */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	drawScene(aspect, time);
	
	/* Objekte anzeigen */
	glutSwapBuffers();

	/* Framerate anzeigen */
	char fpsBuffer[100];
	snprintf(fpsBuffer, 100, "Aufgabe 5: Shader | Daniel & Nico | %.2f FPS", frameRate());
	glutSetWindowTitle(fpsBuffer);
}

/**
 * Registrierung der GLUT-Callback-Routinen.
 */
static void registerCallbacks(void)
{
	/* Mouse-Button-Callback (wird ausgefuehrt, wenn eine Maustaste
	 * gedrueckt oder losgelassen wird) */
	glutMouseFunc(cbMouseButton);

	/* Mouse-Motion-Callback (wird ausgefuehrt, wenn die Maus bewegt wird,
	 * waehrend eine Maustaste gedrueckt wird) */
	glutMotionFunc(cbMouseMotion);

	/* Mouse-Motion-Callback (wird ausgefuehrt, wenn die Maus bewegt wird,
	 * waehrend keine Maustaste gedrueckt wird) */
	glutPassiveMotionFunc(cbMousePassiveMotion);

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

void drawHelp(void)
{
	#define OUT(text) printf("%s\n", (text))

	OUT("Shader");
	OUT("======");
	OUT("h/H         - Diese Hilfe anzeigen");
	OUT("Scrollen    - Zoomen");
	OUT("MMB         - Kamera drehen");
	OUT("F1          - Wireframe an/aus");
	OUT("F2          - Fullscreen an/aus");
	OUT("F3          - Pausieren");
	OUT("s/S         - Sinus an/aus");
	OUT("t/T         - Texturen an/aus");
	OUT("l/L         - Phong / Gouraud");
	OUT("m/M         - Heightmap an/aus");
	OUT("b/B         - Bumpmap an/aus");
	OUT("q/Q/ESC     - Beenden");

	#undef OUT
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
	glutInitDisplayMode(WINDOW_PROFILE | GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(width, height);

	glutInitWindowPosition(
		(glutGet(GLUT_SCREEN_WIDTH) - width) / 2,
		(glutGet(GLUT_SCREEN_HEIGHT) - height) / 2);
	
	/* Unter macOS existieren die folgenden Methoden nicht */
	#ifndef __APPLE__
	/* Es wird explizit ein Context mit der Version 3.3 und dem Kern-
	 * Profil angefordert. */
	glutInitContextVersion(3, 3);
	glutInitContextProfile(GLUT_CORE_PROFILE);

	#endif

	/* Fenster erzeugen */
	windowID = glutCreateWindow(title);

	if (windowID)
	{
		INFO(("...fertig.\n\n"));
		
		/* Initialisieren von GLEW.
		 * Wenn glewExperimental auf GL_TRUE gesetzt wird, läd GLEW
		 * zusätzliche Extensions. In diesem konkreten Programm wird dies
		 * beispielsweise benötigt, um glGenVertexArrays() aufzurufen. (Ohne
		 * glewExperimental auf GL_TRUE zu setzen tritt ein Speicherzugriffs-
		 * fehler auf.) */
		INFO(("Initialisiere GLEW...\n"));
		glewExperimental = GL_TRUE;
		GLenum error = glewInit();
		if (error == GLEW_OK)
		{
			GLGETERROR; // Verschluckt OpenGL Fehlermeldung durch glewExperimental
			INFO(("...fertig.\n\n"));

			printf("OpenGL-Version: %s\n", glGetString(GL_VERSION));

			INFO(("Initialisiere Szene...\n"));

			if (initScene())
			{
				INFO(("...fertig.\n\n"));
				INFO(("Lade und initialisiere Texturen...\n"));

				if (initTextures())
				{
					INFO(("...fertig.\n\n"));
					INFO(("Registriere Callbacks...\n"));

					registerCallbacks();

					INFO(("...fertig.\n\n"));
					INFO(("Trete in Schleife der Ereignisbehandlung ein...\n"));
					
					GLSTATE;
					glutMainLoop();
				}
				else
				{
					INFO(("...fehlgeschlagen.\n\n"));

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
