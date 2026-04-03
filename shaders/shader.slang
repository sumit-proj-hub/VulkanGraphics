struct VertexInput {
    float3 pos;
    float3 color;
    float2 texCoord;
}

struct UniformBuffer {
    float4x4 model;
    float4x4 view;
    float4x4 proj;
}
ConstantBuffer<UniformBuffer> ubo;

struct VertexOutput {
    float3 color;
    float4 pos : SV_Position;
    float2 texCoord;
};

[shader("vertex")]
VertexOutput vertMain(VertexInput vin) {
    VertexOutput output;
    output.pos = mul(ubo.proj, mul(ubo.view, mul(ubo.model, float4(vin.pos, 1.0))));
    output.color = vin.color;
    output.texCoord = vin.texCoord;
    return output;
}

Sampler2D texture;

[shader("fragment")]
float4 fragMain(VertexOutput inVert) : SV_Target {
    return texture.Sample(inVert.texCoord);
}
