struct Vertex
{
    float3 position;
    float3 color;
    float2 uv_Coords;
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

[shader("vertex")]
VertOutput vertMain(uint32_t vertID: SV_VertexID, uint32_t instID: SV_InstanceID)
{
    VertOutput output;

    output.color = ptrs[instID].vertices[vertID].color;
    output.sv_pos = float4(ptrs[instID].vertices[vertID].position, 1);
    output.texID = ptrs[instID].textureID;
    output.uv = ptrs[instID].vertices[vertID].uv_Coords;

    return output;
}

[shader("fragment")]
float4 fragMain(VertOutput input)
    : SV_Target
{
    float4 color = textures.Sample(float3(input.uv, input.texID)) * float4(input.color, 1.0);
    return color;
}

