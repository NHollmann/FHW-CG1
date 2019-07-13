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


/* ---- Funktionen ---- */

 /**
 * Zeichen-Funktion.
 * Stellt die Szene dar. Je nach Spielzustand wird ggf. ein Overlay mit
 * der Gewinn-/Verlustnachricht oder die Hilfe angezeigt.
 */
void drawScene(void);

/**
 * Wechselt den Wireframe-Modus.
 */
void toggleWireframeMode();

/**
 * Initialisierung der Szene (inbesondere der OpenGL-Statusmaschine).
 * Setzt Hintergrund- und Zeichenfarbe.
 * @return Rueckgabewert: im Fehlerfall 0, sonst 1.
 */
int initScene(void);

#endif
