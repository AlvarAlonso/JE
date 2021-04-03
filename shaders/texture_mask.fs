
varying vec3 v_position;
varying vec3 v_world_position;
varying vec3 v_normal;
varying vec2 v_uv;
varying vec4 v_color;

uniform vec4 u_color;
uniform sampler2D u_texture;
uniform sampler2D u_rocks;
uniform sampler2D u_grass;
uniform sampler2D u_mask;
uniform float u_time;

void main()
{
	vec2 uv = v_uv;
	vec4 black = vec4(1,1,1,1);
	gl_FragColor = u_color * (texture2D( u_mask, uv) * texture2D( u_grass, uv * 10)) + (texture2D( u_rocks, uv * 10) * (black - texture2D(u_mask, uv)));
	
}
