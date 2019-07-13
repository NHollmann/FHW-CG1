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
#include "renderObjects3D.h"
#include "renderObjects.h"
#include "types.h"
#include "debugGL.h"

/* ---- Globale Variablen ---- */

/** 
 * Speichert die erste Displayliste und macht
 * damit alle weiteren Zugreifbar.
 */
GLuint g_renderObjects3D = 0;

/* ---- Interne Funktionen ---- */

/**
 * Gibt die ID einer Displayliste anhand eines Objektes zurueck.
 * 
 * @param renderObject das gewuenschte Objekt (In)
 * @return die ID der Displayliste
 */
static GLuint getRenderObject3D(RenderObject3D renderObject)
{
	return g_renderObjects3D + renderObject;
}

/* Zeichnet einen Wuerfel mit 6 Oberflaechen. */
static void drawCube()
{
	/* Vorne */
	glPushMatrix();
	{
		glTranslatef(0.0f, 0.0f, 0.5f);
		drawSquare(0);
	}
	glPopMatrix();

	/* Rechts */
	glPushMatrix();
	{
		glTranslatef(0.5f, 0.0f, 0.0f);
		glRotatef(90, 0.0f, 1.0f, 0.0f);
		drawSquare(0);
	}
	glPopMatrix();

	/* Hinten */
	glPushMatrix();
	{
		glTranslatef(0.0f, 0.0f, -0.5f);
		glRotatef(180, 0.0f, 1.0f, 0.0f);
		drawSquare(0);
	}
	glPopMatrix();

	/* Links */
	glPushMatrix();
	{
		glTranslatef(-0.5f, 0.0f, 0.0f);
		glRotatef(270, 0.0f, 1.0f, 0.0f);
		drawSquare(0);
	}
	glPopMatrix();

	/* Oben */
	glPushMatrix();
	{
		glTranslatef(0.0f, 0.5f, 0.0f);
		glRotatef(-90, 1.0f, 0.0f, 0.0f);
		drawSquare(0);
	}
	glPopMatrix();

	/* Unten */
	glPushMatrix();
	{
		glTranslatef(0.0f, -0.5f, 0.0f);
		glRotatef(90, 1.0f, 0.0f, 0.0f);
		drawSquare(0);
	}
	glPopMatrix();
}

/**
 * Zeichnet den Boden fuer ein Tile.
 * 
 * @param object das RenderObject, fuer ein Bodentile.
 */
static void drawTileFloor(RenderObject object)
{
	glPushMatrix();
	{
		glTranslatef(0.0f, -0.5f, 0.0f);
		glRotatef(-90, 1.0f, 0.0f, 0.0f);
		renderObject(object);
	}
	glPopMatrix();
}

/**
 * Zeichnet ein Tile in Form eines Wuerfels.
 * 
 * @param object das RenderObject, das gerendert werden soll.
 */
static void drawTileCube(RenderObject object)
{
	/* Frontflaeche */
	glPushMatrix ();
	{
		glTranslatef (0.0f, 0.0f, 0.5f);
		renderObject(object);
	}
	glPopMatrix ();

	/* rechte Seitenflaeche */
	glPushMatrix ();
	{
		glRotatef (90.0f, 0.0f, 1.0f, 0.0f);
		glTranslatef (0.0f, 0.0f, 0.5f);
		renderObject(object);
	}
	glPopMatrix ();

	/* Rueckseitenflaeche */
	glPushMatrix ();
	{
		glRotatef (180.0f, 0.0f, 1.0f, 0.0f);
		glTranslatef (0.0f, 0.0f, 0.5f);
		renderObject(object);
	}
	glPopMatrix ();

	/* linke Seitenflaeche */
	glPushMatrix ();
	{
		glRotatef (270.0f, 0.0f, 1.0f, 0.0f);
		glTranslatef (0.0f, 0.0f, 0.5f);
		renderObject(object);
	}
	glPopMatrix ();

	/* Deckelflaeche */
	glPushMatrix ();
	{
		glRotatef (-90.0f, 1.0f, 0.0f, 0.0f);
		glTranslatef (0.0f, 0.0f, 0.5f);
		renderObject(object);
	}
	glPopMatrix ();
}

/**
 * Zeichnet den Boden fuer ein Sandsacktile. Die Sandsaecke muessen separat gerendert werden.
 */
static void drawTileSandfloor(void)
{
	setDiffuseMaterial(0.5f, 0.25f, 0.0f);
	setSpecularMaterial(0.0f, 0.0f, 0.0f, 0.0f);

	glPushMatrix();
	{
		glNormal3f(0.0f, 1.0f, 0.0f);
		glTranslatef(0.0f, -0.5f, 0.0f);
		glRotatef(-90, 1.0f, 0.0f, 0.0f);
		drawSquare(TILE_SUBDIVS);
	}
	glPopMatrix();
}

/**
 * Zeichnet die Sandsaecke fuer ein entsprechendes Tile. Der Boden muss separat gerendert werden.
 */
static void drawTileSandbag(void)
{
	GLUquadricObj* quadric = gluNewQuadric();
	gluQuadricNormals(quadric, GLU_SMOOTH);

	setDiffuseMaterial(1.0f, 0.7f, 0.4f);
	setSpecularMaterial(0.0f, 0.0f, 0.0f, 0.0f);
	
	/* Oberer Sandsack */
	glPushMatrix();
	{
		glTranslatef(0.0f, -0.15f, 0.0f);
		glScalef(0.2f, 0.15f, 0.2f);
		gluSphere(quadric, 1.0f, 10, 10);
	}
	glPopMatrix();

	/* Drei untere Sandsaecke */
	for (int i = 0; i < 3; i++)
	{
		glPushMatrix();
		{
			glRotatef(120.0f * i, 0.0f, 1.0f, 0.0f);
			glTranslatef(0.25f, -0.35f, 0.0f);
			glScalef(0.2f, 0.15f, 0.2f);
			gluSphere(quadric, 1.0f, 10, 10);
		}
		glPopMatrix();
	}

	gluDeleteQuadric(quadric);
}

/**
 * Zeichnet den Spieler fuer ein entsprechendes Tile. Der Boden muss separat gerendert werden.
 */
static void drawPlayer()
{
	setDiffuseMaterial(0.01f, 0.01f, 0.01f);
	setSpecularMaterial(0.4f, 0.4f, 0.4f, 10.0f);

	GLUquadricObj* quadric = gluNewQuadric();
	gluQuadricNormals(quadric, GLU_SMOOTH);

	/* Kopf */
	glPushMatrix();
	{
		glTranslatef(0.0f, 0.35f, 0.0f);
		glScalef(0.15f, 0.15f, 0.15f);
		gluSphere(quadric, 1.0f, 15, 15);
	}
	glPopMatrix();

	gluDeleteQuadric(quadric);

	/* Torso */
	glPushMatrix();
	{
		glScalef(0.35f, 0.4f, 0.2f);
		drawCube();
	}
	glPopMatrix();

	/* 1. Bein */
	glPushMatrix();
	{
		glTranslatef(0.1f, -0.3f, 0.0f);
		glScalef(0.1f, 0.3f, 0.2f);
		drawCube();
	}
	glPopMatrix();

	/* 2. Bein */
	glPushMatrix();
	{
		glTranslatef(-0.1f, -0.3f, 0.0f);
		glScalef(0.1f, 0.3f, 0.2f);
		drawCube();
	}
	glPopMatrix();

	/* 1. Arm */
	glPushMatrix();
	{
		glTranslatef(0.25f, 0.05f, 0.0f);
		glRotatef(10, 0.0f, 0.0f, 1.0f);
		glScalef(0.1f, 0.3f, 0.1f);
		drawCube();
	}
	glPopMatrix();

	/* 2. Arm */
	glPushMatrix();
	{
		glTranslatef(-0.25f, 0.05f, 0.0f);
		glRotatef(-10, 0.0f, 0.0f, 1.0f);
		glScalef(0.1f, 0.3f, 0.1f);
		drawCube();
	}
	glPopMatrix();
}

/**
 * Zeichnet die Wellen fuer das Wassertile. Die Waende muessen aufgrund der  
 * Transparenz nachtraeglich gerendert werden.
 */
static void drawTileWater(void)
{
	glPushMatrix();
	{
		setDiffuseMaterial(0.2f, 0.4f, 1.0f);
		setSpecularMaterial(0.8f, 0.8f, 1.0f, 120.0f);

		drawWaterWave(-0.45f, -0.3f, 0.2f);
		drawWaterWave(-0.48f,  0.0f, 0.0f);
		drawWaterWave( 0.008f,  0.37f, 0.2f);

		glRotatef(90, 0.0f, 1.0f, 0.0f);

		drawWaterWave( 0.021f, -0.25f, 0.0f);
		drawWaterWave( 0.024f,  0.05f, -0.2f);
		
		drawWaterWave(-0.40f,  0.27f, 0.0f);
	}
	glPopMatrix();
}

/* ---- Oeffentliche Funktionen ---- */

void initDisplayList3D(void) 
{
	if (g_renderObjects3D != 0)
	{
		glDeleteLists(g_renderObjects3D, (GLsizei) RO_3D_SIZE);
	}

    // Alle benoetigten Listen auf einmal erzeugen
	g_renderObjects3D = glGenLists((GLsizei) RO_3D_SIZE);
	if (g_renderObjects3D != 0) {
		for (GLuint index = 0; index < RO_3D_SIZE; index++)
		{
            // Displayliste bereit machen
			glNewList(g_renderObjects3D + index, GL_COMPILE);
			
			switch (index)
			{
				case RO_3D_PLAYER:
					drawPlayer();
					break;
				case RO_3D_WALL:
					drawTileCube(RO_WALL);
					break;
				case RO_3D_FREE:
					drawTileFloor(RO_FREE);
					break;
				case RO_3D_GOAL:
					drawTileFloor(RO_GOAL);
					break;
				case RO_3D_WATER:
					drawTileWater();
					break;
				case RO_3D_SANDFLOOR:
					drawTileSandfloor();
					break;
				case RO_3D_SANDBAG:
					drawTileSandbag();
					break;
				case RO_3D_SUGAR:
					drawTileCube(RO_SUGAR);
					break;
			}

            // Displayliste abschliessen
			glEndList();
		}
	} else {
		CG_ERROR(("Konnte 3D Displaylisten nicht erzeugen\n"));
	}
}

void renderObject3D(RenderObject3D object)
{
    glCallList(getRenderObject3D(object));
}
