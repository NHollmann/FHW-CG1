#ifndef __RENDER_OBJECTS_H__
#define __RENDER_OBJECTS_H__
/**
 * @file
 * Schnittstelle des Darstellungs-Moduls.
 * Das Modul kapselt die Rendering-Funktionalitaet fuer Displaylisten und haufig verwendete
 * Grundobjekte.
 *
 * Bestandteil einer Uebung im Rahmen des Moduls Praktikum Grundlagen der Computergrafik 
 * an der FH Wedel.
 *
 * @author Nicolas Hollmann, Daniel Klintworth
 */

/* ---- Datentypen ---- */

/* Aufzählungstyp fuer alle Objekte in Display Listen */
typedef enum {
	RO_PLAYER = 0,
	RO_CAKE,
	RO_WALL,
	RO_FREE,
	RO_GOAL,
	RO_WATER,
	RO_SANDBAG,
	RO_SUGAR,

	RO_SIZE
} RenderObject;

/* ---- Funktionen ---- */

/**
 * Zeichnet ein Rechteck mit der Breite und Hoehe 1.
 */
void drawSquare(void);

/**
 * Initialisiert die Display Liste mit allen Objekten.
 */
void initDisplayList(void);

/**
 * Rendert ein Objekt aus einer Display Liste.
 * 
 * @param object das Objekt, das gerendert werden soll (In)
 */
void renderObject(RenderObject object);

#endif
