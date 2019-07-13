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

/* ---- Eigene Header einbinden ---- */
#include "water.h"

/* ---- Konstanten ---- */

/* PI ist nicht immer als Konstante definiert, deshalb haben wir eine eigenes TAU */
#define TAU (acos(-1.0) * 2.f)

/* Anzahl der Subdivisionen der Insel. */
#define ISLE_SIDE_SUBDIVS (1)

/* ---- Macros ---- */

/* Ermittelt das Minimum aus zwei Intwerten */
#define MIN_INT(a, b) ((a) > (b) ? (b) : (a))

/* Ermittelt das Maximum aus zwei Intwerten */
#define MAX_INT(a, b) ((a) < (b) ? (b) : (a))

/* Beschraenkt einen Intwert auf den definierten Wertebereich */
#define CLAMP_INT(a, min, max) (MIN_INT(MAX_INT(a, min), max))

/* Umrechnung von Grad zu Radian */
#define TO_RADIANS(x) ((x) * TAU / 360.0f)

/* ---- Typedeklarationen - Anaglyph ---- */

/* Modus der Anaglyphendarstellung */
typedef enum {
    ANAGLYPH_OFF = 0,
    ANAGLYPH_GRAY,
    ANAGLYPH_COLOR,

    ANAGLYPH_MODE_COUNT,
} AnaglyphState;

/* Die Augen fuer das Rendering in Anaglyph 3D */
typedef enum {
    EYE_LEFT = -1,
    EYE_CENTER = 0,
    EYE_RIGHT = 1
} AnaglyphEye;

/* ---- Typedeklarationen - Kamera ---- */

/* Informationen zur Kameraposition auf der Halbkugel. */
typedef struct {
    GLfloat radius;
    GLfloat azimuthAngle;
    GLfloat polarAngle;
} CameraOrientation;

/* Konstante fuer die default Kameraorientierung */
#define EMPTY_CAMERA_ORIENTATION {8.0f, 45.0f, 90.0f}

/* ---- Typedeklarationen - Gamestate ---- */

/** Zusammenfassung aller fuer die Simulation relevanter Daten */
typedef struct {
    CameraOrientation camera; // Kameraeinstellung
    GLboolean showHelp; // Wird die Hilfe angezeigt?
    GLfloat fps; // Frames per Second
    WaterGrid grid; // Daten ueber das Wasser
} Gamestate;

/* Konstante fuer einen leeren Spielzustand */
#define EMPTY_GAMESTATE {EMPTY_CAMERA_ORIENTATION, GL_FALSE, 0.0f, WATER_GRID_EMPTY}

#endif
