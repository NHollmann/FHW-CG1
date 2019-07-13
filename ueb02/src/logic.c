/**
 * @file
 * Logik-Modul.
 * Das Modul kapselt die Programmlogik. Die Programmlogik besteht wesentlich aus 
 * der Verwaltung der Spielerposition, sowie der Verwaltung und Berechnung der
 * Wasserbewegung, der Zuckerwürfel und der Sandsaecke. Die Programmlogik ist 
 * weitgehend unabhaengig von Ein-/Ausgabe (io.h/c) und Darstellung (scene.h/c).
 *
 * Bestandteil einer Uebung im Rahmen des Moduls Praktikum Grundlagen der Computergrafik 
 * an der FH Wedel.
 *
 * @author Nicolas Hollmann, Daniel Klintworth
 */

/* ---- System Header einbinden ---- */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

/* ---- Eigene Header einbinden ---- */
#include "logic.h"

/* ---- Makros ---- */

#define CHECK_BOUNDARY(x) ((x) >= 0 && (x) < (LEVELSIZE))

/* ---- Konstanten ---- */

#define UPDATE_COOLDOWN_TIME (0.8f)
#define PLAYER_COOLDOWN_TIME (0.2f)

/* ---- Leveldaten ---- */

/* Erstes Level */
const wmLevel level1 =
{ 
    {WM_BLACK, WM_WALL, WM_WALL, WM_WALL, WM_BLACK, WM_WALL, WM_WALL, WM_WALL, WM_BLACK},
    {WM_WALL, WM_WALL, WM_START, WM_WALL, WM_WALL, WM_WALL, WM_GOAL, WM_WALL, WM_WALL},
    {WM_WALL, WM_FREE, WM_FREE, WM_FREE, WM_SAND, WM_FREE, WM_WATER, WM_FREE, WM_WALL},
    {WM_WALL, WM_FREE, WM_FREE, WM_FREE, WM_SAND, WM_FREE, WM_WATER, WM_FREE, WM_WALL},
    {WM_WALL, WM_WALL, WM_FREE, WM_FREE, WM_SAND, WM_FREE, WM_FREE, WM_WALL, WM_WALL},
    {WM_BLACK, WM_WALL, WM_WALL, WM_FREE, WM_SAND, WM_FREE, WM_WALL, WM_WALL, WM_BLACK},
    {WM_BLACK, WM_BLACK, WM_WALL, WM_WALL, WM_SAND, WM_WALL, WM_WALL, WM_BLACK, WM_BLACK},
    {WM_BLACK, WM_BLACK, WM_BLACK, WM_WALL, WM_WALL, WM_WALL, WM_BLACK, WM_BLACK, WM_BLACK},
    {WM_BLACK, WM_BLACK, WM_BLACK, WM_BLACK, WM_WALL, WM_BLACK, WM_BLACK, WM_BLACK, WM_BLACK},
};

/* Zweites Level */
const wmLevel level2 =
{ 
    {WM_WALL, WM_WALL, WM_WALL, WM_WALL, WM_WALL, WM_WALL, WM_WALL, WM_WALL, WM_WALL},
    {WM_WALL, WM_FREE, WM_START, WM_FREE, WM_WALL, WM_FREE, WM_FREE, WM_FREE, WM_WALL},
    {WM_WALL, WM_SAND, WM_FREE, WM_FREE, WM_WALL, WM_FREE, WM_FREE, WM_FREE, WM_WALL},
    {WM_WALL, WM_FREE, WM_SAND, WM_FREE, WM_WALL, WM_FREE, WM_FREE, WM_FREE, WM_WALL},
    {WM_WALL, WM_SAND, WM_FREE, WM_SAND, WM_WALL, WM_FREE, WM_FREE, WM_WATER, WM_WALL},
    {WM_WALL, WM_FREE, WM_SAND, WM_FREE, WM_WALL, WM_FREE, WM_FREE, WM_FREE, WM_WALL},
    {WM_WALL, WM_SAND, WM_FREE, WM_SAND, WM_FREE, WM_SUGAR, WM_FREE, WM_FREE, WM_WALL},
    {WM_WALL, WM_FREE, WM_SAND, WM_FREE, WM_GOAL, WM_FREE, WM_SUGAR, WM_FREE, WM_WALL},
    {WM_WALL, WM_WALL, WM_WALL, WM_WALL, WM_WALL, WM_WALL, WM_WALL, WM_WALL, WM_WALL},
};

/* Drittes Level */
const wmLevel level3 =
{ 
    {WM_WALL, WM_WALL, WM_WALL, WM_WALL, WM_WALL, WM_WALL, WM_WALL, WM_WALL, WM_WALL},
    {WM_WALL, WM_START, WM_FREE, WM_FREE, WM_FREE, WM_SAND, WM_WATER, WM_SAND, WM_WALL},
    {WM_WALL, WM_SAND, WM_FREE, WM_FREE, WM_FREE, WM_FREE, WM_SAND, WM_FREE, WM_WALL},
    {WM_WALL, WM_FREE, WM_SAND, WM_FREE, WM_FREE, WM_SAND, WM_FREE, WM_FREE, WM_WALL},
    {WM_WALL, WM_SAND, WM_FREE, WM_SAND, WM_FREE, WM_SAND, WM_SUGAR, WM_SUGAR, WM_WALL},
    {WM_WALL, WM_FREE, WM_SAND, WM_FREE, WM_SAND, WM_SAND, WM_FREE, WM_FREE, WM_WALL},
    {WM_WALL, WM_SAND, WM_FREE, WM_SAND, WM_FREE, WM_SAND, WM_FREE, WM_SUGAR, WM_WALL},
    {WM_WALL, WM_FREE, WM_SAND, WM_FREE, WM_SAND, WM_SAND, WM_SUGAR, WM_GOAL, WM_WALL},
    {WM_WALL, WM_WALL, WM_WALL, WM_WALL, WM_WALL, WM_WALL, WM_WALL, WM_WALL, WM_WALL},
};

/* Viertes extra Level */
const wmLevel level4 =
{ 
    {WM_BLACK, WM_WALL, WM_WALL, WM_WALL, WM_BLACK, WM_WALL, WM_WALL, WM_WALL, WM_BLACK},
    {WM_WALL, WM_WALL, WM_START, WM_WALL, WM_WALL, WM_WALL, WM_FREE, WM_WALL, WM_WALL},
    {WM_WALL, WM_FREE, WM_FREE, WM_FREE, WM_SAND, WM_FREE, WM_FREE, WM_FREE, WM_WALL},
    {WM_WALL, WM_FREE, WM_FREE, WM_FREE, WM_SAND, WM_FREE, WM_WATER, WM_GOAL, WM_WALL},
    {WM_WALL, WM_WALL, WM_FREE, WM_FREE, WM_SAND, WM_FREE, WM_FREE, WM_WALL, WM_WALL},
    {WM_BLACK, WM_WALL, WM_WALL, WM_FREE, WM_SAND, WM_FREE, WM_WALL, WM_WALL, WM_BLACK},
    {WM_BLACK, WM_BLACK, WM_WALL, WM_WALL, WM_SAND, WM_WALL, WM_WALL, WM_BLACK, WM_BLACK},
    {WM_BLACK, WM_BLACK, WM_BLACK, WM_WALL, WM_WALL, WM_WALL, WM_BLACK, WM_BLACK, WM_BLACK},
    {WM_BLACK, WM_BLACK, WM_BLACK, WM_BLACK, WM_WALL, WM_BLACK, WM_BLACK, WM_BLACK, WM_BLACK},
};

/* ---- Globale Daten ---- */

/* Spielzustand */
static Gamestate g_gamestate = EMPTY_GAMESTATE;

/* ---- Interne Funktionen ---- */

/**
 * Behandelt die Bewegung des Spielers.
 * 
 * @param offsetX X-Verschiebung der Bewegungsrichtung (In)
 * @param offsetY Y-Verschiebung der Bewegungsrichtung (In)
 */
static void handleMovement(int offsetX, int offsetY)
{
	/* Ermitteln, ob die Bewegung moeglich ist. */
	GLboolean canMove = GL_FALSE;

	/* Das Feld, auf das der Spieler sich bewegen moechte. */
	wmFieldType fstField = g_gamestate.level[g_gamestate.playerY + offsetY]
											[g_gamestate.playerX + offsetX];

	/* Das darauf folgende Feld. Relevant, wenn das erste Feld ein Sandsack ist. */
	wmFieldType sndField = g_gamestate.level[g_gamestate.playerY + offsetY * 2]
											[g_gamestate.playerX + offsetX * 2];

	if (fstField == WM_FREE || fstField == WM_GOAL) {
		canMove = GL_TRUE;
	} else if (fstField == WM_SAND && (
			   sndField == WM_FREE || 
			   sndField == WM_WATER )) {

		g_gamestate.level[g_gamestate.playerY + offsetY    ]
						 [g_gamestate.playerX + offsetX    ] = WM_FREE;
		g_gamestate.level[g_gamestate.playerY + offsetY * 2]
						 [g_gamestate.playerX + offsetX * 2] = WM_SAND;

		canMove = GL_TRUE;
	}

	/* Bewegung wenn erlaubt ausfuehren. */
	if (canMove) {
		g_gamestate.playerX += offsetX;
		g_gamestate.playerY += offsetY;

		g_gamestate.playerCooldown = PLAYER_COOLDOWN_TIME;
	}
}

/**
 * Prueft, ob die Verlierbedingung erfuellt wurde.
 */
static void checkLosingCondition(void)
{
	wmFieldType playerField = g_gamestate.level[g_gamestate.playerY]
											   [g_gamestate.playerX];
	
	if (playerField == WM_WATER) {
		g_gamestate.stage = stageLost;
	}
}

/**
 * Prueft, ob die Gewinnbedingung erfuellt wurde.
 */
static void checkVictoryCondition(void)
{
	wmFieldType playerField = g_gamestate.level[g_gamestate.playerY]
											   [g_gamestate.playerX];

	if (playerField == WM_GOAL) {
		g_gamestate.stage = stageWon;
	}
}

/**
 * Aktualisiert die Expansion eines einzelnen Wasserfeldes. 
 * Anliegende Zuckerfelder werden nass.
 * 
 * @param x X-Koordinate des Wasserfeldes (In)
 * @param y Y-Koordinate des Wasserfeldes (In)
 */
static void updateWaterTile(int y, int x)
{
	if (CHECK_BOUNDARY(x) && CHECK_BOUNDARY(y)) 
	{
		if (g_gamestate.level[y][x] == WM_FREE) 
		{
			g_gamestate.level[y][x] = WM_NEWWATER;
		} 
		else if (g_gamestate.level[y][x] == WM_SUGAR) 
		{
			g_gamestate.level[y][x] = WM_TOUCHED_SUGAR;
			sugar_add(&g_gamestate.sugarCubes, x, y);
		}
	} 
}

/**
 * Aktualisiert die Expansion aller Wasserfelder.
 */
static void updateWater()
{
	/* Alle Felder einzeln aktualisieren. */
	for (int y = 0; y < LEVELSIZE; y++)
	{
		for (int x = 0; x < LEVELSIZE; x++)
		{
			if (g_gamestate.level[y][x] == WM_WATER) {
				updateWaterTile(y,     x - 1);
				updateWaterTile(y,     x + 1);
				updateWaterTile(y + 1, x);
				updateWaterTile(y - 1, x);
			}
		}
	}

	/* Neue Felder in normale umwandeln. */
	for (int y = 0; y < LEVELSIZE; y++)
	{
		for (int x = 0; x < LEVELSIZE; x++)
		{
			if (g_gamestate.level[y][x] == WM_NEWWATER) {
				g_gamestate.level[y][x] = WM_WATER;
			}
		}
	}
}

/**
 * Loest ein Zuckerfeld auf und ersetzt es durch ein freies.
 * @param x X-Koordinate des Zuckerfeldes (In)
 * @param y Y-Koordinate des Zuckerfeldes (In)
 */
static void dissolveSugar(int x, int y)
{
	g_gamestate.level[y][x] = WM_FREE;
}

/* ---- Oeffentliche Funktionen ---- */

void updateLogic(double interval)
{
	/* Wenn das Level pausiert oder beendet, soll es nicht weiter laufen. */
	if (!g_gamestate.showHelp && g_gamestate.stage == stageRunning)
	{
		/* Cooldown fuer Spielerbewegung */
		if (g_gamestate.playerCooldown > -0.1f) 
		{
			g_gamestate.playerCooldown -= interval;
		}

		checkLosingCondition();

		/* Cooldown fuer Wasserbewegung */
		g_gamestate.updateCooldown -= interval;
		if (g_gamestate.updateCooldown <= 0.0f) 
		{
			g_gamestate.updateCooldown += UPDATE_COOLDOWN_TIME;

			checkVictoryCondition();
			updateWater();
		}

		sugar_update(&g_gamestate.sugarCubes, dissolveSugar, interval);
	}
}

void movePlayer(PlayerDirection direction)
{
	if (!g_gamestate.showHelp && 
		g_gamestate.stage == stageRunning &&
		g_gamestate.playerCooldown <= 0.0f) {
		switch (direction)
		{
			case dirDown:
				handleMovement(0,  1);
				break;
			case dirUp:
				handleMovement(0, -1);
				break;
			case dirLeft:
				handleMovement(-1, 0);
				break;
			case dirRight:
				handleMovement( 1, 0);
				break;
		}
	}
}

void initLevel(int levelId)
{
	/* Level bestimmen */
	wmLevelPointer levelSource = NULL;
	switch (levelId)
	{
		case LEVEL_1:
			levelSource = level1;
			break;
		case LEVEL_2:
			levelSource = level2;
			break;
		case LEVEL_3:
			levelSource = level3;
			break;
		case LEVEL_4:
			levelSource = level4;
			break;
	}

	/* Leveldaten und Spielerposition laden. */
	for (int y = 0; y < LEVELSIZE; y++)
	{
		for (int x = 0; x < LEVELSIZE; x++)
		{
			g_gamestate.level[y][x] = levelSource[y][x];
			if (levelSource[y][x] == WM_START) {
				g_gamestate.playerX = x;
				g_gamestate.playerY = y;
				g_gamestate.level[y][x] = WM_FREE;
			}
		}
	}

	/* Spielzustand Initialisieren */
	g_gamestate.levelId = levelId;
	g_gamestate.stage = stageRunning;
	g_gamestate.showHelp = GL_FALSE;
	g_gamestate.playerCooldown = 0.0f;
	g_gamestate.updateCooldown = UPDATE_COOLDOWN_TIME;
	g_gamestate.loseMessageId = rand() % LOSE_MESSAGE_COUNT;

	sugar_free(&g_gamestate.sugarCubes);
}

Gamestate* getGamestate(void)
{
	return &g_gamestate;
}

void toggleHelp(void)
{
	g_gamestate.showHelp = !g_gamestate.showHelp;
}


void pressedActionKey(void)
{
	switch (g_gamestate.stage) {
		case stageRunning:
			// Keine Aktion
			break;
		case stageLost:
			initLevel(g_gamestate.levelId);
			break;
		case stageWon:
			if (g_gamestate.levelId + 1 > LEVEL_LAST) {
				initLevel(LEVEL_1);
			} else {
				initLevel(g_gamestate.levelId + 1);
			}
			break;
	}
}

void cleanup(void)
{
	sugar_free(&g_gamestate.sugarCubes);
}
