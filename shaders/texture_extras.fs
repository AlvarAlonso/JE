
varying vec3 v_position;
varying vec3 v_world_position;
varying vec3 v_normal;
varying vec2 v_uv;
varying vec4 v_color;

uniform vec4 u_color;
uniform sampler2D u_texture;
uniform sampler2D u_texture2;

void main()
{
	vec2 uv = v_uv;

	gl_FragColor = u_color * texture2D( u_texture, uv * 10 );

	//if(gl_FragColor.w == 0) discard;
}
