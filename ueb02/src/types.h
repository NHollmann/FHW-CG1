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

/* ---- Typedeklarationen - Level ---- */

/* Hoehe/Breite des (quadratischen) Levels */
#define LEVELSIZE (9)

/*
 * Art von Levelfeldern
 * 
 * WM_FREE: leeres Feld
 * WM_BLACK: schwarzes Feld
 * WM_WALL: Unzerstörbares Feld
 * WM_GOAL: Zielfeld
 * WM_WATER : Wasser
 * WM_NEWWATER: Neu erzeugtes Wasserfeld
 * WM_SAND : Sandsack, bewegbares Feld
 * WM_SUGAR : Zuckerstück, bewegbares Feld
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

#define EMPTY_LEVEL {{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}}


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
    int playerX, playerY; // Spielerposition
    wmLevel level; // Leveldaten
    int levelId; // ID des Levels
    Gamestage stage; // Spielzustand
    GLboolean showHelp; // Wird die Hilfe angezeigt?
    float updateCooldown; // Verbleibende Zeit bis das Level aktualisiert wird
    float playerCooldown; // Verbleibende Zeit bis der Spieler sich wieder bewegen kann
    int loseMessageId; // Die ID der Nachricht, die beim Verlieren des Levels angezeigt wird
    SugarList sugarCubes; // Dynamisches Array mit allen nassen Zuckerwuerfeln
} Gamestate;

#define EMPTY_GAMESTATE {0, 0, EMPTY_LEVEL, 0, stageRunning, GL_FALSE, 0.0f, 0.0f, 0, EMPTY_SUGAR_LIST}

#endif
