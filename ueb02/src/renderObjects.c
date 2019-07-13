/**
 * @file
 * Darstellungs-Modul.
 * Das Modul kapselt die Rendering-Funktionalitaet fuer Displaylisten und haufig verwendete
 * Grundobjekte.
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
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include <stdlib.h>

/* ---- Eigene Header einbinden ---- */
#include "renderObjects.h"
#include "types.h"
#include "debugGL.h"

/* ---- Globale Variablen ---- */

/** 
 * Speichert die erste Displayliste und macht
 * damit alle weiteren Zugreifbar.
 */
GLuint g_renderObjects;

/* ---- Interne Funktionen ---- */

/**
 * Gibt die ID einer Displayliste anhand eines Objektes zurueck.
 * 
 * @param renderObject das gewuenschte Objekt (In)
 * @return die ID der Displayliste
 */
static GLuint getRenderObject(RenderObject renderObject)
{
	return g_renderObjects + renderObject;
}

/**
 * Rendert eine Fuge fuer einen Mauerblock.
 * 
 * @param vertical soll die Fuge vertikal oder horizontal sein? (In)
 * @param length die Laenge der Fuge (In)
 */
static void drawJoint(GLboolean vertical, float length)
{
	glPushMatrix();
	{
		if (vertical) {
			glScalef(0.03f, length, 1.0f);
		} else {
			glScalef(length, 0.03f, 1.0f);
		}
		drawSquare();
	}
	glPopMatrix();
}

/**
 * Rendert eine Sinuswelle.
 * 
 * @param degrees der maximale Winkel der Welle (In)
 * @param slices die Anzahl der Unterteilungen in der Welle (In)
 */
static void drawSineWave(float degrees, int slices) 
{
	glBegin(GL_LINE_STRIP);

	for (int i = 0; i <= slices; i++) {
		float currentDegree = (degrees / (float) slices) * (float) i;
		glVertex2f(currentDegree, sinf(currentDegree));
	}

	glEnd();
}

/**
 * Rendert eine einzelne Wasserwelle.
 * 
 * @param x die X-Position (In)
 * @param y die Y-Position (In)
 */
static void drawWaterWave(float x, float y) 
{
	glPushMatrix();
	{
		glTranslatef(x, y, 0.0f);
		glScalef(0.023f, 0.04f, 1.0f);

		drawSineWave(TAU * 3, 30);
	}
	glPopMatrix();
}

/**
 * Rendert einen einzelnen Sandsack.
 * 
 * @param quadric eine Quadric fuer das Zeichnen von Kreisen (In)
 * @param x die X-Position (In)
 * @param y die Y-Position (In)
 */
static void drawSandbag(GLUquadricObj* quadric, float x, float y) 
{
	glPushMatrix();
	{
		glTranslatef(x, y, 0.0f);
		glScalef(0.2f, 0.1f, 1.0f);

		glColor3f(0.3f, 0.15f, 0.0f);
		gluDisk(quadric, 0, 1.0f, 32, 1);

		glScalef(0.85f, 0.85f, 1.0f);

		glColor3f(8.0f, 0.7f, 0.4f);
		gluDisk(quadric, 0, 1.0f, 32, 1);
	}
	glPopMatrix();
}

/**
 * Rendert den Spielcharakter.
 */
static void drawPlayer(void) 
{
	GLUquadricObj* quadric = gluNewQuadric();

	glPushMatrix();
	{
		glColor3f(0.0f, 0.0f, 0.0f);

		// Kopf
		glPushMatrix();
		{
			glTranslatef(0.0f, 0.32f, 0.0f);
			glScalef(0.25f, 0.25f, 1.0f);
			gluDisk(quadric, 0, 0.5f, 32, 1);
		}
		glPopMatrix();

		// Rumpf
		glPushMatrix();
		{
			glScalef(0.25f, 0.35f, 1.0f);
			drawSquare();
		}
		glPopMatrix();

		// Rechter Arm
		glPushMatrix();
		{
			glTranslatef(0.2f, 0.03f, 0.0f);
			glRotatef(8.0f, 0.0f, 0.0f, 1.0f);
			glScalef(0.1f, 0.26f, 1.0f);
			drawSquare();
		}
		glPopMatrix();

		// Linker Arm
		glPushMatrix();
		{
			glTranslatef(-0.2f, 0.03f, 0.0f);
			glRotatef(-8.0f, 0.0f, 0.0f, 1.0f);
			glScalef(0.1f, 0.26f, 1.0f);
			drawSquare();
		}
		glPopMatrix();

		// Rechtes Bein
		glPushMatrix();
		{
			glTranslatef(0.1f, -0.25f, 0.0f);
			glRotatef(8.0f, 0.0f, 0.0f, 1.0f);
			glScalef(0.1f, 0.26f, 1.0f);
			drawSquare();
		}
		glPopMatrix();

		// Linkes Bein
		glPushMatrix();
		{
			glTranslatef(-0.1f, -0.25f, 0.0f);
			glRotatef(-8.0f, 0.0f, 0.0f, 1.0f);
			glScalef(0.1f, 0.26f, 1.0f);
			drawSquare();
		}
		glPopMatrix();
	}
	glPopMatrix();

	gluDeleteQuadric(quadric);
}

/**
 * Rendert das Topping fuer einen Kuchen.
 * 
 * @param quadric eine Quadric fuer das Zeichnen von Kreisen (In)
 */
static void drawTopping(GLUquadricObj* quadric)
{
	glColor3f(0.73f, 0.33f, 0.85f);
	glPushMatrix();
	{
		glTranslatef(0.0f, 0.05f, 0.0f);
		glScalef(0.4f, 0.25f, 1.0f);

		for (int i = 0; i < 8; i++) 
		{
			glPushMatrix();
			{
				float angle = (float)i * (TAU / 8.0f) + (TAU / 16.0f);
				glTranslatef(cosf(angle) * 0.4f, sinf(angle) * 0.4f, 0.0f);
				glScalef(0.08f, 0.09f, 1.0f);
				gluDisk(quadric, 0, 0.5f, 32, 1);
			}
			glPopMatrix();
		}
	}
	glPopMatrix();
}

/**
 * Rendert einen Kuchen.
 */
static void drawCake(void)
{
	GLUquadricObj* quadric = gluNewQuadric();

	glPushMatrix();
	{
		// Boden
		glColor3f(0.55f, 0.27f, 0.07f);
		glPushMatrix();
		{
			glTranslatef(0.0f, -0.06f, 0.0f);
			glScalef(0.4f, 0.25f, 1.0f);
			gluDisk(quadric, 0, 0.5f, 32, 1);
		}
		glPopMatrix();

		// Mitte
		glColor3f(0.8f, 0.52f, 0.25f);
		glPushMatrix();
		{
			glTranslatef(0.0f, -0.04f, 0.0f);
			glScalef(0.4f, 0.25f, 1.0f);
			gluDisk(quadric, 0, 0.5f, 32, 1);
		}
		glPopMatrix();

		// Mitte
		glPushMatrix();
		{
			glScalef(0.4f, 0.1f, 1.0f);
			drawSquare();
		}
		glPopMatrix();

		// Oben
		glColor3f(1.0f, 1.0f, 1.0f);
		glPushMatrix();
		{
			glTranslatef(0.0f, 0.05f, 0.0f);
			glScalef(0.4f, 0.25f, 1.0f);
			gluDisk(quadric, 0, 0.5f, 32, 1);
		}
		glPopMatrix();

		drawTopping(quadric);

		// Kerze
		glColor3f(1.0f, 0.0f, 0.0f);
		glPushMatrix();
		{
			glTranslatef(0.0f, 0.12f, 0.0f);
			glScalef(0.02f, 0.15f, 1.0f);
			drawSquare();
		}
		glPopMatrix();

		// Feuer
		glColor3f(1.0f, 0.8f, 0.05f);
		glPushMatrix();
		{
			glTranslatef(0.0f, 0.22f, 0.0f);
			glScalef(0.03f, 0.03f, 1.0f);
			gluDisk(quadric, 0, 0.5f, 32, 1);
		}
		glPopMatrix();
	}
	glPopMatrix();

	gluDeleteQuadric(quadric);
}

/**
 * Rendert eine Wand.
 */
static void drawTileWall(void)
{
	glPushMatrix();
	{
		glColor3f(0.63f, 0.137f, 0.168f);
		drawSquare();

		glColor3f(0.9f, 0.9f, 0.9f);

		// Horizontale Fugen
		glPushMatrix();
		{
			glTranslatef(0.0f, -0.5f, 0.0f);
			for (int i = 0; i < 3; i++) 
			{
				glTranslatef(0.0f, 0.25f, 0.0f);
				drawJoint(GL_FALSE, 1.0f);
			}
		}
		glPopMatrix();

		// Vertikale Fugen
		glPushMatrix();
		{
			glTranslatef(0.0f, -0.5f - 0.125f, 0.0f);
			for (int i = 1; i <= 4; i++) 
			{
				glTranslatef(0.0f, 0.25f, 0.0f);

				glPushMatrix();
				{
					glTranslatef(-0.5f - ((i % 2) * 0.167f), 0.0f, 0.0f);
					for (int j = 0; j < (i % 2) + 2; j++) 
					{
						glTranslatef(0.33f, 0.0f, 0.0f);
						drawJoint(GL_TRUE, 0.25f);
					}
				}
				glPopMatrix();
			}
		}
		glPopMatrix();
	}
	glPopMatrix();
}

/**
 * Rendert ein leeres Feld.
 */
static void drawTileFree(void)
{
	glPushMatrix();
	{
		glColor3f(0.5f, 0.5f, 0.5f);
		drawSquare();
	}
	glPopMatrix();
}

/**
 * Rendert das Zielfeld.
 */
static void drawTileGoal(void)
{
	glPushMatrix();
	{
		glColor3f(0.2f, 0.8f, 0.3f);
		drawSquare();
	}
	glPopMatrix();
}

/**
 * Rendert das Wasserfeld.
 */
static void drawTileWater()
{
	glPushMatrix();
	{
		glColor3f(0.2f, 0.5f, 0.8f);
		drawSquare();

		glColor3f(0.0f, 0.2f, 0.8f);
		drawWaterWave(-0.45f, -0.3f);
		drawWaterWave(-0.48f,  0.0f);
		drawWaterWave(-0.40f,  0.27f);

		drawWaterWave( 0.021f, -0.25f);
		drawWaterWave( 0.024f,  0.05f);
		drawWaterWave( 0.008f,  0.37f);
	}
	glPopMatrix();
}

/**
 * Rendert das Feld mit den Sandsaecken.
 */
static void drawTileSandbag()
{
	GLUquadricObj* quadric = gluNewQuadric();

	glPushMatrix();
	{
		glColor3f(0.5f, 0.25f, 0.0f);
		drawSquare();

        // Unterste Reihe
		drawSandbag(quadric, -0.28f, -0.26f);
		drawSandbag(quadric,  0.28f, -0.26f);
		drawSandbag(quadric,  0.0f,  -0.26f);

        // Mittlere Reihe
		drawSandbag(quadric, -0.12f, -0.12f);
		drawSandbag(quadric,  0.12f, -0.12f);

        // Oberste Reihe
		drawSandbag(quadric,  0.0f,   0.01f);
	}
	glPopMatrix();

	gluDeleteQuadric(quadric);
}

/**
 * Rendert einen Zuckerwuerfel.
 */
static void drawTileSugar()
{
	glPushMatrix();
	{
		glColor3f(1.0f, 1.0f, 1.0f);
		drawSquare();

		glColor3f(0.4f, 0.4f, 0.4f);
		for (int i = 0; i < 30; i++) {
			float randPosX = ((float)(rand() % 30) / (float) 30) - 0.5f;
			float randPosY = ((float)(rand() % 30) / (float) 30) - 0.5f;
			
			glPushMatrix();
			{
				glTranslatef(randPosX, randPosY, 0.0f);
				glScalef(0.02f, 0.02f, 1.0f);
				drawSquare();
			}
			glPopMatrix();
		}
	}
	glPopMatrix();
}

/* ---- Oeffentliche Funktionen ---- */

void drawSquare(void)
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

void initDisplayList(void) 
{
    // Alle benoetigten Listen auf einmal erzeugen
	g_renderObjects = glGenLists((GLsizei) RO_SIZE);
	if (g_renderObjects != 0) {
		for (GLuint index = 0; index < RO_SIZE; index++)
		{
            // Displayliste bereit machen
			glNewList(g_renderObjects + index, GL_COMPILE);
			
			switch (index)
			{
				case RO_PLAYER:
					drawPlayer();
					break;
				case RO_CAKE:
					drawCake();
					break;
				case RO_WALL:
					drawTileWall();
					break;
				case RO_FREE:
					drawTileFree();
					break;
				case RO_GOAL:
					drawTileGoal();
					break;
				case RO_WATER:
					drawTileWater();
					break;
				case RO_SANDBAG:
					drawTileSandbag();
					break;
				case RO_SUGAR:
					drawTileSugar();
					break;
			}

            // Displayliste abschliessen
			glEndList();
		}
	} else {
		CG_ERROR(("Konnte Displaylisten nicht erzeugen\n"));
	}
}

void renderObject(RenderObject object)
{
    glCallList(getRenderObject(object));
}
