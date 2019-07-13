/**
 * @file
 * Texturen-Modul.
 * Das Modul kapselt die Textur-Funktionalitaet (insbesondere das Laden und
 * Binden) des Programms.
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
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#endif

#include <stdlib.h>

/* ---- Eigene Header einbinden ---- */
#include "texture.h"
#include "debugGL.h"

/* Bibliothek um Bilddateien zu laden. Es handelt sich um eine
 * Bibliothek, die sowohl den Header als auch die Quelle in einer Datei
 * bereitstellt. Die Quelle kann durch die Definition des Makros
 * STB_IMAGE_IMPLEMENTATION eingebunden werden. Genauere Informationen
 * sind der "stb_image.h" zu entnehmen.
 *
 * Quelle: https://github.com/nothings/stb */
 // define global functions as static inline
#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h> 

/* ---- Typen ---- */

/* Die Daten einer Textur */
typedef struct
{
	GLuint id;
	char *filename;
} Texture;

/* ---- Konstanten ---- */

/** Anzahl der Texturen. */
#define TEX_COUNT (4)

/* ---- Globale Daten ---- */

/* Array mit den Texturen */
static Texture g_textures[TEX_COUNT];

/* ---- Interne Funktionen ---- */

/**
 * Initialisiert das Array mit den Texturen.
 * 
 * @return true, wenn kein Fehler aufgetreten ist
 */
static int initTextureArray(void)
{
	GLuint texIDs[TEX_COUNT];
	glGenTextures(TEX_COUNT, texIDs);

	for (int i = 0; i < TEX_COUNT; i++)
	{
		g_textures[i].id = texIDs[i];
	}

	g_textures[texSky].filename = "../content/textures/sky.png";
	g_textures[texWater].filename = "../content/textures/water.png";
	g_textures[texDirt].filename = "../content/textures/dirt.jpg";
	g_textures[texLand].filename = "../content/textures/land.jpg";

	return (GLGETERROR == GL_NO_ERROR);
}

/**
 * Berechnet fuer eine uebergebene Zahl den entsprechenden Bitmap-Modus.
 * 
 * @param n die umzuwandelnde Zahl
 * 
 * @return den ermittelten Bitmap-Modus
 */
static unsigned int calculateGLBitmapMode(int n)
{
	switch (n)
	{
		case 1:
			return GL_LUMINANCE;
		case 2:
			return GL_LUMINANCE_ALPHA;
		case 3:
			return GL_RGB;
		case 4:
			return GL_RGBA;
		default:
			return GL_RGB;
	}
}

/**
 * Laedt die Texturen.
 * 
 * @return 0, wenn ein Fehler aufgetreten ist
 */
static int loadTextures(void)
{
	unsigned char *data;
	int width, height;
	int channels;
	
	if (initTextureArray())
	{
		/* Alle Texturen nacheinander laden. */
		for (int i = 0; i < TEX_COUNT; i++)
		{
			/* Die 0 erlaubt es, Bilder mit beliebig vielen Kanaelen zu laden. */
			data = stbi_load(g_textures[i].filename, &width, &height, &channels, 0);

			if (data != NULL)
			{
				glBindTexture(GL_TEXTURE_2D, g_textures[i].id);

				gluBuild2DMipmaps(GL_TEXTURE_2D,
									channels,
									width,
									height,
									calculateGLBitmapMode(channels),
									GL_UNSIGNED_BYTE, data);

				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

				stbi_image_free(data);
			}
			else
			{
				INFO(("Textur %s konnte nicht geladen werden!\n", g_textures[i].filename));
				return 0;
			}
		}
	
		/* Alles in Ordnung? */
		return (GLGETERROR == GL_NO_ERROR);
	}
	else
	{
		INFO(("Erzeugen des Textur-Arrays fehlgeschlagen!\n"));
		return 0;
	}
}

/* ---- Oeffentliche Funktionen ---- */

void bindTexture(TexName texture)
{
	glBindTexture(GL_TEXTURE_2D, g_textures[texture].id);
}

int initTextures(void)
{
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	/* Texturen laden */
	return loadTextures();
}
