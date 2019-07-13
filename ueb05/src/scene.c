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

#include <GL/glew.h>

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

/* ---- Eigene Header einbinden ---- */
#include "scene.h"
#include "logic.h"
#include "debugGL.h"
#include "texture.h"
#include "utility.h"

/* ---- Konstanten ---- */

/** Anzahl der Vertices pro Dimension im Grid */
#define GRID_SUBDIVS (80)

/** Anzahl der Wiederholungen der Textur im Grid */
#define GRID_TEX_FACTOR (2)

#define GRID_VERTEX_COUNT (GRID_SUBDIVS * GRID_SUBDIVS)
#define GRID_INDEX_COUNT ((GRID_SUBDIVS - 1) * (GRID_SUBDIVS - 1) * 6)

/* ---- Typen ---- */

/* Die Flags zum Ein- und Ausschalten verschiedener Optionen. */
typedef struct 
{
    unsigned char textures : 1;
    unsigned char sines : 1;
    unsigned char heightmap : 1;
    unsigned char phong : 1;
    unsigned char bumpmap : 1;
} SceneFlags;

/* Defaultwerte fuer die Flags der Optionen */
#define SCENE_FLAGS_DEFAULT {1, 1, 1, 1, 0}

/**
 * Aufzaehlung aller Uniforms im Terrain Shader.
 */
typedef enum
{
	UNI_PROJECTION = 0,
	UNI_MODELVIEW,
	UNI_HEIGHTMAP,
	UNI_ROCK_TEX,
	UNI_SNOW_TEX,
	UNI_NORMAL_TEX,
	UNI_TIME,
	UNI_CAM_POS,
	UNI_SCENE_FLAGS,

	UNI_SIZE
} UniformLocations;

/**
 * Terrain Shader Daten
 */
typedef struct
{
	GLuint shaderId;
	GLuint vertexArrayObject;
	GLuint indexBuffer;

	GLint uniformLocations[UNI_SIZE];
} ShaderData;

#define SHADER_DATA_DEFAULT { 0, 0, 0, { 0 } }

/**
 * Alle Daten eines Vertex.
 */
typedef struct {
    float x, y, z; /* Position */
    float s, t;    /* Textur-Koordinate */
} Vertex;

/* ---- Globale Daten ---- */

/**
 * Flags fuer die Darstellung der Szene.
 */
static SceneFlags g_sceneFlags = SCENE_FLAGS_DEFAULT;

/**
 * Relevante Daten fuer den Terrain Shader.
 */
static ShaderData g_shaderData = SHADER_DATA_DEFAULT;

/* ---- Macros ---- */

/** Einfacher Zugriff auf Uniform Locations */
#define UNI_LOC(x) (g_shaderData.uniformLocations[(x)])

/* ---- Interne Funktionen ---- */

/**
 * Laedt den Vertex- und Fragment-Shader fuer die Darstellung des Terrains
 * und bestimmt die Uniform Locations.
 */
static void loadShader(void)
{
	g_shaderData.shaderId = createProgram(
		"../content/shaders/terrain.vert", 
		"../content/shaders/terrain.frag"
	);

	const char *uniformNames[UNI_SIZE] = {
		"Projection",
		"ModelView",
		"Heightmap",
		"RockTex",
		"SnowTex",
		"NormalTex",
		"Time",
		"CameraPos",
		"SceneFlags"
	};

	for (int i = 0; i < UNI_SIZE; i++)
	{
		UNI_LOC(i) = glGetUniformLocation(g_shaderData.shaderId, uniformNames[i]);
	}
}

/**
 * Erstellt einen Array Buffer (Vertex Buffer) fuer ein Grid.
 */
static GLuint createArrayBuffer(void)
{
	Vertex vertices[GRID_VERTEX_COUNT];

	for (int y = 0; y < GRID_SUBDIVS; y++)
	{
		for (int x = 0; x < GRID_SUBDIVS; x++)
		{
			int index = GRID_TO_IDX(x, y, GRID_SUBDIVS);

			vertices[index].x = 0.5 - ((float)x) / ((float) GRID_SUBDIVS - 1);
            vertices[index].y = 0.0f;
            vertices[index].z = 0.5 - ((float)y) / ((float) GRID_SUBDIVS - 1);

			vertices[index].s = ((float)x) / ((float) GRID_SUBDIVS - 1) * GRID_TEX_FACTOR;
            vertices[index].t = ((float)y) / ((float) GRID_SUBDIVS - 1) * GRID_TEX_FACTOR;
		}
	}

	GLuint arrayBuffer;
	glGenBuffers(1, &arrayBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, arrayBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	return arrayBuffer;
}

/**
 * Erstellt einen Element Array Buffer (Index Buffer) fuer ein Grid.
 */
static GLuint createElementArrayBuffer(void)
{
	GLuint indices[GRID_INDEX_COUNT];

	for (int y = 0; y < GRID_SUBDIVS - 1; y++)
	{
		for (int x = 0; x < GRID_SUBDIVS - 1; x++)
		{
			int index = GRID_TO_IDX(x, y, GRID_SUBDIVS - 1) * 6;

			indices[index + 0] = GRID_TO_IDX(x,     y,     GRID_SUBDIVS); // Oben Links
            indices[index + 1] = GRID_TO_IDX(x,     y + 1, GRID_SUBDIVS); // Unten Links
            indices[index + 2] = GRID_TO_IDX(x + 1, y,     GRID_SUBDIVS); // Oben Rechts

            indices[index + 3] = GRID_TO_IDX(x + 1, y,     GRID_SUBDIVS); // Oben Rechts
            indices[index + 4] = GRID_TO_IDX(x,     y + 1, GRID_SUBDIVS); // Unten Links
            indices[index + 5] = GRID_TO_IDX(x + 1, y + 1, GRID_SUBDIVS); // Unten Rechts
		}
	}

	GLuint elementArrayBuffer;
	glGenBuffers(1, &elementArrayBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementArrayBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	return elementArrayBuffer;
}

/**
 * Erstellt ein Vertex-Array-Object fuer das Terrain.
 * Hier werden auch der Array Buffer und der Element Array Buffer angelegt.
 */
static void createVAO(void)
{
	const GLuint positionLocation = 0;
	const GLuint texCoordLocation = 1;    

	glGenVertexArrays(1, &g_shaderData.vertexArrayObject);
	glBindVertexArray(g_shaderData.vertexArrayObject);

	glBindBuffer(GL_ARRAY_BUFFER, createArrayBuffer());
	g_shaderData.indexBuffer = createElementArrayBuffer();

	/* Erster Attribut-Pointer für die Vertex-Koordinaten */
	glEnableVertexAttribArray(positionLocation);
	glVertexAttribPointer(
		positionLocation,            /* location (siehe Shader) */
		3,                           /* Dimensionalität */
		GL_FLOAT,                    /* Datentyp im Buffer */
		GL_FALSE,                    /* Keine Normierung notwendig */
		sizeof(Vertex),              /* Offset zum nächsten Vertex */
		(void*)offsetof(Vertex, x)); /* Offset zum ersten Vertex */

	/* Zweiter Attribut-Pointer für die Textur-Koordinaten */
	glEnableVertexAttribArray(texCoordLocation);
	glVertexAttribPointer(
		texCoordLocation, 
		2, 
		GL_FLOAT, 
		GL_FALSE, 
		sizeof(Vertex), 
		(void*)offsetof(Vertex, s));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
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
 * Zeichnet die Welt.
 * Der Shader muss bereits aktiviert sein.
 * 
 * @param time die Zeit seit Programmstart (In)
 */
static void drawWorld(float time)
{
	/* Übergeben der Texturen an den Shader. */
	glActiveTexture(GL_TEXTURE0);
	bindTexture(texHeightmap);
	glUniform1i(UNI_LOC(UNI_HEIGHTMAP), 0);

	glActiveTexture(GL_TEXTURE1);
	bindTexture(texRock);
	glUniform1i(UNI_LOC(UNI_ROCK_TEX), 1);

	glActiveTexture(GL_TEXTURE2);
	bindTexture(texSnow);
	glUniform1i(UNI_LOC(UNI_SNOW_TEX), 2);

	glActiveTexture(GL_TEXTURE3);
	bindTexture(texNormal);
	glUniform1i(UNI_LOC(UNI_NORMAL_TEX), 3);

	/* Aktuelle Zeit setzen */
	glUniform1f(UNI_LOC(UNI_TIME), time);

	/* Szenen-Flags setzen */
	GLuint flags = 0;
	flags |= g_sceneFlags.textures;
	flags |= g_sceneFlags.sines << 1;
	flags |= g_sceneFlags.heightmap << 2;
	flags |= g_sceneFlags.phong << 3;
	flags |= g_sceneFlags.bumpmap << 4;
	glUniform1ui(UNI_LOC(UNI_SCENE_FLAGS), flags);

	/* Aktivieren des Vertex-Array-Objekts (VAO). */
	glBindVertexArray(g_shaderData.vertexArrayObject);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_shaderData.indexBuffer);

	/* Rendern der Dreiecke. */
	glDrawElements(GL_TRIANGLES, GRID_INDEX_COUNT, GL_UNSIGNED_INT, 0);

	/* Zurücksetzen des OpenGL-Zustands, um Seiteneffekte zu verhindern */
	glBindVertexArray(0);
}

/* ---- Oeffentliche Funktionen ---- */

void drawScene(float aspect, float time)
{
	Gamestate *gamestate = getGamestate();

	glEnable(GL_DEPTH_TEST);

	glUseProgram(g_shaderData.shaderId);

	float projectionMatrix[16];
    /* Perspektivische Darstellung */
	perspective(70,     /* Oeffnungswinkel */
				aspect, /* Seitenverhaeltnis */
				0.05,	/* nahe Clipping-Ebene */
				50, 	/* ferne Clipping-Ebene */
				projectionMatrix);

    glUniformMatrix4fv(UNI_LOC(UNI_PROJECTION), 1, GL_FALSE, projectionMatrix);

	/* Kameraposition */
	float viewMatrix[16];
	GLfloat eyeX, eyeY, eyeZ;
	getEyePosition(gamestate, &eyeX, &eyeY, &eyeZ);

	lookAt( eyeX, eyeY, eyeZ,  /* Augpunkt */ 
			0.0f, 0.0f, 0.0f,  /* Mittelpunkt */ 
			0.0f, 1.0f, 0.0f,  /* Up-Vektor */
			viewMatrix);

	glUniformMatrix4fv(UNI_LOC(UNI_MODELVIEW), 1, GL_FALSE, viewMatrix);

	/* Kameraposition uebergeben */
	glUniform3f(UNI_LOC(UNI_CAM_POS), eyeX, eyeY, eyeZ);

	drawWorld(time);

	glUseProgram(0);

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

void toggleTextures(void)
{
	g_sceneFlags.textures = !g_sceneFlags.textures;
}

void toggleHeightmap(void)
{
	g_sceneFlags.heightmap = !g_sceneFlags.heightmap;
}

void toggleSines(void)
{
	g_sceneFlags.sines = !g_sceneFlags.sines;
}

void togglePhong(void)
{
	g_sceneFlags.phong = !g_sceneFlags.phong;
}

void toggleBumpmap(void)
{
	g_sceneFlags.bumpmap = !g_sceneFlags.bumpmap;
}

int initScene(void)
{
	/* Hintergrundfarbe */
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	/* Backface Culling aktivieren TODO benötigt? */
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	/* Linienbreite */
	glLineWidth(1.f);

	loadShader();
	createVAO();

	/* Alles in Ordnung? */
	return (GLGETERROR == GL_NO_ERROR);
}
