use crate::game_of_life::Gol;
use crate::color::{Color, ColorHSV};
use crate::image::write_png_file;

use rayon::prelude::*;

pub struct MlGol {
    grid_size: usize,
    layers: Vec<Gol>,
    combined: Vec<Color>,
    dependent: Vec<Color>
}

impl MlGol {
    pub fn new(grid_size: u32, num_layers: u32, density: f32) -> MlGol {
        let mut layers = Vec::new();
        let mut combined = Vec::new();
        let mut dependent = Vec::new();

        // Add 2 to the grid size to account for the ghost cells
        let grid_size = (grid_size + 2) as usize; 
        let num_layers = num_layers as usize;

        for _ in 0..num_layers {
            layers.push(Gol::new(grid_size, density));
        }

        // Initialize the combined and dependent grids with black color
        for _ in 0..grid_size * grid_size {
            combined.push(Color { r: 0, g: 0, b: 0 });
            dependent.push(Color { r: 0, g: 0, b: 0 });
        }

        println!("Initialized multilayer game of life with {} layers and grid size {}", num_layers, grid_size - 2);
        MlGol::print_layers_colors(layers.len());

        MlGol {
            grid_size,
            layers,
            combined,
            dependent
        }
    }

    fn print_layers_colors(num_layers: usize) {
        println!("Colors for the layers:");
        for i in 0..num_layers {
            let color = MlGol::get_color_for_layer(i, num_layers);
            println!("Layer {}: {}", i, Color::color_to_hex(&color));
        }
    }

    pub fn start_game(&mut self, num_steps: usize) {
        for step in 0..num_steps {
            for layer in self.layers.iter_mut() {
                layer.fill_ghost_cells();
                layer.step();    
                layer.swap_grids();   
            }

            self.calculate_combined();
            self.calculate_dependent();

            self.create_png_for_step(step);
            self.reset_combined_and_dependent();
        }
    }

    pub fn start_game_par(&mut self, num_steps: usize) {
        for step in 0..num_steps {
            for layer in self.layers.iter_mut() {
                layer.fill_ghost_cells();
                layer.step_par();    
                layer.swap_grids();   
            }

            self.calculate_combined_par();
            self.calculate_dependent_par();

            self.create_png_for_step_par(step);
            self.reset_combined_and_dependent_par();
        }
    }

    fn get_color_for_layer(layer: usize, num_layers: usize) -> Color {
        // Calculate the angle for the hue based on the layer
        let hue = (360.0 * layer as f64) / num_layers as f64;

        let hsv_color = ColorHSV {
            h: hue,
            s: 1.0,
            v: 1.0,
        };

        hsv_color.to_rgb()
    }

    fn calculate_combined(&mut self) {

        for k in 0..self.layers.len() {
            let color = MlGol::get_color_for_layer(k, self.layers.len());

            for i in 0..self.grid_size {
                for j in 0..self.grid_size {
                    let idx = (i * self.grid_size + j) as usize;

                    if !self.layers[k].current[idx] {
                        continue;
                    }

                    self.combined[idx] = Color::add_colors(&self.combined[idx], &color);
                }
            }
        }
    }

    fn calculate_combined_par(&mut self) {
        self.combined = (0..self.combined.len())
            .into_par_iter()
            .map(|idx| {
                let point = Gol::point(self.grid_size, idx);

                if point.x == 0 || point.x == self.grid_size - 1 || point.y == 0 || point.y == self.grid_size - 1 {
                    return Color { r: 0, g: 0, b: 0 };
                }

                let mut color = Color { r: 0, g: 0, b: 0 };

                for k in 0..self.layers.len() {
                    let layer = &self.layers[k];
                    let layer_color = MlGol::get_color_for_layer(k, self.layers.len());

                    if layer.current[idx] {
                        color = Color::add_colors(&color, &layer_color);
                    }
                }

                color
            })
            .collect();
    }

    fn create_png_for_grid(grid: &Vec<Color>, grid_size: usize, step: usize, folder: &str) {
        let mut buffer = vec![0u8; (grid_size * grid_size * 4) as usize];

        for i in 0..grid_size {
            for j in 0..grid_size {
                let idx = ((i * grid_size + j) * 4) as usize;
                let grid_idx = (i * grid_size + j) as usize;

                buffer[idx] = grid[grid_idx].r;
                buffer[idx + 1] = grid[grid_idx].g;
                buffer[idx + 2] = grid[grid_idx].b;
                buffer[idx + 3] = 255;
            }
        }

        let filename = format!("output/{}/{}{:04}.png", folder, folder, step);
        write_png_file(&filename, grid_size as u32, grid_size as u32, &buffer);
    }

    fn create_png_for_grid_par(grid: &Vec<Color>, grid_size: usize, step: usize, folder: &str) {
        let buffer: Vec<u8> = (0..grid_size * grid_size)
            .into_par_iter()
            .flat_map(|idx| {
                let i = idx / grid_size;
                let j = idx % grid_size;

                let grid_idx = (i * grid_size + j) as usize;

                vec![grid[grid_idx].r, grid[grid_idx].g, grid[grid_idx].b, 255]
            })
            .collect();

        let filename = format!("output/{}/{}{:04}.png", folder, folder, step);
        write_png_file(&filename, grid_size as u32, grid_size as u32, &buffer);
    }

    fn create_png_for_step(&mut self, step: usize) {
        let grid_size = self.grid_size - 2;

        MlGol::create_png_for_grid(&self.combined, grid_size, step, "combined");
        MlGol::create_png_for_grid(&self.dependent, grid_size, step, "dependent");
    }

    fn create_png_for_step_par(&mut self, step: usize) {
        let grid_size = self.grid_size - 2;

        MlGol::create_png_for_grid_par(&self.combined, grid_size, step, "combined");
        MlGol::create_png_for_grid_par(&self.dependent, grid_size, step, "dependent");
    }

    fn reset_combined_and_dependent(&mut self) {
        for i in 0..self.combined.len() {
            self.combined[i] = Color { r: 0, g: 0, b: 0 };
            self.dependent[i] = Color { r: 0, g: 0, b: 0 };
        }
    }

    fn reset_combined_and_dependent_par(&mut self) {
        self.combined = (0..self.combined.len())
            .into_par_iter()
            .map(|_| Color { r: 0, g: 0, b: 0 })
            .collect();

        self.dependent = (0..self.dependent.len())
            .into_par_iter()
            .map(|_| Color { r: 0, g: 0, b: 0 })
            .collect();
    }

    fn count_dependent_alive_neighbors(&self, i: usize, j: usize) -> u8 {
        let mut count = 0;

        for x in -1..=1 {
            for y in -1..=1 {
                if x == 0 && y == 0 {
                    continue;
                }

                let idx = ((i as i32 + x) * self.grid_size as i32 + (j as i32 + y)) as usize;

                if self.combined[idx].r > 0 && self.combined[idx].g > 0 && self.combined[idx].b > 0 {
                    count += 1;
                }
            }
        }

        count
    }

    fn calculate_dependent(&mut self) {
        for i in 1..self.grid_size - 1 {
            for j in 1..self.grid_size - 1 {
                let idx = (i * self.grid_size + j) as usize;
                
                let alive_neighbors = self.count_dependent_alive_neighbors(i, j);

                let channel_value = ((alive_neighbors as f32) / 8.0 * 255.0) as u8;
                
                self.dependent[idx] = Color { r: channel_value, g: channel_value, b: channel_value };
                
            }
        }
    }

    fn calculate_dependent_par(&mut self) {
        self.dependent = (0..self.dependent.len())
            .into_par_iter()
            .map(|idx| {
                let point = Gol::point(self.grid_size, idx);

                if point.x == 0 || point.x == self.grid_size - 1 || point.y == 0 || point.y == self.grid_size - 1 {
                    return Color { r: 0, g: 0, b: 0 };
                }

                let alive_neighbors = self.count_dependent_alive_neighbors(point.x, point.y);
                let channel_value = ((alive_neighbors as f32) / 8.0 * 255.0) as u8;

                Color { r: channel_value, g: channel_value, b: channel_value }
            })
            .collect();
    }


}