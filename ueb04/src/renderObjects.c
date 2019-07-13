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

// Anzahl der Komponenten, aus denen die Insel besteht.
#define ISLE_PARTS (12)

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
 * Zeichnet eine Sphere auf dem Wasser.
 */
static void drawWaterSphere(void)
{
	GLUquadricObj* quadric = gluNewQuadric();

	gluQuadricNormals(quadric, GLU_SMOOTH);
	gluQuadricTexture(quadric, GL_TRUE);

	glPushMatrix();
	{
		gluSphere(quadric, 0.01, 10, 10);
	}
	glPopMatrix();

	gluDeleteQuadric(quadric);
}

/** 
 * Zeichnet den Aussenring der Insel.
 */
static void drawIsleRing(void)
{
	glPushMatrix();
	{
		for (int i = 0; i < ISLE_PARTS; i++) 
		{
			glPushMatrix();
			{
				float factor = 2.0f * sinf((TAU / ISLE_PARTS) / 2.0f);

				glRotatef(i * (360.0f / ISLE_PARTS), 0.0f, 1.0f, 0.0f);
				glTranslatef(0.0f, -factor, 0.96f);
				
				glScalef(factor, factor * 2, factor);
				drawSquare(ISLE_SIDE_SUBDIVS);
			}
			glPopMatrix();
		}
	}
	glPopMatrix();
}

/** 
 * Zeichnet den Deckel der Insel.
 */
static void drawIsleTop(void)
{
	glPushMatrix();
	{
		glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);

		if (getNormalState())
		{
			glBegin(GL_LINES);
			{
				glVertex3f(0.0f, 0.0f, 0.0f);
				glVertex3f(0.0f, 0.0f, 1.0f);
			}
			glEnd();
		}

		glBegin(GL_TRIANGLE_FAN);
		{
			glNormal3f(0.0f, 0.0f, 1.0f);
			glTexCoord2f(0.5f, 0.5f);
			glVertex3f(0.0f, 0.0f, 0.0f);

			for (int i = 0; i <= ISLE_PARTS; i++)
			{
				float x = cosf((i + 0.5f) * (TAU / ISLE_PARTS));
				float y = sinf((i + 0.5f) * (TAU / ISLE_PARTS));

				glTexCoord2f((x + 1.0f) / 2.0f, (y + 1.0f) / 2.0f);
				glVertex3f(x, y, 0.0f);
			}
		}
		glEnd();
	}
	glPopMatrix();
}

/** 
 * Zeichnet die Sphere fuer die Skybox.
 */
static void drawSkySphere(void)
{
	GLUquadricObj* quadric = gluNewQuadric();
	gluQuadricNormals(quadric, GLU_SMOOTH);
	gluQuadricTexture(quadric, GL_TRUE);
	gluQuadricOrientation(quadric, GLU_INSIDE);

	glPushMatrix();
	{
		glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
		glRotatef(105.0f, 0.0f, 0.0f, 1.0f);
		
		gluSphere(quadric, 1.0, 32, 32);
	}
	glPopMatrix();

	gluDeleteQuadric(quadric);
}

/* ---- Oeffentliche Funktionen ---- */

void drawSquare(int subDivs)
{
	if (getNormalState())
	{
		glBegin(GL_LINES);
		{
			glVertex3f(0.0f, 0.0f, 0.0f);
			glVertex3f(0.0f, 0.0f, 1.0f);
		}
		glEnd();
	}

  	for (int y = 0; y < subDivs + 1; y++)
    {
      	glBegin(GL_QUAD_STRIP);
		{
			glNormal3f(0.0f, 0.0f, 1.0f);
			for (int x = 0; x <= subDivs + 1; x++)
			{
				glTexCoord2f(x / (subDivs + 1.0f), y / (subDivs + 1.0f));
				glVertex3f(-0.5f + x / (subDivs + 1.0f),
							0.5f - y / (subDivs + 1.0f),
							0.0f);

				glTexCoord2f(x / (subDivs + 1.0f), (y + 1) / (subDivs + 1.0f));
				glVertex3f(-0.5f +    x    / (subDivs + 1.0f),
							0.5f - (y + 1) / (subDivs + 1.0f),
							0.0f);
			}
		}
      	glEnd();
    }
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
				case RO_SPHERE:
					drawWaterSphere();
					break;
				case RO_ISLE_TOP:
					drawIsleTop();
					break;
				case RO_ISLE_RING:
					drawIsleRing();
					break;
				case RO_SKY:
					drawSkySphere();
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

