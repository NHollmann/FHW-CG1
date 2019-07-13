/**
 * @file
 * Logik-Modul.
 * Das Modul kapselt die Programmlogik. Die Programmlogik ist 
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
#include "water.h"

/* ---- Konstanten ---- */

#define CAMERA_RADIUS_MIN ( 1.0f)
#define CAMERA_RADIUS_MAX (18.0f)

#define CAMERA_POLAR_MIN (0.0f)
#define CAMERA_POLAR_MAX (360.0f)

#define CAMERA_AZIMUTH_MIN (5.0f)
#define CAMERA_AZIMUTH_MAX (89.0f)

#define WATER_GRID_DEFAULT_SIZE (20)

/* ---- Globale Daten ---- */

/* Spielzustand */
static Gamestate g_gamestate = EMPTY_GAMESTATE;

/* ---- Oeffentliche Funktionen ---- */

void updateLogic(double interval)
{
	/* Wenn die Simulation pausiert ist, soll es nicht weiter laufen. */
	if (!g_gamestate.showHelp)
	{
		updateWaterMotion(&g_gamestate.grid, interval);
	}
}

void initLogic()
{
	/* Spielzustand Initialisieren */
	initWaterGrid(&g_gamestate.grid, WATER_GRID_DEFAULT_SIZE);
}

Gamestate* getGamestate(void)
{
	return &g_gamestate;
}

void toggleHelp(void)
{
	g_gamestate.showHelp = !g_gamestate.showHelp;
}

void changeCameraOrientation(GLfloat deltaRadius, GLfloat deltaPolar, GLfloat deltaAzimuth)
{
	g_gamestate.camera.radius += deltaRadius;
	g_gamestate.camera.polarAngle += deltaPolar;
	g_gamestate.camera.azimuthAngle += deltaAzimuth;

	if (g_gamestate.camera.radius < CAMERA_RADIUS_MIN) {
		g_gamestate.camera.radius = CAMERA_RADIUS_MIN;
	}

	if (g_gamestate.camera.radius > CAMERA_RADIUS_MAX) {
		g_gamestate.camera.radius = CAMERA_RADIUS_MAX;
	}

	if (g_gamestate.camera.polarAngle < CAMERA_POLAR_MIN) {
		g_gamestate.camera.polarAngle += CAMERA_POLAR_MAX;
	}

	if (g_gamestate.camera.polarAngle > CAMERA_POLAR_MAX) {
		g_gamestate.camera.polarAngle -= CAMERA_POLAR_MAX;
	}

	if (g_gamestate.camera.azimuthAngle < CAMERA_AZIMUTH_MIN) {
		g_gamestate.camera.azimuthAngle = CAMERA_AZIMUTH_MIN;
	}

	if (g_gamestate.camera.azimuthAngle > CAMERA_AZIMUTH_MAX) {
		g_gamestate.camera.azimuthAngle = CAMERA_AZIMUTH_MAX;
	}
}

void handleMousePick(int pickName, GLboolean leftButton)
{
	changeWaterHeight(&g_gamestate.grid, pickName, leftButton);
}

void changeWaterGrid(GLboolean increase)
{
	changeWaterGridSize(&g_gamestate.grid, increase);
}

void cleanup(void)
{
	cleanupWater(&g_gamestate.grid);
}
