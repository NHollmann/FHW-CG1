/**
 * @file
 * Einfache Funktion zum Zeichnen von Text fuer GLUT-Programme.
 */

/* ---- System Header einbinden ---- */
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

/* ---- Eigene Header einbinden ---- */

#include "stringOutput.h"

/* ---- Oeffentliche Funktionen ---- */

void drawString (GLfloat x, GLfloat y, GLfloat * color, const char *format, ...)
{
    GLint matrixMode;             /* Zwischenspeicher akt. Matrixmode */
    va_list args;                 /* variabler Teil der Argumente */
    char buffer[255];             /* der formatierte String */
    char *s;                      /* Zeiger/Laufvariable */
    va_start (args, format);
    vsnprintf (buffer, 255, format, args);
    va_end (args);

    /* aktuelle Zeichenfarbe (u.a. Werte) sichern */
    glPushAttrib (GL_COLOR_BUFFER_BIT | GL_CURRENT_BIT | GL_ENABLE_BIT);

    /* aktuellen Matrixmode speichern */
    glGetIntegerv (GL_MATRIX_MODE, &matrixMode);
    glMatrixMode (GL_PROJECTION);

    /* aktuelle Projektionsmatrix sichern */
    glPushMatrix ();

    /* neue orthogonale 2D-Projektionsmatrix erzeugen */
    glLoadIdentity ();
    gluOrtho2D (0.0, 1.0, 1.0, 0.0);

    glMatrixMode (GL_MODELVIEW);

    /* aktuelle ModelView-Matrix sichern */
    glPushMatrix ();

    /* neue ModelView-Matrix zuruecksetzen */
    glLoadIdentity ();

    /* Tiefentest ausschalten */
    glDisable (GL_DEPTH_TEST);

    /* Licht ausschalten */
    glDisable (GL_LIGHTING);

    /* Nebel ausschalten */
    glDisable (GL_FOG);

    /* Blending ausschalten */
    glDisable (GL_BLEND);

    /* Texturierung ausschalten */
    glDisable (GL_TEXTURE_1D);
    glDisable (GL_TEXTURE_2D);
    /* glDisable (GL_TEXTURE_3D); */

    /* neue Zeichenfarbe einstellen */
    glColor3fv (color);

    /* an uebergebenene Stelle springen */
    glRasterPos2f (x, y);

    /* Zeichenfolge zeichenweise zeichnen */
    for (s = buffer; *s; s++)
    {
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *s);
    }

    /* alte ModelView-Matrix laden */
    glPopMatrix ();
    glMatrixMode (GL_PROJECTION);

    /* alte Projektionsmatrix laden */
    glPopMatrix ();

    /* alten Matrixmode laden */
    glMatrixMode (matrixMode);

    /* alte Zeichenfarbe und Co. laden */
    glPopAttrib ();
}
