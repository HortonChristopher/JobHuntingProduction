#include"Sprite.hlsli"

Texture2D<float4> tex : register(t0); // Regular color
Texture2D<float> texSSAO : register(t1); // SSAO

// For bilateral upsampling
Texture2D<float4> normTex : register(t2);     // Normal size normal texture
Texture2D<float> depthTex : register(t3);     // Normal size depth texture
Texture2D<float4> halfNormTex : register(t4); // 1/4 size normal texture
Texture2D<float> halfDepthTex : register(t5); // 1/4 size depth texture

SamplerState smp : register(s0);

float Gaussian(float sigma, float x)
{
    return exp(-(x * x) / (2.0 * sigma * sigma));
}

float JoinedBilateralGaussianBlur(float2 uv)
{
    float g_sigmaX = 3;
    float g_sigmaV = 0.03 * -0.3 + 0.001;
    float g_sigmaY = 3;
    float w, h, miplevels;

    texSSAO.GetDimensions(0, w, h, miplevels);

    float2 g_pixelSize = (1.0f / w, 1.0f / h);

    const float c_halfSamplesX = 4.;
    const float c_halfSamplesY = 4.;

    float total = 0.0;
    float ret = 0.0;

    float2 pivot = texSSAO.Sample(smp, uv);

    for (float iy = -c_halfSamplesY; iy <= c_halfSamplesY; iy++)
    {
        float fy = Gaussian(g_sigmaY, iy);
        float offsety = iy * g_pixelSize.y;

        for (float ix = -c_halfSamplesX; ix <= c_halfSamplesX; ix++)
        {
            float fx = Gaussian(g_sigmaX, ix);
            float offsetx = ix * g_pixelSize.x;

            float2 value = texSSAO.Sample(smp, uv + float2(offsetx, offsety));

            float fv = Gaussian(g_sigmaV, abs(value.y - pivot.y));

            total += fx * fy * fv;
            ret += fx * fy * fv * value.r;
        }
    }

    return ret / total;
}

float4 BilateralUpsampling(float2 uv)
{
    // Get full size texture size
    float texW, texH, texMiplevels;

    tex.GetDimensions(0, texW, texH, texMiplevels);

    // Number of current texels
    int nTexcel = (int)(uv.x * texW) % 2 + (((int)(uv.y * texH) % 2) * 2);

    float4 returnColor = { 1, 1, 1, 1 };

    // Get size of reduced texture
    float w, h, miplevels;
    halfNormTex.GetDimensions(0, w, h, miplevels);

    // Calculate texel differences in reduced textures
    float dx = 1.0f / w;
    float dy = 1.0f / h;

    // Set weights equivalent to bilateral filters
    float4 vBilinearWeights[4] =
    {
        float4(9.0f / 16.0f, 3.0f / 16.0f, 3.0f / 16.0f, 1.0f / 16.0f), // 0
        float4(3.0f / 16.0f, 9.0f / 16.0f, 1.0f / 16.0f, 3.0f / 16.0f), // 1
        float4(3.0f / 16.0f, 1.0f / 16.0f, 9.0f / 16.0f, 3.0f / 16.0f), // 2
        float4(1.0f / 16.0f, 3.0f / 16.0f, 3.0f / 16.0f, 9.0f / 16.0f), // 3
    };

    // Set offset by 4 texels in reduced buffer
    float2 uvOffset[4] =
    {
        { dx * -0.5f, dy * -0.5f},
        { dx * 0.5f, dy * -0.5f},
        { dx * -0.5f, dy * 0.5f},
        { dx * 0.5f, dy * 0.5f},
    };

    // Initialize weights for normals
    float vNormalWeights[4] =
    {
       0,0,0,0
    };

    // Sampling values from full size normal texture
    float4 vNormalHiRes = normTex.Sample(smp, uv);

    // For the current UV perimeter of 4 pixels in the normal reduction texture
    for (int i = 0; i < 4; i++)
    {
        // Sampling values from a reduced texture
        float4 vNormalCoarse = halfNormTex.Sample(smp, uv + uvOffset[i]);

        // Calculating Weights
        vNormalWeights[i] = dot(vNormalCoarse, vNormalHiRes);
        vNormalWeights[i] = pow(vNormalWeights[i], 32);
    }

    // Initialize weights for depth values
    float vDepthWeights[4] =
    {
        0,0,0,0
    };

    // Sampling values from full size depth textures
    float fDepthHiRes = depthTex.Sample(smp, uv);

    // For depth-reduced textures with a current UV perimeter of 4 pixels
    for (int i = 0; i < 4; i++)
    {
        // Sampling values from a reduced texture
        float fDepthCoarse = halfDepthTex.Sample(smp, uv + uvOffset[i]);

        // Calculating Weights
        float fDepthDiff = fDepthHiRes - fDepthCoarse;
        vDepthWeights[i] = 1.0f / (2.22045e-016 + abs(fDepthDiff));
    }

    // Initialize final weights
    float fTotalWeight = 0;
    float vUpsampled = 0;

    for (int i = 0; i < 4; i++)
    {
        // Multiply normals, depths, and filter weights for 4 texels
        float fWeight = vNormalWeights[i] * vDepthWeights[i] * vBilinearWeights[nTexcel][i];

        fTotalWeight += fWeight;

        vUpsampled += texSSAO.Sample(smp, uv + uvOffset[i]) * fWeight;
    }

    // Normalize to fit between 0 and 1
    returnColor = vUpsampled /= fTotalWeight;

    return returnColor;
}

float4 PSmain(VSOutput input) : SV_TARGET
{
    // Bilateral upsampling
    float4 col = tex.Sample(smp, input.uv);

    const float4 ssaoColor = BilateralUpsampling(input.uv);

    float4 returnColor = float4(col.rgb * ssaoColor.rgb, col.a);

    return returnColor;
}