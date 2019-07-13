/**
 * @file
 * Verwaltet die Animationszustaende fuer Zuckerwuerfel als dynamisches Array.
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
#include "sugar.h"

/* ---- Oeffentliche Funktionen ---- */

void sugar_add(SugarList *list, int x, int y)
{
    assert(list != NULL);

    list->count++;
    list->list = (Sugar *) realloc(list->list, list->count * sizeof(Sugar));

    if (list->list == NULL) {
        CG_ERROR("Cannot resize sugar list!");
    }
    
    list->list[list->count - 1].lifetime = SUGAR_LIFETIME;
    list->list[list->count - 1].x = x;
    list->list[list->count - 1].y = y;
}

float sugar_get_lifetime(SugarList *list, int x, int y) 
{
    assert(list != NULL);

    float lifetime = 0.0f;
    for (int i = 0; i < list->count; i++) 
    {
        if (list->list[i].x == x && list->list[i].y == y)
        {
            lifetime = list->list[i].lifetime;
        }
    }

    return lifetime > 0.0f ? lifetime : 0.0f;
}

void sugar_update(SugarList *list, SugarDissolved callback, float interval)
{
    assert(list != NULL);

    for (int i = 0; i < list->count; i++) 
    {
        if (list->list[i].lifetime > 0.0f) 
        {
            list->list[i].lifetime -= interval;
            if (list->list[i].lifetime <= 0.0f) 
            {
                callback(list->list[i].x, list->list[i].y);
            }
        }
    }
}

void sugar_free(SugarList *list)
{
    assert(list != NULL);

    free(list->list);
    list->list = NULL; 
    list->count = 0;
}
