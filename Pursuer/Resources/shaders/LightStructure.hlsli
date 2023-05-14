// Number of parallel light sources
static const int DIRLIGHT_NUM = 3;
struct DirLight
{
    float3 lightV; // Unit vector of direction to the light
    float3 lightColor; // Light color (RGB)
};

// Positional light source
static const int POINTLIGHT_NUM = 9;
struct PointLight
{
    float3 lightpos; // Light coordinate
    float3 lightColor; // Light Color
    float3 lightAtten; // Distance attenuation coefficient of lights
    uint active;
};

// Spotlight
static const int SPOTLIGHT_NUM = 3;
struct SpotLight
{
    float3 lightv; // Unit vector of direction to the light
    float3 lightpos; // Light coordinate
    float3 lightcolor; // Light Color
    float3 lightatten; // Distance attenuation coefficient of lights
    float2 lightfactoranglecos; // Cosine of light attenuation angle
    uint active;
};

// Number of round shadows
static const int CIRCLESHADOW_NUM = 1;
struct CircleShadow
{
    float3 dir; // Inverse vector of projection direction (unit vector)
    float3 casterPos; // Caster coordinates
    float distanceCasterLight; // Distance between casters and lights
    float3 atten; // Distance attenuation coefficient
    float2 factorAngleCos; // Cosine of damping angle
    uint active;
};