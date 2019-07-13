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
#include <float.h>
#include <stdio.h>
#include <stdlib.h>

/* ---- Eigene Header einbinden ---- */
#include "logic.h"

/* ---- Konstanten ---- */

#define CAMERA_COOLDOWN (1.5f)
#define CAMERA_SPEED (15.0f)

#define CAMERA_RADIUS_MIN (0.3f)
#define CAMERA_RADIUS_MAX (2.0f)

#define CAMERA_POLAR_MIN (0.0f)
#define CAMERA_POLAR_MAX (360.0f)

#define CAMERA_AZIMUTH_MIN (5.0f)
#define CAMERA_AZIMUTH_MAX (89.0f)

/* ---- Globale Daten ---- */

/* Spielzustand */
static Gamestate g_gamestate = EMPTY_GAMESTATE;

/* ---- Interne Funktionen ---- */

/**
 * Aendert die Kameraorientierung ohne den Cooldown zurueck zu setzen.
 * 
 * @param deltaRadius die Veraenderung des Radius (In)
 * @param deltaPolar die Veraenderung des Polars (In)
 * @param deltaAzimuth die Veraenderung des Azimuth (In)
 */
void updateCameraOrientation(GLfloat deltaRadius, GLfloat deltaPolar, GLfloat deltaAzimuth)
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

/* ---- Oeffentliche Funktionen ---- */

void updateLogic(double interval)
{
	/* Automatische Kamerarotation */
	if (g_gamestate.cameraCooldown <= 0.0f) 
	{
		updateCameraOrientation(0.0f, interval * CAMERA_SPEED, 0.0f);
	} 
	else 
	{
		g_gamestate.cameraCooldown -= interval;
	}
}

Gamestate* getGamestate(void)
{
	return &g_gamestate;
}

void changeCameraOrientation(GLfloat deltaRadius, GLfloat deltaPolar, GLfloat deltaAzimuth)
{
	if ( fabs(deltaPolar) + fabs(deltaAzimuth) > FLT_EPSILON)
	{
		g_gamestate.cameraCooldown = CAMERA_COOLDOWN;
	}

	updateCameraOrientation(deltaRadius, deltaPolar, deltaAzimuth);
}
