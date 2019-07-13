#ifndef __SCENE_H__
#define __SCENE_H__
/**
 * @file
 * Schnittstelle des Darstellungs-Moduls.
 * Das Modul kapselt die Rendering-Funktionalitaet (insbesondere der OpenGL-
 * Aufrufe) des Programms.
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
 * Zeichen-Funktion.
 * Stellt die Szene dar.
 */
void drawScene(float aspect, float time);

/**
 * Schaltet den Wireframemodus an/aus.
 */
void toggleWireframeMode(void);

/**
 * Schaltet die Texturen an/aus.
 */
void toggleTextures(void);

/**
 * Schaltet die Heightmap an/aus.
 */
void toggleHeightmap(void);

/**
 * Schaltet die Sinuswellen an/aus.
 */
void toggleSines(void);

/**
 * Wechselt zwischen Phong und Gouraud Lichtmodell.
 */
void togglePhong(void);

/**
 * Schaltet die Bumpmap an/aus.
 */
void toggleBumpmap(void);

/**
 * Initialisierung der Szene (inbesondere der OpenGL-Statusmaschine).
 * Setzt Hintergrund- und Zeichenfarbe.
 * 
 * @return Rueckgabewert: im Fehlerfall 0, sonst 1.
 */
int initScene(void);

#endif
