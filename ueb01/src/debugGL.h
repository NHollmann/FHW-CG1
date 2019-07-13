#ifndef __DEBUG_GL_H__
#define __DEBUG_GL_H__
/**
 * @file
 * Schnittstelle des Debug-Moduls fuer OpenGL-Programme.
 * Genutzt werden sollten die Makros #CG_ERROR, #INFO, #GLSTATE und #GLGETERROR.
 *
 * @author copyright (C) Fachhochschule Wedel 1999-2011. All rights reserved.
 */

/* ---- System Header einbinden ---- */
#ifdef WIN32
#include <windows.h>
#endif

#ifdef __APPLE__
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#endif

#include <stdio.h>
#include <stdarg.h>

/* ---- Funktionen ---- */

/**
 * Hilfsfunktion: nicht direkt verwenden.
 * Ausgabe wie mit fprintf auf stderr.
 * @return Rueckgabewert der Ausgabefunktion (vfprintf).
 */
int dbgPrint(const char *template, ...);

/**
 * Hilfsfunktion: nicht direkt verwenden.
 * Ausgabe eines GL-Fehlercode als String auf stderr.
 * @param state GL-Fehlercode (In).
 * @return Rueckgabewert der Ausgabefunktion (fprintf).
 */
int dbgPrintGLState(GLenum state);

/**
 * Hilfsfunktion: nicht direkt verwenden.
 * GL-Fehlercode ermitteln und als String auf stderr ausgeben.
 * @return GL-Fehlercode (von glGetError()).
 */
int dbgGetGLError(void);

/**
 * Hilfsfunktion: nicht direkt verwenden.
 * GL-Fehlercode ermitteln und als String auf stderr ausgeben.
 * @return Rueckgabewert der Ausgabe-Funktion.
 */
int dbgGL(void);

/* ---- Makros ---- */

#ifdef DEBUG
#define CG_ERROR(ARGS)     \
  (dbgPrint("ERR     <%s[%i]>: ", __FILE__, __LINE__), dbgPrint ARGS, exit(1))
#define INFO(ARGS)      \
  (dbgPrint("INFO    <%s[%i]>: ", __FILE__, __LINE__), dbgPrint ARGS)
#define GLSTATE         \
  (dbgPrint("GLSTATE <%s[%i]>: ", __FILE__, __LINE__), dbgGL())
#define GLGETERROR      \
  (dbgPrint("GLSTATE <%s[%i]>: ", __FILE__, __LINE__), dbgGetGLError())
#else
/**
 * Abbruch des Programms. Kompiliert mit -DDEBUG, wie: <code>fprintf (stderr,
 * ".."); exit (1);</code> (mit Ausgabe des Dateinamens und der
 * Zeilennummer).<br>
 * sonst, wie: <code>exit (1);</code><br>
 * Anwendungsbeispiel: <code>CG_ERROR (("Fehler! zahl=%d\n", zahl));</code>
 */
#define CG_ERROR(ARGS)     exit(1)
/**
 * Ausgabe von Informationen. Kompiliert mit -DDEBUG, wie: <code>fprintf
 * (stderr, "..");</code><br> sonst, wie: NULL<br>
 * Anwendungsbeispiel: <code>INFO (("Info! zahl=%d\n", zahl))</code>;
 */
#define INFO(ARGS)
/**
 * Ausgabe des GL-Status (ersetzt GL-Fehlercodes durch entsprechende Strings).
 * Kompiliert mit -DDEBUG: GL-Status ausgeben<br>
 * sonst: nichts tun<br>
 * Anwendungsbeispiel: <code>GLSTATE;</code>
 */
#define GLSTATE
/**
 * Aktuellen GL-Status ermitteln. Kompiliert mit -DDEBUG, wie:
 * <code>glGetError();</code>, mit gleichzeitiger Ausgabe des
 * GL-Fehlercodes<br>
 * sonst, wie: <code>glGetError();</code><br>
 * Anwendungsbeispiel: <code>if (GLGETERROR != GL_NO_ERROR) exit(1);</code>
 */
#define GLGETERROR      glGetError()
#endif

#endif /* __DEBUG_GL_H__ */
