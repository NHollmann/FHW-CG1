/**
 * @file
 * Hauptprogramm. Initialisierung und Starten der Ergeignisverarbeitung.
 *
 * Bestandteil einer Uebung im Rahmen des Moduls Praktikum Grundlagen der Computergrafik 
 * an der FH Wedel.
 *
 * @author Nicolas Hollmann, Daniel Klintworth
 */

/* ---- System Header einbinden ---- */
#include <stdio.h>

/* ---- Eigene Header einbinden ---- */
#include "io.h"

/* ---- Funktionen ---- */

/**
 * Hauptprogramm.
 * Initialisierung und Starten der Ereignisbehandlung.
 * 
 * @param argc Anzahl der Kommandozeilenparameter (In)
 * @param argv Kommandozeilenparameter (In)
 * @return Rueckgabewert im Fehlerfall ungleich Null
 */
int main(int argc, char **argv)
{
	(void)argc;
	(void)argv;

	/* Initialisierung des I/O-Sytems
	 (inkl. Erzeugung des Fensters und Starten der Ereignisbehandlung) */
	if (!initAndStartIO("Aufgabe 3: Wasser Marsch 2000! (3D) | Daniel & Nico", 500, 500))
	{
		fprintf(stderr, "Initialisierung fehlgeschlagen!\n");
		return 1;
	}

	return 0;
}
