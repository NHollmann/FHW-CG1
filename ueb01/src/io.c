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
#include "logic.h"
#include "scene.h"
#include "debugGL.h"

/* ---- Konstanten ---- */

/** Anzahl der Aufrufe der Timer-Funktion pro Sekunde */
#define TIMER_CALLS_PS 60

/* ---- Globale Daten ---- */

/** Speichert, ob das Spiel aktuell pausiert ist */
static GLboolean g_paused = GL_FALSE;

/* ---- Interne Funktionen ---- */

/**
 * Setzen der Projektionsmatrix.
 * Setzt die Projektionsmatrix unter Beruecksichtigung des Seitenverhaeltnisses
 * des Anzeigefensters, sodass das Seitenverhaeltnisse der Szene unveraendert
 * bleibt und gleichzeitig entweder in x- oder y-Richtung der Bereich von -1
 * bis +1 zu sehen ist.
 * 
 * @param aspect Seitenverhaeltnis des Anzeigefensters (In)
 */
static void setProjection(GLdouble aspect)
{
	/* Nachfolgende Operationen beeinflussen Projektionsmatrix */
	glMatrixMode(GL_PROJECTION);
	/* Matrix zuruecksetzen - Einheitsmatrix laden */
	glLoadIdentity();

	/* Koordinatensystem bleibt quadratisch */
	if (aspect <= 1)
	{
		gluOrtho2D(-1.0, 1.0,					 /* links, rechts */
					 -1.0 / aspect, 1.0 / aspect); /* unten, oben */
	}
	else
	{
		gluOrtho2D(-1.0 * aspect, 1.0 * aspect, /* links, rechts */
					 -1.0, 1.0);					/* unten, oben */
	}
}

/**
 * Umschalten zw. Vollbild- und Fenstermodus.
 * Beim Wechsel zum Fenstermodus wird vorherige Position und Groesse
 * wiederhergestellt. HINWEIS: Fenster wird nicht korrekt auf alte Position
 * gesetzt, da GLUT_WINDOW_WIDTH/HEIGHT verfaelschte Werte liefert.
 */
static void toggleFullscreen (void)
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
			windowPos[0] = glutGet (GLUT_WINDOW_X);
			windowPos[1] = glutGet (GLUT_WINDOW_Y);
			windowSize[0] = glutGet (GLUT_WINDOW_WIDTH);
			windowSize[1] = glutGet (GLUT_WINDOW_HEIGHT);
			/* In den Fullscreen-Modus wechseln */
			glutFullScreen ();
		}
	else
		{
			/* alte Fenstereinstellungen wiederherstellen */
			glutReshapeWindow (windowSize[0], windowSize[1]);

			/* HINWEIS:
				 Auskommentiert, da es sonst Probleme mit der Vollbildarstellung bei
				 Verwendung von FreeGlut gibt */
			glutPositionWindow (windowPos[0], windowPos[1]);
		}
}

/**
 * Gibt eine Hilfe fuer das Programm auf der Konsole aus.
 */
static void printHelp(void)
{
	printf("Pong Help\n");
	printf("=========\n");
	printf("Up/Down    - Move paddle up & down\n");
	printf("Left/Right - Rotate paddle\n");
	printf("h/H        - Show this help\n");
	printf("l/L        - Enable Wireframe\n");
	printf("f/F        - Disable Wireframe\n");
	printf("b/B        - Toggle random shapes\n");
	printf("g/G        - Toggle guidelines\n");
	printf("F2         - Toggle Fullscreen\n");
	printf("F3         - Toggle Pause\n");
	printf("q/Q/ESC    - Quit\n");
	fflush(stdout);
}

/**
 * Verarbeitung eines Tasturereignisses.
 * Pfeiltasten rauf/runter steuern die Position des Spielerschlaegers.
 * Pfeiltasten links/rechts steuern die Rotation des Spielerschlaegers.
 * h/H gibt die Hilfe aus.
 * l/L aktiviert den Wireframe-Modus.
 * f/F deaktiviert den Wireframe-Modus.
 * b/B wechselt zwischen verschiedenen Formen und Farben fuer den Spielball.
 * g/G schaltet Hilfslinien ein und aus.
 * F2-Taste schaltet zwischen Fenster und Vollbilddarstellung um.
 * F3-Taste pausiert das Spiel bzw. setzt es wieder fort.
 * ESC-Taste und q, Q beenden das Programm.
 * 
 * @param key Taste, die das Ereignis ausgeloest hat. (ASCII-Wert oder WERT des
 *        GLUT_KEY_<SPECIAL> (In)
 * @param status Status der Taste, GL_TRUE=gedrueckt, GL_FALSE=losgelassen (In)
 * @param isSpecialKey ist die Taste eine Spezialtaste? (In)
 * @param x x-Position des Mauszeigers zum Zeitpunkt der Ereignisausloesung (In)
 * @param y y-Position des Mauszeigers zum Zeitpunkt der Ereignisausloesung (In)
 */
static void handleKeyboardEvent(int key, int status, GLboolean isSpecialKey, int x,
								int y)
{
/** Keycode der ESC-Taste */
#define ESC 27

	/* Taste gedrueckt */
	if (status == GLUT_DOWN)
	{
		/* Spezialtaste gedrueckt */
		if (isSpecialKey)
		{
			switch (key)
			{
			/* Rotation des Spielerschlaegers in entsprechende Richtung starten */
			case GLUT_KEY_LEFT:
				setPaddleRotation(rotCounterClockwise, GL_TRUE);
				break;
			case GLUT_KEY_RIGHT:
				setPaddleRotation(rotClockwise, GL_TRUE);
				break;
			/* Bewegung des Spielerschlaegers in entsprechende Richtung starten */
			case GLUT_KEY_UP:
				setPaddleMovement(dirUp, GL_TRUE);
				break;
			case GLUT_KEY_DOWN:
				setPaddleMovement(dirDown, GL_TRUE);
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
			/* Wireframemodus aktivieren */
			case 'l':
			case 'L':
				setWireframeMode(GL_TRUE);
				break;
			/* Wireframemodus deaktivieren */
			case 'f':
			case 'F':
				setWireframeMode(GL_FALSE);
				break;
			/* Hilfslinien ein-/ausschalten */
			case 'g':
			case 'G':
				toggleGuidelines();
				break;
			/* Formwechsel fuer den Ball ein-/ausschalten */
			case 'b':
			case 'B':
				toggleRandomShape();
				break;
			/* Hilfe ausgeben */
			case 'h':
			case 'H':
				printHelp();
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
	/* Taste losgelassen */
	else
	{
		/* Spezialtaste losgelassen */
		if (isSpecialKey)
		{
			switch (key)
			{
			/* Rotation des Spielerschlaegers in entsprechende Richtung beenden */
			case GLUT_KEY_LEFT:
				setPaddleRotation(rotCounterClockwise, GL_FALSE);
				break;
			case GLUT_KEY_RIGHT:
				setPaddleRotation(rotClockwise, GL_FALSE);
				break;
			/* Bewegung des Spielerschlaegers in entsprechende Richtung beenden */
			case GLUT_KEY_UP:
				setPaddleMovement(dirUp, GL_FALSE);
				break;
			case GLUT_KEY_DOWN:
				setPaddleMovement(dirDown, GL_FALSE);
				break;
			}
		}
	}
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

	if (g_paused) {
		interval = 0.0;
	}

	/* neue Position berechnen (zeitgesteuert) */
	calcBall(interval);

	/* neue Farbwerte fuer die Eckpunkte berechnen (zeitgesteuert) */
	calcPaddles(interval);

	/* Wieder als Timer-Funktion registrieren */
	glutTimerFunc(1000 / TIMER_CALLS_PS, cbTimer, thisCallTime);

	/* Neuzeichnen anstossen */
	glutPostRedisplay();
}

/**
 * Callback fuer Aenderungen der Fenstergroesse.
 * Initiiert Anpassung der Projektionsmatrix an veraenderte Fenstergroesse.
 * 
 * @param w Fensterbreite (In)
 * @param h Fensterhoehe (In)
 */
static void cbReshape(int w, int h)
{
	/* Das ganze Fenster ist GL-Anzeigebereich */
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);

	/* Anpassen der Projektionsmatrix an das Seitenverhaeltnis des Fensters */
	setProjection((GLdouble)w / (GLdouble)h);
}

/**
 * Zeichen-Callback.
 * Loescht die Buffer, ruft das Zeichnen der Szene auf und tauscht den Front-
 * und Backbuffer.
 */
static void cbDisplay(void)
{
	/* Buffer zuruecksetzen */
	glClear(GL_COLOR_BUFFER_BIT);

	/* Nachfolgende Operationen beeinflussen Modelviewmatrix */
	glMatrixMode(GL_MODELVIEW);
	
	/* Matrix zuruecksetzen - Einheitsmatrix laden */
	glLoadIdentity();

	/* Szene zeichnen */
	drawScene();

	/* Objekt anzeigen */
	glutSwapBuffers();
}

/**
 * Registrierung der GLUT-Callback-Routinen.
 */
static void registerCallbacks(void)
{
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
	glutTimerFunc(1000 / TIMER_CALLS_PS,	   		/* msecs - bis Aufruf von func */
					cbTimer,					   	/* func  - wird aufgerufen    */
					glutGet(GLUT_ELAPSED_TIME)); 	/* value - Parameter, mit dem
													func aufgerufen wird */

	/* Reshape-Callback - wird ausgefuehrt, wenn neu gezeichnet wird (z.B. nach
	 * Erzeugen oder Groessenaenderungen des Fensters) */
	glutReshapeFunc(cbReshape);

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
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB); /* fuer DoubleBuffering */
	glutInitWindowSize(width, height);
	glutInitWindowPosition(750, 100);
	
	glutInitWindowPosition(
		(glutGet(GLUT_SCREEN_WIDTH) - width)/2, 
		(glutGet(GLUT_SCREEN_HEIGHT) - height)/2
	);


	/* Fenster erzeugen */
	windowID = glutCreateWindow(title);

	if (windowID)
	{
		INFO(("...fertig.\n\n"));
		INFO(("Initialisiere Szene...\n"));

		if (initScene())
		{
			INFO(("...fertig.\n\n"));
			INFO(("Initialisiere Zufallsgenerator...\n"));

			srand(time(0));

			INFO(("...fertig.\n\n"));
			INFO(("Initialisiere Logik...\n"));
			
			initRound(sideLeft);

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
