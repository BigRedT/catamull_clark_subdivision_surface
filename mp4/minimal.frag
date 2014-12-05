uniform sampler2D img;
/*varying vec3 vertex_light_position;
varying vec3 vertex_normal;
varying vec3 vertex_eye;

void main() {
	vec4 color = texture2D(img, gl_TexCoord[0].st);
	float diffuse_value = dot(vertex_normal, vertex_eye);
	gl_FragColor = color*diffuse_value;
	
	//gl_FragColor.a = color.a;
}*/
varying vec3 N;
varying vec3 v;

void main(void)
{
   vec3 L = normalize(gl_LightSource[0].position.xyz - v);   
   vec4 Idiff = gl_FrontLightProduct[0].diffuse * max(dot(N,L), 0.0);  
   Idiff = clamp(Idiff, 0.0, 1.0); 
   vec4 color = texture2D(img, gl_TexCoord[0].st);
   //vec4 color={1,0,0,1};
   gl_FragColor = color*Idiff;
}
