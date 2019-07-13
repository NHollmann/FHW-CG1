/**
 * @file
 * Verwaltet die verschiedenen Faces des Wassers fuer die korrekte Reihenfolge beim Rendern.
 *
 * Bestandteil einer Uebung im Rahmen des Moduls Praktikum Grundlagen der Computergrafik 
 * an der FH Wedel.
 *
 * @author Nicolas Hollmann, Daniel Klintworth
 */

/* ---- System Header einbinden ---- */
#include <stdlib.h>
#include <assert.h>

/* ---- Eigene Header einbinden ---- */
#include "debugGL.h"
#include "types.h"
#include "water.h"
#include "renderObjects.h"

/* ---- Konstanten ---- */

/* Schrittgroesse beim Picking */
#define STEP_HEIGHT (0.1f)

/* Grenzen fuer die Bestimmung, ab wann Wasser als tief oder hoch gilt */
#define LOWER_THRS (0.2)
#define HIGHER_THRS (0.4)

/* Farbwerte fuer niedriges Wasser */
#define COLOR_BOTTOM_R (0.2)
#define COLOR_BOTTOM_G (0.2)
#define COLOR_BOTTOM_B (0.8)

/* Farbwerte fuer mittelhohes Wasser */
#define COLOR_MIDDLE_R (0.4)
#define COLOR_MIDDLE_G (0.6)
#define COLOR_MIDDLE_B (0.8)

/* Farbwerte fuer hohes Wasser */
#define COLOR_TOP_R (0.8)
#define COLOR_TOP_G (0.8)
#define COLOR_TOP_B (1.0)

/* Speicherindizes fuer die Werte im Vertexarray */
#define VA_X (0)
#define VA_Y (1)
#define VA_Z (2)
#define VA_R (3)
#define VA_G (4)
#define VA_B (5)
#define VA_NX (6)
#define VA_NY (7)
#define VA_NZ (8)
#define VA_U (9)
#define VA_V (10)

/* Faktor fuer die groesse einer Wassersaeule */
#define COLUMN_FACTOR (1.5)

/* Die Propagation der Wassersimulation */
#define PROPAGATION (0.9)

/* Das Dampening der Wassersimulation */
#define DAMPENING (0.98)

/* ---- Macros ---- */

/* Berechnet aus den Gridkoordinaten den Index im Vertexarray */
#define GRID_TO_IDX(x, y, s) ((y) * (s) + (x))

/* ---- Interne Funktionen ---- */

/**
 * Gibt die Groesse des Grids zurueck.
 * 
 * @param grid Zeiger auf das Wassergrid. (In)
 * @return die Groesse des Grids
 */
static GLuint getGridSize(WaterGrid *grid)
{
    return grid->sideLength * grid->sideLength;
}

/**
 * Gibt fuer einen gegebenen Index die Hoehe des Wassers zurueck.
 * 
 * @param grid Zeiger auf das Wassergrid. (In)
 * @param index der Index der Stelle im Grid. (In)
 * @return die Hoehe des Wassers
 */
static double getHeightAtIndex(WaterGrid *grid, int index)
{
    return grid->vertices[index][VA_Y];
}

/**
 * Berechnet fuer einen gegebenen Index die Vertexfarbe und setzt diese.
 * 
 * @param grid Zeiger auf das Wassergrid. (InOut)
 * @param index der Index der Stelle im Grid. (In)
 */
static void calcAndSetVertexColor(WaterGrid *grid, int index)
{
    double newColor[3] = {
        COLOR_BOTTOM_R, 
        COLOR_BOTTOM_G, 
        COLOR_BOTTOM_B
    };

    if (getHeightAtIndex(grid, index) > HIGHER_THRS) 
    {
        newColor[0] = COLOR_TOP_R;
        newColor[1] = COLOR_TOP_G;
        newColor[2] = COLOR_TOP_B;
    }
    else if (getHeightAtIndex(grid, index) > LOWER_THRS) 
    {
        newColor[0] = COLOR_MIDDLE_R;
        newColor[1] = COLOR_MIDDLE_G;
        newColor[2] = COLOR_MIDDLE_B;
    }

    grid->vertices[index][VA_R] = newColor[0];
    grid->vertices[index][VA_G] = newColor[1];
    grid->vertices[index][VA_B] = newColor[2];
}

/**
 * Gibt fuer Koordinaten im Grid des begrenzten Index im Vertexarray zurueck.
 * 
 * @param grid Zeiger auf das Wassergrid. (In)
 * @param x, y die Koordinaten im Grid. (In)
 * @return der begrenzte Index im Vertexarray
 */
static int getClampedIndex(WaterGrid *grid, int x, int y) 
{
    x = CLAMP_INT(x, 0, grid->sideLength - 1);
    y = CLAMP_INT(y, 0, grid->sideLength - 1);
    return GRID_TO_IDX(x, y, grid->sideLength);
}

/**
 * Gibt fuer die uebergebenen Koordinaten den Hoehenwert an der begrenzten Stelle zurueck.
 * 
 * @param grid Zeiger auf das Wassergrid. (In)
 * @param x, y die Koordinaten im Grid. (In)
 * @return der Hoehenwert
 */
static double getHeightAtClampedCoord(WaterGrid *grid, int x, int y)
{
    return getHeightAtIndex(grid, getClampedIndex(grid, x, y));
}

/**
 * Berechnet fuer das uebergebene Wassergrid die Normals und setzt diese.
 *
 * @param grid Zeiger auf das Wassergrid. (InOut)
 */
static void calcAndSetNormals(WaterGrid *grid)
{
    for (int y = 0; y < grid->sideLength; y++)
    {
        for (int x = 0; x < grid->sideLength; x++)
        {
            int index = GRID_TO_IDX(x, y, grid->sideLength);
            
            double rightX = 0.5 - ((double)x + 1) / ((double) grid->sideLength - 1);
            double rightY = grid->vertices[getClampedIndex(grid, x + 1, y)][VA_Y];
            double rightZ = 0.5 - ((double)y) / ((double) grid->sideLength - 1);

            double downX = 0.5 - ((double)x) / ((double) grid->sideLength - 1);
            double downY = grid->vertices[getClampedIndex(grid, x, y + 1)][VA_Y];
            double downZ = 0.5 - ((double)y + 1) / ((double) grid->sideLength - 1);

            double leftX = 0.5 - ((double)x - 1) / ((double) grid->sideLength - 1);
            double leftY = grid->vertices[getClampedIndex(grid, x - 1, y)][VA_Y];
            double leftZ = 0.5 - ((double)y) / ((double) grid->sideLength - 1);

            double upX = 0.5 - ((double)x) / ((double) grid->sideLength - 1);
            double upY = grid->vertices[getClampedIndex(grid, x, y - 1)][VA_Y];
            double upZ = 0.5 - ((double)y - 1) / ((double) grid->sideLength - 1);

            double rightVectorX = rightX - leftX;
            double rightVectorY = rightY - leftY;
            double rightVectorZ = rightZ - leftZ;

            double downVectorX = downX - upX;
            double downVectorY = downY - upY;
            double downVectorZ = downZ - upZ;

            double normalX = downVectorY * rightVectorZ - downVectorZ * rightVectorY;
            double normalY = downVectorZ * rightVectorX - downVectorX * rightVectorZ;
            double normalZ = downVectorX * rightVectorY - downVectorY * rightVectorX;

            double length = sqrt(normalX * normalX + normalY * normalY + normalZ * normalZ);

            grid->vertices[index][VA_NX] = normalX / length;
            grid->vertices[index][VA_NY] = normalY / length;
            grid->vertices[index][VA_NZ] = normalZ / length;
        }
    }
}

/* ---- Oeffentliche Funktionen ---- */

void changeWaterHeight(WaterGrid *grid, int index, GLboolean increase)
{
    assert(grid != NULL);

    if (index >= 0 && index < getGridSize(grid))
    {
        grid->vertices[index][VA_Y] += increase ? STEP_HEIGHT : -STEP_HEIGHT;
        calcAndSetVertexColor(grid, index);
    }

    calcAndSetNormals(grid);
}



void updateWaterMotion(WaterGrid *grid, double interval)
{
    assert(grid != NULL);
    
    GLdouble *heightValues = malloc(sizeof(GLdouble) * getGridSize(grid));
    double h = COLUMN_FACTOR / grid->sideLength;

    /* Wassersimulation durchfuehren */
    for (int y = 0; y < grid->sideLength; y++)
    {
        for (int x = 0; x < grid->sideLength; x++)
        {
            double force = PROPAGATION * PROPAGATION * (
                    getHeightAtClampedCoord(grid, x + 1, y) +
                    getHeightAtClampedCoord(grid, x - 1, y) +
                    getHeightAtClampedCoord(grid, x, y + 1) +
                    getHeightAtClampedCoord(grid, x, y - 1) -
                    4 * getHeightAtClampedCoord(grid, x, y)) / (h * h);
            
            int index = GRID_TO_IDX(x, y, grid->sideLength);
            grid->velocities[index] = (grid->velocities[index] + force * interval) * DAMPENING;

            heightValues[index] = getHeightAtClampedCoord(grid, x, y) + grid->velocities[index] * interval;
        }
    }

    /* Neu berechnete Hoehen zurueck kopieren und Farbe berechnen */
    for (int y = 0; y < grid->sideLength; y++)
    {
        for (int x = 0; x < grid->sideLength; x++)
        {
            int index = GRID_TO_IDX(x, y, grid->sideLength);
            grid->vertices[index][VA_Y] = heightValues[index];

            calcAndSetVertexColor(grid, index);
        }
    }

    calcAndSetNormals(grid);

    free(heightValues);
}

void changeWaterGridSize(WaterGrid *grid, GLboolean increase)
{
    assert(grid != NULL);

    GLdouble *heightValues = malloc(sizeof(GLdouble) * getGridSize(grid));
    double *velocities = malloc(sizeof(double) * getGridSize(grid));

    int oldSize = grid->sideLength;

    /* Alte Werte sichern */
    for (int y = 0; y < oldSize; y++)
    {
        for (int x = 0; x < oldSize; x++)
        {
            int index = y * oldSize + x;
            heightValues[index] = grid->vertices[index][VA_Y];
            velocities[index] = grid->velocities[index];
        }
    }

    cleanupWater(grid);
    initWaterGrid(grid, MAX_INT(2, oldSize + (increase ? 1 : -1)) );

    /* Alte Werte in neues Wassergrid uebernehmen */
    for (int y = 0; y < grid->sideLength; y++)
    {
        for (int x = 0; x < grid->sideLength; x++)
        {
            int oldIndex = MIN_INT(y, oldSize - 1) * oldSize +  MIN_INT(x, oldSize - 1);
            grid->vertices[GRID_TO_IDX(x, y, grid->sideLength)][VA_Y] = heightValues[oldIndex];
            grid->velocities[GRID_TO_IDX(x, y, grid->sideLength)] = velocities[oldIndex];
        }
    }

    free(heightValues);
    free(velocities);

    /* Normalen neu berechnen */
    calcAndSetNormals(grid);
}

void initWaterGrid(WaterGrid *grid, GLuint newSize)
{
    assert(grid != NULL);
    
    grid->sideLength = newSize;
    GLuint sizeSqr = getGridSize(grid);

    grid->vertices = malloc(sizeSqr * sizeof(WaterVertex));
    grid->velocities = malloc(sizeSqr * sizeof(double));

    // 3 Indizes pro Dreieck / 2 Dreiecke pro Kasten
    grid->indices = malloc(sizeSqr * 6 * sizeof(GLuint));

    /* Verticies, Colors, Normalen, TexCoords und Velocities initialisieren */
    for (int y = 0; y < newSize; y++)
    {
        for (int x = 0; x < newSize; x++)
        {
            int vertexIdx = GRID_TO_IDX(x, y, newSize);

            grid->vertices[vertexIdx][VA_X] = 0.5 - ((double)x) / ((double) newSize - 1);
            grid->vertices[vertexIdx][VA_Y] = 0.0f;
            grid->vertices[vertexIdx][VA_Z] = 0.5 - ((double)y) / ((double) newSize - 1);

            calcAndSetVertexColor(grid, vertexIdx);

            grid->vertices[vertexIdx][VA_NX] = 0.0;
            grid->vertices[vertexIdx][VA_NY] = 1.0;
            grid->vertices[vertexIdx][VA_NZ] = 0.0;

            grid->vertices[vertexIdx][VA_U] = ((double)x) / ((double) newSize - 1);
            grid->vertices[vertexIdx][VA_V] = ((double)y) / ((double) newSize - 1);

            grid->velocities[vertexIdx] = 0.0;
        }
    }

    /* Indecies initialisieren */
    for (int y = 0; y < newSize - 1; y++)
    {
        for (int x = 0; x < newSize - 1; x++)
        {
            int indexStart = (y * (newSize - 1) + x) * 6;

            grid->indices[indexStart + 0] = GRID_TO_IDX(x,     y,     newSize); // Oben Links
            grid->indices[indexStart + 1] = GRID_TO_IDX(x,     y + 1, newSize); // Unten Links
            grid->indices[indexStart + 2] = GRID_TO_IDX(x + 1, y,     newSize); // Oben Rechts

            grid->indices[indexStart + 3] = GRID_TO_IDX(x + 1, y,     newSize); // Oben Rechts
            grid->indices[indexStart + 4] = GRID_TO_IDX(x,     y + 1, newSize); // Unten Links
            grid->indices[indexStart + 5] = GRID_TO_IDX(x + 1, y + 1, newSize); // Unten Rechts
        }
    }

    /* Zeiger auf das Vertex-Array setzen */
    glVertexPointer(3, GL_DOUBLE, sizeof(WaterVertex), &(grid->vertices[0][VA_X]));
    glColorPointer(3, GL_DOUBLE, sizeof(WaterVertex), &(grid->vertices[0][VA_R]));
    glNormalPointer(GL_DOUBLE, sizeof(WaterVertex), &(grid->vertices[0][VA_NX]));
    glTexCoordPointer(2, GL_DOUBLE, sizeof(WaterVertex), &(grid->vertices[0][VA_U]));
}

void drawWater(WaterGrid *grid)
{
    assert(grid != NULL);

    glDrawElements(
        GL_TRIANGLES, /* Zeichenmodus */
        (grid->sideLength - 1) * (grid->sideLength - 1) * 6,  /* Anzahl Indizes */
        GL_UNSIGNED_INT, /* Datentyp der Indizes */
        grid->indices /* Zeiger auf die Indizes */
    );
}

void drawWaterNormals(WaterGrid *grid)
{
    assert(grid != NULL);

    for (int y = 0; y < grid->sideLength; y++)
    {
        for (int x = 0; x < grid->sideLength; x++)
        {
            int index = GRID_TO_IDX(x, y, grid->sideLength);
            float posX = grid->vertices[index][VA_X];
            float posY = grid->vertices[index][VA_Y];
            float posZ = grid->vertices[index][VA_Z];
            float normX = grid->vertices[index][VA_NX] * 0.1f;
            float normY = grid->vertices[index][VA_NY] * 0.1f;
            float normZ = grid->vertices[index][VA_NZ] * 0.1f;

            glBegin(GL_LINES);
                glVertex3f(posX, posY, posZ);
                glVertex3f(posX + normX, posY + normY, posZ + normZ);
            glEnd();
        }
    }
}

void drawWaterSpheres(WaterGrid *grid)
{
    assert(grid != NULL);

    for (int y = 0; y < grid->sideLength; y++)
    {
        for (int x = 0; x < grid->sideLength; x++)
        {
            int index = GRID_TO_IDX(x, y, grid->sideLength);
            glPushName(index);
            {
                glPushMatrix();
                {
                    glTranslated(
                        grid->vertices[index][VA_X],
                        grid->vertices[index][VA_Y],
                        grid->vertices[index][VA_Z]
                    );
                    glColor3d(
                        grid->vertices[index][VA_R],
                        grid->vertices[index][VA_G],
                        grid->vertices[index][VA_B]
                    );
                    setDiffuseMaterial(
                        grid->vertices[index][VA_R],
                        grid->vertices[index][VA_G],
                        grid->vertices[index][VA_B]
                    );
                    setSpecularMaterial(
                        grid->vertices[index][VA_R],
                        grid->vertices[index][VA_G],
                        grid->vertices[index][VA_B],
                        10.0f
                    );
                    renderObject(RO_SPHERE);
                }
                glPopMatrix();
            }
            glPopName();
        }
    }
}

void cleanupWater(WaterGrid *grid)
{
    assert(grid != NULL);

    free(grid->indices);
    free(grid->vertices);
    free(grid->velocities);

    grid->indices = NULL;
    grid->vertices = NULL;
    grid->velocities = NULL;
    grid->sideLength = 0;
}
