#version 330 core


// FONTE: https://github.com/yiwenl/glsl-fbm.git

#define NUM_OCTAVES 3

float mod289(float x){return x - floor(x * (1.0 / 289.0)) * 289.0;}
vec4 mod289(vec4 x){return x - floor(x * (1.0 / 289.0)) * 289.0;}
vec4 perm(vec4 x){return mod289(((x * 34.0) + 1.0) * x);}

float noise(vec3 p){
    vec3 a = floor(p);
    vec3 d = p - a;
    d = d * d * (3.0 - 2.0 * d);

    vec4 b = a.xxyy + vec4(0.0, 1.0, 0.0, 1.0);
    vec4 k1 = perm(b.xyxy);
    vec4 k2 = perm(k1.xyxy + b.zzww);

    vec4 c = k2 + a.zzzz;
    vec4 k3 = perm(c);
    vec4 k4 = perm(c + 1.0);

    vec4 o1 = fract(k3 * (1.0 / 41.0));
    vec4 o2 = fract(k4 * (1.0 / 41.0));

    vec4 o3 = o2 * d.z + o1 * (1.0 - d.z);
    vec2 o4 = o3.yw * d.x + o3.xz * (1.0 - d.x);

    return o4.y * d.y + o4.x * (1.0 - d.y);
}


float fbm(vec3 x) {
	float v = 0.0;
	float a = 0.5;
	vec3 shift = vec3(100);
	for (int i = 0; i < NUM_OCTAVES; ++i) {
		v += a * noise(x);
		x = x * 2.0 + shift;
		a *= 0.5;
	}
	return v;
}

float rand(float n){
    return fract(sin(n) * 43758.5453123);
}

float noise(float p){
	float fl = floor(p);
    float fc = fract(p);
	return mix(rand(fl), rand(fl + 1.0), fc);
}

float fbm(float x) {
	float v = 0.0;
	float a = 0.5;
	float shift = float(100);
	for (int i = 0; i < NUM_OCTAVES; ++i) {
		v += a * noise(x);
		x = x * 2.0 + shift;
		a *= 0.5;
	}
	return v;
}


//FONTE: https://gist.github.com/983/e170a24ae8eba2cd174f
vec3 hsv2rgb(vec3 c){
    vec4 K = vec4(1.0, 2.0/3.0, 1.0/3.0, 3.0);
    vec3 p = abs(fract(c.x + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

in vec4 interpolated_color;
uniform sampler2D TextureImage;
in vec2 texcoords;
in vec3 lambert_diffuse_term;
in vec3 Ia;
in vec3 position_model;
in float random_seed_out;

out vec4 color;

void main()
{
    // Base colors: light, mid, and dark blue
    float seed = random_seed_out;
    float baseHue = rand(seed);

    // Low, realistic saturations
    float s1 = 0.05 + 0.30 * rand(seed+1.0);
    float s2 = 0.10 + 0.20 * rand(seed+2.0);
    float s3 = 0.02 + 0.10 * rand(seed+3.0);

    // Moderate, not bright values
    float v1 = 0.35 + 0.35 * rand(seed+4.0);
    float v2 = 0.60 + 0.40 * rand(seed+5.0);
    float v3 = 0.75 + 0.40 * rand(seed+6.0);

    // Slight hue offsets (kept small to avoid rainbow colors)
    vec3 color1 = hsv2rgb(vec3(baseHue + 0.02 * rand(seed+7.0), s1, v1));
    vec3 color2 = hsv2rgb(vec3(baseHue + 0.04 * rand(seed+8.0), s2, v2));
    vec3 color3 = hsv2rgb(vec3(baseHue + 0.07 * rand(seed+9.0), s3, v3));

    float max_freq1 = 3.0;
    float max_freq2 = 8.0;

    float freq1 = max_freq1 + max_freq1 * sin(seed);
    float freq2 = max_freq1 + max_freq1 * sin(seed+1);
    float freq3 = max_freq1 + max_freq1 * sin(seed+2);
    float freq4 = max_freq2 + max_freq2 * sin(seed+3);
    float freq5 = max_freq2 + max_freq2 * sin(seed+4);
    float freq6 = max_freq2 + max_freq2 * sin(seed+5);

    // Warp coordinates using FBM
    float warpStrength = 0.8;
    vec3 q = vec3(
        fbm(position_model * freq1),
        fbm(position_model * freq2 + vec3(5.2, 1.3, 2.1)),
        fbm(position_model * freq3 + vec3(1.7, 9.2, 3.1))
    );
    vec3 warpedPos = position_model + warpStrength * q;

    // Generate FBM patterns
    float f1 = fbm(warpedPos * freq4 + vec3(1.7, 9.2, 3.1));
    float f2 = fbm(warpedPos * freq5 + vec3(5.2, 1.3, 2.1));
    float f3 = fbm(warpedPos * freq6 + vec3(8.3, 2.8, 4.5));

    // Blend base colors based on FBM patterns
    vec3 Kd = 0.33*(f1*color1 + f2*color2 + f3*color3);

    Kd = clamp(Kd, 0.0, 1.0);
    Kd = pow(Kd, vec3(1.0/2.2));

    vec3 ambient_term = Ia * Kd;
    color = vec4(Kd * lambert_diffuse_term + ambient_term, 1.0);
}
