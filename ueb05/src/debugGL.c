/**
 * @file
 * Debug-Modul fuer OpenGL-Programme.
 * Genutzt werden sollten die Makros #CG_ERROR, #INFO, #GLSTATE und #GLGETERROR.
 *
 * @author copyright (C) Fachhochschule Wedel 1999-2011. All rights reserved.
 */

/* ---- Eigene Header einbinden ---- */
#include "debugGL.h"

/* ---- Funktionen ---- */

/**
 * Hilfsfunktion: nicht direkt verwenden.
 * Ausgabe wie mit fprintf auf stderr.
 * @return Rueckgabewert der Ausgabefunktion (vfprintf).
 */
int dbgPrint(const char *template, ...)
{
  va_list ap;
  int result;

  va_start(ap, template);
  result = vfprintf (stderr, template, ap);
  va_end(ap);
  return result;
}

/**
 * Hilfsfunktion: nicht direkt verwenden.
 * Ausgabe eines GL-Fehlercode als String auf stderr.
 * @param state GL-Fehlercode (In).
 * @return Rueckgabewert der Ausgabefunktion (fprintf).
 */
int dbgPrintGLState(GLenum state)
{
  if (state == GL_NO_ERROR)
    {
      return fprintf (stderr, "OK: %s\n", gluErrorString (state));
    }
  else
    {
      return fprintf (stderr, "NOT OK: %s\n", gluErrorString (state));
    }
}

/**
 * Hilfsfunktion: nicht direkt verwenden.
 * GL-Fehlercode ermitteln und als String auf stderr ausgeben.
 * @return GL-Fehlercode (von glGetError()).
 */
int dbgGetGLError(void)
{
  GLenum errorCode = glGetError ();

  dbgPrintGLState (errorCode);

  return errorCode;
}

/**
 * Hilfsfunktion: nicht direkt verwenden.
 * GL-Fehlercode ermitteln und als String auf stderr ausgeben.
 * @return Rueckgabewert der Ausgabe-Funktion.
 */
int dbgGL(void)
{
  /* GLStatuswert ermitteln */
  GLenum errorCode = glGetError ();
  /* In String umwandeln und zurueckgeben */
  return dbgPrintGLState (errorCode);
}
