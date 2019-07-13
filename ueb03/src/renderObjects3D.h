#ifndef __RENDER_OBJECTS_3D_H__
#define __RENDER_OBJECTS_3D_H__
/**
 * @file
 * Schnittstelle des 3D Darstellungs-Moduls.
 * Das Modul kapselt die 3D Rendering-Funktionalitaet fuer Displaylisten und haufig verwendete
 * Grundobjekte.
 *
 * Bestandteil einer Uebung im Rahmen des Moduls Praktikum Grundlagen der Computergrafik 
 * an der FH Wedel.
 *
 * @author Nicolas Hollmann, Daniel Klintworth
 */

/* ---- Datentypen ---- */

/* Aufzaehlungstyp fuer alle Objekte in Display Listen */
typedef enum {
	RO_3D_PLAYER = 0,
	RO_3D_WALL,
	RO_3D_FREE,
	RO_3D_GOAL,
	RO_3D_WATER,
	RO_3D_SANDFLOOR,
	RO_3D_SANDBAG,
	RO_3D_SUGAR,

	RO_3D_SIZE
} RenderObject3D;

/* ---- Funktionen ---- */

/**
 * Initialisiert die 3D Display Liste mit allen Objekten.
 */
void initDisplayList3D(void);

/**
 * Rendert ein 3D Objekt aus einer Display Liste.
 * 
 * @param object das 3D Objekt, das gerendert werden soll (In)
 */
void renderObject3D(RenderObject3D object);

#endif
