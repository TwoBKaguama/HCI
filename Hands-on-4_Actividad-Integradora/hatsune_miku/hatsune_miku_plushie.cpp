// Lopez Muñoz,Pablo Antonio - 217782231
// Interaccion humano computadora - 2026

// hatsune_miku_plushie.cpp
// hatsune_miku_plushie Matematicas 2D/3D con raylib modelo Hatsune Miku Plushie
// https://sketchfab.com/3d-models/hatsune-miku-plushie-22e25b5fbd444102a175c088e14eb541

// Compilar (Linux/Mac):
//   g++ hatsune_miku_plushie.cpp -o hatsune_miku_plushie -lraylib -lm
// Compilar (Windows):
//   g++ hatsune_miku_plushie.cpp -o hatsune_miku_plushie.exe -lraylib -lgdi32 -lwinmm

#include "raylib.h"
#include "rlgl.h"
#include "raymath.h"
#include <cmath>

// Dibuja flecha 2D
void DrawArrow2D(Vector2 origin, Vector2 end, float size, Color color)
{
    DrawLineEx(origin, end, 2.0f, color);
    float dx = end.x - origin.x, dy = end.y - origin.y;
    float ang = atan2f(dy, dx);
    Vector2 L = { end.x - size * cosf(ang - 0.45f), end.y - size * sinf(ang - 0.45f) };
    Vector2 R = { end.x - size * cosf(ang + 0.45f), end.y - size * sinf(ang + 0.45f) };
    DrawLineV(end, L, color);
    DrawLineV(end, R, color);
}

// pos3D: traslacion, rotAxis: eje de rotacion, rotAngle: grados, scale: escala uniforme
void DrawMiku(Model& model, Vector3 pos3D, Vector3 rotAxis, float rotAngle, float scale)
{
    DrawModelEx(model, pos3D, rotAxis, rotAngle, { scale, scale, scale }, WHITE);
}

//  Escenas
enum Scene { MENU = 0, TRASLACION, ROTACION, REBOTE, SENOIDAL, TRAYECTORIA, ORBITA };

int main()
{
    const int W = 1280, H = 720;
    InitWindow(W, H, "hatsune_miku_plushie Matematicas — Hatsune Miku Plushie");
    SetTargetFPS(60);

    // Camara 3D
    Camera3D cam = { 0 };
    cam.position   = { 0.0f, 3.0f, 8.0f };
    cam.target     = { 0.0f, 1.5f, 0.0f };
    cam.up         = { 0.0f, 1.0f, 0.0f };
    cam.fovy       = 45.0f;
    cam.projection = CAMERA_PERSPECTIVE;

    // Cargar modelo GLB 
    Model miku = LoadModel("hatsune_miku_plushie.glb");

    // Calcular bounding box para escalar automaticamente
    BoundingBox bb = GetModelBoundingBox(miku);
    float modelHeight = bb.max.y - bb.min.y;
    float autoScale   = 2.5f / modelHeight;   // escalar a ~2.5 unidades de alto

    // Offset para que la base del modelo quede en y=0
    float yOffset = -bb.min.y * autoScale;

    Scene scene = MENU;

    // 1. TRASLACION
    float tx = -4.0f;

    // 2. ROTACION
    float rotAngle = 0.0f;

    // 3. REBOTE
    float bx     = -4.0f;
    float bVel   = 3.5f;   // unidades/s

    // 4. SENOIDAL
    float t = 0.0f;
    float A = 1.4f, omega = 2.0f;

    // 5. TRAYECTORIA
    float trayX   = -4.0f;
    float trayVel = 2.5f;
    float trayDir = 1.0f;

    // 6. ORBITA
    float orbitTheta = 0.0f;
    float orbitR     = 3.5f;

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();

        // Navegacion
        if (IsKeyPressed(KEY_ESCAPE)) scene = MENU;
        if (scene == MENU)
        {
            if (IsKeyPressed(KEY_ONE))   scene = TRASLACION;
            if (IsKeyPressed(KEY_TWO))   scene = ROTACION;
            if (IsKeyPressed(KEY_THREE)) scene = REBOTE;
            if (IsKeyPressed(KEY_FOUR))  scene = SENOIDAL;
            if (IsKeyPressed(KEY_FIVE))  scene = TRAYECTORIA;
            if (IsKeyPressed(KEY_SIX))   scene = ORBITA;
        }

        switch (scene)
        {
            case TRASLACION:
                // P' = P + (vx*dt, 0, 0)
                tx += 2.8f * dt;
                if (tx > 4.0f) tx = -4.0f;
                UpdateCamera(&cam, CAMERA_ORBITAL);
                break;

            case ROTACION:
                // theta += omega * dt
                rotAngle += 90.0f * dt;   // 90 grados/s
                UpdateCamera(&cam, CAMERA_ORBITAL);
                break;

            case REBOTE:
                // pos += vel * dt  ;  vel = -vel si pos >= limite
                bx += bVel * dt;
                if (bx >= 4.0f || bx <= -4.0f) bVel = -bVel;
                UpdateCamera(&cam, CAMERA_ORBITAL);
                break;

            case SENOIDAL:
                // y(t) = A * sin(omega * t)
                t += dt;
                UpdateCamera(&cam, CAMERA_ORBITAL);
                break;

            case TRAYECTORIA:
                // x += vx*dt  ;  z = A*sin(k*x)
                trayX += trayVel * trayDir * dt;
                if (trayX >= 4.0f || trayX <= -4.0f) trayDir = -trayDir;
                UpdateCamera(&cam, CAMERA_ORBITAL);
                break;

            case ORBITA:
                // x = R cos(theta) , z = R sin(theta)
                orbitTheta += 1.4f * dt;
                UpdateCamera(&cam, CAMERA_ORBITAL);
                break;

            default: break;
        }

        // DIBUJO
        BeginDrawing();
        ClearBackground({ 12, 12, 22, 255 });

        //  MENU
        if (scene == MENU)
        {
            DrawText("hatsune_miku_plushie MATEMATICAS", 200, 55, 40, WHITE);
            DrawText("Hatsune Miku Plushie — raylib 3D", 330, 110, 24, { 100, 220, 255, 255 });
            DrawText("Selecciona una transformacion:", 330, 160, 24, LIGHTGRAY);

            const char* items[] = {
                "[1]  Traslacion       P' = P + T",
                "[2]  Rotacion         R(theta) * v",
                "[3]  Rebote           v' = -v",
                "[4]  Senoidal         y = A sin(wt)",
                "[5]  Trayectoria      x+=vt, z=A sin(kx)",
                "[6]  Orbita           x=R cos(t), z=R sin(t)",
            };
            Color cols[] = { SKYBLUE, ORANGE, RED, GREEN, GOLD, PURPLE };
            for (int i = 0; i < 6; i++)
                DrawText(items[i], 340, 210 + i * 58, 26, cols[i]);

            DrawText("ESC en cualquier escena = volver al menu", 330, 580, 20, DARKGRAY);
            DrawText("Camara: mouse drag para orbitar | rueda para zoom", 310, 610, 20, DARKGRAY);
        }

        //  ESCENAS 3D
        else
        {
            BeginMode3D(cam);
            DrawGrid(20, 1.0f);

            switch (scene)
            {
                // 1. TRASLACION
                case TRASLACION:
                {
                    // Eje X de referencia
                    DrawLine3D({ -5.0f, 0.05f, 0.0f }, { 5.0f, 0.05f, 0.0f }, RED);

                    // Modelo trasladado
                    DrawMiku(miku,
                        { tx, yOffset, 0.0f },          // posicion = (tx, 0, 0)
                        { 0.0f, 1.0f, 0.0f }, 0.0f,     // sin rotacion adicional
                        autoScale
                    );

                    // Vector desplazamiento
                    DrawLine3D({ -4.0f, 0.1f, 0.0f }, { tx, 0.1f, 0.0f }, { 255, 200, 0, 200 });
                    break;
                }

                // 2. ROTACION
                case ROTACION:
                {
                    DrawMiku(miku,
                        { 0.0f, yOffset, 0.0f },
                        { 0.0f, 1.0f, 0.0f }, rotAngle,  // eje Y, angulo acumulado
                        autoScale
                    );

                    // Indicador de eje Y
                    DrawLine3D({ 0.0f, 0.0f, 0.0f }, { 0.0f, 4.0f, 0.0f }, GREEN);
                    break;
                }

                // 3. REBOTE
                case REBOTE:
                {
                    // Limites visuales
                    DrawLine3D({ -4.0f, 0.0f, 0.0f }, { -4.0f, 3.0f, 0.0f }, { 200, 60, 60, 200 });
                    DrawLine3D({  4.0f, 0.0f, 0.0f }, {  4.0f, 3.0f, 0.0f }, { 200, 60, 60, 200 });

                    DrawMiku(miku,
                        { bx, yOffset, 0.0f },
                        { 0.0f, 1.0f, 0.0f }, 0.0f,
                        autoScale
                    );

                    // Vector velocidad
                    DrawLine3D({ bx, 1.5f, 0.0f }, { bx + bVel * 0.25f, 1.5f, 0.0f }, GOLD);
                    break;
                }

                // 4. SENOIDAL
                case SENOIDAL:
                {
                    float yVal = A * sinf(omega * t);

                    // Curva senoidal de referencia
                    for (int i = 0; i < 100; i++)
                    {
                        float t1 = (float)i       / 100.0f * 8.0f;
                        float t2 = (float)(i + 1) / 100.0f * 8.0f;
                        float y1 = A * sinf(omega * (t1 - t));
                        float y2 = A * sinf(omega * (t2 - t));
                        DrawLine3D({ -4.0f + t1, y1 + 1.5f, 1.0f },
                                   { -4.0f + t2, y2 + 1.5f, 1.0f },
                                   { 50, 150, 255, 120 });
                    }

                    DrawMiku(miku,
                        { 0.0f, yOffset + yVal + 1.5f, 0.0f },
                        { 0.0f, 1.0f, 0.0f }, 0.0f,
                        autoScale
                    );

                    // Linea de equilibrio
                    DrawLine3D({ -4.0f, 1.5f, 0.0f }, { 4.0f, 1.5f, 0.0f }, { 80, 80, 100, 180 });
                    break;
                }

                // 5. TRAYECTORIA
                case TRAYECTORIA:
                {
                    // Curva-guia en el plano XZ
                    for (int i = 0; i < 200; i++)
                    {
                        float x1 = -4.0f + (float)i       / 200.0f * 8.0f;
                        float x2 = -4.0f + (float)(i + 1) / 200.0f * 8.0f;
                        float z1 = A * sinf(x1 * 1.2f);
                        float z2 = A * sinf(x2 * 1.2f);
                        DrawLine3D({ x1, 0.05f, z1 }, { x2, 0.05f, z2 }, { 255, 200, 50, 160 });
                    }

                    float trayZ = A * sinf(trayX * 1.2f);
                    DrawMiku(miku,
                        { trayX, yOffset, trayZ },
                        { 0.0f, 1.0f, 0.0f },
                        -trayDir * 15.0f,               // leve inclinacion segun direccion
                        autoScale
                    );
                    break;
                }

                // 6. ORBITA
                case ORBITA:
                {
                    // Planeta central
                    DrawSphere({ 0.0f, 1.5f, 0.0f }, 0.5f, { 100, 200, 255, 255 });

                    // Circunferencia de orbita
                    for (int i = 0; i < 64; i++)
                    {
                        float a1 = (float)i       / 64.0f * 2.0f * PI;
                        float a2 = (float)(i + 1) / 64.0f * 2.0f * PI;
                        DrawLine3D(
                            { orbitR * cosf(a1), 0.1f, orbitR * sinf(a1) },
                            { orbitR * cosf(a2), 0.1f, orbitR * sinf(a2) },
                            { 120, 120, 150, 160 }
                        );
                    }

                    float sx = orbitR * cosf(orbitTheta);
                    float sz = orbitR * sinf(orbitTheta);

                    // Radio vectorial
                    DrawLine3D({ 0.0f, 1.5f, 0.0f }, { sx, 1.5f, sz }, { 200, 200, 255, 180 });

                    // Miku orbita mirando hacia el centro
                    float lookAngle = RAD2DEG * (atan2f(-sz, -sx));
                    DrawMiku(miku,
                        { sx, yOffset, sz },
                        { 0.0f, 1.0f, 0.0f }, lookAngle,
                        autoScale
                    );
                    break;
                }

                default: break;
            }

            EndMode3D();

            // ── HUD por escena ─────
            switch (scene)
            {
                case TRASLACION:
                    DrawText("TRASLACION 3D", 30, 20, 32, SKYBLUE);
                    DrawText("P' = P + T(tx, 0, 0)", 30, 62, 22, WHITE);
                    DrawText(TextFormat("tx = %.3f", tx), 30, 92, 20, GOLD);
                    DrawText("Codigo:", 30, 132, 18, LIGHTGRAY);
                    DrawText("  tx += 2.8f * dt;", 30, 155, 18, GREEN);
                    DrawText("  DrawModelEx(miku, {tx,y,0}, ...);", 30, 177, 18, GREEN);
                    break;

                case ROTACION:
                    DrawText("ROTACION 3D  (eje Y)", 30, 20, 32, ORANGE);
                    DrawText("Ry(theta) aplicada sobre eje Y", 30, 62, 22, WHITE);
                    DrawText(TextFormat("theta = %.1f grados", fmodf(rotAngle, 360.0f)), 30, 92, 20, GOLD);
                    DrawText("Codigo:", 30, 132, 18, LIGHTGRAY);
                    DrawText("  rotAngle += 90.0f * dt;", 30, 155, 18, GREEN);
                    DrawText("  DrawModelEx(miku,pos,{0,1,0},rotAngle,...);", 30, 177, 18, GREEN);
                    break;

                case REBOTE:
                    DrawText("REBOTE", 30, 20, 32, RED);
                    DrawText("pos += vel * dt  ;  vel = -vel @ limite", 30, 62, 22, WHITE);
                    DrawText(TextFormat("vel = %.2f u/s", bVel), 30, 92, 20, GOLD);
                    DrawText(TextFormat("x   = %.3f", bx), 30, 115, 20, LIGHTGRAY);
                    DrawText("Codigo:", 30, 155, 18, LIGHTGRAY);
                    DrawText("  bx += bVel * dt;", 30, 178, 18, GREEN);
                    DrawText("  if (bx>=4 || bx<=-4) bVel=-bVel;", 30, 200, 18, GREEN);
                    break;

                case SENOIDAL:
                    DrawText("MOVIMIENTO SENOIDAL", 30, 20, 32, GREEN);
                    DrawText("y(t) = A * sin(omega * t)", 30, 62, 22, WHITE);
                    DrawText(TextFormat("A = %.1f   omega = %.2f", A, omega), 30, 92, 20, GOLD);
                    DrawText(TextFormat("y = %.3f", A * sinf(omega * t)), 30, 115, 20, LIGHTGRAY);
                    DrawText("Codigo:", 30, 155, 18, LIGHTGRAY);
                    DrawText("  t += dt;", 30, 178, 18, GREEN);
                    DrawText("  float y = A * sinf(omega * t);", 30, 200, 18, GREEN);
                    break;

                case TRAYECTORIA:
                    DrawText("TRAYECTORIA COMPUESTA", 30, 20, 32, GOLD);
                    DrawText("x+=vt  ;  z = A*sin(k*x)", 30, 62, 22, WHITE);
                    DrawText(TextFormat("x = %.2f   z = %.2f", trayX, A * sinf(trayX * 1.2f)), 30, 92, 20, GOLD);
                    DrawText("Codigo:", 30, 132, 18, LIGHTGRAY);
                    DrawText("  trayX += vel * dir * dt;", 30, 155, 18, GREEN);
                    DrawText("  float trayZ = A * sinf(trayX * 1.2f);", 30, 177, 18, GREEN);
                    break;

                case ORBITA:
                    DrawText("ORBITA 3D", 30, 20, 32, PURPLE);
                    DrawText("x = R cos(wt)   z = R sin(wt)", 30, 62, 22, WHITE);
                    DrawText(TextFormat("R = %.2f   theta = %.3f rad", orbitR, orbitTheta), 30, 92, 20, GOLD);
                    DrawText(TextFormat("x = %.2f   z = %.2f",
                        orbitR * cosf(orbitTheta), orbitR * sinf(orbitTheta)), 30, 115, 20, LIGHTGRAY);
                    DrawText("Codigo:", 30, 155, 18, LIGHTGRAY);
                    DrawText("  orbitTheta += 1.4f * dt;", 30, 178, 18, GREEN);
                    DrawText("  sx = R * cosf(orbitTheta);", 30, 200, 18, GREEN);
                    DrawText("  sz = R * sinf(orbitTheta);", 30, 222, 18, GREEN);
                    break;

                default: break;
            }

            DrawText("ESC = menu", W - 200, H - 40, 18, { 80, 80, 100, 200 });
        }

        EndDrawing();
    }

    UnloadModel(miku);
    CloseWindow();
    return 0;
}
