/**
 * @file
 * Darstellungs-Modul.
 * Das Modul kapselt die Rendering-Funktionalitaet (insbesondere der OpenGL-
 * Aufrufe) des Programms.
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
#include "scene.h"
#include "logic.h"
#include "types.h"
#include "vector.h"
#include "debugGL.h"

/* ---- Interne Funktionen ---- */

/**
 * Zeichnet ein Rechteck mit der Breite und Hoehe 1.
 */
static void drawSquare(void)
{
	glBegin(GL_QUADS);
	{
		glVertex2f(-0.5, -0.5);
		glVertex2f(0.5, -0.5);
		glVertex2f(0.5, 0.5);
		glVertex2f(-0.5, 0.5);
	}
	glEnd();
}

/**
 * Zeichnet ein regulaeres Polygon mit pointCount Ecken und
 * stellt somit naeherungsweise einen Einheitskreis dar.
 *
 * @param pointCount Zahl der Ecken. Je hoeher desto "runder" der Kreis. (In)
 */
static void drawCircle(int pointCount)
{
	glBegin(GL_POLYGON);
	{
		for (int i = 0; i < pointCount; i++)
		{
			glVertex2f(cosf(TAU / pointCount * i),
					   sinf(TAU / pointCount * i));
		}
	}
	glEnd();
}

/**
 * Zeichnet einen Stern mit der Breite und Hoehe 1.
 */
static void drawStar(void)
{
	glBegin(GL_TRIANGLE_FAN);
	{
		// Mittelpunkt
		glVertex2f(0.f, 0.f);

		for (int i = 0; i < STAR_CORNER_COUNT; i++)
		{
			glVertex2f(cosf(TAU / STAR_CORNER_COUNT * i) * STAR_INNER_RADIUS,
					   sinf(TAU / STAR_CORNER_COUNT * i) * STAR_INNER_RADIUS);

			glVertex2f(cosf(TAU / STAR_CORNER_COUNT * (i + .5f)),
					   sinf(TAU / STAR_CORNER_COUNT * (i + .5f)));
		}

		// Abschluss
		glVertex2f(STAR_INNER_RADIUS, // cos(0) * STAR_INNER_RADIUS
				   0.f);			  // sin(0) * STAR_INNER_RADIUS
	}
	glEnd();
}

/**
 * Zeichnet den Spielball an seiner entsprechenden Position mit seiner 
 * entsprechenden Rotation. Je nach Modus ist dieser entweder ein Kreis, 
 * ein Dreieick oder ein Stern. 
 *
 * @param position die Position des Balls. (In)
 * @param rotation die Rotation des Balls. (In)
 */
static void drawBall(CGPoint2f position, GLfloat rotation)
{
	glColor3fv((GLfloat *)(&getBall()->color));

	glPushMatrix();
	{
		// Positionierung
		glTranslatef(position[0], position[1], 0.0f);
		glRotatef(rotation, 0.0f, 0.0f, 1.0f);
		glScalef(BALL_SIZE, BALL_SIZE, 1.0f);

		// Form bestimmen
		switch (getBall()->shape)
		{
		case BALL_SHAPE_CIRCLE:
			drawCircle(CIRCLE_POINT_COUNT);
			break;
		case BALL_SHAPE_TRIANGLE:
			drawCircle(3);
			break;
		case BALL_SHAPE_STAR:
			drawStar();
			break;
		}
	}
	glPopMatrix();
}

/**
 * Zeichnet einen Spielschlaeger an seiner entsprechenden Position mit seiner 
 * entsprechenden Rotation. 
 * 
 * @param position die Position des Spielschlaegers. (In)
 * @param rotation die Rotation des Spielschlaegers. (In)
 */
static void drawPaddle(CGPoint2f position, GLfloat rotation)
{
	glColor3f(1.0f, 1.0f, 1.0f);

	glPushMatrix();
	{
		glTranslatef(position[0], position[1], 0.0f);
		glRotatef(rotation, 0.0f, 0.0f, 1.0f);
		glScalef(PADDLE_THICKNESS, PADDLE_HEIGHT, 1.0f);

		drawSquare();
	}
	glPopMatrix();
}

/**
 * Zeichnet einen Teil der Spielfeldbegrenzung.
 *
 * @param posY Position der Begrenzung auf der Y-Achse. (In)
 */
static void drawBorder(GLfloat posY)
{
	glColor3f(1.0f, 1.0f, 1.0f);

	glPushMatrix();
	{
		glTranslatef(0.0f, posY, 0.0f);
		glScalef(BORDER_RIGHT - BORDER_LEFT, BORDER_THICKNESS, 1.0f);

		drawSquare();
	}
	glPopMatrix();
}

/**
 * Zeichnet einen Vektor.
 * 
 * @param position die Position des Vektors (In)
 * @param direction die Richtung des Vektors (In)
 */
static void drawVector(CGPoint2f position, CGVector2f direction) {
	CGVector2f end = {0, 0};
	vector_add(end, position);
	vector_add(end, direction);

	glBegin(GL_LINES);
	{
		glVertex2fv(position);
		glVertex2fv(end);
	}
	glEnd();
}

/**
 * Zeichnet Hilfslinien zum Debuggen des Ballrueckstosswinkels.
 */
static void drawDebugGuideline(void) 
{
	Guidelines* guidelines = getGuidelines();

	if (guidelines->enabled) 
	{	
		glColor3f(0.0f, 1.0f, 0.0f);
		drawVector(guidelines->position, guidelines->input);

		glColor3f(1.0f, 1.0f, 0.0f);
		drawVector(guidelines->position, guidelines->normal);

		glColor3f(1.0f, 0.0f, 0.0f);
		drawVector(guidelines->position, guidelines->output);

		CGVector2f mirrorStart = {0, 0};
		vector_add(mirrorStart, guidelines->position);
		vector_add(mirrorStart, guidelines->input);

		glColor3f(0.0f, 0.0f, 1.0f);
		drawVector(mirrorStart, guidelines->mirrorNorm);
	}
}

/* ---- Oeffentliche Funktionen ---- */

void drawScene(void)
{
	drawBall(getBall()->position, getBall()->rotation);

	drawPaddle(getHumanPlayer()->paddlePosition, getHumanPlayer()->paddleRotation);
	drawPaddle(getBotPlayer()->paddlePosition, getBotPlayer()->paddleRotation);

	/* Rahmen zeichnen */
	drawBorder(BORDER_TOP);
	drawBorder(BORDER_BOTTOM);

	drawDebugGuideline();

	/* DEBUG-Ausgabe */
	GLSTATE;
}

void setWireframeMode(GLboolean wireframe)
{
	if (wireframe)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}

int initScene(void)
{
	/* Setzen der Farbattribute */
	/* Hintergrundfarbe */
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	/* Zeichenfarbe */
	glColor3f(1.0f, 1.0f, 1.0f);

	/* Backface Culling aktivieren */
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	/* Linienbreite */
	glLineWidth(1.f);

	/* Alles in Ordnung? */
	return (GLGETERROR == GL_NO_ERROR);
}
