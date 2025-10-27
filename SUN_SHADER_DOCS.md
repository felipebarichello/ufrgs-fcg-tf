# Sun Shader Implementation

## Overview
This implementation adds a custom sun shader with glow and Fresnel effects to transform the main planet into a realistic sun.

## Features

### 1. Emissive Lighting
The sun shader uses emissive lighting, meaning it generates its own light rather than reflecting external light sources. This ensures the sun always appears bright regardless of scene lighting.

### 2. Fresnel Effect (Rim Lighting)
The Fresnel effect creates a bright halo around the edges of the sun by making the surface brighter at grazing angles. This is achieved using the formula:
```glsl
fresnel = pow(1.0 - max(0.0, dot(n, v)), fresnel_power)
```
where:
- `n` is the surface normal
- `v` is the view direction (from surface to camera)
- `fresnel_power` controls the width of the bright rim

### 3. Configurable Parameters
The SunDrawable class provides four configurable parameters:
- **sun_color**: Base color of the sun (default: warm yellow-orange `vec3(1.0, 0.9, 0.6)`)
- **base_intensity**: Intensity of the emissive glow (default: `2.0`)
- **fresnel_intensity**: Intensity of the rim glow (default: `3.0`)
- **fresnel_power**: Sharpness of the rim effect (default: `3.0`)

## Files Added/Modified

### New Files
1. `src/engine/shaders/SunVertexShader.glsl` - Vertex shader for the sun
2. `src/engine/shaders/SunFragmentShader.glsl` - Fragment shader with glow and Fresnel
3. `src/engine/vobject/components/SunDrawable.hpp` - Sun drawable header
4. `src/engine/vobject/components/SunDrawable.cpp` - Sun drawable implementation

### Modified Files
1. `src/engine/EngineController.hpp` - Added sun_program_id
2. `src/engine/EngineController.cpp` - Load sun shaders
3. `src/game/scenes/MainScene.cpp` - Use Sun() instead of Planet() for central star

## Usage

To create a sun in the scene:
```cpp
// Basic usage with default parameters
VObjectConfig sun = Sun(planet_info);

// Custom sun with specific color and intensity
auto sun_drawable = new SunDrawable(
    "sphere.obj",
    glm::vec3(1.0f, 0.5f, 0.1f),  // Orange color
    2.5f,                           // Base intensity
    4.0f,                           // Fresnel intensity
    2.5f                            // Fresnel power
);
```

## Technical Details

### Shader Pipeline
1. **Vertex Shader**: Transforms vertices to clip space and passes world position and normals to fragment shader
2. **Fragment Shader**: 
   - Calculates view direction from camera to surface point
   - Computes Fresnel term based on angle between normal and view direction
   - Combines emissive and Fresnel components
   - Applies gamma correction for sRGB monitors

### Performance Considerations
The shader is lightweight and performs minimal calculations per fragment:
- 1 inverse matrix operation (view matrix)
- 2 normalize operations
- 1 dot product
- 1 pow operation
- Basic arithmetic operations

### Visual Effect
The resulting sun has:
- A bright, warm core (emissive component)
- A glowing rim that intensifies at the edges (Fresnel component)
- Smooth gradients that create a realistic stellar appearance
- Proper gamma correction for accurate color reproduction
