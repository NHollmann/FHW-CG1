#ifndef __LOGIC_H__
#define __LOGIC_H__
/**
 * @file
 * Logik-Modul.
 * Das Modul kapselt die Programmlogik. Die Programmlogik besteht wesentlich aus 
 * der Verwaltung der Spielerposition, sowie der Verwaltung und Berechnung der
 * Wasserbewegung, der Zuckerwuerfel und der Sandsaecke. Die Programmlogik ist 
 * weitgehend unabhaengig von Ein-/Ausgabe (io.h/c) und Darstellung (scene.h/c).
 *
 * Bestandteil einer Uebung im Rahmen des Moduls Praktikum Grundlagen der Computergrafik 
 * an der FH Wedel.
 *
 * @author Nicolas Hollmann, Daniel Klintworth
 */

/* ---- Eigene Header einbinden ---- */
#include "types.h"

/* ---- Konstanten ---- */

#define PLAYER_COOLDOWN_TIME (0.2f)

/* ---- Typen ---- */

/* Im Spiel vorhandene Level. */
enum e_Level {
    LEVEL_1 = 1,
    LEVEL_2,
    LEVEL_3,
    LEVEL_4,

    LEVEL_LAST = LEVEL_4
};


/* ---- Funktionen ---- */

/**
 * Zustand der Spiellogik updaten.
 * 
 * @param interval die vergangene Zeit seit dem letzten Frame (In)
 */
void updateLogic(double interval);

/**
 * Den Spieler bewegen.
 * 
 * @param direction die Richtung, in der der Spieler sich bewegen will (In)
 */
void movePlayer(PlayerDirection direction);

/**
 * Initialisiert das uebergebene Level neu.
 * 
 * @param levelId Identifikationsnummer des Levels (In)
 */
void initLevel(int levelId);

/**
 * Gibt einen Zeiger auf den Spielzustand zurueck.
 * 
 * @return Zeiger auf den Spielzustand
 */
Gamestate* getGamestate(void);

/**
 * Schaltet die Hilfe ein bzw. aus.
 */
void toggleHelp(void);

/**
 * Benachrichtigt die Logik, dass die Aktionstaste betaetigt wurde.
 */
void pressedActionKey(void);

/**
 * Aendert die Kameraorientierung.
 * 
 * @param deltaRadius die Veraenderung des Radius (In)
 * @param deltaPolar die Veraenderung des Polars (In)
 * @param deltaAzimuth die Veraenderung des Azimuth (In)
 */
void changeCameraOrientation(GLfloat deltaRadius, GLfloat deltaPolar, GLfloat deltaAzimuth);

/**
 * Beendet das Spiel sauber und gibt reservierten Speicher wieder frei.
 */
void cleanup(void);

#endif
