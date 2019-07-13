#ifndef __LOGIC_H__
#define __LOGIC_H__
/**
 * @file
 * Logik-Modul.
 * Das Modul kapselt die Programmlogik. Die Programmlogik besteht wesentlich aus 
 * der Verwaltung und Berechnung der Bewegungsrichtung des Spielballs, und der
 * Kollisionsberechnung mit den Schlaegern des Spielers und des Bots. Die
 * Programmlogik ist weitgehend unabhaengig von Ein-/Ausgabe (io.h/c) und
 * Darstellung (scene.h/c).
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
 * Spiel-Update fuer den Ball.
 * Die Bewegung des Balls wird fuer den angegebenen Zeitabschnitt berechnet.
 * 
 * @param interval die vergangene Zeit seit dem letzten Frame (In)
 */
void calcBall (double interval);

/**
 * Spiel-Update fuer die Schlaeger.
 * Die Bewegung der Schlaeger wird fuer den angegebenen Zeitabschnitt berechnet.
 * 
 * @param interval die vergangene Zeit seit dem letzten Frame (In)
 */
void calcPaddles (double interval);

/**
 * Aktiviert die Bewegung das menschlichen Schlaegers in die definierte Richtung.
 * 
 * @param direction die Bewegungsrichtung (In)
 * @param status startet die Bewegung oder endet sie? (In)
 */
void setPaddleMovement(PaddleDirection direction, GLboolean status);

/**
 * Aktiviert die Rotation das menschlichen Schlaegers in die definierte Richtung.
 * 
 * @param rotation die Rotationsrichtung (In)
 * @param status startet die Rotation oder endet sie? (In)
 */
void setPaddleRotation(PaddleRotation rotation, GLboolean status);

/**
 * Schaltet die Anzeige der Hilfslinien um.
 */
void toggleGuidelines(void);

/**
 * Schaltet die Anzeige von zufaelligen Ballformen um.
 */
void toggleRandomShape(void);

/**
 * Gibt einen Zeiger auf den Spielball zurueck.
 * 
 * @return Zeiger auf den Ball
 */
Ball* getBall(void);

/**
 * Gibt einen Zeiger auf den menschlichen Spieler zurueck.
 * 
 * @return Zeiger auf den menschlichen Spieler
 */
Player *getHumanPlayer(void);

/**
 * Gibt einen Zeiger auf den KI-Spieler zurueck.
 * 
 * @return Zeiger auf den KI Spieler
 */
Player *getBotPlayer(void);

/**
 * Gibt einen Zeiger auf die Vektoren fuer die Hilfslinien zurueck.
 * 
 * @return Zeiger auf die Vektoren fuer die Hilfslinien
 */
Guidelines *getGuidelines(void);

/**
 * Initialisiert eine neue Runde.
 * Der Ball wird je nach startPos korrekt positioniert
 * und eine zufaellige Richtung wird ausgewaehlt.
 * 
 * @param startPos startet der Ball rechts oder links? (In)
 */
void initRound(CGSide startPos);

#endif
