#ifndef __TYPES_H__
#define __TYPES_H__
/**
 * @file
 * Typenschnittstelle.
 * Das Modul kapselt die Typdefinitionen und globalen Konstanten des Programms.
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

#include <math.h>

/* ---- Konstanten ---- */

/* PI ist nicht immer als Konstante definiert, deshalb haben wir eine eigenes TAU */
#define TAU (acos(-1.0) * 2.f)

/* ---- Macros ---- */

/* Umrechnung von Grad zu Radian */
#define TO_RADIANS(x) ((x) * TAU / 360.0f)

/* Berechnet aus den Gridkoordinaten den Index im Vertexarray */
#define GRID_TO_IDX(x, y, s) ((y) * (s) + (x))

/* ---- Typedeklarationen - Kamera ---- */

/* Informationen zur Kameraposition auf der Halbkugel. */
typedef struct {
    GLfloat radius;
    GLfloat azimuthAngle;
    GLfloat polarAngle;
} CameraOrientation;

/* Konstante fuer die default Kameraorientierung */
#define EMPTY_CAMERA_ORIENTATION {1.0f, 45.0f, 90.0f}

/* ---- Typedeklarationen - Gamestate ---- */

/** Zusammenfassung aller fuer die Simulation relevanter Daten */
typedef struct {
    CameraOrientation camera; // Kameraeinstellung
    float cameraCooldown; // Cooldown fuer die automatische Kamerarotation
} Gamestate;

/* Konstante fuer einen leeren Spielzustand */
#define EMPTY_GAMESTATE {EMPTY_CAMERA_ORIENTATION, 0.0f}

#endif
