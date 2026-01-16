<p align="right">
  <a href="README.md"><b>English</b></a> | <a href="README.ko.md">한국어</a>
</p>


# 📌 Project Overview

This project is a **GL-like 3D software renderer** designed based on the OpenGL 3.3 Core Specification, without relying on any external math libraries or graphics APIs.

Centered around a `VirtualGPU` layer that emulates real GPU behavior, the renderer defines and implements VRAM-backed resource management, pipeline state handling, and shader execution flow entirely in software.

In addition, the project directly implements an OpenGL 3.3–style API and driver layer based on the official OpenGL Registry headers, enabling developers to write renderers and rendering code in a manner similar to OpenGL.

The core modules (`core`, `resource`, `virtual_gpu`) are validated through unit tests.  
However, the GL-like API layer has not yet been fully covered by tests, and some behaviors may differ from the specification in certain edge cases.

# 🪩 Rendering Samples
### Blinn–Phong Shading
![blinn_phong.gif](https://github.com/Hobanghann/HORenderer3/blob/develop/custom_renderers/samples/screenshots/blinn_phong.gif)

<details>
<summary><b>Vertex Shader Code</b></summary>

```cpp
void BLINN_PHONG_VS(size_t vertex_index, VirtualGPU::Varying& out) {
        // fetch attributes
        Vector3 a_position = FetchAttribute<Vector3>(0, vertex_index);
        Vector3 a_normal = FetchAttribute<Vector3>(1, vertex_index);
        Vector4 a_tangent = FetchAttribute<Vector4>(2, vertex_index);
        Vector2 a_texcoord = FetchAttribute<Vector2>(3, vertex_index);

        // fetch uniforms
        Matrix4x4 u_model = FetchUniform<Matrix4x4>("u_model"_vg);
        Matrix4x4 u_view_projection = FetchUniform<Matrix4x4>("u_view_projection"_vg);

        // pass data to fragment shader
        out.vg_Position = u_view_projection * u_model * a_position.ToHomogeneous();

        Vector3 world_pos = (u_model * a_position.ToHomogeneous()).ToCartesian();
        out.Out("world_pos"_vg, world_pos);

        float handedness = a_tangent.w;
        out.OutFlat("handedness"_vg, handedness);

        Vector3 tangent = u_model.ToMatrix3x3() * Vector3(a_tangent);
        out.Out("tangent"_vg, tangent);

        Vector3 normal = u_model.ToMatrix3x3() * a_normal;
        out.Out("normal"_vg, normal);

        Vector2 uv = a_texcoord;
        out.Out("uv"_vg, uv);
    }
```
</details>

<details>
<summary><b>Fragment Shader Code</b></summary>

```cpp
void BLINN_PHONG_FS(const VirtualGPU::Fragment& in, VirtualGPU::FSOutputs& out) {
        // read vertex shader outputs
        Vector3 v_world_pos = in.In<Vector3>("world_pos"_vg);
        Vector3 v_tangent = in.In<Vector3>("tangent"_vg).Normalized();
        float v_handedness = in.InFlat<float>("handedness"_vg);
        Vector3 v_normal = in.In<Vector3>("normal"_vg).Normalized();
        Vector2 v_uv = in.In<Vector2>("uv"_vg);

        // fetch uniforms
        int u_diffuse_sampler = FetchUniform<int>("u_diffuse_sampler"_vg);
        int u_specular_sampler = FetchUniform<int>("u_specular_sampler"_vg);
        int u_normal_sampler = FetchUniform<int>("u_normal_sampler"_vg);

        Vector3 bitangent = v_handedness * v_normal.Cross(Vector3(v_tangent));
        Matrix3x3 tbn = Matrix3x3(Vector3(v_tangent), bitangent, v_normal).Transpose();
        // normal map sampling
        Vector3 normal = Texture2D<Color128>(u_normal_sampler, v_uv).ToVector3();
        normal = (tbn * normal).Normalized();

        // diffuse map sampling
        Color128 diffuse = Texture2D<Color128>(u_diffuse_sampler, v_uv);

        // specular map sampling
        Color128 specular = Texture2D<Color128>(u_specular_sampler, v_uv);

        Vector3 light_dir_raw = FetchUniform<Vector3>("u_light_directions"_vg, 0);
        Vector3 L = -light_dir_raw.Normalized();
        Vector3 N = normal.Normalized();
        Vector3 V = (FetchUniform<Vector3>("u_eyePosition"_vg) - v_world_pos).Normalized();
        Vector3 H = (V + L).Normalized();

        Color128 ambient_term = Color128(0.03f, 0.03f, 0.03f);
        Color128 diffuse_term = math::Max(N.Dot(L), 0.f) * diffuse;

        float shininess = 32.0f;
        float specular_factor = math::Pow(math::Max(N.Dot(H), 0.f), shininess);
        Color128 specular_term =
            specular_factor * Color128(specular.r, specular.r, specular.r);  // load metallic as specular

        Vector3 light_color_raw = FetchUniform<Vector3>("u_light_colors"_vg, 0);
        Color128 light_color = Color128(light_color_raw.x, light_color_raw.y, light_color_raw.z);

        Color128 color = (ambient_term + diffuse_term + specular_term) * light_color;

        Color128 final_color;
        final_color.r = color.r;
        final_color.g = color.g;
        final_color.b = color.b;
        final_color.a = 1.f;

        // output final color to draw buffer 0
        out.Out(0, final_color);
    }
```
</details>

---

### PBR Shading + Shadow Mapping
![pbr.gif](https://github.com/Hobanghann/HORenderer3/blob/develop/custom_renderers/samples/screenshots/pbr.gif)

<details>
<summary><b>Vertex Shader Code</b></summary>

```cpp
void DEPTHMAP_VS(size_t vertex_index, VirtualGPU::Varying& out) {
        // fetch attributes
        Vector3 a_position = FetchAttribute<Vector3>(0, vertex_index);

        // fetch uniforms
        Matrix4x4 u_model = FetchUniform<Matrix4x4>("u_model"_vg);
        Matrix4x4 u_view_projection = FetchUniform<Matrix4x4>("u_view_projection"_vg);

        // pass data to fragment shader
        out.vg_Position = u_view_projection * u_model * a_position.ToHomogeneous();
    }

void PBR_VS(size_t vertex_index, VirtualGPU::Varying& out) {
        // fetch attributes
        Vector3 a_position = FetchAttribute<Vector3>(0, vertex_index);
        Vector3 a_normal = FetchAttribute<Vector3>(1, vertex_index);
        Vector4 a_tangent = FetchAttribute<Vector4>(2, vertex_index);
        Vector2 a_texcoord = FetchAttribute<Vector2>(3, vertex_index);

        // fetch uniforms
        Matrix4x4 u_model = FetchUniform<Matrix4x4>("u_model"_vg);
        Matrix4x4 u_view = FetchUniform<Matrix4x4>("u_view"_vg);
        Matrix4x4 u_projection = FetchUniform<Matrix4x4>("u_projection"_vg);
        Matrix4x4 u_light_view_projection = FetchUniform<Matrix4x4>("u_light_view_projection"_vg);

        // pass data to fragment shader
        out.vg_Position = u_projection * u_view * u_model * a_position.ToHomogeneous();

        Vector3 world_pos = (u_model * a_position.ToHomogeneous()).ToCartesian();
        out.Out("world_pos"_vg, world_pos);

        float handedness = a_tangent.w;
        out.OutFlat("handedness"_vg, handedness);

        Vector3 tangent = u_model.ToMatrix3x3() * Vector3(a_tangent);
        out.Out("tangent"_vg, tangent);

        Vector3 normal = u_model.ToMatrix3x3() * a_normal;
        out.Out("normal"_vg, normal);

        Vector2 uv = a_texcoord;
        out.Out("uv"_vg, uv);

        Vector4 light_space_pos = u_light_view_projection * u_model * a_position.ToHomogeneous();
        out.Out("light_space_pos"_vg, light_space_pos);
    }
```
</details>

<details>
<summary><b>Fragment Shader Code</b></summary>

```cpp
void DEPTHMAP_FS(const VirtualGPU::Fragment& in, VirtualGPU::FSOutputs& out) {
        (void)in;
        (void)out;
    }

void PBR_FS(const VirtualGPU::Fragment& in, VirtualGPU::FSOutputs& out) {
        // read vertex shader outputs
        Vector3 v_world_pos = in.In<Vector3>("world_pos"_vg);
        Vector3 v_tangent = in.In<Vector3>("tangent"_vg).Normalized();
        float v_handedness = in.InFlat<float>("handedness"_vg);
        Vector3 v_normal = in.In<Vector3>("normal"_vg).Normalized();
        Vector2 v_uv = in.In<Vector2>("uv"_vg);
        Vector4 v_light_space_pos = in.In<Vector4>("light_space_pos"_vg);

        // fetch uniforms
        int u_normal_sampler = FetchUniform<int>("u_normal_sampler"_vg);
        int u_albedo_sampler = FetchUniform<int>("u_albedo_sampler"_vg);
        int u_metallic_roughness_sampler = FetchUniform<int>("u_metallic_roughness_sampler"_vg);
        int u_emission_sampler = FetchUniform<int>("u_emission_sampler"_vg);
        int u_ao_sampler = FetchUniform<int>("u_ao_sampler"_vg);
        int u_depthmap_sampler = FetchUniform<int>("u_depthmap_sampler"_vg);

        Vector3 bitangent = v_handedness * v_normal.Cross(Vector3(v_tangent));
        Matrix3x3 tbn = Matrix3x3(Vector3(v_tangent), bitangent, v_normal).Transpose();
        // normal map sampling
        Vector3 normal = Texture2D<Color128>(u_normal_sampler, v_uv).ToVector3();
        normal = (tbn * normal).Normalized();

        // albedo map sampling
         Color128 albedo = Texture2D<Color128>(u_albedo_sampler, v_uv);
        albedo = albedo.sRGBToLinear();

        // metallic roughness map sampling
        Color128 metallic_roughness = Texture2D<Color128>(u_metallic_roughness_sampler, v_uv);

        // emission map sampling
        Color128 emission = Texture2D<Color128>(u_emission_sampler, v_uv);
        emission = emission.sRGBToLinear();

        // ao map sampling
        Color128 ao = Texture2D<Color128>(u_ao_sampler, v_uv);

        // fetch uniforms
        Vector3 u_light_positions[1];
        u_light_positions[0] = FetchUniform<Vector3>("u_light_positions"_vg, 0);

        Vector3 u_light_colors[1];
        u_light_colors[0] = FetchUniform<Vector3>("u_light_colors"_vg, 0);

        Vector3 u_eye_position = FetchUniform<Vector3>("u_eye_position"_vg);

        Vector3 N = normal.Normalized();
        Vector3 V = (u_eye_position - v_world_pos).Normalized();

        Vector3 F0 = Vector3(0.04f, 0.04f, 0.04f);
        F0 = F0 * (1.f - metallic_roughness.b) + Vector3(albedo.r, albedo.g, albedo.b) * metallic_roughness.b;

        Vector3 Lo = Vector3(0.f, 0.f, 0.f);

        for (int i = 0; i < 1; i++) {
            Vector3 L = (u_light_positions[i] - v_world_pos).Normalized();
            Vector3 H = (V + L).Normalized();
            float distance = (u_light_positions[i] - v_world_pos).Magnitude();
            float attenuation = 1.f / (distance * distance);
            Vector3 radiance = u_light_colors[i] * attenuation;

            float NDF = DistributionGGX(N, H, metallic_roughness.g);
            float G = GeometrySmith(N, V, L, metallic_roughness.g);
            Vector3 F = FresnelSchlick(math::Max(H.Dot(V), 0.f), F0);

            Vector3 kS = F;
            Vector3 kD = Vector3(1.f, 1.f, 1.f) - kS;
            kD *= 1.f - metallic_roughness.b;

            Vector3 numerator = NDF * G * F;
            float denominator = 4.f * math::Max(N.Dot(V), 0.f) * math::Max(N.Dot(L), 0.f) + 0.0001f;
            Vector3 specular = numerator / denominator;

            float NdotL = math::Max(N.Dot(L), 0.f);

            float shadow = CalculateShadow(u_depthmap_sampler, v_light_space_pos, N, L);

            Lo +=
                (kD * Vector3(albedo.r, albedo.g, albedo.b) / math::PI + specular) * radiance * NdotL * (1.f - shadow);
        }

        Vector3 up = Vector3(0.0f, 1.0f, 0.0f);

        float sky_weight = math::Max(N.Dot(up) * 0.5f + 0.5f, 0.0f);

        Vector3 ground_color = Vector3(0.2f, 0.12f, 0.08f);
        Vector3 sky_color = Vector3(0.2f, 0.4f, 0.8f);

        Vector3 hemi_ambient = ground_color * (1.f - sky_weight) + sky_color * sky_weight;

        Vector3 ambient = hemi_ambient * Vector3(albedo.r, albedo.g, albedo.b) * ao.r;

        Vector3 color = ambient + Lo + Vector3(emission.r, emission.g, emission.b);

        Vector3 R = (N * 2.0f * math::Max(N.Dot(V), 0.0f) - V).Normalized();
        float reflect_weight = math::Max(R.Dot(up) * 0.5f + 0.5f, 0.0f);
        Vector3 fake_reflection = (ground_color * (1.f - reflect_weight) + sky_color * reflect_weight) * 2.0f;

        color += fake_reflection * F0 * (1.0f - metallic_roughness.g);

        color = color / (color + Vector3(1.f, 1.f, 1.f));

        Color128 final_color;
        final_color.r = color.x;
        final_color.g = color.y;
        final_color.b = color.z;
        final_color.a = 1.f;

        // output final color to draw buffer 0
        out.Out(0, final_color.LinearTosRGB());
    }
```
</details>



# 🧱 Build Environment and Instructions

### 🛠️ Build Environment
- OS: Windows 10 / 11
- Compiler:
  - MSVC (`cl`)
  - Clang (`clang`, `clang++`)
- CMake: 3.20 or later
- C++ Standard: C++17

---

### 📦 Generate Build Files
- MSVC (Visual Studio 2022)
```
cmake -S . -B <build_directory> -G "Visual Studio 17 2022"
```
- Clang + Ninja (Debug)
```
cmake -S . -B <build_directory> -G Ninja -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_BUILD_TYPE=Debug
```
- Clang + Ninja (Release)
```
cmake -S . -B <build_directory> -G Ninja -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_BUILD_TYPE=Release
```

---

### 🔧 Build
- MSVC (Debug)
```
cmake --build <build_directory> --config Debug
```
- MSVC (Release)
```
cmake --build <build_directory> --config Release
```
- Clang + Ninja (Debug/Release)
```
cmake --build <build_directory>
```

# 🗂️ Project Structure
![project_structure.png](https://github.com/Hobanghann/HORenderer3/blob/develop/docs/project_structure.svg)


# 📜 How to Write a Renderer
This project operates by having the platform layer reference a single renderer instance and invoke its public APIs on a per-frame basis.

The renderer follows the frame lifecycle shown below:

```mermaid
graph LR;
    Initialize-->PreUpdate;
    PreUpdate-->Render;
    Render-->PostUpdate;
    PostUpdate-->PreUpdate;
    PostUpdate-->Quit
```


After writing shaders, users define a renderer class that uses those shaders and register it to the renderer adapter to render scenes according to custom logic.

Renderer classes must inherit from the Renderer class in the renderer module.
Each virtual function has the following responsibilities:

---

### Initialize()
> - Initialize the VirtualGPU state.<br><br>
> - Create cameras, lights, and model instances.<br><br>
> - Load models and upload them to the VirtualGPU.<br><br>
> - Models can be loaded via `ResourceLoader::LoadModel()` from the core module.<br><br>
> - Loaded models are registered in the renderer-owned `resource_manager_`.<br><br>
> - Models are referenced via `ResourceID`.<br><br>
> - Currently supported formats: `obj`, `gltf`.<br><br>
> - Model paths must be specified relative to the assets directory.

---

### <b>PreUpdate(float delta_time)</b>
> - Update state based on input and time progression.<br><br>
> - The `Renderer` class provides the following input state members:<br><br>
>   - `input_states_`: keyboard and mouse button states<br><br>
>   - `mouse_x_`, `mouse_y_`: current mouse position (relative to the renderer window)<br><br>
>   - `mouse_delta_x_`, `mouse_delta_y_`: mouse movement since the previous frame<br><br>
>   - `mouse_wheel_delta_`: mouse wheel delta for the current frame

---

### <b>Render()</b>
> - Issue draw calls based on cameras, lights, and model instances.<br><br>
> - All rendering logic is implemented in this function.

---

### <b>PostUpdate(float delta_time)</b>
> - Perform post-render state updates.<br><br>
> - `mouse_wheel_delta_` must be reset here, as it represents an event-based input.<br><br>
> - Failing to reset it may result in continuous wheel input behavior.

---

### <b>Quit()</b>
> - Called once when the renderer shuts down.<br><br>
> - Responsible for resource cleanup and shutdown procedures.

---
