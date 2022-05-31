bool isApproximatelyEqual(float a, float b)
{
    return abs(a - b) <= (abs(a) < abs(b) ? abs(b) : abs(a)) * 0.00001;
}

bool isApproximatelyEqual(float3 val, float b)
{
    float a = val.x + val.y + val.z;
    return abs(a - b) <= (abs(a) < abs(b) ? abs(b) : abs(a)) * 0.00001;
}

bool isApproximatelyEqual(float4 val, float b)
{
    float a = val.x + val.y + val.z + val.w;
    return abs(a - b) <= (abs(a) < abs(b) ? abs(b) : abs(a)) * 0.00001;
}

float max3(float3 v)
{
    return max(max(v.x, v.y), v.z);
}