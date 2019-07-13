#ifndef __INPUT_H__
#define __INPUT_H__
/**
 * @file
 * Schnittstelle fuer das Modul zur Verarbeitung der Benutzereingaben.
 *
 * Bestandteil einer Uebung im Rahmen des Moduls Praktikum Grundlagen der Computergrafik 
 * an der FH Wedel.
 *
 * @author Nicolas Hollmann, Daniel Klintworth
 */

/* ---- Eigene Header einbinden ---- */
#include "types.h"

/* ---- Typen ---- */

/** Die verschiedenen Mausereignisse. */
typedef enum { 
	mouseButton, 
	mouseMotion, 
	mousePassiveMotion 
} CGMouseEventType;


/* ---- Funktionen ---- */

/**
 * Verarbeitung eines Mausereignisses.
 * Durch Bewegung der Maus bei gedrueckter Maustaste kann die aktuelle
 * Zeichenfarbe beeinflusst werden.
 * Falls Debugging aktiviert ist, wird jedes Mausereignis auf stdout
 * ausgegeben.
 * @param x x-Position des Mauszeigers zum Zeitpunkt der Ereignisausloesung.
 * @param y y-Position des Mauszeigers zum Zeitpunkt der Ereignisausloesung.
 * @param eventType Typ des Ereignisses.
 * @param button ausloesende Maustaste (nur bei Ereignissen vom Typ mouseButton).
 * @param buttonState Status der Maustaste (nur bei Ereignissen vom Typ mouseButton).
 */
void handleMouseEvent(int x, int y, CGMouseEventType eventType, 
					  int button, int buttonState);

/**
 * Verarbeitung eines Tasturereignisses.
 * 
 * @param key Taste, die das Ereignis ausgeloest hat. (ASCII-Wert oder WERT des
 *        GLUT_KEY_<SPECIAL> (In)
 * @param status Status der Taste, GL_TRUE=gedrueckt, GL_FALSE=losgelassen (In)
 * @param isSpecialKey ist die Taste eine Spezialtaste? (In)
 * @param x x-Position des Mauszeigers zum Zeitpunkt der Ereignisausloesung (In)
 * @param y y-Position des Mauszeigers zum Zeitpunkt der Ereignisausloesung (In)
 */
void handleKeyboardEvent(int key, int status, GLboolean isSpecialKey, int x, int y);

/**
 * Gibt wieder, ob das Spiel pausiert ist.
 * 
 * @return true, wenn das Spiel pausiert ist.
 */
GLboolean isPaused(void);

/**
 * Gibt wieder, ob der Anaglyphmodus aktiv ist.
 * 
 * @return true, wenn der Anaglyphmodus aktiv ist.
 */
AnaglyphState getAnaglyphMode(void);

#endif
