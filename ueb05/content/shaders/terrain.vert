#version 330 core

/** 
 * Position des Vertex. 
 */
layout (location = 0) in vec4 vPosition;

/**
 * Textur-Koordinate des Vertex. 
 */
layout (location = 1) in vec2 vTexCoord;

/** 
 * Textur-Koordinaten
 */
out vec2 fTexCoord;

/**
 * Vertex-Positon.
 */
out vec4 fFragPos;

/**
 * Vertex-Normale.
 */
out vec3 fNormal;

/**
 * Vertex-Tangente.
 */
out vec3 fTangent;

/**
 * Vertex-Binormale.
 */
out vec3 fBinormal;

/**
 * Gouraud Intensitaet.
 */
out vec4 fIntensity;

/** 
 * Projektions-Matrix.
 */
uniform mat4 Projection;

/**
 * View-Matrix. 
 */
uniform mat4 ModelView;

/**
 * Zeit seit Programmstart.
 */
uniform float Time;

/** 
 * Position der Kamera. 
 */
uniform vec3 CameraPos;

/** 
 * Die Szenen-Schalter. 
 */
uniform uint SceneFlags;

/**
 * Heightmap.
 */
uniform sampler2D Heightmap;
const float heightmapFactor = 0.25;

/**
 * Sinuswelle.
 */
const float sineAmplitude = 0.025;
const float sineFreq = 17;

/**
 * Normalenverschiebung.
 */
const float normalOffset = 0.05;

/**
 * Gouraud Modell - Einstellungen Material
 */
const vec3 materialAmbi = vec3(0.1, 0.1, 0.1);
const vec3 materialDiff = vec3(0.85, 0.85, 0.85);
const vec3 materialSpec = vec3(0.5, 0.5, 0.5);
const float materialShi = 20.0;

/**
 * Gouraud Modell - Einstellungen Licht
 */
 const vec3 lightPosition = vec3(0, 0.25, 0);
 const vec3 lightAmbi = vec3(1, 1, 1);
 const vec3 lightDiff = vec3(1, 1, 1);
 const vec3 lightSpec = vec3(1, 1, 1);

/**
 * Berechnet die Farbe/Intensitaet nach dem Gouraud Modell.
 *
 * @param position die Position des Vertex
 * @param normal die Normale dieses Vertex
 * @return Intensitaet
 */
vec4 calcGouraud(vec4 position, vec3 normal)
{
    vec3 lightVec = normalize(lightPosition - position.xyz);
    vec3 reflectVec = reflect(-lightVec, normal);
    vec3 viewVec = normalize(CameraPos - position.xyz);

    vec3 iAmbi = materialAmbi * lightAmbi;
    vec3 iDiff = max(dot(normal, lightVec), 0) * materialDiff * lightDiff; // Backfaces ausschl.
    vec3 iSpec = pow(max(dot(viewVec, reflectVec), 0), materialShi) * materialSpec * lightSpec;

    return vec4(iAmbi + iDiff + iSpec, 1);
}

/**
 * Berechnet die neuen Koordinaten eines Vertex
 * ausgehend von dem aktuellen. Es wird eine Sinuswelle
 * und eine Heightmap mit einbezogen.
 * 
 * @param offset virtuelle Vertex-Verschiebung
 * @return Vektor mit der errechneten Vertex-Position
 */
vec4 calcElevatedPosition(vec2 offset)
{
    vec4 elevatedPosition;
    elevatedPosition.x = vPosition.x + offset.x;
    elevatedPosition.y = vPosition.y;
    elevatedPosition.z = vPosition.z + offset.y;
    elevatedPosition.w = vPosition.w;

    // Pruefen, ob Sinuswelle aktiv ist.
    if (((SceneFlags >> 1u) & 1u) == 1u)
    {
        elevatedPosition.y += sineAmplitude * sin(Time + sineFreq * (elevatedPosition.x + elevatedPosition.z));
    }
    
    // Pruefen, ob Heightmap aktiv ist.
    if (((SceneFlags >> 2u) & 1u) == 1u)
    {
        vec4 height = texture(Heightmap, vTexCoord - offset);
        elevatedPosition.y += height.y * heightmapFactor;
    }

    return elevatedPosition;
}

/**
 * Berechnet die Normalen an diesem Vertex.
 *
 * @return die approximierte Normale
 */
vec3 calcNormal()
{
    vec3 up = calcElevatedPosition(vec2(0, -normalOffset)).xyz;
    vec3 down = calcElevatedPosition(vec2(0, normalOffset)).xyz;
    vec3 right = calcElevatedPosition(vec2(normalOffset, 0)).xyz;
    vec3 left = calcElevatedPosition(vec2(-normalOffset, 0)).xyz;

    vec3 horizontal = right - left;
    vec3 vertical = down - up;

    fTangent = horizontal;
    fBinormal = vertical;

    vec3 normal = cross(vertical, horizontal);
    return normalize(normal);
}

/**
 * Hauptprogramm des Vertex-Shaders.
 */
void main(void)
{
    vec4 elevatedPosition = calcElevatedPosition(vec2(0, 0));
    vec3 vertNormal = calcNormal();

    // Preufen, ob Gouraud aktiviert ist.
    if (((SceneFlags >> 3u) & 1u) == 0u)
    {
        fIntensity = calcGouraud(elevatedPosition, vertNormal);
    }
    else 
    {
        fIntensity = vec4(0, 0, 0, 0);
    }

    fNormal = vertNormal;
    fTexCoord = vTexCoord;
    fFragPos = elevatedPosition;
    gl_Position = Projection * ModelView * elevatedPosition;
}
