#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;
in vec2 TextCoords;
in vec3 FragPos;
in vec3 Normal;

uniform vec3 camPos;

uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;
uniform sampler2D aoMap;

const float PI = 3.14159265359;

struct PointLight {
    vec3 position;
    vec3 lightColor;

    float constant;
    float linear;
    float quadratic;
};

#define NR_POINT_LIGHTS 3
uniform PointLight pointLights[NR_POINT_LIGHTS];

vec3 getNormalFromMap()
{
    vec3 tangentNormal = texture(normalMap, TextCoords).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(FragPos);
    vec3 Q2  = dFdy(FragPos);
    vec2 st1 = dFdx(TextCoords);
    vec2 st2 = dFdy(TextCoords);

    vec3 N   = normalize(Normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}


float DistributionGGX(vec3 N, vec3 H, float roughness){
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return a2 / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness){
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;
    float denom = NdotV * (1.0 - k) + k;

    return NdotV / denom;
}
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness){
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0){
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}


void main(){

    vec3 albedo     = pow(texture(albedoMap, TextCoords).rgb, vec3(2.2));
    float metallic  = texture(metallicMap, TextCoords).r;
    float roughness = texture(roughnessMap, TextCoords).r;
    float ao        = texture(aoMap, TextCoords).r;

    vec3 N = getNormalFromMap();
    vec3 V = normalize(camPos - FragPos);
    vec3 Lo = vec3(0.0);
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

    for(int i = 0; i < NR_POINT_LIGHTS; ++i){
        vec3 L = normalize(pointLights[i].position - FragPos);
        vec3 H = normalize(V + L);

        float distance    = length(pointLights[i].position - FragPos);
        float attenuation = 1.0 / (pointLights[i].constant + pointLights[i].linear * distance +
          			     pointLights[i].quadratic * (distance * distance));
        vec3 radiance     = pointLights[i].lightColor * attenuation;

        // cook-torrance brdf
        vec3 F  = fresnelSchlick(max(dot(H, V), 0.0), F0);
        float NDF = DistributionGGX(N, H, roughness);
        float G   = GeometrySmith(N, V, L, roughness);

        vec3 numerator    = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0)  + 0.0001;
        vec3 specular     = numerator / denominator;

        vec3 kD = vec3(1.0) - F;
        kD *= 1.0 - metallic;

        float NdotL = max(dot(N, L), 0.0);
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;
    }
    vec3 ambient = vec3(0.03) * albedo * ao;
    vec3 color = ambient + Lo;

//     color = color / (color + vec3(1.0)); // tone mapping
//     color = pow(color, vec3(1.0/2.2)); // correction gamma (2.2)

    FragColor = vec4(color, 1.0);

    float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));
            if(brightness > 1.0)
                BrightColor = vec4(color, 1.0);
            else
                BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
}