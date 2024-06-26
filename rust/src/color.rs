pub struct Color {
    pub r: u8,
    pub g: u8,
    pub b: u8,
}

impl Color {
    pub fn add_colors(c1: &Color, c2: &Color) -> Color {
        let result = Color {
            r: (c1.r + c2.r),
            g: (c1.g + c2.g),
            b: (c1.b + c2.b),
        };
        result
    }

    pub fn color_to_hex(color: &Color) -> String {
        format!("#{:02x}{:02x}{:02x}", color.r, color.g, color.b)
    }
}

pub struct ColorHSV {
    pub h: f64,
    pub s: f64,
    pub v: f64,
}

impl ColorHSV {

    pub fn to_rgb(&self) -> Color {
        let c = self.v * self.s;
        let x = c * (1.0 - ((self.h / 60.0) % 2.0 - 1.0).abs());
        let m = self.v - c;

        let (r, g, b) = if self.h < 60.0 {
            (c, x, 0.0)
        } else if self.h < 120.0 {
            (x, c, 0.0)
        } else if self.h < 180.0 {
            (0.0, c, x)
        } else if self.h < 240.0 {
            (0.0, x, c)
        } else if self.h < 300.0 {
            (x, 0.0, c)
        } else {
            (c, 0.0, x)
        };

        Color {
            r: ((r + m) * 255.0) as u8,
            g: ((g + m) * 255.0) as u8,
            b: ((b + m) * 255.0) as u8,
        }
    }
}
