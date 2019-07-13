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

/* ---- Typedeklarationen - Wasser ---- */

/** Die Position eines Wassertiles auf dem Wuerfel. */
typedef enum {
    WFACE_TOP,
    WFACE_BOTTOM,
    WFACE_FRONT,
    WFACE_BACK,
    WFACE_LEFT,
    WFACE_RIGHT,
} WaterSide;

/** Ein einzelnes Wasserface. */
typedef struct {
    WaterSide side;
    float x, y, z;
    float camDistSqr;
} WaterFace;

/** Eine Kollektion von Wasserfaces. */
typedef struct {
    int count;
    WaterFace *list;
} WaterFaceList;


#define EMPTY_WATER_FACE_LIST {0, NULL}

/* ---- Funktionen ---- */

/**
 * Fuegt ein neues Wasserfeld mit 6 Faces hinzu.
 * 
 * @param list Das Array zum Speichern der Faces (InOut)
 * @param x die X-Koordinate (In)
 * @param z die Z-Koordinate (In)
 */
void water_add(WaterFaceList *list, float x, float z);

/**
 * Berechnet fuer alle im Array befindlichen Faces den Abstand zur Kamera.
 * 
 * @param list das Array mit Faces (InOut)
 * @param camX die X-Koordinate (In)
 * @param camY die Y-Koordinate (In)
 * @param camZ die Z-Koordinate (In)
 */
void water_calcDistances(WaterFaceList *list, float camX, float camY, float camZ);

/**
 * Sortiert die im Array befindlichen Faces absteigend nach der Distanz zur Kamera. 
 * Zuvor muessen per water_calcDistances die Distanzen berechnet worden sein.
 * 
 * @param list das Array mit Faces (InOut)
 */
void water_sortDistances(WaterFaceList *list);


/**
 * Gibt den Speicher des dynamischen Arrays wieder frei.
 * 
 * @param list Das zu loeschende Array (InOut)
 */
void water_free(WaterFaceList *list);

#endif
