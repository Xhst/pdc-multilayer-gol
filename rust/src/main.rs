mod color;
mod game_of_life;
mod image;
mod ml_gol;

use std::env;
use std::process;
use std::time::Instant;

use ml_gol::MlGol;

const DEFAULT_GRID_SIZE: u32 = 512;
const DEFAULT_NUM_LAYERS: u32 = 3;
const DEFAULT_DENSITY: f32 = 0.3;
const DEFAULT_NUM_STEPS: u32 = 64;

fn main() {
    let args: Vec<String> = env::args().collect();

    let grid_size = args.get(1)
        .and_then(|arg| arg.parse().ok())
        .unwrap_or(DEFAULT_GRID_SIZE);

    let num_layers = args.get(2)
        .and_then(|arg| arg.parse().ok())
        .unwrap_or(DEFAULT_NUM_LAYERS);

    let density = args.get(3)
        .and_then(|arg| arg.parse().ok())
        .unwrap_or(DEFAULT_DENSITY);

    let num_steps = args.get(4)
        .and_then(|arg| arg.parse().ok())
        .unwrap_or(DEFAULT_NUM_STEPS);

    let _seed = args.get(5)
        .and_then(|arg| arg.parse().ok())
        .unwrap_or(0);


    if grid_size == 0 || num_layers == 0 || num_steps == 0 {
        eprintln!("Invalid input");
        process::exit(1);
    }

    println!("Starting Multilayer Game of Life.");

    let num_steps = num_steps as usize;

    let mut tstart = Instant::now();
    MlGol::new(grid_size, num_layers, density).start_game(num_steps);
    let mut elapsed = tstart.elapsed();

    println!("Standard version, elapsed time: {:.2?}", elapsed);

    tstart = Instant::now();
    MlGol::new(grid_size, num_layers, density).start_game_par(num_steps);
    elapsed = tstart.elapsed();

    println!("Rayon version, elapsed time: {:.2?}", elapsed);
}
