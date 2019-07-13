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
 * Stellt die Szene dar. Der Spielfeldrand, die zwei Schlaeger und der 
 * Ball werden immer gezeichnet. Je nach Modus werden auch Hilfslinien
 * am Spielerschlaeger gezeichnet.
 */
void drawScene(void);

/**
 * Setzt den Wireframe-Modus.
 *
 * @param wireframe ob die Spielelemente als Wireframe gezeichnet werden. (In)
 */
void setWireframeMode(GLboolean wireframe);

/**
 * Initialisierung der Szene (inbesondere der OpenGL-Statusmaschine).
 * Setzt Hintergrund- und Zeichenfarbe.
 * @return Rueckgabewert: im Fehlerfall 0, sonst 1.
 */
int initScene(void);

#endif
