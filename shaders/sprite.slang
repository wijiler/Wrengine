static const int scaleFactor = 100;

struct Vertex
{
    float3 position;
    float2 scale;
    float rotation;
}

struct Mesh
{
    uint64_t textureID;
    Vertex *vertices;
}

[vk_push_constant]
const Mesh *ptrs;

[vk_binding(0)]
Sampler2DArray textures;

struct VertOutput
{
    float3 color : fragColor;
    float2 uv : vertUVCOORDS;
    uint64_t texID : meshTextureID;
    float4 sv_pos : SV_Position;
}

static const float2 uvs[4] =
    {
        float2(1.0f, 1.0f),
        float2(1.0f, 0.0f),
        float2(0.0f, 1.0f),
        float2(0.0f, 0.0f),
    };
static const float4 vpositions[4] =
    {
        float4(1.0f, -1.0f, 0.0f, 1.0f),
        float4(1.0f, 1.0f, 0.0f, 1.0f),
        float4(-1.0f, -1.0f, 0.0f, 1.0f),
        float4(-1.0f, 1.0f, 0.0f, 1.0f),
    };

float3 rot(float3 p, float3 o, float a)
{
    return float3((p.x - o.x) * cos(a) - (p.y - o.y) * sin(a) + o.x, (p.x - o.x) * sin(a) + (p.y - o.y) * cos(a) + o.y, p.z);
}

float3 scale(float3 p, float3 o, float sf)
{
    return float3(o.x + (p.x / sf), o.y + (p.y / sf), p.z);
}

float3 transform(Vertex vert, uint32_t id)
{
    float3 rPos = float3(vpositions[id].xyz) + vert.position;
    rPos = rot(rPos, vert.position, vert.rotation);
    rPos.x *= vert.scale.x;
    rPos.y *= vert.scale.y;

    rPos = scale(rPos, vert.position, scaleFactor);
    return rPos;
}

[shader("vertex")]
VertOutput vertMain(uint32_t vertID: SV_VertexID, uint32_t instID: SV_InstanceID)
{
    VertOutput output;

    output.sv_pos = float4(transform(ptrs.vertices[instID], vertID), 1.0f);
    output.color = float3(1.0f, 1.0f, 1.0f);
    output.texID = ptrs.textureID;
    output.uv = uvs[vertID];

    return output;
}

[shader("fragment")]
float4 fragMain(VertOutput input)
    : SV_Target
{
    float4 color = textures.Sample(float3(input.uv, input.texID)) * float4(input.color, 1.0);

    return color;
}

