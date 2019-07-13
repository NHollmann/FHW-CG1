#ifndef __TYPES_H__
#define __TYPES_H__
/**
 * @file
 * Typenschnittstelle.
 * Das Modul kapselt die Typdefinitionen und globalen Konstanten des Programms.
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

#include <math.h>

/* ---- Eigene Header einbinden ---- */

#include "sugar.h"

/* ---- Konstanten ---- */

/* PI ist nicht immer als Konstante definiert, deshalb haben wir eine eigene */
#define TAU (acos(-1.0) * 2.f)

/* Die Anzahl der Nachrichten, die im GameOver-Bildschirm angezeigt werden koennten */
#define LOSE_MESSAGE_COUNT 5

/* Anzahl der Subdivisionen aller Tiles. Beeinflusst die Berechneung des Lichtes. */
#define TILE_SUBDIVS (4)

/* ---- Macros ---- */

/* Umrechnung von Grad zu Radian */
#define TO_RADIANS(x) ((x) * TAU / 360.0f)

/* ---- Typedeklarationen - Anaglyph ---- */
typedef enum {
    ANAGLYPH_OFF = 0,
    ANAGLYPH_GRAY,
    ANAGLYPH_COLOR,

    ANAGLYPH_MODE_COUNT,
} AnaglyphState;

/* Die Augen fuer das Rendering in Anaglyph 3D */
typedef enum {
    EYE_LEFT = -1,
    EYE_CENTER = 0,
    EYE_RIGHT = 1
} AnaglyphEye;

/* ---- Typedeklarationen - Level ---- */

/* Hoehe/Breite des (quadratischen) Levels */
#define LEVELSIZE (9)

/*
 * Art von Levelfeldern
 * 
 * WM_FREE: leeres Feld
 * WM_BLACK: schwarzes Feld
 * WM_WALL: Unzerstoerbares Feld
 * WM_GOAL: Zielfeld
 * WM_WATER : Wasser
 * WM_NEWWATER: Neu erzeugtes Wasserfeld
 * WM_SAND : Sandsack, bewegbares Feld
 * WM_SUGAR : Zuckerstueck, bewegbares Feld
 * WM_TOUCHED_SUGAR: Nasser Zucker
 * WM_START: Startfeld
 */
typedef enum { 
    WM_FREE, 
    WM_BLACK, 
    WM_WALL, 
    WM_GOAL,
    WM_WATER, 
    WM_NEWWATER, 
    WM_SAND, 
    WM_SUGAR, 
    WM_TOUCHED_SUGAR, 
    WM_START
} wmFieldType;

/* Spielfeld */
typedef wmFieldType wmLevel[LEVELSIZE][LEVELSIZE];

/* Zeiger auf ein Spielfeld */
typedef const wmFieldType (*wmLevelPointer)[LEVELSIZE];

/* Konstante fuer ein leeres Level */
#define EMPTY_LEVEL {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}}

/* ---- Typedeklarationen - Kamera ---- */

/* Informationen zur Kameraposition auf der Halbkugel. */
typedef struct {
    GLfloat radius;
    GLfloat azimuthAngle;
    GLfloat polarAngle;
} CameraOrientation;

/* Konstante fuer die default Kameraorientierung */
#define EMPTY_CAMERA_ORIENTATION {8.0f, 45.0f, 90.0f}

/* ---- Typedeklarationen - Player ---- */

/** Bewegungsrichtungen des Spielers. */
typedef enum { 
    dirUp, 
    dirDown, 
    dirLeft, 
    dirRight 
} PlayerDirection;

/* ---- Typedeklarationen - Gamestate ---- */

/** Allgemeiner Spielzustand */
typedef enum {
    stageRunning,
    stageWon,
    stageLost
} Gamestage;

/** Zusammenfassung aller fuer das Spiel relevanter Daten */
typedef struct {
    CameraOrientation camera;
    int playerX, playerY; // Spielerposition
    PlayerDirection lastDirection; // Letzte Richtung
    int lastSandbagX, lastSandbagY; // Letzter Bewegter Sandsack
    wmLevel level; // Leveldaten
    int levelId; // ID des Levels
    Gamestage stage; // Spielzustand
    GLboolean showHelp; // Wird die Hilfe angezeigt?
    float updateCooldown; // Verbleibende Zeit bis das Level aktualisiert wird
    float playerCooldown; // Verbleibende Zeit bis der Spieler sich wieder bewegen kann
    int loseMessageId; // Die ID der Nachricht, die beim Verlieren des Levels angezeigt wird
    SugarList sugarCubes; // Dynamisches Array mit allen nassen Zuckerwuerfeln
} Gamestate;

/* Konstante fuer einen leeren Spielzustand */
#define EMPTY_GAMESTATE {EMPTY_CAMERA_ORIENTATION, 0, 0, dirDown, -1, -1, EMPTY_LEVEL, 0, \
                         stageRunning, GL_FALSE, 0.0f, 0.0f, 0, EMPTY_SUGAR_LIST}

#endif
