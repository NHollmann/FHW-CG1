#ifndef __UTILITY_H__
#define __UTILITY_H__
/**
 * @file
 * Schnittstelle des Utility-Moduls.
 * Das Modul bietet mehrere Utility-Funktionen, die vor allem beim Umgang mit Shadern nuetzlich sind.
 *
 * Bestandteil einer Uebung im Rahmen des Moduls Praktikum Grundlagen der Computergrafik 
 * an der FH Wedel.
 *
 * @author Nicolas Hollmann, Daniel Klintworth
 */

/* ---- System Header einbinden ---- */

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
/* Anstelle der "glut.h" wird die "freeglut.h" eingebunden, da nur diese
 * das explizite Einstellen einer OpenGL-Version erlaubt. */
#include <GL/freeglut.h>
#endif


/* ---- Funktionen ---- */

/**
 * Erzeugt eine perspektivische Projektions-Matrix.
 *
 * @param fov Öffnungswinkel des View-Frustums in deg. (In)
 * @param aspect Verhältnis zwischen Breite und Höhe des View-Frustums. (In)
 * @param znear Abstand zur Near-Plane. (In)
 * @param zfar Abstand zur Far-Plane. (In)
 * @param m Pointer auf die Matrix, die mit Werten gefüllt werden soll. (Out)
 *
 * @remarks Diese Funktion entspricht funktional gluPerspective.
 */
void perspective(float fov, float aspect, float znear, float zfar, float* m);

/**
 * Berechnet eine View-Matrix.
 *
 * @param center Betrachterposition. (In)
 * @param target Betrachtungspunkt. (In)
 * @param up Oben. (In)
 * @param m Pointer auf eine Matrix, die mit Werten gefüllt werden soll. (Out)
 *
 * @remarks Diese Funktion entspricht funktional gluLookAt.
 */
void lookAt(float centerX, float centerY, float centerZ, float targetX, float targetY, float targetZ, float upX, float upY, float upZ, float* m);

/**
 * Erstellt ein neues Program.
 *
 * @param vertexShaderFilename Dateiname des Vertex-Shaders. (In)
 * @param fragmentShaderFilename Dateiname des Fragment-Shaders. (In)
 * @return Id des neu erstellten Programm-Objekts.
 */
GLuint createProgram(const char* vertexShaderFilename, const char* fragmentShaderFilename);

#endif
