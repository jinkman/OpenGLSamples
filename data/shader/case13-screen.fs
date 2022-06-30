#version 330 core
out vec4 FragColor;
in vec2 TexCoord;

#define texNum 18
uniform sampler2D texSrc;
uniform sampler2D tex[texNum];

uniform float mixFactor;
uniform vec2 resolution;

vec4 filterChar()
{
    vec2 charXYNum = vec2(resolution.x / 6., resolution.y / 6.);
    //charXYNum = vec2(resolution.x / 20., resolution.y / 20.);
    // ����uv�������м�
    vec2 uv = (vec2(0.5) + floor(TexCoord * charXYNum)) / charXYNum;

    // ���ػҶ�ֵ
    vec4 color = texture(texSrc, uv);
    float grey = (color.r + color.g + color.b) / 3.0;

    // ���ݻҶ�ֵѡ������
    int index = int(floor(grey * float(texNum - 1) + 0.5)); // ��������

    float charCol = texture(tex[index], TexCoord * charXYNum).r;

    // ��ɫ
    charCol = 1.0 - charCol;
    
    return vec4(vec3(charCol), color.a);
}

void main() 
{
    if(TexCoord.x>mixFactor)
    {
        FragColor = filterChar();
    }
    else
    {
        FragColor = texture(texSrc, TexCoord);
    }
}