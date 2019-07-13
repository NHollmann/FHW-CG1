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
#include "scene.h"
#include "types.h"
#include "debugGL.h"

/* ---- Konstanten ---- */

/* Die Helligkeit der Umgebungsbeleuchtung der Szene. */
#define AMBIENT_BRIGHTNESS (0.1f)

/* ---- Globale Variablen ---- */

/** 
 * Speichert die erste Displayliste und macht
 * damit alle weiteren Zugreifbar.
 */
GLuint g_renderObjects = 0;

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
 * @param width die Breite der Fuge
 */
static void drawJoint(GLboolean vertical, float length, float width)
{
	glPushMatrix();
	{
		if (vertical) {
			glScalef(width, length, 1.0f);
		} else {
			glScalef(length, width, 1.0f);
		}
		drawSquare(0);
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
			drawSquare(0);
		}
		glPopMatrix();

		// Rechter Arm
		glPushMatrix();
		{
			glTranslatef(0.2f, 0.03f, 0.0f);
			glRotatef(8.0f, 0.0f, 0.0f, 1.0f);
			glScalef(0.1f, 0.26f, 1.0f);
			drawSquare(0);
		}
		glPopMatrix();

		// Linker Arm
		glPushMatrix();
		{
			glTranslatef(-0.2f, 0.03f, 0.0f);
			glRotatef(-8.0f, 0.0f, 0.0f, 1.0f);
			glScalef(0.1f, 0.26f, 1.0f);
			drawSquare(0);
		}
		glPopMatrix();

		// Rechtes Bein
		glPushMatrix();
		{
			glTranslatef(0.1f, -0.25f, 0.0f);
			glRotatef(8.0f, 0.0f, 0.0f, 1.0f);
			glScalef(0.1f, 0.26f, 1.0f);
			drawSquare(0);
		}
		glPopMatrix();

		// Linkes Bein
		glPushMatrix();
		{
			glTranslatef(-0.1f, -0.25f, 0.0f);
			glRotatef(-8.0f, 0.0f, 0.0f, 1.0f);
			glScalef(0.1f, 0.26f, 1.0f);
			drawSquare(0);
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
			drawSquare(0);
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
			drawSquare(0);
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
		setDiffuseMaterial(0.63f, 0.137f, 0.168f);
		setSpecularMaterial(0.3f, 0.3f, 0.3f, 15.0f);

		drawSquare(TILE_SUBDIVS);


		setDiffuseMaterial(0.9f, 0.9f, 0.9f);
		setSpecularMaterial(0.1f, 0.1f, 0.1f, 1.0f);

		// Horizontale Fugen
		glPushMatrix();
		{
			glTranslatef(0.0f, -0.5f, 0.001f);

			for (int i = 0; i < 3; i++) 
			{
				glTranslatef(0.0f, 0.25f, 0.0f);
				drawJoint(GL_FALSE, 1.0f, 0.03f);
			}
		}
		glPopMatrix();

		// Horizontale Abschluss Fugen
		glPushMatrix();
		{
			glTranslatef(0.0f, -0.5f + 0.015f / 2.0f, 0.001f);
			drawJoint(GL_FALSE, 1.0f, 0.015f);

			glTranslatef(0.0f,  1.0f - 0.015f, 0.0f);
			drawJoint(GL_FALSE, 1.0f, 0.015f);
		}
		glPopMatrix();

		// Vertikale Fugen
		glPushMatrix();
		{
			glTranslatef(0.0f, -0.5f - 0.125f, 0.001f);
			for (int i = 1; i <= 4; i++) 
			{
				glTranslatef(0.0f, 0.25f, 0.0f);

				glPushMatrix();
				{
					glTranslatef(-0.5f - ((i % 2) * 0.167f), 0.0f, 0.0f);
					for (int j = 0; j < (i % 2) + 2; j++) 
					{
						glTranslatef(0.33f, 0.0f, 0.0f);
						drawJoint(GL_TRUE, 0.22f, 0.03f);
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
	setDiffuseMaterial(0.5f, 0.5f, 0.5f);
	setSpecularMaterial(0.1f, 0.1f, 0.1f, 10.0f);

	glPushMatrix();
	{
		drawSquare(TILE_SUBDIVS);
	}
	glPopMatrix();
}

/**
 * Rendert das Zielfeld.
 */
static void drawTileGoal(void)
{
	setDiffuseMaterial(0.2f, 0.8f, 0.3f);
	setSpecularMaterial(0.5f, 0.7f, 0.5f, 30.0f);

	glPushMatrix();
	{
		drawSquare(TILE_SUBDIVS);
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
		setDiffuseMaterial(0.2f, 0.5f, 0.8f);
		setSpecularMaterial(0.4f, 0.4f, 0.4f, 80.0f);

		drawSquare(TILE_SUBDIVS);

		setDiffuseMaterial(0.0f, 0.2f, 0.8f);
		setSpecularMaterial(0.8f, 0.8f, 1.0f, 120.0f);
		glTranslatef(0.0f, 0.0f, 0.001f);
		drawWaterWave(-0.45f, -0.3f, 0.0f);
		drawWaterWave(-0.48f,  0.0f, 0.0f);
		drawWaterWave(-0.40f,  0.27f, 0.0f);

		drawWaterWave( 0.021f, -0.25f, 0.0f);
		drawWaterWave( 0.024f,  0.05f, 0.0f);
		drawWaterWave( 0.008f,  0.37f, 0.0f);
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
		drawSquare(0);

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
		setDiffuseMaterial(1.0f, 1.0f, 1.0f);
		setSpecularMaterial(0.0f, 0.0f, 0.0f, 0.0f);
		drawSquare(TILE_SUBDIVS);

		setDiffuseMaterial(0.4f, 0.4f, 0.4f);
		setSpecularMaterial(1.0f, 1.0f, 1.0f, 120.0f);
		for (int i = 0; i < 50; i++) {
			float randPosX = ((float)(rand() % 30) / (float) 30) - 0.5f;
			float randPosY = ((float)(rand() % 30) / (float) 30) - 0.5f;
			
			glPushMatrix();
			{
				glTranslatef(randPosX, randPosY, 0.001f);
				glScalef(0.03f, 0.03f, 1.0f);
				drawSquare(0);
			}
			glPopMatrix();
		}
	}
	glPopMatrix();
}

/* ---- Oeffentliche Funktionen ---- */

void drawSquare(int subDivs)
{
	int x, y;

	if (getNormalState())
	{
		glBegin(GL_LINES);
		{
			glVertex3f(0.0f, 0.0f, 0.0f);
			glVertex3f(0.0f, 0.0f, 1.0f);
		}
		glEnd();
	}

  	for (y = 0; y < subDivs + 1; y++)
    {
      	glBegin(GL_QUAD_STRIP);
		{
			glNormal3f(0.0f, 0.0f, 1.0f);
			for (x = 0; x <= subDivs + 1; x++)
			{
				glVertex3f(-0.5f + x / (subDivs + 1.0f),
							0.5f - y / (subDivs + 1.0f),
							0.0f);

				glVertex3f(-0.5f +    x    / (subDivs + 1.0f),
							0.5f - (y + 1) / (subDivs + 1.0f),
							0.0f);
			}
		}
      	glEnd();
    }
}

void drawWaterWave(float x, float y, float z) 
{
	glPushMatrix();
	{
		glTranslatef(x, y, z);
		glScalef(0.023f, 0.04f, 1.0f);

		drawSineWave(TAU * 3, 30);
	}
	glPopMatrix();
}

void initDisplayList(void) 
{
	if (g_renderObjects != 0)
	{
		glDeleteLists(g_renderObjects, (GLsizei) RO_SIZE);
	}

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

void setDiffuseMaterialAlpha(float r, float g, float b, float a)
{
	float matDiffuse[] = { r, g, b, a };
	float matAmbient[] = { 
		r * AMBIENT_BRIGHTNESS, 
		g * AMBIENT_BRIGHTNESS, 
		b * AMBIENT_BRIGHTNESS,
		a * AMBIENT_BRIGHTNESS 
	};

	glMaterialfv (GL_FRONT, GL_DIFFUSE, matDiffuse);
	glMaterialfv (GL_FRONT, GL_AMBIENT, matAmbient);

	glColor4fv(matDiffuse);
}

void setDiffuseMaterial(float r, float g, float b)
{
	setDiffuseMaterialAlpha(r, g, b, 1.0f);
}

void setSpecularMaterial(float r, float g, float b, float shiny)
{
	float matSpecular[] = { r, g, b };

	glMaterialfv (GL_FRONT, GL_SPECULAR, matSpecular);
	glMaterialfv (GL_FRONT, GL_SHININESS, &shiny);
}

