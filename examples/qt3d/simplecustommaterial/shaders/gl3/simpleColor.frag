#version 440

out vec4 fragColor;

layout(binding = 7, offset = 0) uniform atomic_uint myatomiccounter;

void main()
{
    uint cval = atomicCounterIncrement(myatomiccounter);
    float val = float(cval);
    fragColor = vec4(val/100000.0, val/1000000.0, val/10000000.0,1.0);
}

