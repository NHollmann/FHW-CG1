#ifndef __VECTOR_H__
#define __VECTOR_H__
/**
 * @file
 * Vektor-Modul.
 * Das Modul kapselt alle Vektorrechnungen auf Float Arrays der Laenge 2.
 *
 * Bestandteil einer Uebung im Rahmen des Moduls Praktikum Grundlagen der Computergrafik 
 * an der FH Wedel.
 *
 * @author Nicolas Hollmann, Daniel Klintworth
 */

/* ---- System Header einbinden ---- */
#ifdef WIN32
#include <windows.h>
#endif

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif


/* ---- Typedeklarationen ---- */

/** Punkt im 2D-Raum */
typedef GLfloat CGPoint2f[2];

#define POINT_ZERO {0, 0}

/** Vektor im 2D-Raum */
typedef GLfloat CGVector2f[2];

#define VECTOR_ZERO {0, 0}

/* ---- Funktionen ---- */

/**
 * Kopiert die Werte von einem Vektor in einen anderen.
 * 
 * @param a der zu veraendernde Vektor (Out)
 * @param b die Quelle (In)
 */
void vector_set(CGVector2f a, CGVector2f b);

/**
 * Addiert die Werte von einem Vektor auf einen anderen.
 * 
 * @param a 1. Summand und Summe (InOut)
 * @param b 2. Summand (In)
 */
void vector_add(CGVector2f a, CGVector2f b);

/**
 * Subtrahiert die Werte von einem Vektor von einem anderen.
 * 
 * @param a Minuend und DIiferenz (InOut)
 * @param b Subtrahend (In)
 */
void vector_sub(CGVector2f a, CGVector2f b);

/**
 * Multipliziert einen Vektor mit einem Skalar.
 * 
 * @param a Zu skalierender Vektor (InOut)
 * @param scalar Skalar (In)
 */
void vector_mul(CGVector2f a, GLfloat scalar);

/**
 * Bestimmt einen Richtungsvektor aus einem Winkel auf 
 * dem Einheitskreis.
 * 
 * @param a Ergebnisvektor (Out)
 * @param angle Winkel (In)
 */
void vector_circle(CGVector2f a, GLfloat angle);

/**
 * Normalisiert einen Vektor.
 * 
 * @param a der zu normaliserende Vektor und das Ergebnis (InOut)
 */
void vector_norm(CGVector2f a);

/**
 * Berechnet das Skalarprodukt zweier Vektoren.
 * 
 * @param a 1. Vektor (In)
 * @param b 2. Vektor (In)
 * @return das Skalarprodukt
 */
GLfloat vector_dot(CGVector2f a, CGVector2f b);

/**
 * Berechnet den Schnittpunkt zweier Vektoren 
 * ausgehend von defineirten Startpunkten.
 * Bei parallelen Vektoren bleibt intersection unberuehrt.
 * 
 * @param intersection der gefundene Schnittpunkt (Out)
 * @param posA erster Startpunkt (In)
 * @param dirA erster Richtungsvektor (In)
 * @param posB zweiter Startpunkt (In)
 * @param dirB zweiter Richtungsvektor (In)
 */
void vector_intersect(CGPoint2f intersection, 
                      CGPoint2f posA, CGVector2f dirA, 
                      CGPoint2f posB, CGVector2f dirB);

/**
 * Berechnet die Distanz zweier Punkte.
 * 
 * @param a 1. Punkt (In)
 * @param b 2. Punkt (In)
 * @return ddie Distanz
 */
GLfloat point_distance(CGPoint2f a, CGPoint2f b);

#endif
