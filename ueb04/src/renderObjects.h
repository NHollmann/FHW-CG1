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

/* Aufzaehlungstyp fuer alle Objekte in Display Listen */
typedef enum {
	RO_SPHERE = 0,
	RO_ISLE_TOP,
	RO_ISLE_RING,
	RO_SKY,

	RO_SIZE
} RenderObject;

/* ---- Funktionen ---- */

/**
 * Zeichnet ein Rechteck mit der Breite und Hoehe 1.
 * 
 * @param subDivs die Anzahl der Subdivisionen. Mehr Subdivisionen erlauben feinere Beleuchtung.
 */
void drawSquare(int subDivs);

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

/**
 * Setzt das diffuse Material.
 * 
 * @param r der Rotanteil.
 * @param g der Gruenanteil.
 * @param b der Blauanteil.
 */
void setDiffuseMaterial(float r, float g, float b);

/**
 * Setzt das Diffuse Material mit Alpha.
 * 
 * @param r der Rotanteil.
 * @param g der Gruenanteil.
 * @param b der Blauanteil.
 * @param a der Alphawert.
 */
void setDiffuseMaterialAlpha(float r, float g, float b, float a);

/**
 * Setzt das spekulare Material.
 * 
 * @param r der Rotanteil.
 * @param g der Gruenanteil.
 * @param b der Blauanteil.
 * @param shiny die Spiegeligkeit des Materials.
 */
void setSpecularMaterial(float r, float g, float b, float shiny);

#endif
