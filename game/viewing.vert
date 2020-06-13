#version 410

in vec4 vPosition;
in vec4 vNormal;
in vec2 vTexcoord ;
out vec4 fNormal;
out vec4 fPosition;
out vec2 fTexcoord ;
out vec4 fColorFront;
out vec4 fColorBack;

in vec4 vColor;
in vec2 aTexCoord;
out vec4 fColor;
out vec2 TexCoord;

uniform mat4 M;
uniform mat4 P;
uniform mat4 V;
uniform int ColorMode;

uniform vec3 Ia = vec3(1.0); ////incident ambient light intensity
uniform vec3 Il = vec3(1.2); //incident light intensity - white light
uniform vec3 Ka ;// ambient reflection coefficient
uniform vec3 Ks ;// specular coefficient
uniform vec3 Kd ;// diffuse coefficient
uniform float c[3] = float[3](0.01, 0.001, 0.0);
uniform float n ;
uniform vec4 LightPos_wc = vec4(5, 5, 3, 1); //light position w.r.t world coordinate system

vec4 shading(vec3 LightPos_ec, vec3 vPosition_ec, vec3 vNormal_ec)
{
    vec3 N = normalize(vNormal_ec);
    vec3 L = LightPos_ec - vPosition_ec;
    float d = length(L); L = L/d;
    vec3 V = normalize(vec3(0.0) - vPosition_ec);
    vec3 R = reflect(-L, N);

    float fatt = min(1.0 / (c[0] + c[1]*d + c[2]*d*d), 1.0);

    float cos_theta = max(dot(N,L),0);
    float cos_alpha = max(dot(V,R),0);

    vec3 I = Ia * Ka + fatt * Il * (Kd * cos_theta + Ks * pow(cos_alpha, n))/**vec3(fColor)*/;

    return vec4(I,1);
}

void phong_shading()
{
    mat4 VMT = V*M;
    mat4 U = transpose(inverse(VMT));
    vec3 vNormal_ec = vec3(normalize(U*vNormal));
    vec3 vPosition_ec = vec3(VMT * vPosition);
    vec3 LightPos_ec = vec3(V * LightPos_wc);

    gl_Position = P * vec4(vPosition_ec, 1);
    fNormal = vec4(vNormal_ec, 1);
    fPosition = vec4(vPosition_ec, 1);
    fTexcoord = vTexcoord ;
}

void gouraud_shading()
{
    
    mat4 VMT = V*M ; // eye coordinate
    mat4 U = transpose(inverse(VMT));
    vec3 vNormal_ec = vec3(normalize(U*vNormal));
    vec3 vPosition_ec = vec3(VMT * vPosition); // vPosition은 model c.s에 있으므로, M을 곱해서 world c.s로 바꾸고, V를 곱해서 eye c.s로 바꿔줌.
    vec3 LightPos_ec = vec3(V * LightPos_wc); // Light position은 이미 world coordinate에 있으므로, V만 곱해서 eye c.s로 바꿔줌.

    gl_Position = P * vec4(vPosition_ec, 1); // clip coordinate
    fColorFront = shading(LightPos_ec, vPosition_ec, vNormal_ec);
    fColorBack = shading(LightPos_ec, vPosition_ec, -vNormal_ec);
}

void main()
{
    if(ColorMode == 0){
        gl_Position = M * vPosition;
        TexCoord = aTexCoord;
    }
    else if(ColorMode == 2){
        phong_shading() ;
    }
    else{
        gouraud_shading() ;
    }
}

