use std::fs::File;
use std::path::Path;
use png::{ColorType, Encoder};

pub fn write_png_file(filename: &str, width: u32, height: u32, buffer: &[u8]) {
    let path = Path::new(filename);
    let file = File::create(&path).expect("Failed to create file");

    let mut encoder = Encoder::new(file, width, height);
    encoder.set_color(ColorType::Rgba);
    encoder.set_depth(png::BitDepth::Eight);

    let mut writer = encoder.write_header().expect("Failed to write PNG header");
    writer.write_image_data(buffer).expect("Failed to write PNG image data");
}
