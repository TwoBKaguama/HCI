use raylib::prelude::*;

fn main() {
    let (mut rl, thread) = raylib::init()
        .size(1300, 800)
        .title("Practica 5 - Movimientos Basicos en 3D")
        .build();
  
    let mut camera = Camera3D::perspective(
        Vector3::new(0.0, 18.0, 18.0), // Posición
        Vector3::new(0.0, 0.0, 0.0),  // Target
        Vector3::new(0.0, 1.0, 0.0),  // Up
        45.0,                         // FOVY
    );

    rl.set_target_fps(60);

    let mut rebote_y = 1.0f32;
    let mut velocidad_y = 0.08f32;

    while !rl.window_should_close() {

        rl.update_camera(&mut camera, CameraMode::CAMERA_ORBITAL);

        let t = rl.get_time() as f32;

        // Senoidal
        let seno_y = 2.0 + (t * 2.0).sin() * 1.5;

        // Rebote
        rebote_y += velocidad_y;
        if rebote_y > 4.0 || rebote_y < 1.0 {
            velocidad_y *= -1.0;
        }

        // Orbita
        let radio = 2.0f32;
        let orbita_x = t.cos() * radio;
        let orbita_z = t.sin() * radio;

        // Trayectoria
        let trayectoria_x = -2.5 + (t * 1.5 % 5.0);
        let trayectoria_z = (t * 3.0).sin() * 1.2;

        let mut d = rl.begin_drawing(&thread);
        d.clear_background(Color::RAYWHITE);

        {
            let mut d3d = d.begin_mode3D(&camera);

            d3d.draw_grid(30, 1.0);

            // Planos
            d3d.draw_cube(Vector3::new(-9.0, 0.05, 4.0), 5.0, 0.1, 5.0, Color::LIGHTGRAY);
            d3d.draw_cube(Vector3::new(-3.0, 0.05, 4.0), 5.0, 0.1, 5.0, Color::LIGHTGRAY);
            d3d.draw_cube(Vector3::new( 3.0, 0.05, 4.0), 5.0, 0.1, 5.0, Color::LIGHTGRAY);
            d3d.draw_cube(Vector3::new( 9.0, 0.05, 4.0), 5.0, 0.1, 5.0, Color::LIGHTGRAY);

            // Esfera Senoidal
            d3d.draw_sphere(Vector3::new(-9.0, seno_y, 4.0), 0.45, Color::BLUE);
            d3d.draw_line_3D(Vector3::new(-9.0, 0.0, 4.0), Vector3::new(-9.0, 4.0, 4.0), Color::BLUE);
            d3d.draw_sphere(Vector3::new(-9.0, 4.0, 4.0), 0.12, Color::BLUE);
            d3d.draw_sphere(Vector3::new(-9.0, 1.0, 4.0), 0.12, Color::BLUE);

            // Esfera Rebote
            d3d.draw_sphere(Vector3::new(-3.0, rebote_y, 4.0), 0.45, Color::RED);
            d3d.draw_cube(Vector3::new(-3.0, 0.15, 4.0), 2.0, 0.3, 2.0, Color::GRAY);
            d3d.draw_line_3D(Vector3::new(-3.0, 0.3, 4.0), Vector3::new(-3.0, 4.0, 4.0), Color::RED);

            // Esfera Orbita
            d3d.draw_sphere(Vector3::new(3.0 + orbita_x, 1.0, 4.0 + orbita_z), 0.45, Color::GREEN);
            d3d.draw_sphere(Vector3::new(3.0, 1.0, 4.0), 0.2, Color::DARKGREEN);
            d3d.draw_sphere_wires(Vector3::new(3.0, 1.0, 4.0), radio, 24, 24, Color::DARKGREEN);

            // Trayectoria Ondulada
            for i in 0..80 {
                let x1 = -2.5 + i as f32 * (5.0 / 80.0);
                let x2 = -2.5 + (i + 1) as f32 * (5.0 / 80.0);
                let z1 = (x1 * 3.0).sin() * 1.2;
                let z2 = (x2 * 3.0).sin() * 1.2;

                d3d.draw_line_3D(
                    Vector3::new(9.0 + x1, 1.0, 4.0 + z1),
                    Vector3::new(9.0 + x2, 1.0, 4.0 + z2),
                    Color::ORANGE
                );
            }
            d3d.draw_sphere(Vector3::new(9.0 + trayectoria_x, 1.0, 4.0 + trayectoria_z), 0.45, Color::ORANGE);
        }

        d.draw_fps(45, 50);
        d.draw_text("Practica 5 - Movimiento en 3D", 20, 20, 30, Color::DARKGRAY);
        d.draw_text("1. Movimiento senoidal", 120, 90, 22, Color::BLUE);
        d.draw_text("2. Rebote", 410, 90, 22, Color::RED);
        d.draw_text("3. Orbita", 690, 90, 22, Color::GREEN);
        d.draw_text("4. Trayectoria", 970, 90, 22, Color::ORANGE);
        d.draw_text("ESC para salir", 20, 760, 20, Color::GRAY);
    }
}
