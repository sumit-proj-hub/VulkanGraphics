struct VertexInput {
    float3 pos;
    float3 color;
    float2 texCoord;
};

struct Camera {
    float4x4 view;
    float4x4 proj;
};
[[vk::binding(0, 0)]] ConstantBuffer<Camera> scene;

[[vk::binding(0, 1)]] Sampler2D texture;

struct PushConstants {
    float4x4 model;
};
[[vk::push_constant]] PushConstants push;

struct VertexOutput {
    float3 color;
    float4 pos : SV_Position;
    float2 texCoord;
};

[shader("vertex")]
VertexOutput vertMain(VertexInput vin) {
    VertexOutput output;
    output.pos = mul(scene.proj, mul(scene.view, mul(push.model, float4(vin.pos, 1.0))));
    output.color = vin.color;
    output.texCoord = vin.texCoord;
    return output;
}

[shader("fragment")]
float4 fragMain(VertexOutput inVert) : SV_Target {
    return texture.Sample(inVert.texCoord);
}
