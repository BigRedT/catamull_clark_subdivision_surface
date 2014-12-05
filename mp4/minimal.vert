/*varying vec3 vertex_light_position;
varying vec3 vertex_normal;
varying vec3 vertex_eye;

void main(void) {
	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex;
	gl_TexCoord[0] = gl_MultiTexCoord0;

	vertex_normal = normalize(gl_NormalMatrix * gl_Normal);
	//vertex_light_position = normalize(gl_LightSource[0].position.xyz);
	vec3 vertexCoord = normalize(gl_Position.xyz);
	vertex_eye = normalize(-1* vertexCoord);
}*/

varying vec3 N;
varying vec3 v;

void main(void)
{
   gl_TexCoord[0] = gl_MultiTexCoord0;
   v = vec3(gl_ModelViewMatrix * gl_Vertex);       
   N = normalize(gl_NormalMatrix * gl_Normal);
   gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}