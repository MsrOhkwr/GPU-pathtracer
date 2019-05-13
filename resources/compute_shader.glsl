#version 450 core

#define PI (3.14159265359)

#define DEPTH (16)

#define DELTA (0.01)

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
layout(rgba32f, binding = 0) uniform image2D img_input;
layout(rgba32f, binding = 1) uniform image2D img_output;
layout(rgba32ui, binding = 2) uniform uimage2D img_seed;
layout(binding = 3) uniform sampler2D img_back;
layout(std430, binding = 0) buffer accum
{
	uint accumN;
};
layout(std430, binding = 1) buffer theta_id
{
	float theta;
};
layout(std430, binding = 2) buffer phi_id
{
	float phi;
};
layout(std430, binding = 3) buffer move_x_id
{
	float move_x;
};
layout(std430, binding = 4) buffer move_y_id
{
	float move_y;
};
layout(std430, binding = 5) buffer scale_id
{
	float scale;
};

ivec3 groupNum = ivec3(gl_NumWorkGroups) * ivec3(gl_WorkGroupSize);
ivec3 groupIdx = ivec3(gl_GlobalInvocationID);

uvec4 xors;

float rand()
{
	const uint t = (xors[0] ^ (xors[0] << 11));
	xors[0] = xors[1];
	xors[1] = xors[2];
	xors[2] = xors[3];
	xors[3] = (xors[3] ^ (xors[3] >> 19)) ^ (t ^ (t >> 18));
	return xors[3] / 4294967295.0f;
}

float pow2(const in float x)
{
	return x * x;
}

float pow5(const in float x)
{
	return x * x * x * x * x;
}

struct ray 
{
	vec3 origin;
	vec3 direction;
	vec3 emission;
	vec3 scatter;
	uint depth;
};

struct hit
{
	float t;
	vec3 position;
	vec3 normal;
	uint mat;
	vec3 color;
};

struct sphere
{
	vec3 center;
	float radius;
	vec3 color;
};

bool hit_sphere(const in sphere s, const in ray r, inout hit h)
{
	const vec3 oc = r.origin - s.center;
	const float a = dot(r.direction, r.direction);
	const float b = dot(oc, r.direction);
	const float c = dot(oc, oc) - pow2(s.radius);
	const float D = pow2(b) - a * c;
	float t;

	if (D > 0)
	{
		t = (-b - sqrt(D)) / a;
		if (0 < t && t < h.t)
		{
			h.t = t;
			h.position = r.origin + t * r.direction;
			h.normal = normalize(h.position - s.center);
			h.color = s.color;
			return true;
		}
		t = (-b + sqrt(D)) / a;
		if (0 < t && t < h.t)
		{
			h.t = t;
			h.position = r.origin + t * r.direction;
			h.normal = normalize(h.position - s.center);
			h.color = s.color;
			return true;
		}
	}

	return false;
}

void mat_background(inout ray r, const in hit h)
{
	r.depth = DEPTH;
	r.emission = texture(img_back, vec2((PI - atan(-r.direction.x, -r.direction.z)) / (2 * PI), acos(r.direction.y) / PI)).rgb;
}

void mat_light(inout ray r, const in hit h)
{
	r.depth = DEPTH;
	r.emission = vec3(100.0f) * h.color;
}

void mat_diffuse(inout ray r, const in hit h)
{
	if (dot(-r.direction, h.normal) < 0)
	{
		r.depth = DEPTH;
		r.scatter = vec3(0.0f);
		return;
	}
	r.depth++;
	r.direction.y = sqrt(rand());
	const float d = sqrt(1 - pow2(r.direction.y));
	const float v = rand() * 2.0f * PI;
	const vec3 ex = vec3(1.0f, 0.0f, 0.0f);
	const vec3 ey = vec3(0.0f, 1.0f, 0.0f);
	const vec3 ez = vec3(0.0f, 0.0f, 1.0f);
	const float dx = abs(dot(h.normal, ex));
	const float dy = abs(dot(h.normal, ey));
	const float dz = abs(dot(h.normal, ez));
	const vec3 vy = (dy < dx) ? (dz < dy) ? ez : ey : (dz < dx) ? ez : ex;
	const vec3 vx = normalize(cross(vy, h.normal));
	const vec3 vz = normalize(cross(vx, h.normal));

	r.direction = normalize(vx * d * cos(v) + h.normal * r.direction.y + vz * d * sin(v));
	r.origin = h.position + h.normal * DELTA;
	r.scatter *= h.color;
}

void mat_mirror(inout ray r, const in hit h)
{
	if (dot(-r.direction, h.normal) < 0)
	{
		r.depth = DEPTH;
		return;
	}
	r.depth++;
	r.origin = h.position + h.normal * DELTA;
	r.direction = 2 * dot(-r.direction, h.normal) * h.normal + r.direction;
	r.scatter *= h.color;
}

float fresnel(const in float n, const in float u)
{
	const float f0 = pow2((n - 1) / (n + 1));
	return f0 + (1 - f0) * pow5(1 - u);
}

void mat_glass(inout ray r, const in hit h)
{
	r.depth++;
	float n = 1.5;
	vec3 N;
	float t = dot(-r.direction, h.normal);
	if (t > 0.0f)
	{
		n = 1.0f / n;
		N = h.normal;
		t = t;
	}
	else
	{
		n = n / 1.0f;
		N = -h.normal;
		t = -t;
	}
	if (rand() < fresnel(n, t))
	{
		mat_mirror(r, h);
	}
	else
	{
		r.origin = h.position - N * DELTA;
		r.direction = n * r.direction + (n * t - sqrt(1 - pow2(n) * (1 - pow2(t)))) * N;
		r.scatter *= h.color;
	}
}

void mat_stone(inout ray r, const in hit h)
{
	r.depth++;
	float n = 1.5;
	vec3 N;
	float t = dot(-r.direction, h.normal);
	if (t > 0.0f)
	{
		n = 1.0f / n;
		N = h.normal;
		t = t;
	}
	else
	{
		n = n / 1.0f;
		N = -h.normal;
		t = -t;
	}
	if (rand() < fresnel(n, t))
	{
		mat_mirror(r, h);
	}
	else
	{
		mat_diffuse(r, h);
	}
}

vec4 toneMap(const in vec4 color, const in float white)
{
	return clamp(color * (1 + color / white) / (1 + color), 0, 1);
}

vec4 gammaCorrect(const in vec4 color, const in float gamma)
{
	vec4 result;

	const float g = 1.0f / gamma;

	result.r = pow(color.r, g);
	result.g = pow(color.g, g);
	result.b = pow(color.b, g);
	result.a = 1.0f;

	return result;
}

void main()
{
	xors ^= imageLoad(img_seed, groupIdx.xy);

	vec4 color_present = (accumN <= 2) ? vec4(0.0f) : imageLoad(img_input, groupIdx.xy);
	vec4 color_next = vec4(0.0f);

	const mat3 M1 = mat3(
			 cos(theta), 0, sin(theta),
			 0, 1, 0,
			 -sin(theta), 0, cos(theta)
			 );
	const mat3 M2 = mat3(
			 1, 0, 0,
			 0, cos(phi), -sin(phi),
			 0, sin(phi), cos(phi)
			 );

	const vec3 eye = vec3(0.0f, 0.0f, 9.0f);
	const vec3 position_screen = 
	{
		float(groupIdx.x + rand()) / groupNum.x * 16.0f - 8.0f,
		float(groupIdx.y + rand()) / groupNum.y * 9.0f - 4.5f,
		eye.z - 9.0f,
	};

	const sphere s1 = 
	{
		vec3(8.05f, 0.0f, 0.0f),
		4.0f,
		vec3(0.75f),
	};
	const sphere s2 = 
	{
		vec3(0.0f),
		4.0f,
		vec3(0.75f),
	};
	const sphere s3 = 
	{
		vec3(-8.05f, 0.0f, 0.0f),
		4.0f,
		vec3(0.75f),
	};
	const sphere s4 = 
	{
		vec3(0.0f, -10000.05f, 0.0f),
		9996.0f,
		vec3(0.75f),
	};

	ray r = 
	{
		M1 * M2 * (eye + vec3(move_x, move_y, 9.0f * scale)),
		M1 * M2 * normalize(position_screen - eye),
		vec3(0.0f),
		vec3(1.0f),
		0,
	};

	hit h = 
	{
		1000.0f,
		vec3(0.0f),
		vec3(0.0f),
		0,
		vec3(0.0f),
	};

	while (r.depth < DEPTH)
	{
		if (hit_sphere(s1, r, h))
		{
			h.mat = 3;
		}
		if (hit_sphere(s2, r, h))
		{
			h.mat = 4;
		}
		if (hit_sphere(s3, r, h))
		{
			h.mat = 5;
		}
		if (hit_sphere(s4, r, h))
		{
			h.mat = 5;
		}

		switch(h.mat)
		{
			case 0:
			{
				mat_background(r, h);
				break;
			}
			case 1: 
			{
				mat_light(r, h);
				break;
			} 
			case 2:
			{
				mat_diffuse(r, h);
				break;
			}
			case 3:
			{
				mat_mirror(r, h);
				break;
			}
			case 4:
			{
				mat_glass(r, h);
				break;
			}
			case 5:
			{
				mat_stone(r, h);
				break;
			}
		}

		color_next = vec4(r.emission * r.scatter, 1.0f);
		
		h.t = 10000.0f;
		h.mat = 0;
	}

	color_present += (color_next - color_present) / accumN;

	imageStore(img_input, groupIdx.xy, color_present);
	imageStore(img_output, groupIdx.xy, gammaCorrect(toneMap(color_present, 10000.0f), 2.2));
	imageStore(img_seed, groupIdx.xy, xors);
	
	if (groupIdx == ivec3(0)) 
	{
		accumN++;
	}
}
