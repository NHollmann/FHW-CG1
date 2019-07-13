#ifndef __TEXTURE_H__
#define __TEXTURE_H__
/**
 * @file
 * Schnittstelle des Texturen-Moduls.
 * Das Modul kapselt die Textur-Funktionalitaet (insbesondere das Laden und
 * Binden) des Programms.
 * 
 * Bestandteil einer Uebung im Rahmen des Moduls Praktikum Grundlagen der Computergrafik 
 * an der FH Wedel.
 *
 * @author Nicolas Hollmann, Daniel Klintworth
 */

/**
 * Typ fuer Texturenbezeichner.
 */
typedef enum {
	texWater, 
	texSky,
	texDirt,
	texLand
} TexName;

/**
 * Laed Texturen und initialisiert das Texturmapping.
 * 
 * @return 1, wenn Laden und Initialisieren erfolgreich war, sonst 0.
 */
int initTextures(void);

/**
 * Bindet die Textur texture, so dass sie fuer alle nachfolgende gezeichneten
 * Primitiven verwendet wird.
 * 
 * @param texture Bezeichner der Textur, die gebunden werden soll.
 */
void bindTexture(TexName texture);

#endif
