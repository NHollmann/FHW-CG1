#ifndef __IO_H__
#define __IO_H__
/**
 * @file
 * Schnittstelle des Ein-/Ausgabe-Moduls.
 * Das Modul kapselt die Ein- und Ausgabe-Funktionalitaet (insbesondere die GLUT-
 * Callbacks) des Programms.
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
 * Verarbeitung des Picking-Ereignisses.
 * Verarbeitet das Klickereignis zum Picken.
 * 
 * @param x,y  Koordinaten des Klickereignisses (In).
 * @param leftButton true, wenn das Picking mit der linken Maustaste getaetigt wurde (In).
 */
void pick(int x, int y, GLboolean leftButton);

/**
 * Initialisiert das Programm (inkl. I/O und OpenGL) und startet die
 * Ereignisbehandlung.
 * 
 * @param title Beschriftung des Fensters (In)
 * @param width Breite des Fensters (In)
 * @param height Hoehe des Fensters (In)
 * @return ID des erzeugten Fensters, 0 im Fehlerfall
 */
int initAndStartIO(char *title, int width, int height);

#endif
