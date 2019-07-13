#ifndef __SUGAR_H__
#define __SUGAR_H__
/**
 * @file
 * Verwaltet die Animationszustaende fuer Zuckerwuerfel als dynamisches Array.
 *
 * Bestandteil einer Uebung im Rahmen des Moduls Praktikum Grundlagen der Computergrafik 
 * an der FH Wedel.
 *
 * @author Nicolas Hollmann, Daniel Klintworth
 */

/* ---- Konstanten ---- */

/** Die Lebenszeit eines nassen Zuckerwuerfels */
#define SUGAR_LIFETIME (2.0f)

/* ---- Typedeklarationen - Callback ---- */

/** Funktionszeiger fuer Callbacks, wenn ein Zuckerwuerfel aufgeloest wurde */
typedef void (*SugarDissolved)(int x, int y);

/* ---- Typedeklarationen - Zucker ---- */

/** Eigenschaften eines Zuckerwuerfels */
typedef struct {
    float lifetime;
    int x, y;
} Sugar;

/** Dynamisches Array aus Zuckerwuerfeln */
typedef struct {
    int count;
    Sugar *list;
} SugarList;


#define EMPTY_SUGAR_LIST {0, NULL}

/* ---- Funktionen ---- */

/**
 * Fuegt einen neuen Zuckerwuerfel mit gegebener Position 
 * zu einem dynamischen Array hinzu. Die Lbenszeit wird dabei mit
 * SUGAR_LIFETIME initialisiert.
 * 
 * @param list Das zu verandernde Array (InOut)
 * @param x die X-Koordinate (In)
 * @param y die Y-Koordinate (In)
 */
void sugar_add(SugarList *list, int x, int y);

/**
 * Ruft die verbleibende Lebenszeit eines Zuckerwuerfels ab.
 * 
 * @param list Das Zuckerwuerfel Array (In)
 * @param x die X-Koordinate (In)
 * @param y die Y-Koordinate (In)
 * @return die verbleibende Lebenszeit in Sekunden
 */
float sugar_get_lifetime(SugarList *list, int x, int y);

/**
 * Aktualisiert die Lebenszeiten aller Zuckerwuerfel in einem Array.
 * Wenn ein Wuerfel sich aufloest wird ein Callback aufgerufen.
 * 
 * @param list Das zu verandernde Array (InOut)
 * @param callback das Aufloesungs-Callback (In)
 * @param interval die Zeit, die seit dem letzten Update vergangen ist (In)
 */
void sugar_update(SugarList *list, SugarDissolved callback, float interval);

/**
 * Gibt den Speicher des dynamischen Arrays wieder frei.
 * 
 * @param list Das zu loeschende Array (InOut)
 */
void sugar_free(SugarList *list);

#endif
