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
#include "water.h"

/* ---- Interne Funktionen ---- */

/**
 * Fuegt einem Wasserfeld ein neues Face hinzu.
 * 
 * @param list das Array fuer die Faces (InOut)
 * @param x die X-Koordinate (In)
 * @param y die Y-Koordinate (In)
 * @param z die Z-Koordinate (In)
 * @param side die Seite an der das Face sich befinden soll (In)
 */
void water_addFace(WaterFaceList *list, float x, float y, float z, WaterSide side)
{
    assert(list != NULL);

    list->count++;
    list->list = (WaterFace *) realloc(list->list, list->count * sizeof(WaterFace));

    if (list->list == NULL) {
        CG_ERROR("Cannot resize water list!");
    }
    
    list->list[list->count - 1].x = x;
    list->list[list->count - 1].y = y;
    list->list[list->count - 1].z = z;
    list->list[list->count - 1].side = side;
    list->list[list->count - 1].camDistSqr = 0.0f;
}

/**
 * Vergleicht zwei Faces anhand deren Distanz zur Kamera.
 * 
 * @param a die X-Koordinate (In)
 * @param b die Y-Koordinate (In)
 * 
 * @return 0 bei gleicher Distanz, 1 wenn a naeher an der Kamera ist. Sonst -1.
 */
int water_compare(const void *a, const void *b)
{
    const WaterFace *waterA = (WaterFace *) a;
    const WaterFace *waterB = (WaterFace *) b;

    if (waterA->camDistSqr < waterB->camDistSqr)
    {
        return 1;
    }

    if (waterA->camDistSqr > waterB->camDistSqr)
    {
        return -1;
    }

    return 0;
}

/* ---- Oeffentliche Funktionen ---- */

void water_add(WaterFaceList *list, float x, float z)
{
    assert(list != NULL);

    water_addFace(list, x, 0.5f, z, WFACE_TOP);
    water_addFace(list, x, -0.5f, z, WFACE_BOTTOM);
    water_addFace(list, x, 0.0f, z + 0.5f, WFACE_FRONT);
    water_addFace(list, x, 0.0f, z - 0.5f, WFACE_BACK);
    water_addFace(list, x + 0.5f, 0.0f, z, WFACE_LEFT);
    water_addFace(list, x - 0.5f, 0.0f, z, WFACE_RIGHT);
}

void water_calcDistances(WaterFaceList *list, float camX, float camY, float camZ)
{
    assert(list != NULL);

    for (int i = 0; i < list->count; i++)
    {
        float distX = list->list[i].x - camX;
        float distY = list->list[i].y - camY;
        float distZ = list->list[i].z - camZ;

        list->list[i].camDistSqr = distX * distX + distY * distY + distZ * distZ;
    }
}

void water_sortDistances(WaterFaceList *list)
{
    assert(list != NULL);

    qsort(list->list, list->count, sizeof(WaterFace), water_compare);
}

void water_free(WaterFaceList *list)
{
    assert(list != NULL);

    free(list->list);
    list->list = NULL; 
    list->count = 0;
}
