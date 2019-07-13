#ifndef __WATER_H__
#define __WATER_H__
/**
 * @file
 * Verwaltet die Faces fuer Wasserfelder. Bietet Funktionen fuer deren korrektes Rendern an.
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

/* ---- Typdeklaration - Vertex ---- */

/* Ein einzelner Wasservertex */
typedef GLdouble WaterVertex[11];

/* ---- Typedeklarationen - Wasser ---- */

/* Ein Wassergrid aus Wasservertices und weiteren Daten */
typedef struct {
    WaterVertex *vertices;
    GLuint *indices;
    double *velocities;
    GLuint sideLength;

} WaterGrid;

/* Leeres Grid zur Initialisierung */
#define WATER_GRID_EMPTY {NULL, NULL, NULL, 0}

/* ---- Funktionen ---- */

/**
 * Veraendert die Wasserhohe an dem uebergebenen Index.
 * 
 * @param grid Zeiger auf das Wassergrid. (InOut)
 * @param index der Index, an dessen Stelle im Grid die Hoehe veraendert werden soll. (In)
 * @param increase true, wenn die Hohe erhoeht, false wenn sie verringert werden soll. (In)
 */
void changeWaterHeight(WaterGrid *grid, int index, GLboolean increase);

/**
 * Aktualisiert die Wassersimulation.
 * 
 * @param grid Zeiger auf das Wassergrid. (InOut)
 * @param interval der Intervall seit dem letzten Update. (In)
 */
void updateWaterMotion(WaterGrid *grid, double interval);

/**
 * Veraendert die Groesse des Wassergrids.
 * 
 * @param grid Zeiger auf das Wassergrid. (InOut)
 * @param increase true, wenn das Grid vergroessert, false wenn es verkleinert werden soll. (In)
 */
void changeWaterGridSize(WaterGrid *grid, GLboolean increase);

/**
 * Initialisiert das Wassergrid.
 * 
 * @param grid Zeiger auf das Wassergrid. (Out)
 * @param newSize die Groesse des Wassergrids. (In)
 */
void initWaterGrid(WaterGrid *grid, GLuint newSize);

/**
 * Zeichnet das Wasser.
 * 
 * @param grid Zeiger auf das Wassergrid. (In)
 */
void drawWater(WaterGrid *grid);

/**
 * Zeichnet die Normalen des Wassers.
 * 
 * @param grid Zeiger auf das Wassergrid. (In)
 */
void drawWaterNormals(WaterGrid *grid);

/**
 * Zeichnet die Pickingspheres auf dem Wasser.
 * 
 * @param grid Zeiger auf das Wassergrid. (In)
 */
void drawWaterSpheres(WaterGrid *grid);

/**
 * Befreit den fuer das Wasser belegten Speicher.
 * 
 * @param grid Zeiger auf das Wassergrid. (InOut)
 */
void cleanupWater(WaterGrid *grid);

#endif
