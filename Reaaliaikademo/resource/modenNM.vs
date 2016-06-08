attribute vec3 tangent;
attribute vec3 bitangent;

varying vec3 N;
varying vec3 V;
varying vec3 E;

varying vec3 T;
varying vec3 B;

void main()
{
    N = normalize(gl_NormalMatrix*gl_Normal);
    V = vec3(gl_ModelViewMatrix*gl_Vertex);
    E = normalize(-V);

    T = normalize(gl_NormalMatrix*tangent);
    B = normalize(gl_NormalMatrix*bitangent);

    gl_TexCoord[0] = gl_MultiTexCoord0;
    gl_Position = gl_ModelViewProjectionMatrix*gl_Vertex;
}