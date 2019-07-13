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

/* ---- Eigene Header einbinden ---- */
#include "vector.h"


/* ---- Konstanten ---- */

/* PI ist nicht immer als Konstante definiert, deshalb haben wir eine eigene */
#define TAU (acos(-1.0) * 2.f)

/** x-Koordinate linker Rand */
#define BORDER_LEFT   -0.9f

/** x-Koordinate rechter Rand */
#define BORDER_RIGHT   0.9f

/** y-Koordinate oberer Rand */
#define BORDER_TOP     0.9f

/** y-Koordinate unterer Rand */
#define BORDER_BOTTOM -0.9f

/** Dicke der Spielfeldbegrenzung */
#define BORDER_THICKNESS 0.1f

/** Dicke der Spielschlaeger */
#define PADDLE_THICKNESS 0.1f

/** Hoehe der Spielschlaeger */
#define PADDLE_HEIGHT 0.3f

/** Groesse des Spielballs */
#define BALL_SIZE 0.1f

/** Anzahl der Punkte des runden Spielballs */
#define CIRCLE_POINT_COUNT 20

/** Anzahl der Ecken des sternfoermigen Spielballs */
#define STAR_CORNER_COUNT 8

/** Innerer Radius sternfoermigen Spielballs */
#define STAR_INNER_RADIUS 0.5f


/* ---- Macros ---- */

/* Umrechnung von Grad zu Radian */
#define TO_RADIANS(x) ((x) * TAU / 360.0f)


/* ---- Typedeklarationen - Color ---- */

/** RGB-Farbwert */
typedef GLfloat CGColor3f[3];

/** Die Farbe Weiss */
#define COLOR_WHITE   {1.0f, 1.0f, 1.0f}

/** Die verwendeten Farben */
enum e_Color {
    COLOR_RED,
    COLOR_GREEN,
    COLOR_BLUE,
    COLOR_YELLOW,
    COLOR_MAGENTA,
    COLOR_CYAN,

    COLOR_FIRST = COLOR_RED,
    COLOR_LAST = COLOR_CYAN
};

/** Typdefinition fuer die verwendeten Farben gemaess des Enums */
typedef enum e_Color CGColor;


/* ---- Typedeklarationen - Side ---- */

/** Kollisionsseiten. */
enum e_Side { 
    sideNone, 
    sideLeft, 
    sideRight, 
    sideTop, 
    sideBottom 
};

/** Datentyp fuer Kollisionsseiten. */
typedef enum e_Side CGSide;


/* ---- Typedeklarationen - Player ---- */

/* struct mit den Spielerdaten */
struct s_Player {
    CGPoint2f paddlePosition;
    GLfloat paddleRotation;
    int lives;
    GLboolean isHuman;
};

/* Datentyp fuer die Spielerdaten */
typedef struct s_Player Player;


/* ---- Typedeklarationen - Paddle ---- */

/** Bewegungsrichtungen des Schlaegers. */
enum e_PaddleDirection
{ dirUp, dirDown };

/** Datentyp fuer Bewegungsrichtungen. */
typedef enum e_PaddleDirection PaddleDirection;

/** Rotationsrichtungen des Schlaegers. */
enum e_PaddleRotation
{ rotClockwise, rotCounterClockwise };

/** Datentyp fuer Rotationsrichtungen. */
typedef enum e_PaddleRotation PaddleRotation;


/* ---- Typedeklarationen - Ball ---- */

/* Die Form des Balls. */
enum e_BallShape {
    BALL_SHAPE_CIRCLE,
    BALL_SHAPE_TRIANGLE,
    BALL_SHAPE_STAR,

    BALL_SHAPE_FIRST = BALL_SHAPE_CIRCLE,
    BALL_SHAPE_LAST = BALL_SHAPE_STAR
};

/** Datentyp fuer die Form des Balls. */
typedef enum e_BallShape BallShape;

/* struct zur Verwaltung der Spielballdaten. */
struct s_Ball {
    /** der Mittelpunkt des Spielballs */
    CGPoint2f position;
    /** der normierte Richtungsvektor des Spielballs */
    CGVector2f direction;
    /** die Geschwindigkeit des Spielballs */
    GLfloat speed;
    /** die Rotation des Balls */
    GLfloat rotation;
    /** die Farbe des Spielballs */
    CGColor3f color;
    /** die Form des Balls */
    BallShape shape;    
};

/** Datentyp zur Verwaltung der Spielballdaten. */
typedef struct s_Ball Ball;

/* Konstante zur Initialisierung des Spielballs. */
#define BALL_EMPTY {POINT_ZERO, VECTOR_ZERO, 0.0f, 0.0f, COLOR_WHITE, BALL_SHAPE_CIRCLE}


/* ---- Typedeklarationen - Ball ---- */

/* struct zur Verwaltung der Hilfslinien. */
struct s_Guidelines {
    GLboolean enabled;
    CGPoint2f position;

    CGVector2f input;
    CGVector2f normal;
    CGVector2f output;
    CGVector2f mirrorNorm;
};

/** Datentyp fuer die Hilfslinien. */
typedef struct s_Guidelines Guidelines;

/* Konstante zur Initialisierung der Hilfslinien. */
#define GUIDELINES_EMPTY {GL_FALSE, POINT_ZERO, VECTOR_ZERO, VECTOR_ZERO, VECTOR_ZERO, VECTOR_ZERO}

#endif
