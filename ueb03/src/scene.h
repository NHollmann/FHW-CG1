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
    unsigned char firstPerson : 1;
    unsigned char showNormals : 1;
    unsigned char animation : 1;
} SceneFlags;

/* Defaultwerte fuer die Flags der Optionen */
#define SCENE_FLAGS_DEFAULT {1, 0, 0, 1}

/* ---- Funktionen ---- */

 /**
 * Zeichen-Funktion fuer 2D.
 * Stellt die Szene dar. Je nach Spielzustand wird ggf. ein Overlay mit
 * der Gewinn-/Verlustnachricht oder die Hilfe angezeigt.
 */
void drawScene2D(void);

 /**
 * Zeichen-Funktion fuer 3D.
 * Stellt die Szene dar. Je nach Spielzustand wird ggf. ein Overlay mit
 * der Gewinn-/Verlustnachricht oder die Hilfe angezeigt.
 * 
 * @param eye der Augenabstand fuer das Anaglyph-Rendering.
 */
void drawScene3D(AnaglyphEye eye);

/**
 * Schaltet den Wireframemodus an/aus.
 */
void toggleWireframeMode(void);

/**
 * Schaltet die Animation an/aus.
 */
void toggleAnimation(void);

/**
 * Wechselt die Kameraperspektive.
 */
void toggleFirstPerson(void);

/**
 * Schaltet den Lichtmodus an/aus.
 */
void toggleLighting(void);

/**
 * Schaltet das Weltlicht an/aus.
 */
void toggleWorldlight(void);

/**
 * Schaltet die Taschenlampe an/aus.
 */
void toggleSpotlight(void);

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
 * @return Rueckgabewert: im Fehlerfall 0, sonst 1.
 */
int initScene(void);

#endif
