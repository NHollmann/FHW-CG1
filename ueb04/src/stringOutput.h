#ifndef __STRING_OUTPUT_H__
#define __STRING_OUTPUT_H__
/**
 * @file
 * Einfache Funktion zum Zeichnen von Text fuer GLUT-Programme.
 */

/* ---- System Header einbinden ---- */
#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

/* ---- Funktionsprototypen ---- */

/**
 * Zeichnen einer Zeichfolge in den Vordergrund. Gezeichnet wird mit Hilfe von
 * <code>glutBitmapCharacter(...)</code>. Kann wie <code>printf</code> genutzt werden.
 * 
 * @param x x-Position des ersten Zeichens 0 bis 1 (In).
 * @param y y-Position des ersten Zeichens 0 bis 1 (In).
 * @param color Textfarbe (In).
 * @param format Formatstring fuer die weiteren Parameter (In).
 */
void drawString(GLfloat x, GLfloat y, GLfloat * color, const char *format, ...);

#endif
