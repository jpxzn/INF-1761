#version 410

in VS_OUT {
    vec4 color;
    vec4 lightPos;
} fs_in;

out vec4 fcolor;

// Shadow map (sampler2DShadow expects comparison mode enabled)
uniform sampler2DShadow shadowMap;

float sampleShadow(vec4 lightPos) {
    // perspective divide
    vec3 projCoords = lightPos.xyz / lightPos.w;
    // transform to [0,1]
    projCoords = projCoords * 0.5 + 0.5;
    // outside light frustum => not in shadow
    if (projCoords.z > 1.0 || projCoords.x < 0.0 || projCoords.x > 1.0 || projCoords.y < 0.0 || projCoords.y > 1.0)
        return 0.0;
    float bias = 0.002; // constant bias (could be improved with slope)

    // simple 3x3 PCF
    float shadow = 0.0;
    float texelSize = 1.0 / float(textureSize(shadowMap, 0).x);
    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            vec3 offset = vec3(projCoords.xy + vec2(x,y) * texelSize, projCoords.z - bias);
            shadow += texture(shadowMap, offset);
        }
    }
    shadow /= 9.0; // average
    // texture() returns 1.0 when lit (depth >= reference), so convert to shadow factor
    return 1.0 - shadow;
}

void main() {
    float shadowFactor = sampleShadow(fs_in.lightPos);
    // mix lit color with a dimmed version when in shadow
    vec4 lit = fs_in.color;
    vec4 shadowed = fs_in.color * 0.25; // darken in shadow
    fcolor = mix(shadowed, lit, 1.0 - shadowFactor);
}
