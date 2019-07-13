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

/* Stellt moderne OpenGL-Funktionalität bereit. */
#include <GL/glew.h>

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
	GLboolean mipmap;
} Texture;

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

	g_textures[texHeightmap].filename = "../content/textures/heightmap.png";
	g_textures[texHeightmap].mipmap = GL_FALSE;
	g_textures[texRock].filename = "../content/textures/rock.jpg";
	g_textures[texRock].mipmap = GL_TRUE;
	g_textures[texSnow].filename = "../content/textures/snow.jpg";
	g_textures[texSnow].mipmap = GL_TRUE;
	g_textures[texNormal].filename = "../content/textures/normal.jpg";
	g_textures[texNormal].mipmap = GL_TRUE;

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

				GLint format = calculateGLBitmapMode(channels);

				glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
				
				if (g_textures[i].mipmap)
				{
					glGenerateMipmap(GL_TEXTURE_2D);
					glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				}
				else
				{
					glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				}

				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				
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
	/* Texturen laden */
	return loadTextures();
}
