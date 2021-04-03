
varying vec3 v_position;
varying vec3 v_world_position;
varying vec3 v_normal;
varying vec2 v_uv;
varying vec4 v_color;

uniform vec4 u_color;
uniform sampler2D u_texture;
uniform float u_time;
uniform float u_mode;

void main()
{
	vec2 uv = v_uv;
	if(u_mode == 0.0f)
	{
		gl_FragColor = u_color * texture2D( u_texture, uv );
	}
	else
	{
		gl_FragColor = vec4(1, 0.7, 0.7, 1) * texture2D( u_texture, uv );
	}
	 

	if(gl_FragColor.w == 0) discard;
}
