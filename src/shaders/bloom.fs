#version 330 core
out vec4 FragColor;

in vec2 texCoords;

uniform sampler2D scene;
uniform sampler2D bloomBlur;

void main()
{
//     const float gamma = 2.2;
//     vec3 hdrColor = texture(scene, texCoords).rgb;
//     vec3 bloomColor = texture(bloomBlur, texCoords).rgb;
//     if(bloom)
//         hdrColor += bloomColor; // additive blending
//
// //     hdrColor += bloomColor; // additive blending
// //     tone mapping
//     vec3 result = vec3(1.0) - exp(-hdrColor * exposure);
// //     also gamma correct while we're at it
//     result = pow(result, vec3(1.0 / gamma));
//     FragColor = vec4(result, 1.0);
//     FragColor = vec4(texture(scene, texCoords).rgb, 1.0);

    vec3 fragment = texture(scene, texCoords).rgb;
    vec3 bloom = texture(bloomBlur, texCoords).rgb;

    vec3 color = fragment + bloom;

    float exposure = 0.8f;
    vec3 toneMapped = vec3(1.0f) - exp(-color * exposure);

    FragColor.rgb = pow(toneMapped, vec3(1.0f / 2.2f));
}