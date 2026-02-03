#include "Shaders.h"
#include "Trace.h"

// Use appropriate GLSL version for platform
// Desktop GL 3.0 uses GLSL 130, OpenGL ES 2.0 uses GLSL 100
#if defined(PS_PLATFORM_ANDROID) || defined(PS_PLATFORM_IOS)
#define GLSL(src) "#version 100\n" #src
#else
// Desktop GL - use version 130 which supports precision qualifiers as no-ops
#define GLSL(src) "#version 130\n" #src
#endif

const char simpleVertexShaderStr[] = GLSL(
    precision mediump float;
    uniform mat4 u_mvpMatrix;
    attribute vec4 a_position;
    attribute vec4 a_colour;
    varying lowp vec4 v_colour;
    void main()
    {
        gl_Position = u_mvpMatrix * a_position;
        v_colour = a_colour;
    }
);

const char simpleFragmentShaderStr[] = GLSL(
    precision lowp float;
    varying vec4 v_colour;
    void main()
    {
        gl_FragColor = v_colour;
    }
);

const char controllerVertexShaderStr[] = GLSL(
    precision mediump float;
    uniform mat4 u_mvpMatrix;
    attribute vec4 a_position;
    void main()
    {
        gl_Position = u_mvpMatrix * a_position;
    }
);

const char controllerFragmentShaderStr[] = GLSL(
    precision lowp float;
    uniform vec4 u_colour;
    void main()
    {
        gl_FragColor = u_colour;
    }
);

const char skyboxVertexShaderStr[] = GLSL(
    precision mediump float;
    uniform mat4   u_mvpMatrix;
    attribute vec4 a_position;
    attribute vec2 a_texCoord;
    varying lowp vec2 v_texCoord;
    void main()
    {
        gl_Position = u_mvpMatrix * a_position;
        v_texCoord = a_texCoord;
    }
);

const char skyboxFragmentShaderStr[] = GLSL(
    precision mediump float;
    varying vec2 v_texCoord;
    uniform sampler2D u_texture;
    void main()
    {
        gl_FragColor = texture2D(u_texture, v_texCoord);
    }
);

const char plainVertexShaderStr[] = GLSL(
    precision mediump float;
    uniform mat4 u_mvpMatrix;
    uniform mat4 u_textureMatrix;
    attribute vec4 a_position;
    attribute lowp vec4 a_colour;
    varying lowp vec2 v_texCoord;
    varying lowp vec4 v_colour;
    void main()
    {
        gl_Position = u_mvpMatrix * a_position;
        v_texCoord = (u_textureMatrix * a_position).xy;
        v_colour = a_colour;
    }
);

const char plainFragmentShaderStr[] = GLSL(
    precision lowp float;
    varying vec2 v_texCoord;
    uniform sampler2D u_texture;
    varying vec4 v_colour;
    void main()
    {
        gl_FragColor = texture2D(u_texture, v_texCoord);
        gl_FragColor *= v_colour;
    }
);

const char terrainVertexShaderStr[] = GLSL(
    precision mediump float;
    uniform mat4 u_mvpMatrix;
    uniform mat4 u_textureMatrix0;
    uniform mat4 u_textureMatrix1;
    attribute vec4 a_position;
    varying lowp vec2 v_texCoord1;
    varying lowp vec2 v_texCoord0;
    void main()
    {
        gl_Position = u_mvpMatrix * a_position;
        v_texCoord0 = (u_textureMatrix0 * a_position).xy;
        v_texCoord1 = (u_textureMatrix1 * a_position).xy;
    }
);

const char terrainFragmentShaderStr[] = GLSL(
    precision lowp float;
    varying vec2 v_texCoord0;
    varying vec2 v_texCoord1;
    uniform sampler2D u_texture0;
    uniform sampler2D u_texture1;
    void main()
    {
        gl_FragColor = texture2D(u_texture0, v_texCoord0);
        gl_FragColor *= texture2D(u_texture1, v_texCoord1);
    }
);

const char terrainPanoramaVertexShaderStr[] = GLSL(
    precision mediump float;
    uniform mat4 u_mvpMatrix;
    attribute vec4 a_position;
    void main()
    {
        gl_Position = u_mvpMatrix * a_position;
    }
);

const char terrainPanoramaFragmentShaderStr[] = GLSL(
    precision lowp float;
    void main()
    {
        gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);
    }
);

const char overlayVertexShaderStr[] = GLSL(
    precision mediump float;
    uniform mat4 u_mvpMatrix;
    attribute vec4 a_position;
    attribute vec2 a_texCoord;
    varying lowp vec2 v_texCoord;
    void main()
    {
        gl_Position = u_mvpMatrix * a_position;
        v_texCoord = a_texCoord;
    }
);

const char overlayFragmentShaderStr[] = GLSL(
    precision lowp float;
    varying vec2 v_texCoord;
    uniform sampler2D u_texture;
    uniform vec4 u_colour;
    void main()
    {
        gl_FragColor = u_colour * texture2D(u_texture, v_texCoord);
    }
);

const char modelVertexShaderStr[] = GLSL(
    precision mediump float;
    uniform mat4 u_mvpMatrix;
    uniform mat3 u_normalMatrix;
    attribute vec4 a_position;
    attribute vec3 a_normal;
    attribute vec4 a_colour;
    varying lowp vec4 v_colour;
    varying lowp vec3 v_normal;
    void main()
    {
        gl_Position = u_mvpMatrix * a_position;
        v_normal    = u_normalMatrix * a_normal;
        v_colour    = a_colour;
    }
);

const char modelFragmentShaderStr[] = GLSL(
    precision lowp float;
    uniform vec3      u_lightDir[5];
    uniform vec4      u_lightDiffuseColour[5];
    uniform vec4      u_lightSpecularColour[5];
    uniform vec4      u_lightAmbientColour[5];
    uniform float     u_specularAmount;
    uniform float     u_specularExponent;
    varying vec4      v_colour;
    varying lowp vec3 v_normal;

    vec4 processLight(
        vec3 normal,
        vec3 lightDir,
        vec4 lightDiffuseColour,
        vec4 lightSpecularColour,
        vec4 lightAmbientColour)
    {
        vec4 colour = lightAmbientColour * v_colour;
        // Diffuse
        lowp float ndotl = max(0.0, dot(normal, lightDir));
        colour += ndotl * lightDiffuseColour * v_colour;
        // Specular
        lowp vec3 h_vec = normalize(lightDir + vec3(0,0,1));
        lowp float ndoth = dot(normal, h_vec);
        if (ndoth > 0.0)
        {
            colour += (pow(ndoth, u_specularExponent) * 
                vec4(u_specularAmount, u_specularAmount, u_specularAmount, 1) * lightSpecularColour);
        }
        return colour;
    }

    void main()
    {
        vec3 normal = normalize(v_normal);
        gl_FragColor  = processLight(normal, u_lightDir[0], u_lightDiffuseColour[0], u_lightSpecularColour[0], u_lightAmbientColour[0]);
        gl_FragColor += processLight(normal, u_lightDir[1], u_lightDiffuseColour[1], u_lightSpecularColour[1], u_lightAmbientColour[1]);
        gl_FragColor += processLight(normal, u_lightDir[2], u_lightDiffuseColour[2], u_lightSpecularColour[2], u_lightAmbientColour[2]);
        gl_FragColor += processLight(normal, u_lightDir[3], u_lightDiffuseColour[3], u_lightSpecularColour[3], u_lightAmbientColour[3]);
        gl_FragColor += processLight(normal, u_lightDir[4], u_lightDiffuseColour[4], u_lightSpecularColour[4], u_lightAmbientColour[4]);
        gl_FragColor.a = v_colour.a;
    }
);

const char texturedModelVertexShaderStr[] = GLSL(
    precision mediump float;
    uniform mat4 u_mvpMatrix;
    uniform mat3 u_normalMatrix;
    attribute vec4 a_position;
    attribute vec3 a_normal;
    attribute vec4 a_colour;
    attribute vec2 a_texCoord;
    varying lowp vec4 v_colour;
    varying lowp vec3 v_normal;
    varying lowp vec2 v_texCoord;
    void main()
    {
        gl_Position = u_mvpMatrix * a_position;
        v_normal    = u_normalMatrix * a_normal;
        v_colour    = a_colour;
        v_texCoord = a_texCoord;
    }
);

const char texturedModelFragmentShaderStr[] = GLSL(
    precision lowp float;
    uniform vec3      u_lightDir[5];
    uniform vec4      u_lightDiffuseColour[5];
    uniform vec4      u_lightSpecularColour[5];
    uniform vec4      u_lightAmbientColour[5];
    uniform float     u_specularAmount;
    uniform float     u_specularExponent;
    uniform sampler2D u_texture;
    uniform float     u_texBias;
    varying vec4      v_colour;
    varying lowp vec3 v_normal;
    varying vec2      v_texCoord;

    vec4 processLight(
        vec3 normal,
        vec3 lightDir,
        vec4 lightDiffuseColour,
        vec4 lightSpecularColour,
        vec4 lightAmbientColour)
    {
        vec4 colour = lightAmbientColour * v_colour;
        // Diffuse
        lowp float ndotl = max(0.0, dot(normal, lightDir));
        colour += ndotl * lightDiffuseColour * v_colour;
        // Specular
        lowp vec3 h_vec = normalize(lightDir + vec3(0,0,1));
        lowp float ndoth = dot(normal, h_vec);
        if (ndoth > 0.0)
        {
            colour += (pow(ndoth, u_specularExponent) * 
                vec4(u_specularAmount, u_specularAmount, u_specularAmount, 1) * lightSpecularColour);
        }
        return colour;
    }

    void main()
    {
        vec3 normal = normalize(v_normal);
        gl_FragColor  = processLight(normal, u_lightDir[0], u_lightDiffuseColour[0], u_lightSpecularColour[0], u_lightAmbientColour[0]);
        gl_FragColor += processLight(normal, u_lightDir[1], u_lightDiffuseColour[1], u_lightSpecularColour[1], u_lightAmbientColour[1]);
        gl_FragColor += processLight(normal, u_lightDir[2], u_lightDiffuseColour[2], u_lightSpecularColour[2], u_lightAmbientColour[2]);
        gl_FragColor += processLight(normal, u_lightDir[3], u_lightDiffuseColour[3], u_lightSpecularColour[3], u_lightAmbientColour[3]);
        gl_FragColor += processLight(normal, u_lightDir[4], u_lightDiffuseColour[4], u_lightSpecularColour[4], u_lightAmbientColour[4]);
        gl_FragColor.a = v_colour.a;
        gl_FragColor = min(gl_FragColor, 1.0);
        gl_FragColor *= texture2D(u_texture, v_texCoord, u_texBias);
    }
);

const char texturedModelSeparateSpecularFragmentShaderStr[] = GLSL(
    precision lowp float;
    uniform vec3      u_lightDir[5];
    uniform vec4      u_lightDiffuseColour[5];
    uniform vec4      u_lightSpecularColour[5];
    uniform vec4      u_lightAmbientColour[5];
    uniform float     u_specularAmount;
    uniform float     u_specularExponent;
    uniform sampler2D u_texture;
    uniform float     u_texBias;
    varying vec4      v_colour;
    varying lowp vec3 v_normal;
    varying vec2      v_texCoord;

    vec4 processLight(
        vec4 fragColour,
        vec3 normal,
        vec3 lightDir,
        vec4 lightDiffuseColour,
        vec4 lightSpecularColour,
        vec4 lightAmbientColour)
    {
        vec4 colour = lightAmbientColour * fragColour;
        // Diffuse
        lowp float ndotl = max(0.0, dot(normal, lightDir));
        colour += ndotl * lightDiffuseColour * fragColour;
        // Specular
        lowp vec3 h_vec = normalize(lightDir + vec3(0,0,1));
        lowp float ndoth = dot(normal, h_vec);
        if (ndoth > 0.0)
        {
            colour += (pow(ndoth, u_specularExponent) * 
                vec4(u_specularAmount, u_specularAmount, u_specularAmount, 1) * lightSpecularColour);
        }
        return colour;
    }

    void main()
    {
        vec3 normal = normalize(v_normal);
        vec4 texColour = texture2D(u_texture, v_texCoord, u_texBias);
        vec4 fragColour = v_colour * texColour;
        gl_FragColor  = processLight(fragColour, normal, u_lightDir[0], u_lightDiffuseColour[0], u_lightSpecularColour[0], u_lightAmbientColour[0]);
        gl_FragColor += processLight(fragColour, normal, u_lightDir[1], u_lightDiffuseColour[1], u_lightSpecularColour[1], u_lightAmbientColour[1]);
        gl_FragColor += processLight(fragColour, normal, u_lightDir[2], u_lightDiffuseColour[2], u_lightSpecularColour[2], u_lightAmbientColour[2]);
        gl_FragColor += processLight(fragColour, normal, u_lightDir[3], u_lightDiffuseColour[3], u_lightSpecularColour[3], u_lightAmbientColour[3]);
        gl_FragColor += processLight(fragColour, normal, u_lightDir[4], u_lightDiffuseColour[4], u_lightSpecularColour[4], u_lightAmbientColour[4]);
        gl_FragColor.a = v_colour.a * texColour.a;
        gl_FragColor = min(gl_FragColor, 1.0);
    }
);


const char shadowVertexShaderStr[] = GLSL(
    precision mediump float;
    uniform mat4 u_mvpMatrix;
    uniform mat4 u_textureMatrix;
    attribute vec4 a_position;
    attribute vec4 a_texCoord;
    varying lowp vec2 v_texCoord;
    void main()
    {
        gl_Position = u_mvpMatrix * a_position;
        v_texCoord = (u_textureMatrix * a_texCoord).xy;
    }
);

const char shadowFragmentShaderStr[] = GLSL(
    precision lowp float;
    varying vec2 v_texCoord;
    uniform sampler2D u_texture;
    uniform vec4 u_colour;
    void main()
    {
        gl_FragColor = u_colour * texture2D(u_texture, v_texCoord);
    }
);

#if 0
// Shadow blur shader - uses vertex shader same as regular shadow
// Uses 9-tap blur for shadowBlur <= 1.0, 25-tap for shadowBlur > 1.0
const char shadowBlurFragmentShaderStr[] = GLSL(
    precision mediump float;
    varying vec2 v_texCoord;
    uniform sampler2D u_texture;
    uniform vec4 u_colour;
    uniform float u_blurAmount;
    uniform vec2 u_texelSize;
    void main()
    {
        vec4 color = vec4(0.0);

        if (u_blurAmount > 1.0)
        {
            // 21-tap Gaussian-weighted blur (normalized weights sum to 1.0)
            // Based on Gaussian kernel with sigma ~1.0, excluding far corners
            vec2 t1 = u_texelSize * u_blurAmount;        // 1 texel offset
            vec2 t2 = u_texelSize * u_blurAmount * 2.0;  // 2 texel offset

            // Center (weight 0.1524)
            color += texture2D(u_texture, v_texCoord) * 0.1524;

            // 4 adjacent at distance 1 (weight 0.0966 each)
            color += texture2D(u_texture, v_texCoord + vec2(t1.x, 0.0)) * 0.0966;
            color += texture2D(u_texture, v_texCoord - vec2(t1.x, 0.0)) * 0.0966;
            color += texture2D(u_texture, v_texCoord + vec2(0.0, t1.y)) * 0.0966;
            color += texture2D(u_texture, v_texCoord - vec2(0.0, t1.y)) * 0.0966;

            // 4 diagonal at distance sqrt(2) (weight 0.0595 each)
            color += texture2D(u_texture, v_texCoord + vec2(t1.x, t1.y)) * 0.0595;
            color += texture2D(u_texture, v_texCoord + vec2(-t1.x, t1.y)) * 0.0595;
            color += texture2D(u_texture, v_texCoord + vec2(t1.x, -t1.y)) * 0.0595;
            color += texture2D(u_texture, v_texCoord + vec2(-t1.x, -t1.y)) * 0.0595;

            // 4 far adjacent at distance 2 (weight 0.0260 each)
            color += texture2D(u_texture, v_texCoord + vec2(t2.x, 0.0)) * 0.0260;
            color += texture2D(u_texture, v_texCoord - vec2(t2.x, 0.0)) * 0.0260;
            color += texture2D(u_texture, v_texCoord + vec2(0.0, t2.y)) * 0.0260;
            color += texture2D(u_texture, v_texCoord - vec2(0.0, t2.y)) * 0.0260;

            // 8 knight-move positions at distance sqrt(5) (weight 0.0149 each)
            color += texture2D(u_texture, v_texCoord + vec2(t2.x, t1.y)) * 0.0149;
            color += texture2D(u_texture, v_texCoord + vec2(t2.x, -t1.y)) * 0.0149;
            color += texture2D(u_texture, v_texCoord + vec2(-t2.x, t1.y)) * 0.0149;
            color += texture2D(u_texture, v_texCoord + vec2(-t2.x, -t1.y)) * 0.0149;
            color += texture2D(u_texture, v_texCoord + vec2(t1.x, t2.y)) * 0.0149;
            color += texture2D(u_texture, v_texCoord + vec2(-t1.x, t2.y)) * 0.0149;
            color += texture2D(u_texture, v_texCoord + vec2(t1.x, -t2.y)) * 0.0149;
            color += texture2D(u_texture, v_texCoord + vec2(-t1.x, -t2.y)) * 0.0149;
        }
        else
        {
            // 9-tap 3x3 Gaussian-weighted blur (normalized weights sum to 1.0)
            vec2 offset1 = u_texelSize * u_blurAmount;
            vec2 offset2 = offset1 * 0.7071; // diagonal offset (1/sqrt(2))

            // Center (weight 0.1884)
            color += texture2D(u_texture, v_texCoord) * 0.1884;

            // 4 adjacent taps (weight 0.1614 each)
            color += texture2D(u_texture, v_texCoord + vec2(offset1.x, 0.0)) * 0.1614;
            color += texture2D(u_texture, v_texCoord - vec2(offset1.x, 0.0)) * 0.1614;
            color += texture2D(u_texture, v_texCoord + vec2(0.0, offset1.y)) * 0.1614;
            color += texture2D(u_texture, v_texCoord - vec2(0.0, offset1.y)) * 0.1614;

            // 4 diagonal taps (weight 0.0415 each)
            color += texture2D(u_texture, v_texCoord + offset2) * 0.0415;
            color += texture2D(u_texture, v_texCoord - offset2) * 0.0415;
            color += texture2D(u_texture, v_texCoord + vec2(offset2.x, -offset2.y)) * 0.0415;
            color += texture2D(u_texture, v_texCoord + vec2(-offset2.x, offset2.y)) * 0.0415;
        }

        gl_FragColor = u_colour * color;
    }
);

#else

// Shadow blur shader - uses vertex shader same as regular shadow
// Optimized using bilinear filtering: 5-tap for shadowBlur <= 1.0, 9-tap for shadowBlur > 1.0
const char shadowBlurFragmentShaderStr[] = GLSL(
    precision mediump float;
    varying vec2 v_texCoord;
    uniform sampler2D u_texture;
    uniform vec4 u_colour;
    uniform float u_blurAmount;
    uniform vec2 u_texelSize;
    void main()
    {
        vec4 color = vec4(0.0);

        if (u_blurAmount > 1.0)
        {
            // 9-tap optimized (was 21-tap) using bilinear filtering
            // Combines axis + diagonal samples using hardware interpolation
            vec2 t1 = u_texelSize * u_blurAmount;
            vec2 t2 = t1 * 2.0;

            // Weights renormalized so they sum to 1.0 (original sum was 0.8812)
            // Center (weight 0.1730)
            color = texture2D(u_texture, v_texCoord) * 0.1730;

            // Inner ring: 4 bilinear samples combining axis + diagonal
            // Sample position offset: 0.236, weight: 0.1434
            float innerBias = 0.236;
            float innerWeight = 0.1434;
            color += texture2D(u_texture, v_texCoord + vec2(t1.x, t1.y * innerBias)) * innerWeight;
            color += texture2D(u_texture, v_texCoord + vec2(-t1.x, -t1.y * innerBias)) * innerWeight;
            color += texture2D(u_texture, v_texCoord + vec2(t1.x * innerBias, t1.y)) * innerWeight;
            color += texture2D(u_texture, v_texCoord + vec2(-t1.x * innerBias, -t1.y)) * innerWeight;

            // Outer ring: 4 bilinear samples combining far-axis + knights
            // Sample position bias: 0.534, weight: 0.0633
            float outerBias = 0.534;
            float outerWeight = 0.0633;
            color += texture2D(u_texture, v_texCoord + vec2(t2.x, t1.y * outerBias)) * outerWeight;
            color += texture2D(u_texture, v_texCoord + vec2(-t2.x, -t1.y * outerBias)) * outerWeight;
            color += texture2D(u_texture, v_texCoord + vec2(t1.x * outerBias, t2.y)) * outerWeight;
            color += texture2D(u_texture, v_texCoord + vec2(-t1.x * outerBias, -t2.y)) * outerWeight;
        }
        else
        {
            // 5-tap optimized (was 9-tap) using bilinear filtering
            // Combines axis-adjacent (0.1614) with diagonal (0.0415*0.5) samples
            vec2 t = u_texelSize * u_blurAmount;

            // Center (weight 0.1884)
            color = texture2D(u_texture, v_texCoord) * 0.1884;

            // 4 bilinear samples: axis weight + half diagonal weight each
            // Bias toward diagonal: 0.0208 / (0.1614 + 0.0208) = 0.114
            // Combined weight: 0.1614 + 0.0208 = 0.1822 (adjusted for 4 samples)
            // Renormalized: center 0.20, each sample 0.20 (sum = 1.0)
            float bilinearBias = 0.114;
            float combinedWeight = 0.2029;
            color += texture2D(u_texture, v_texCoord + vec2(t.x, t.y * bilinearBias)) * combinedWeight;
            color += texture2D(u_texture, v_texCoord + vec2(-t.x, -t.y * bilinearBias)) * combinedWeight;
            color += texture2D(u_texture, v_texCoord + vec2(t.x * bilinearBias, t.y)) * combinedWeight;
            color += texture2D(u_texture, v_texCoord + vec2(-t.x * bilinearBias, -t.y)) * combinedWeight;
        }

        gl_FragColor = u_colour * color;
    }
);

#endif

const char smokeVertexShaderStr[] = GLSL(
    precision mediump float;
    uniform mat4 u_mvpMatrix;
    attribute vec4 a_position;
    attribute vec2 a_texCoord;
    varying lowp vec2 v_texCoord;
    void main()
    {
        gl_Position = u_mvpMatrix * a_position;
        v_texCoord = a_texCoord.xy;
    }
);

const char smokeFragmentShaderStr[] = GLSL(
    precision lowp float;
    varying vec2 v_texCoord;
    uniform sampler2D u_texture;
    uniform lowp vec4 u_colour;
    void main()
    {
        gl_FragColor = texture2D(u_texture, v_texCoord);
        gl_FragColor *= u_colour;
    }
);

// VR Skybox with depth-based parallax for stereoscopic effect
const char skyboxVRParallaxVertexShaderStr[] = GLSL(
    precision mediump float;
    uniform mat4 u_mvpMatrix;
    attribute vec4 a_position;
    attribute vec2 a_texCoord;
    varying vec2 v_texCoord;
    varying vec3 v_position;
    void main()
    {
        gl_Position = u_mvpMatrix * a_position;
        v_texCoord = a_texCoord;
        v_position = a_position.xyz;
    }
);

const char skyboxVRParallaxFragmentShaderStr[] = GLSL(
    precision mediump float;
    varying vec2 v_texCoord;
    varying vec3 v_position;
    uniform sampler2D u_skyboxTexture;
    uniform sampler2D u_depthTexture;
    uniform float u_eyeOffset;      // -1.0 for left eye, +1.0 for right eye
    uniform float u_ipd;            // interpupillary distance in world units
    uniform float u_nearPlane;
    uniform float u_farPlane;
    uniform vec2 u_screenSize;      // screen width and height
    uniform vec3 u_eyeRightLocal;   // eye separation direction in face-local coords (x=forward)
    uniform float u_tileScale;      // numPerSide - scales parallax for tile coordinates
    uniform vec2 u_tileOffset;      // tile translation offset (y, z components)
    uniform vec2 u_tanFovMin;       // vec2(tanLeft, tanDown) for depth correction
    uniform vec2 u_tanFovMax;       // vec2(tanRight, tanUp) for depth correction

    void main()
    {
        // Sample depth at this fragment's screen position
        vec2 screenUV = gl_FragCoord.xy / u_screenSize;
        float depthSample = texture2D(u_depthTexture, screenUV).r;

        // Convert from normalized depth [0,1] to linear depth. This is the perpendicular distance - 
        // i.e. along the look direction (not the absolute distance).
        float linearDepth = u_nearPlane * u_farPlane /
                            (u_farPlane - depthSample * (u_farPlane - u_nearPlane));
                            
        // Correct for screen position: linearDepth is perpendicular distance,
        // but we need actual radial distance for proper parallax.
        // Map screen UV to view ray direction using FOV tangents.
        vec2 viewRay = mix(u_tanFovMin, u_tanFovMax, screenUV);
        float cosAngle = 1.0 / sqrt(1.0 + dot(viewRay, viewRay));
        linearDepth = linearDepth / cosAngle;
//        linearDepth = linearDepth / (1 + cosAngle * 0.000001);

        // At this point, if we render distance above a flat ground then we get circles around the 
        // observer.

        // For sky pixels (at or near far plane), no parallax (sky is at infinity)
        vec2 uvOffset = vec2(0.0);
        if (depthSample < 0.9999)
        {
            // Skybox position accounting for tile offset
            vec3 skyboxPos;
            skyboxPos.x = v_position.x;
            skyboxPos.y = v_position.y + u_tileOffset.x;
            skyboxPos.z = v_position.z + u_tileOffset.y;
            float x = skyboxPos.x;
            float x2 = x * x;

            // Position shift in eye-right direction, projected to skybox distance
            // parallaxShift = (ipd/2) * eyeOffset * (skyboxDist / objectDist)
            float parallaxShift = (u_ipd * 0.5) * u_eyeOffset * (x / linearDepth);

            // Compute per-pixel UV shift using Jacobian of UV mapping
            // UV = (0.5 - y/(2x), 0.5 - z/(2x)) for the front face vertex data
            // dU = (y·dx - x·dy) / (2x²), dV = (z·dx - x·dz) / (2x²)
            // where (dx, dy, dz) = u_eyeRightLocal direction
            float dU = (skyboxPos.y * u_eyeRightLocal.x - x * u_eyeRightLocal.y) / (2.0 * x2);
            float dV = (skyboxPos.z * u_eyeRightLocal.x - x * u_eyeRightLocal.z) / (2.0 * x2);

            uvOffset = vec2(dU, dV) * parallaxShift * u_tileScale;

            // We need to increase the uv offset away from the face centre to account for the 
            // fact that the pixel is further away from the viewer. Increase by 1/cos(phi)
            float correction = length(skyboxPos) / skyboxPos.x;
            uvOffset *= correction; 
        }
        vec2 offsetUV = v_texCoord + uvOffset;

        // Clamp UV to valid range
        offsetUV = clamp(offsetUV, 0.0, 1.0);

        gl_FragColor = texture2D(u_skyboxTexture, offsetUV);

        // Debug overrides. Need to reference things to avoid errors!
        if (false)
        {
            if (true)
            {
                float g = clamp(linearDepth / 30.0, 0.0, 1.0);
                gl_FragColor = gl_FragColor * 0.001 + vec4(g + v_texCoord.x * 0.0001, g, g, 1.0);
            }
            else
            {
                // Debug visualization:
                // R = original texture (using v_texCoord, no parallax)
                // G = U offset (scaled for visibility, centered at 0.5)
                // B = V offset (scaled for visibility, centered at 0.5)
                vec4 originalColor = texture2D(u_skyboxTexture, v_texCoord);
                float debugScale = 50.0;  // Scale offsets for visibility
                if (true)
                {
                    float g = length(uvOffset) * debugScale;
                    gl_FragColor = gl_FragColor * 0.001 + vec4(g, g, g, 1.0);
                }
                else
                {
                    gl_FragColor = vec4(
                        originalColor.r,
                        0.5 + uvOffset.x * debugScale,
                        0.5 + uvOffset.y * debugScale,
                        1.0
                    );
                }
            }
        }
    }
);

//======================================================================================================================
void Shader::Init(const char* vertexShaderStr, const char* fragmentShaderStr)
{
    mVertexShaderStr = vertexShaderStr;
    mFragmentShaderStr = fragmentShaderStr;
    mShaderProgram = esLoadProgram(vertexShaderStr, fragmentShaderStr);
}

//======================================================================================================================
void Shader::Terminate()
{
    if (mShaderProgram > 0)
        glDeleteProgram(mShaderProgram);
    mShaderProgram = 0;
}

//======================================================================================================================
void Shader::Use() const
{
    IwAssert(ROWLHOUSE, mShaderProgram);
    glUseProgram(mShaderProgram);
}

//======================================================================================================================
static int getUniformLocation(int shaderProgram, const char* str)
{
    int loc = glGetUniformLocation(shaderProgram, str);
    IwAssert(ROWLHOUSE, loc >= 0);
    return loc;
}

//======================================================================================================================
static int getAttribLocation(int shaderProgram, const char* str)
{
    int loc = glGetAttribLocation(shaderProgram, str);
    IwAssert(ROWLHOUSE, loc >= 0);
    return loc;
}

//======================================================================================================================
void SimpleShader::Init()
{
    Shader::Init(simpleVertexShaderStr, simpleFragmentShaderStr);
    u_mvpMatrix = getUniformLocation(mShaderProgram, "u_mvpMatrix");
    a_position  = getAttribLocation(mShaderProgram, "a_position");
    a_colour    = getAttribLocation(mShaderProgram, "a_colour");
}

//======================================================================================================================
void ControllerShader::Init()
{
    Shader::Init(controllerVertexShaderStr, controllerFragmentShaderStr);
    u_mvpMatrix = getUniformLocation(mShaderProgram, "u_mvpMatrix");
    a_position  = getAttribLocation(mShaderProgram, "a_position");
    u_colour  = getUniformLocation(mShaderProgram, "u_colour");
}

//======================================================================================================================
void SkyboxShader::Init()
{
    Shader::Init(skyboxVertexShaderStr, skyboxFragmentShaderStr);
    u_mvpMatrix  = getUniformLocation(mShaderProgram, "u_mvpMatrix");
    a_position   = getAttribLocation(mShaderProgram, "a_position");
    a_texCoord   = getAttribLocation(mShaderProgram, "a_texCoord");
    u_texture    = getUniformLocation(mShaderProgram, "u_texture");
}

//======================================================================================================================
void OverlayShader::Init()
{
    Shader::Init(overlayVertexShaderStr, overlayFragmentShaderStr);
    u_mvpMatrix = getUniformLocation(mShaderProgram, "u_mvpMatrix");
    a_position  = getAttribLocation(mShaderProgram, "a_position");
    a_texCoord  = getAttribLocation(mShaderProgram, "a_texCoord");
    u_texture   = getUniformLocation(mShaderProgram, "u_texture");
    u_colour    = getUniformLocation(mShaderProgram, "u_colour");
}

//======================================================================================================================
void ModelShader::Init()
{
    Shader::Init(modelVertexShaderStr, modelFragmentShaderStr);
    SetupVars();
}

//======================================================================================================================
void ModelShader::SetupVars()
{
    u_mvpMatrix           = getUniformLocation(mShaderProgram, "u_mvpMatrix");
    u_normalMatrix        = getUniformLocation(mShaderProgram, "u_normalMatrix");
    lightShaderInfo[0].u_lightDir         = getUniformLocation(mShaderProgram, "u_lightDir[0]");
    lightShaderInfo[1].u_lightDir         = getUniformLocation(mShaderProgram, "u_lightDir[1]");
    lightShaderInfo[2].u_lightDir         = getUniformLocation(mShaderProgram, "u_lightDir[2]");
    lightShaderInfo[3].u_lightDir         = getUniformLocation(mShaderProgram, "u_lightDir[3]");
    lightShaderInfo[4].u_lightDir         = getUniformLocation(mShaderProgram, "u_lightDir[4]");
    lightShaderInfo[0].u_lightAmbientColour  = getUniformLocation(mShaderProgram, "u_lightAmbientColour[0]");
    lightShaderInfo[1].u_lightAmbientColour  = getUniformLocation(mShaderProgram, "u_lightAmbientColour[1]");
    lightShaderInfo[2].u_lightAmbientColour  = getUniformLocation(mShaderProgram, "u_lightAmbientColour[2]");
    lightShaderInfo[3].u_lightAmbientColour  = getUniformLocation(mShaderProgram, "u_lightAmbientColour[3]");
    lightShaderInfo[4].u_lightAmbientColour  = getUniformLocation(mShaderProgram, "u_lightAmbientColour[4]");
    lightShaderInfo[0].u_lightDiffuseColour  = getUniformLocation(mShaderProgram, "u_lightDiffuseColour[0]");
    lightShaderInfo[1].u_lightDiffuseColour  = getUniformLocation(mShaderProgram, "u_lightDiffuseColour[1]");
    lightShaderInfo[2].u_lightDiffuseColour  = getUniformLocation(mShaderProgram, "u_lightDiffuseColour[2]");
    lightShaderInfo[3].u_lightDiffuseColour  = getUniformLocation(mShaderProgram, "u_lightDiffuseColour[3]");
    lightShaderInfo[4].u_lightDiffuseColour  = getUniformLocation(mShaderProgram, "u_lightDiffuseColour[4]");
    lightShaderInfo[0].u_lightSpecularColour = getUniformLocation(mShaderProgram, "u_lightSpecularColour[0]");
    lightShaderInfo[1].u_lightSpecularColour = getUniformLocation(mShaderProgram, "u_lightSpecularColour[1]");
    lightShaderInfo[2].u_lightSpecularColour = getUniformLocation(mShaderProgram, "u_lightSpecularColour[2]");
    lightShaderInfo[3].u_lightSpecularColour = getUniformLocation(mShaderProgram, "u_lightSpecularColour[3]");
    lightShaderInfo[4].u_lightSpecularColour = getUniformLocation(mShaderProgram, "u_lightSpecularColour[4]");
    u_specularAmount      = getUniformLocation(mShaderProgram, "u_specularAmount");
    u_specularExponent    = getUniformLocation(mShaderProgram, "u_specularExponent");
    a_position            = getAttribLocation(mShaderProgram, "a_position");
    a_normal              = getAttribLocation(mShaderProgram, "a_normal");
    a_colour              = getAttribLocation(mShaderProgram, "a_colour");
}


//======================================================================================================================
void TexturedModelShader::Init()
{
    Shader::Init(texturedModelVertexShaderStr, texturedModelFragmentShaderStr);
    ModelShader::SetupVars();
    a_texCoord            = getAttribLocation(mShaderProgram, "a_texCoord");
    u_texture             = getUniformLocation(mShaderProgram, "u_texture");
    u_texBias             = getUniformLocation(mShaderProgram, "u_texBias");
}

//======================================================================================================================
void TexturedModelSeparateSpecularShader::Init()
{
    Shader::Init(texturedModelVertexShaderStr, texturedModelSeparateSpecularFragmentShaderStr);
    ModelShader::SetupVars();
    a_texCoord            = getAttribLocation(mShaderProgram, "a_texCoord");
    u_texture             = getUniformLocation(mShaderProgram, "u_texture");
    u_texBias             = getUniformLocation(mShaderProgram, "u_texBias");
}

//======================================================================================================================
void PlainShader::Init()
{
    Shader::Init(plainVertexShaderStr, plainFragmentShaderStr);
    u_mvpMatrix     = getUniformLocation(mShaderProgram, "u_mvpMatrix");
    u_textureMatrix = getUniformLocation(mShaderProgram, "u_textureMatrix");
    a_position      = getAttribLocation(mShaderProgram, "a_position");
    a_colour        = getAttribLocation(mShaderProgram, "a_colour");
    u_texture       = getUniformLocation(mShaderProgram, "u_texture");
}

//======================================================================================================================
void TerrainShader::Init()
{
    Shader::Init(terrainVertexShaderStr, terrainFragmentShaderStr);
    u_mvpMatrix      = getUniformLocation(mShaderProgram, "u_mvpMatrix");
    u_textureMatrix0 = getUniformLocation(mShaderProgram, "u_textureMatrix0");
    u_textureMatrix1 = getUniformLocation(mShaderProgram, "u_textureMatrix1");
    a_position       = getAttribLocation(mShaderProgram, "a_position");
    u_texture0       = getUniformLocation(mShaderProgram, "u_texture0");
    u_texture1       = getUniformLocation(mShaderProgram, "u_texture1");
}

//======================================================================================================================
void TerrainPanoramaShader::Init()
{
    Shader::Init(terrainPanoramaVertexShaderStr, terrainPanoramaFragmentShaderStr);
    u_mvpMatrix      = getUniformLocation(mShaderProgram, "u_mvpMatrix");
    a_position       = getAttribLocation(mShaderProgram, "a_position");
}

//======================================================================================================================
void ShadowShader::Init()
{
    Shader::Init(shadowVertexShaderStr, shadowFragmentShaderStr);
    u_mvpMatrix      = getUniformLocation(mShaderProgram, "u_mvpMatrix");
    u_textureMatrix  = getUniformLocation(mShaderProgram, "u_textureMatrix");
    a_position       = getAttribLocation(mShaderProgram, "a_position");
    a_texCoord       = getAttribLocation(mShaderProgram, "a_texCoord");
    u_texture        = getUniformLocation(mShaderProgram, "u_texture");
    u_colour         = getUniformLocation(mShaderProgram, "u_colour");
}

//======================================================================================================================
void ShadowBlurShader::Init()
{
    Shader::Init(shadowVertexShaderStr, shadowBlurFragmentShaderStr);
    u_mvpMatrix      = getUniformLocation(mShaderProgram, "u_mvpMatrix");
    u_textureMatrix  = getUniformLocation(mShaderProgram, "u_textureMatrix");
    a_position       = getAttribLocation(mShaderProgram, "a_position");
    a_texCoord       = getAttribLocation(mShaderProgram, "a_texCoord");
    u_texture        = getUniformLocation(mShaderProgram, "u_texture");
    u_colour         = getUniformLocation(mShaderProgram, "u_colour");
    u_blurAmount     = getUniformLocation(mShaderProgram, "u_blurAmount");
    u_texelSize      = getUniformLocation(mShaderProgram, "u_texelSize");
}

//======================================================================================================================
void SmokeShader::Init()
{
    Shader::Init(smokeVertexShaderStr, smokeFragmentShaderStr);
    u_mvpMatrix      = getUniformLocation(mShaderProgram, "u_mvpMatrix");
    a_position       = getAttribLocation(mShaderProgram, "a_position");
    a_texCoord       = getAttribLocation(mShaderProgram, "a_texCoord");
    u_colour         = getUniformLocation(mShaderProgram, "u_colour");
    u_texture        = getUniformLocation(mShaderProgram, "u_texture");
}

//======================================================================================================================
void SkyboxVRParallaxShader::Init()
{
    Shader::Init(skyboxVRParallaxVertexShaderStr, skyboxVRParallaxFragmentShaderStr);
    u_mvpMatrix      = getUniformLocation(mShaderProgram, "u_mvpMatrix");
    a_position       = getAttribLocation(mShaderProgram, "a_position");
    a_texCoord       = getAttribLocation(mShaderProgram, "a_texCoord");
    u_skyboxTexture  = getUniformLocation(mShaderProgram, "u_skyboxTexture");
    u_depthTexture   = getUniformLocation(mShaderProgram, "u_depthTexture");
    u_eyeOffset      = getUniformLocation(mShaderProgram, "u_eyeOffset");
    u_ipd            = getUniformLocation(mShaderProgram, "u_ipd");
    u_nearPlane      = getUniformLocation(mShaderProgram, "u_nearPlane");
    u_farPlane       = getUniformLocation(mShaderProgram, "u_farPlane");
    u_screenSize     = getUniformLocation(mShaderProgram, "u_screenSize");
    u_eyeRightLocal  = getUniformLocation(mShaderProgram, "u_eyeRightLocal");
    u_tileScale      = getUniformLocation(mShaderProgram, "u_tileScale");
    u_tileOffset     = getUniformLocation(mShaderProgram, "u_tileOffset");
    u_tanFovMin      = getUniformLocation(mShaderProgram, "u_tanFovMin");
    u_tanFovMax      = getUniformLocation(mShaderProgram, "u_tanFovMax");
}


