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

/* ---- Eigene Header einbinden ---- */
#include "scene.h"
#include "renderObjects.h"
#include "logic.h"
#include "debugGL.h"
#include "water.h"
#include "texture.h"

/* ---- Konstanten ---- */

/* Das Sonnenlicht fuer die Beleuchtung der Welt */
#define LIGHT_SUN (GL_LIGHT0)

/* Das Punktlicht */
#define LIGHT_POINTLIGHT (GL_LIGHT1)

/* ---- Globale Daten ---- */

static SceneFlags g_sceneFlags = SCENE_FLAGS_DEFAULT;

/* ---- Interne Funktionen ---- */

/** 
 * Initialisiert die Lichter.
 */
static void initLight(void)
{
	/* Globales, ambientes Licht */
	float globalAmbient[] = {1.0f, 1.0f, 1.0f, 1.0f};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);

	/* Sonnenlicht (gerichtetes Licht) */
	float worldlightColorAmbDif[] = {1.0, 1.0, 1.0, 1.0};
	float worldlightColorSpec[] = {1.0, 1.0, 1.0, 1.0};
	glLightfv(LIGHT_SUN, GL_AMBIENT, worldlightColorAmbDif);
	glLightfv(LIGHT_SUN, GL_DIFFUSE, worldlightColorAmbDif);
	glLightfv(LIGHT_SUN, GL_SPECULAR, worldlightColorSpec);
	glEnable(LIGHT_SUN);

	/* Punktlicht (Punktlicht) */
	float spotlightColorAmbDif[] = {1.0, 1.0, 0.8, 1.0};
	float spotlightColorSpec[] = {1.0, 1.0, 1.0, 1.0};
	glLightfv(LIGHT_POINTLIGHT, GL_AMBIENT, spotlightColorAmbDif);
	glLightfv(LIGHT_POINTLIGHT, GL_DIFFUSE, spotlightColorAmbDif);
	glLightfv(LIGHT_POINTLIGHT, GL_SPECULAR, spotlightColorSpec);
	glEnable(LIGHT_POINTLIGHT);
}

/**
 * Rendert die Skymap entsprechend der Augposition.
 * 
 * @param eyeX, eyeY, eyeZ Koordinaten des Auges (In)
 */
static void renderSkymap(GLfloat eyeX, GLfloat eyeY, GLfloat eyeZ)
{
	glPushMatrix();
	{
		glTranslatef(eyeX, eyeY, eyeZ);
		
		if (g_sceneFlags.textures)
		{	
			glEnable(GL_TEXTURE_2D);
			glColor3f(1.0f, 1.0f, 1.0f);
		}
		else
		{
			glColor3f(0.0f, 0.0f, 0.0f);
		}		

		glDisable(GL_DEPTH_TEST);
		bindTexture(texSky);
		
		renderObject(RO_SKY);

		glEnable(GL_DEPTH_TEST);
		glDisable(GL_TEXTURE_2D);
	}
	glPopMatrix();
}

/**
 * Berechnet die Augenkoordinaten.
 * 
 * @param gamestate der Spielzustand (In)
 * @param eyeX, eyeY, eyeZ Koordinaten des Auges (Out)
 */
static void getEyePosition(Gamestate *gamestate, GLfloat *eyeX, GLfloat *eyeY, GLfloat *eyeZ)
{
	GLfloat radius = gamestate->camera.radius;
	GLfloat polar = TO_RADIANS(gamestate->camera.polarAngle);
	GLfloat azimuth = TO_RADIANS(gamestate->camera.azimuthAngle);

	*eyeX = radius * sinf(azimuth) * cosf(polar);
	*eyeY = radius * cosf(azimuth);
	*eyeZ = radius * sinf(azimuth) * sinf(polar);
}

/**
 * Zeichnet das Wasser in der Welt.
 * 
 * @param gamestate der Spielzustand (In)
 */
static void drawWaterInWorld(Gamestate *gamestate)
{
	glPushMatrix();
	{
		glScalef(10.0f, 10.0f, 10.0f);

		// Wasserbaelle
		if (g_sceneFlags.showSpheres) 
		{
			drawWaterSpheres(&gamestate->grid);
		}

		// Wasseroberflaeche
		glColor3d(1.0f, 1.0f, 1.0f);
		setDiffuseMaterial(1.0f, 1.0f, 1.0f);
		setSpecularMaterial(0.0f, 0.0f, 0.0f, 0.0f);
		
		if (g_sceneFlags.textures)
		{
			glEnable(GL_TEXTURE_2D);
		} 
		else 
		{
			glEnable(GL_COLOR_MATERIAL);
		}
		
		bindTexture(texWater);
		
		drawWater(&gamestate->grid);
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_COLOR_MATERIAL);

		// Wasser Normalen
		if (g_sceneFlags.showNormals) 
		{
			glDisable(GL_LIGHTING);
			drawWaterNormals(&gamestate->grid);

			if (g_sceneFlags.lighting) 
			{
				glEnable(GL_LIGHTING);
			}
		}
	}
	glPopMatrix();
}

/**
 * Zeichnet die Insel in der Welt.
 */
static void drawIsle()
{
	glPushMatrix();
	{
		glTranslatef(0.0f, 0.75f, 0.0f);
		glScalef(1.5f, 1.5f, 1.5f);

		setSpecularMaterial(0.0f, 0.0f, 0.0f, 0.0f);

		if (g_sceneFlags.textures)
		{
			setDiffuseMaterial(1.0f, 1.0f, 1.0f);
			glColor3d(1.0f, 1.0f, 1.0f);
			glEnable(GL_TEXTURE_2D);
		} 
		else 
		{
			setDiffuseMaterial(0.8f, 0.5f, 0.0f);
			glColor3d(0.8f, 0.5f, 0.0f);
		}

		bindTexture(texLand);
		renderObject(RO_ISLE_TOP);

		bindTexture(texDirt);
		renderObject(RO_ISLE_RING);

		glDisable(GL_TEXTURE_2D);
	}
	glPopMatrix();
}

/**
 * Zeichnet die Welt.
 */
static void drawWorld(Gamestate *gamestate)
{
	drawWaterInWorld(gamestate);
	drawIsle();
}

/* ---- Oeffentliche Funktionen ---- */

void drawPickingScene()
{
	Gamestate *gamestate = getGamestate();

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	
	glPushMatrix();
	{
		/* Kameraposition */
		GLfloat eyeX, eyeY, eyeZ;
		getEyePosition(gamestate, &eyeX, &eyeY, &eyeZ);

		gluLookAt(eyeX, eyeY, eyeZ, /* Augpunkt */ 
				0.0f, 0.0f, 0.0f,   /* Mittelpunkt */ 
				0.0f, 1.0f, 0.0f);  /* Up-Vektor */

		glPushMatrix();
		{
			glScalef(10.0f, 10.0f, 10.0f);
			drawWaterSpheres(&gamestate->grid);
		}
		glPopMatrix();
	}
	glPopMatrix();

	/* DEBUG-Ausgabe */
	GLSTATE;
}

void drawScene(AnaglyphEye eye)
{
	Gamestate *gamestate = getGamestate();

	glEnable(GL_DEPTH_TEST);
	
	glPushMatrix();
	{
		glTranslatef(eye * -0.1f, 0.0f, 0.0f);

		/* Kameraposition */
		GLfloat eyeX, eyeY, eyeZ;
		getEyePosition(gamestate, &eyeX, &eyeY, &eyeZ);

		gluLookAt(eyeX, eyeY, eyeZ, /* Augpunkt */ 
				0.0f, 0.0f, 0.0f,   /* Mittelpunkt */ 
				0.0f, 1.0f, 0.0f);  /* Up-Vektor */
		
		renderSkymap(eyeX, eyeY, eyeZ);

		if (g_sceneFlags.lighting) 
		{
			glEnable(GL_LIGHTING);
		}
		
		/* Position des Welt-Lichts setzen */
		float sunlightPos[] = {2.0f, 2.0f, 0.5f, 0.0f};
		glLightfv(LIGHT_SUN, GL_POSITION, sunlightPos);

		/* Position und Richtung des Pointlight setzen */
		float pointlightPos[] = {0.0f, 1.0f, 0.0f, 1.0f};
		glLightfv(LIGHT_POINTLIGHT, GL_POSITION, pointlightPos);

		drawWorld(gamestate);
	}
	glPopMatrix();

	glDisable(GL_LIGHTING);

	/* DEBUG-Ausgabe */
	GLSTATE;
}

void toggleWireframeMode(void)
{
	static GLboolean wireframe = GL_FALSE;

	wireframe = !wireframe;

	if (wireframe)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}

void toggleLighting(void)
{
	g_sceneFlags.lighting = !g_sceneFlags.lighting;
}

void toggleSpheres(void)
{
	g_sceneFlags.showSpheres = !g_sceneFlags.showSpheres;
}

void toggleTextures(void)
{
	g_sceneFlags.textures = !g_sceneFlags.textures;
}

void toggleSunlight(void)
{
	static GLboolean sunlight = GL_TRUE;

	sunlight = !sunlight;

	if (sunlight)
	{
		glEnable(LIGHT_SUN);
	}
	else
	{
		glDisable(LIGHT_SUN);
	}
}

void togglePointlight(void)
{
	static GLboolean pointlight = GL_TRUE;

	pointlight = !pointlight;

	if (pointlight)
	{
		glEnable(LIGHT_POINTLIGHT);
	}
	else
	{
		glDisable(LIGHT_POINTLIGHT);
	}
}

void toggleNormal(void)
{
	g_sceneFlags.showNormals = !g_sceneFlags.showNormals;

	/* Displaylisten mit/ohne Normalen neu erzeugen */
	initDisplayList();
}

GLboolean getNormalState(void)
{
	return g_sceneFlags.showNormals;
}

int initScene(void)
{
	/* Hintergrundfarbe */
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	/* Backface Culling aktivieren */
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	/* Alle Normalen normalisieren */
	glEnable(GL_NORMALIZE);

	/* Vertex-Array Eigenschaften aktivieren */
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	/* Linienbreite */
	glLineWidth(1.f);

	/* Objekte in Displaylisten rendern */
	initDisplayList();

	/* Beleuchtung initialisieren */
	initLight();

	glColorMaterial(GL_FRONT, GL_DIFFUSE);

	/* Alles in Ordnung? */
	return (GLGETERROR == GL_NO_ERROR);
}
