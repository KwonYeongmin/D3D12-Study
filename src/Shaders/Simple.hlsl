struct VS_OUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

VS_OUT vs_main(float4 position : POSITION, float4 color : COLOR)
{
    VS_OUT output;
    output.position = position;
    output.color = color;
    return output;
}

float4 ps_main(VS_OUT input) : SV_TARGET
{
    return input.color;
}
