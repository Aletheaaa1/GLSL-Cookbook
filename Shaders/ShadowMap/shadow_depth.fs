#version 460

float near = 0.01f;
float far = 100.0f;
float nonLinearDepth()
{
    float Zndc = gl_FragCoord.z * 2.0 - 1.0;
    float Zeye = (2.0 * near * far) / (far + near - Zndc * (far - near));
    return (1.0 / near - 1.0 / Zeye) / (1.0 / near - 1.0 / far);
}

void main()
{
//    gl_FragDepth = nonLinearDepth();
}