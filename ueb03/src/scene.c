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
#include "renderObjects3D.h"
#include "logic.h"
#include "water.h"
#include "debugGL.h"

/* ---- Konstanten ---- */

/* Die Groesse einer einzelnen Kachel */
#define TILE_SIZE (2.0f / LEVELSIZE)

/* Das Punktlicht fuer die Beleuchtung der Welt */
#define LIGHT_WORLD (GL_LIGHT0)

/* Das Spotlight fuer die Taschenlampe */
#define LIGHT_SPOTLIGHT (GL_LIGHT1)

/* Konstante fuer die Differenz zum Unterbinden von Z-Fighting */
#define Z_EPSILON (0.0006f)

/* ---- Globale Daten ---- */

static SceneFlags g_sceneFlags = SCENE_FLAGS_DEFAULT;

/* ---- Interne Funktionen ---- */

static void initLight(void)
{
	/* Globales, ambientes Licht */
	float globalAmbient[] = {1.0f, 1.0f, 1.0f, 1.0f};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);

	/* Weltlichtquelle (Punktlicht) */
	float worldlightColorAmbDif[] = {1.0, 1.0, 1.0, 1.0};
	float worldlightColorSpec[] = {1.0, 1.0, 1.0, 1.0};
	glLightfv(LIGHT_WORLD, GL_AMBIENT, worldlightColorAmbDif);
	glLightfv(LIGHT_WORLD, GL_DIFFUSE, worldlightColorAmbDif);
	glLightfv(LIGHT_WORLD, GL_SPECULAR, worldlightColorSpec);
	glEnable(LIGHT_WORLD);

	/* Tachenlampe (Spotlight) */
	float spotlightColorAmbDif[] = {1.0, 1.0, 0.8, 1.0};
	float spotlightColorSpec[] = {1.0, 1.0, 1.0, 1.0};
	glLightfv(LIGHT_SPOTLIGHT, GL_AMBIENT, spotlightColorAmbDif);
	glLightfv(LIGHT_SPOTLIGHT, GL_DIFFUSE, spotlightColorAmbDif);
	glLightfv(LIGHT_SPOTLIGHT, GL_SPECULAR, spotlightColorSpec);
	glLighti(LIGHT_SPOTLIGHT, GL_SPOT_EXPONENT, 15);
	glLighti(LIGHT_SPOTLIGHT, GL_SPOT_CUTOFF, 40);
	glEnable(LIGHT_SPOTLIGHT);
}

/**
 * Zeichnet das Level. Die zu zeichnenden Objekte werden hierzu in Form 
 * einer Displayliste aus renderObjects bezogen.
 * 
 * @param gamestage der Spielzustand (In)
 */
static void drawLevel(Gamestate *gamestate)
{
	glPushMatrix();
	{
		/* Skalierung und Positionierung des Spielfelds. */
		glTranslatef(0.0f, 0.1f, 0.0f);
		glScalef(0.9f, 0.9f, 1.0f);
		for (int y = 0; y < LEVELSIZE; y++) 
		{
			for (int x = 0; x < LEVELSIZE; x++) 
			{
				glPushMatrix();
				{
					/* Kachel an Position x,y zeichnen. */
					float xOffset = -1.0 + ((float) x * (float) TILE_SIZE);
					float yOffset =  1.0 - ((float) y * (float) TILE_SIZE);
					glTranslatef(xOffset + TILE_SIZE / 2.0f, yOffset - TILE_SIZE / 2.0f, 0.0f);
					glScalef(TILE_SIZE - 0.01f, TILE_SIZE - 0.01f, 1.0f);

					wmFieldType field = gamestate->level[y][x];
					switch (field) {
						case WM_FREE:
							renderObject(RO_FREE);
							break;
						case WM_BLACK:
							// Nichts anzeigen
							break;
						case WM_WALL:
							renderObject(RO_WALL);
							break;
						case WM_GOAL:
							renderObject(RO_GOAL);
							break;
						case WM_NEWWATER: // Sollte nie gerendert werden
						case WM_WATER:
							renderObject(RO_WATER);
							break;
						case WM_SAND:
							renderObject(RO_SANDBAG);
							break;
						case WM_SUGAR:
							renderObject(RO_SUGAR);
							break;
						case WM_TOUCHED_SUGAR:
							/* Bei nassem Zucker muss zusaetzlich zum Rendern die verbleibende Lebenszeit gespeichert werden. */
							{								
								renderObject(RO_FREE);
								float sugarSize = sugar_get_lifetime(&gamestate->sugarCubes, x, y) / SUGAR_LIFETIME;
								glPushMatrix();
								{
									glScalef(sugarSize, sugarSize, 1.0f);
									renderObject(RO_SUGAR);
								}
								glPopMatrix();
							}
							break;
						case WM_START:
							INFO(("WM_START in rendering is not allowed!"));
							break;
					}

					if (x == gamestate->playerX && y == gamestate->playerY) {
						renderObject(RO_PLAYER);
					}
				}
				glPopMatrix();
			}
		}
	}
	glPopMatrix();
}

/**
 * Zeichnet ein Sandsacktile in 3D.
 * 
 * @param gamestage der Spielzustand (In)
 * @param x die X-Koordinate des Sandsacks (In)
 * @param y die Y-Koordinate des Sandsacks (In)
 * @param playerAnimationder Fortschritt der Bewegungsanimation (In)
 */
static void drawSandbag3D(Gamestate *gamestate, int x, int y, float playerAnimation)
{
	renderObject3D(RO_3D_SANDFLOOR);

	if (g_sceneFlags.animation &&
		gamestate->lastSandbagY == y &&
		gamestate->lastSandbagX == x)
	{
		switch (gamestate->lastDirection)
		{
			case dirDown:
				glTranslatef(0.0f, 0.0f, playerAnimation);
				break;
			case dirUp:
				glTranslatef(0.0f, 0.0f, -playerAnimation);
				break;
			case dirLeft:
				glTranslatef(-playerAnimation, 0.0f, 0.0f);
				break;
			case dirRight:
				glTranslatef(playerAnimation, 0.0f, 0.0f);
				break;
		}
	}

	renderObject3D(RO_3D_SANDBAG);
}

/**
 * Zeichnet den Spieler in 3D.
 * 
 * @param gamestage der Spielzustand (In)
 * @param playerAnimationder Fortschritt der Bewegungsanimation (In)
 */
static void drawPlayer3D(Gamestate *gamestate, float playerAnimation)
{
	switch (gamestate->lastDirection)
	{
		case dirDown:
			/* Ausrichtung ist ok */
			break;
		case dirUp:
			glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
			break;
		case dirLeft:
			glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
			break;
		case dirRight:
			glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
			break;
	}
	if (g_sceneFlags.animation)
	{
		glTranslatef(0.0f, 0.0f, playerAnimation);
	}
	renderObject3D(RO_3D_PLAYER);
}

/**
 * Zeichnet das Level in 3D. Die zu zeichnenden Objekte werden hierzu in Form 
 * einer Displayliste aus renderObjects bezogen.
 * 
 * @param gamestage der Spielzustand (In)
 */
static void drawLevel3D(Gamestate *gamestate, WaterFaceList *waterList)
{
	float playerAnimation = -(gamestate->playerCooldown / PLAYER_COOLDOWN_TIME);

	glPushMatrix();
	{
		for (int y = 0; y < LEVELSIZE; y++) 
		{
			for (int x = 0; x < LEVELSIZE; x++) 
			{
				glPushMatrix();
				{
					/* Kachel an Position x,y zeichnen. */
					float xOffset = (float)(x - LEVELSIZE / 2);
					float zOffset = (float)(y - LEVELSIZE / 2);
					glTranslatef(xOffset, 0.0f, zOffset);

					wmFieldType field = gamestate->level[y][x];
					switch (field) {
						case WM_FREE:
							renderObject3D(RO_3D_FREE);
							break;
						case WM_BLACK:
							// Nichts anzeigen
							break;
						case WM_WALL:
							renderObject3D(RO_3D_WALL);
							break;
						case WM_GOAL:
							renderObject3D(RO_3D_GOAL);
							break;
						case WM_NEWWATER: // Sollte nie gerendert werden
						case WM_WATER:
							renderObject3D(RO_3D_FREE);
							renderObject3D(RO_3D_WATER);
							water_add(waterList, xOffset, zOffset);
							break;
						case WM_SAND:
							drawSandbag3D(gamestate, x, y, playerAnimation);
							break;
						case WM_SUGAR:
							renderObject3D(RO_3D_SUGAR);
							break;
						case WM_TOUCHED_SUGAR:
							/* Bei nassem Zucker muss zusaetzlich zum Rendern die verbleibende Lebenszeit gespeichert werden. */
							{								
								renderObject3D(RO_3D_FREE);
								float sugarSize = sugar_get_lifetime(&gamestate->sugarCubes, x, y) / SUGAR_LIFETIME;
								glPushMatrix();
								{
									glTranslatef(0.0f, (1.0f - sugarSize) / -2.0f, 0.0f);
									glScalef(sugarSize, sugarSize, sugarSize);
									renderObject3D(RO_3D_SUGAR);
								}
								glPopMatrix();
							}
							break;
						case WM_START:
							INFO(("WM_START in rendering is not allowed!"));
							break;
					}

					if (x == gamestate->playerX && y == gamestate->playerY) {
						drawPlayer3D(gamestate, playerAnimation);
					}
				}
				glPopMatrix();
			}
		}
	}
	glPopMatrix();
}

/**
 * Zeichnet ein Wassertile in 3D.
 * 
 * @param gamestage der Spielzustand (In)
 * @param waterList ein Array mit allen Faces fuer das Wasser (In)
 */
static void drawLevel3DWater(Gamestate *gamestate, WaterFaceList *waterList)
{
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glPushMatrix();
	{
		for (int i = 0; i < waterList->count; i++) 
		{
			glPushMatrix();
			{
				glTranslatef(waterList->list[i].x, waterList->list[i].y, waterList->list[i].z);

				switch (waterList->list[i].side)
				{
					case WFACE_TOP:
						glTranslatef(0.0f, -Z_EPSILON, 0.0f);
						glRotatef(90, 1.0f, 0.0f, 0.0f);
						break;
					case WFACE_BOTTOM:
						glTranslatef(0.0f, -Z_EPSILON, 0.0f);
						glRotatef(-90, 1.0f, 0.0f, 0.0f);
						break;
					case WFACE_FRONT:
						glTranslatef(0.0f, 0.0f, -Z_EPSILON);
						// Rotation ist ok
						break;
					case WFACE_BACK:
						glTranslatef(0.0f, 0.0f, Z_EPSILON);
						glRotatef(180, 0.0f, 1.0f, 0.0f);
						break;
					case WFACE_LEFT:
						glTranslatef(Z_EPSILON, 0.0f, 0.0f);
						glRotatef(90, 0.0f, 1.0f, 0.0f);
						break;
					case WFACE_RIGHT:
						glTranslatef(-Z_EPSILON, 0.0f, 0.0f);
						glRotatef(-90, 0.0f, 1.0f, 0.0f);
						break;
				}

				setDiffuseMaterialAlpha(0.2f, 0.5f, 0.8f, 0.5f);
				setSpecularMaterial(0.4f, 0.4f, 0.4f, 80.0f);
				drawSquare(4);
			}
			glPopMatrix();
		}
	}
	glPopMatrix();
	glDisable(GL_BLEND);
	glEnable(GL_CULL_FACE);
}

/* ---- Oeffentliche Funktionen ---- */

void drawScene2D(void)
{
	Gamestate *gamestate = getGamestate();

	glDisable(GL_DEPTH_TEST);
	drawLevel(gamestate);

	/* DEBUG-Ausgabe */
	GLSTATE;
}

void drawScene3D(AnaglyphEye eye)
{
	Gamestate *gamestate = getGamestate();

	if (g_sceneFlags.lighting) {
		glEnable(GL_LIGHTING);
	}
	
	glEnable(GL_DEPTH_TEST);

	glPushMatrix();
	{
		glTranslatef(eye * -0.1f, 0.0f, 0.0f);

		/* Position des Spielers bestimmen */
		float xPlayerOffset =  ((float)gamestate->playerX) - ((float)LEVELSIZE) / 2.0f + 0.5f;
		float zPlayerOffset =  ((float)gamestate->playerY) - ((float)LEVELSIZE) / 2.0f + 0.5f;
		float xPlayerDir = 0.0f;
		float zPlayerDir = 0.0f;

		/* Animationsphase des Spielers bestimmen */
		float playerAnimation = 0.0f;
		if (g_sceneFlags.animation)
		{
			playerAnimation = gamestate->playerCooldown / PLAYER_COOLDOWN_TIME;
		}

		/* Blickrichtung anhand der letzten Bewegungsrichtung bestimmen */
		switch (gamestate->lastDirection)
		{
			case dirDown:
				zPlayerDir = 1.0f;
				zPlayerOffset -= playerAnimation;
				break;
			case dirLeft:
				xPlayerDir = -1.0f;
				xPlayerOffset += playerAnimation;
				break;
			case dirRight:
				xPlayerDir = 1.0f;
				xPlayerOffset -= playerAnimation;
				break;
			case dirUp:
				zPlayerDir = -1.0f;
				zPlayerOffset += playerAnimation;
				break;
		}

		/* Position des Auges*/
		GLfloat eyeX = 0.0f;
		GLfloat eyeY = 0.0f;
		GLfloat eyeZ = 0.0f;

		if (g_sceneFlags.firstPerson) 
		{
			/* Auskommentierte Werte fuer 3rd Person Ansicht */
			eyeX = xPlayerOffset; // xPlayerOffset - xPlayerDir * 0.6f;
			eyeY = 0.05f; // 0.75f; 
		 	eyeZ = zPlayerOffset; // zPlayerOffset - zPlayerDir * 0.6f;

			gluLookAt(eyeX,                     eyeY, eyeZ,                       /* Augpunkt */ 
					xPlayerOffset + xPlayerDir, 0.0f, zPlayerOffset + zPlayerDir, /* Mittelpunkt */
					0.0f,                       1.0f, 0.0f);  	                  /* Up-Vektor */
		}
		else
		{
			GLfloat radius = gamestate->camera.radius;
			GLfloat polar = TO_RADIANS(gamestate->camera.polarAngle);
			GLfloat azimuth = TO_RADIANS(gamestate->camera.azimuthAngle);

			eyeX = radius * sinf(azimuth) * cosf(polar);
			eyeY = radius * cosf(azimuth);
			eyeZ = radius * sinf(azimuth) * sinf(polar);

			gluLookAt(eyeX, eyeY, eyeZ, /* Augpunkt */ 
					0.0f, 0.0f, 0.0f,   /* Mittelpunkt */ 
					0.0f, 1.0f, 0.0f);  /* Up-Vektor */
		}
		
		/* Position des Welt-Lichts setzen */
		float worldlightPos[] = {0.0, 2.0, 0.0, 1.0};
		glLightfv(LIGHT_WORLD, GL_POSITION, worldlightPos);

		/* Position und Richtung des Spotlights setzen */
		float spotlightPos[] = {xPlayerOffset, 0.05f, zPlayerOffset, 1.0};
		float spotlightDirection[] = {xPlayerDir, -0.5f, zPlayerDir};
		glLightfv(LIGHT_SPOTLIGHT, GL_SPOT_DIRECTION, spotlightDirection);
		glLightfv(LIGHT_SPOTLIGHT, GL_POSITION, spotlightPos);

		/* Liste fuer Wasserflaechen */
		WaterFaceList waterList = EMPTY_WATER_FACE_LIST;

		/* Level rendern (Opack) */
		drawLevel3D(gamestate, &waterList);

		/* Wasserflaechen nach Abstand zur Kammera sortieren */
		water_calcDistances(&waterList, eyeX, eyeY, eyeZ);
		water_sortDistances(&waterList);

		/* Wasserflaechen rendern (Transparent) */
		drawLevel3DWater(gamestate, &waterList);

		water_free(&waterList);
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

void toggleAnimation(void)
{
	g_sceneFlags.animation = !g_sceneFlags.animation;
}

void toggleFirstPerson(void)
{
	g_sceneFlags.firstPerson = !g_sceneFlags.firstPerson;
}

void toggleLighting(void)
{
	g_sceneFlags.lighting = !g_sceneFlags.lighting;
}

void toggleWorldlight(void)
{
	static GLboolean worldlight = GL_TRUE;

	worldlight = !worldlight;

	if (worldlight)
	{
		glEnable(LIGHT_WORLD);
	}
	else
	{
		glDisable(LIGHT_WORLD);
	}
}

void toggleSpotlight(void)
{
	static GLboolean spotlight = GL_TRUE;

	spotlight = !spotlight;

	if (spotlight)
	{
		glEnable(LIGHT_SPOTLIGHT);
	}
	else
	{
		glDisable(LIGHT_SPOTLIGHT);
	}
}

void toggleNormal(void)
{
	g_sceneFlags.showNormals = !g_sceneFlags.showNormals;

	/* Displaylisten mit/ohne Normalen neu erzeugen */
	initDisplayList();
	initDisplayList3D();
}

GLboolean getNormalState(void)
{
	return g_sceneFlags.showNormals;
}

int initScene(void)
{
	/* Hintergrundfarbe */
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	/* Zeichenfarbe */
	glColor3f(1.0f, 1.0f, 1.0f);

	/* Backface Culling aktivieren */
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	/* Alle Normalen normalisieren */
	glEnable(GL_NORMALIZE);

	/* Linienbreite */
	glLineWidth(1.f);

	/* Objekte in Displaylisten rendern */
	initDisplayList();
	initDisplayList3D();

	/* Beleuchtung initialisieren */
	initLight();

	/* Alles in Ordnung? */
	return (GLGETERROR == GL_NO_ERROR);
}
