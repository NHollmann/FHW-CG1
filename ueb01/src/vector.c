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
#include <math.h>

/* ---- Eigene Header einbinden ---- */
#include "vector.h"

/* ---- Interne Funktionen ---- */

/**
 * Setzt einen Vektor auf 0 zurueck.
 * 
 * @param a der zu zuruecksetztende Vektor (Out)
 */
static void vector_zero(CGVector2f a) 
{
    a[0] = 0.0f;
    a[1] = 0.0f;
}

/**
 * Berechnet die Determinante einer Matrix aus zwei Vektoren.
 * 
 * @param a 1. Vektor (In)
 * @param b 2. Vektor (In)
 * @return die Determinante
 */
static GLfloat vector_det(CGVector2f a, CGVector2f b) 
{
    return (a[0] * b[1]) - (a[1] * b[0]);
}

/**
 * Berechnet die Laenge eines Vektors.
 * 
 * @param a der Vektor, dessen Laenge bestimmt werden soll (In)
 * @return die Laenge des Vektors
 */
static GLfloat vector_len(CGVector2f a) 
{
    return sqrtf(a[0] * a[0] + a[1] * a[1]);
}

/* ---- Oeffentliche Funktionen ---- */

void vector_set(CGVector2f a, CGVector2f b) 
{
    a[0] = b[0];
    a[1] = b[1];
}

void vector_add(CGVector2f a, CGVector2f b) 
{
    a[0] += b[0];
    a[1] += b[1];
}

void vector_sub(CGVector2f a, CGVector2f b) 
{
    a[0] -= b[0];
    a[1] -= b[1];
}

void vector_mul(CGVector2f a, GLfloat scalar) 
{
    a[0] *= scalar;
    a[1] *= scalar;
}

void vector_circle(CGVector2f a, GLfloat angle) 
{
    a[0] = cosf(angle);
    a[1] = sinf(angle);
}

void vector_norm(CGVector2f a) 
{
    GLfloat len = vector_len(a);
    if (len != 0.0f) 
    {
        a[0] /= len;
        a[1] /= len;
    }
}

GLfloat vector_dot(CGVector2f a, CGVector2f b) 
{
    return (a[0] * b[0]) + (a[1] * b[1]);
}

void vector_intersect(CGPoint2f intersection, 
                      CGPoint2f posA, CGVector2f dirA, 
                      CGPoint2f posB, CGVector2f dirB) 
{
    // Verbindungsvektor aufstellen
	CGVector2f connection = VECTOR_ZERO;
	vector_add(connection, posB); 
	vector_sub(connection, posA);

	// Determinante von Normale und Kante
	GLfloat det = vector_det(dirB, dirA);

    if (det != 0.0) 
    {
        // Determinante von Verbindungsvektor und Normale
        GLfloat lambda = vector_det(dirB, connection) / det;

        // Schnittpunkt bestimmen
        vector_zero(intersection);
        vector_add(intersection, dirA);
        vector_mul(intersection, lambda);
        vector_add(intersection, posA);
    }
}

GLfloat point_distance(CGPoint2f a, CGPoint2f b)
{
    CGVector2f distVector = VECTOR_ZERO;
	vector_add(distVector, a);
	vector_sub(distVector, b);
	return vector_len(distVector);
}
