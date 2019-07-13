#ifndef __LOGIC_H__
#define __LOGIC_H__
/**
 * @file
 * Logik-Modul.
 * Das Modul kapselt die Programmlogik. Die Programmlogik ist 
 * weitgehend unabhaengig von Ein-/Ausgabe (io.h/c) und Darstellung (scene.h/c).
 *
 * Bestandteil einer Uebung im Rahmen des Moduls Praktikum Grundlagen der Computergrafik 
 * an der FH Wedel.
 *
 * @author Nicolas Hollmann, Daniel Klintworth
 */

/* ---- Eigene Header einbinden ---- */
#include "types.h"

/* ---- Funktionen ---- */

/**
 * Zustand der Spiellogik updaten.
 * 
 * @param interval die vergangene Zeit seit dem letzten Frame (In)
 */
void updateLogic(double interval);

/**
 * Initialisiert die Logik.
 */
void initLogic(void);

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
 * Verarbeitet das Picking mit der Maus.
 * 
 * @param pickName der Name der gepickten Sphere (In)
 * @param leftButton true, wenn das Picking mit der linken Maustaste getaetigt wurde (In)
 */
void handleMousePick(int pickName, GLboolean leftButton);

/**
 * Aendert die Kameraorientierung.
 * 
 * @param deltaRadius die Veraenderung des Radius (In)
 * @param deltaPolar die Veraenderung des Polars (In)
 * @param deltaAzimuth die Veraenderung des Azimuth (In)
 */
void changeCameraOrientation(GLfloat deltaRadius, GLfloat deltaPolar, GLfloat deltaAzimuth);

/**
 * Veraendert die Groesse des Wassergrids.
 * 
 * @param increase true, wenn das Grid vergroessert wird. Bei false wird das Grid verkleinert. (In)
 */
void changeWaterGrid(GLboolean increase);

/**
 * Beendet das Spiel sauber und gibt reservierten Speicher wieder frei.
 */
void cleanup(void);

#endif
