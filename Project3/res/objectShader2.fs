#version 150

uniform vec3 cameraPosition;
uniform float exposure;
uniform float lightDistanceModifier;

uniform sampler2D tex;

uniform struct Light {
   vec3 position;
   vec3 intensities; //a.k.a the color of the light
   float attenuation;
   float ambientCoefficient;
} light;

in vec2 fragTexCoord;
in vec3 fragNormal;
in vec3 fragVert;
in mat4 fragModel;
in float fragMaterialShininess;
in vec3 fragMaterialSpecularColor;

out vec4 finalColor;

void main() {
    vec3 normal = normalize(transpose(inverse(mat3(fragModel))) * fragNormal);
    vec3 surfacePos = vec3(fragModel * vec4(fragVert, 1));
    vec4 surfaceColor = texture(tex, fragTexCoord.xy);
    vec3 surfaceToLight = normalize(light.position - surfacePos);
    vec3 surfaceToCamera = normalize(cameraPosition - surfacePos);

    //ambient
    vec3 ambient = surfaceColor.rgb * light.intensities * light.ambientCoefficient;

    //diffuse
    float diffuseCoefficient = max(0.0, dot(normal, surfaceToLight));
    vec3 diffuse = diffuseCoefficient * surfaceColor.rgb * light.intensities;
    
    //specular
    float specularCoefficient = 0.0;
    if(diffuseCoefficient > 0.0)
        specularCoefficient = pow(max(0.0, dot(surfaceToCamera, reflect(-surfaceToLight, normal))), fragMaterialShininess);
    vec3 specular = specularCoefficient * fragMaterialSpecularColor * light.intensities;
    
    //attenuation
    float distanceToLight = length(light.position - surfacePos);
	distanceToLight *= lightDistanceModifier;
    float attenuation = 1.0 / (1.0 + light.attenuation * pow(distanceToLight, 2));

    //linear color (color before gamma correction)
    vec3 linearColor = ambient + attenuation*(diffuse + specular);
    
    //final color (after gamma correction)
    vec3 gamma = vec3(1.0/2.2);

    vec3 mapped = vec3(1.0) - exp(-linearColor * exposure);
    mapped = pow(mapped, vec3(1.0 / gamma));

    finalColor = vec4(mapped.r, mapped.g, mapped.b, surfaceColor.a);
}
