#version 330 core
out vec4 FragColor;



in vec2 TexCoords;

uniform sampler2D screenTexture;

const float offset = 1.0 / 300.0;

void main()
{
//     vec3 col = texture(screenTexture, TexCoords).rgb;
//     FragColor = vec4(col, 1.0);

//     Inversion des couleurs
//     FragColor = vec4(vec3(1.0 - texture(screenTexture, TexCoords)), 1.0);


//     Transformations des couleurs en niveaux de gris
//     FragColor = texture(screenTexture, TexCoords);
//     float average = 0.2126 * FragColor.r + 0.7152 * FragColor.g + 0.0722 * FragColor.b;
//     FragColor = vec4(average, average, average, 1.0);

//     Utilisation de kernels
//     vec2 offsets[9] = vec2[](
//         vec2(-offset,  offset), // top-left
//         vec2( 0.0f,    offset), // top-center
//         vec2( offset,  offset), // top-right
//         vec2(-offset,  0.0f),   // center-left
//         vec2( 0.0f,    0.0f),   // center-center
//         vec2( offset,  0.0f),   // center-right
//         vec2(-offset, -offset), // bottom-left
//         vec2( 0.0f,   -offset), // bottom-center
//         vec2( offset, -offset)  // bottom-right
//     );
//
// //     Sharpen kernel
// //     float kernel[9] = float[](
// //         -1, -1, -1,
// //         -1,  9, -1,
// //         -1, -1, -1
// //     );
//
// //     kernel flou
//     float kernel[9] = float[](
//         1.0 / 16, 2.0 / 16, 1.0 / 16,
//         2.0 / 16, 4.0 / 16, 2.0 / 16,
//         1.0 / 16, 2.0 / 16, 1.0 / 16
//     );
//
//     vec3 sampleTex[9];
//     for(int i = 0; i < 9; i++)
//     {
//         sampleTex[i] = vec3(texture(screenTexture, TexCoords.st + offsets[i]));
//     }
//     vec3 col = vec3(0.0);
//     for(int i = 0; i < 9; i++)
//         col += sampleTex[i] * kernel[i];
//
//     FragColor = vec4(col, 1.0);

//     HDR

    const float gamma = 2.2;
    vec3 hdrColor = texture(screenTexture, TexCoords).rgb;

    // exposure tone mapping
    vec3 mapped = vec3(1.0) - exp(-hdrColor * 1.0);
    // gamma correction
    mapped = pow(mapped, vec3(1.0 / gamma));

    FragColor = vec4(mapped, 1.0);

//     float brightness = dot(mapped, vec3(0.2126, 0.7152, 0.0722));
//     if(brightness > 0.80)
//         BrightColor = vec4(mapped, 1.0);
//     else
//         BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
}