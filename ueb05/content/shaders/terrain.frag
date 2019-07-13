#version 330 core

/**
 * Interpolierte Textur-Koordinate des Fragments.
 */
in vec2 fTexCoord;

/**
 * Interpolierte Fragment-Koordinate des Fragments.
 */
in vec4 fFragPos;

/**
 * Normale des Fragments.
 */
in vec3 fNormal;

/**
 * Tangente des Fragments.
 */
in vec3 fTangent;

/**
 * Binormale des Fragments.
 */
in vec3 fBinormal;

/**
 * Gouraud Intensitaet.
 */
in vec4 fIntensity;

/**
 * Farbe des Fragments, die auf den Framebuffer geschrieben wird.
 */
out vec4 FragColor;

/**
 * Position der Kamera.
 */
uniform vec3 CameraPos;

/**
 * Szenen-Schalter.
 */
uniform uint SceneFlags;

/**
 * View-Matrix. 
 */
uniform mat4 ModelView;

/**
 * Felsentextur.
 */
uniform sampler2D RockTex;

/**
 * Schneetextur.
 */
uniform sampler2D SnowTex;

/**
 * Bumpmap.
 */
uniform sampler2D NormalTex;

/**
 * Texturuebergang.
 */
const float rockMaxHeight = 0.08;
const float snowMinHeight = 0.15;

/**
 * Phong Modell - Einstellungen Material
 */
const vec3 materialAmbi = vec3(0.1, 0.1, 0.1);
const vec3 materialDiff = vec3(0.85, 0.85, 0.85);
const vec3 materialSpec = vec3(0.5, 0.5, 0.5);
const float materialShi = 20.0;

/**
 * Phong Modell - Einstellungen Licht
 */
 const vec3 lightPosition = vec3(0, 0.25, 0);
 const vec3 lightAmbi = vec3(1, 1, 1);
 const vec3 lightDiff = vec3(1, 1, 1);
 const vec3 lightSpec = vec3(1, 1, 1);

/**
 * Berechnet die Farbe/Intensitaet nach dem Phong Modell.
 *
 * @return Intensitaet
 */
vec4 calcPhong()
{
    vec3 normal = normalize(fNormal); // Renormalisieren um Interpolation entgegen zu wirken.

    // Pruefen, ob Bumpmap aktiviert ist.
    if (((SceneFlags >> 4u) & 1u) == 1u)
    {
        vec3 bumpMap = texture(NormalTex, fTexCoord).rgb;
        bumpMap = (bumpMap * 2.0) - 1.0;
        normal = normalize(normal + bumpMap.x * fTangent + bumpMap.y * fBinormal);
    }

    vec3 lightVec = normalize(lightPosition - fFragPos.xyz);
    vec3 reflectVec = reflect(-lightVec, normal);
    vec3 viewVec = normalize(CameraPos - fFragPos.xyz);

    vec3 iAmbi = materialAmbi * lightAmbi;
    vec3 iDiff = max(dot(normal, lightVec), 0) * materialDiff * lightDiff; // Backfaces ausschl.
    vec3 iSpec = pow(max(dot(viewVec, reflectVec), 0), materialShi) * materialSpec * lightSpec;

    return vec4(iAmbi + iDiff + iSpec, 1);
}

/**
 * Hauptprogramm des Fragment-Shaders.
 */
void main(void)
{
    vec4 preColor = vec4(1, 1, 1, 1);

    // Pruefen, ob Texturen aktiviert sind.
    if (((SceneFlags >> 0u) & 1u) == 1u)
    {
        vec4 rockColor = texture(RockTex, fTexCoord);
        vec4 snowColor = texture(SnowTex, fTexCoord);

        float alpha = smoothstep(rockMaxHeight, snowMinHeight, fFragPos.y);
        preColor = mix(rockColor, snowColor, alpha);
    }

    // Preufen, ob Phong aktiviert ist.
    if (((SceneFlags >> 3u) & 1u) == 1u)
    {
        FragColor = preColor * calcPhong();
    }
    else 
    {
        FragColor = preColor * fIntensity;
    }
}
