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

/* ---- Typen ---- */

/* Die Flags zum Ein- und Ausschalten verschiedener Optionen. */
typedef struct 
{
    unsigned char lighting : 1;
    unsigned char showNormals : 1;
    unsigned char showSpheres : 1;
    unsigned char textures : 1;
} SceneFlags;

/* Defaultwerte fuer die Flags der Optionen */
#define SCENE_FLAGS_DEFAULT {1, 0, 1, 1}

/* ---- Funktionen ---- */

/**
 * Zeichnet die Szene, in der das Picking stattfindet.
 */
void drawPickingScene();

/**
 * Zeichen-Funktion.
 * Stellt die Szene dar.
 * 
 * @param eye der Augenabstand fuer das Anaglyph-Rendering.
 */
void drawScene(AnaglyphEye eye);

/**
 * Schaltet den Wireframemodus an/aus.
 */
void toggleWireframeMode(void);

/**
 * Schaltet den Lichtmodus an/aus.
 */
void toggleLighting(void);

/**
 * Schaltet die Anzeige der Wasserspheres an/aus.
 */
void toggleSpheres(void);

/**
 * Schaltet die Texturen an/aus.
 */
void toggleTextures(void);

/**
 * Schaltet das Sonnenlicht an/aus.
 */
void toggleSunlight(void);

/**
 * Schaltet das Punktlicht an/aus.
 */
void togglePointlight(void);

/**
 * Schaltet die Anzeige der Normalen an/aus.
 */
void toggleNormal(void);

/**
 * Gibt wieder, ob die Normalen angezeigt werden.
 * 
 * @return true, wenn die Normalen angezeigt werden.
 */
GLboolean getNormalState(void);

/**
 * Initialisierung der Szene (inbesondere der OpenGL-Statusmaschine).
 * Setzt Hintergrund- und Zeichenfarbe.
 * 
 * @return Rueckgabewert: im Fehlerfall 0, sonst 1.
 */
int initScene(void);

#endif
