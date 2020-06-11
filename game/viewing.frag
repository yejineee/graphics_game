#version 410

out vec4 FragColor;
in vec4 fPosition;
in vec4 fNormal;
in vec2 fTexcoord ;
in vec4 fColorFront;
in vec4 fColorBack;

uniform int ColorMode;
uniform int ObjectCode ;
uniform sampler2D sampler ;

uniform mat4 M;
uniform mat4 P;
uniform mat4 V;

uniform vec3 Ia = vec3(1.0); ////incident ambient light intensity
uniform vec3 Il = vec3(1.2); //incident light intensity - white light
uniform vec3 Ka ;// ambient reflection coefficient
uniform vec3 Ks ;// specular coefficient
uniform vec3 Kd ;// diffuse coefficient
uniform float c[3] = float[3](0.01, 0.001, 0.0);
uniform float n ;
uniform vec4 LightPos_wc = vec4(5, 5, 3, 1) ;

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
    vec3 LightPos_ec = vec3(V * LightPos_wc);
    if(gl_FrontFacing) FragColor = shading(LightPos_ec, vec3(fPosition), vec3(fNormal));
    else FragColor = shading(LightPos_ec, vec3(fPosition), vec3(-fNormal));
    FragColor *= texture(sampler, fTexcoord) ;
}

void gouraud_shading()
{
    if(gl_FrontFacing){
        FragColor = fColorFront ;
    }
    else {
        FragColor = fColorBack ;
    }
    FragColor *= texture(sampler, fTexcoord) ;
}


void main()
{
    if(ColorMode == 1){
        float nc = ObjectCode / 255.0 ;
        FragColor = vec4(nc, nc, nc, 1) ;
    }
    else if(ColorMode == 2){
        phong_shading() ;
    }
    else if(ColorMode == 3){
        gouraud_shading() ;
    }
}
