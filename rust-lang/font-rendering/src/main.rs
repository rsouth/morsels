#[macro_use]
extern crate log;

use std::io::Error;

use fontdue::{
    layout::{CoordinateSystem, Layout, LayoutSettings, TextStyle},
    Font, FontResult,
};
use raqote::{DrawTarget, SolidSource};

fn main() -> Result<(), Error> {
    // load and parse the font
    let font = match load_font() {
        Ok(font) => font,
        Err(e) => return Err(std::io::Error::new(std::io::ErrorKind::Other, e)),
    };

    // create a draw target and fill the background blue
    let mut dt = DrawTarget::new(500, 220);
    dt.clear(SolidSource::from_unpremultiplied_argb(100, 100, 100, 255));

    // draw the next, centered in the draw target
    // draw_text(&mut dt, "Alpha", 0_f32, 0_f32, 150_f32, &font);

    // dt.write_png("output.png").unwrap();

    let s = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Etiam condimentum varius dui sed congue. Curabitur posuere hendrerit libero vel cursus. Sed id pretium lectus. Donec luctus sapien a urna molestie, at maximus felis maximus. Pellentesque eu neque tortor. Aliquam ut odio eget est venenatis luctus. Morbi eu sagittis neque. Nunc vitae fermentum tellus. Maecenas vel condimentum velit. Morbi malesuada dui eu luctus tincidunt. Quisque in lorem malesuada, fermentum erat sed, sollicitudin massa. Fusce pellentesque odio a dui euismod tincidunt. Sed nec feugiat arcu. Cras aliquet justo vitae purus vulputate, rutrum vehicula neque imperdiet.";

    draw_text(&mut dt, s, 0.0, 0.0, 16.0, &font);
    dt.write_png("output.png").unwrap();

    println!("Hello, world!");

    Ok(())
}

fn load_font() -> FontResult<Font> {
    let font_data = include_bytes!("assets/OpenSans-Regular.ttf") as &[u8];
    // Parse it into the font type.
    let settings = fontdue::FontSettings::default();

    fontdue::Font::from_bytes(font_data, settings)
}

pub fn draw_text(dt: &mut DrawTarget, content: &str, x: f32, y: f32, px: f32, font: &Font) {
    let mut layout = Layout::new(CoordinateSystem::PositiveYDown);
    layout.reset(&LayoutSettings {
        x,
        y,
        max_width: Option::Some(500_f32),
        max_height: Option::Some(200_f32),
        wrap_style: fontdue::layout::WrapStyle::Letter,
        // horizontal_align: fontdue::layout::HorizontalAlign::Center,
        ..LayoutSettings::default()
    });
    layout.append(&[font], &TextStyle::new(&content, px, 0));
    for glyph in layout.glyphs() {
        let (metrics, coverage) = font.rasterize(glyph.key.c, px);
        println!("Metrics: {:?}", glyph);

        // println!("{:?}", coverage);

        // let mut path = PathBuilder::new();
        // path.rect(
        //     glyph.x,
        //     glyph.y,
        //     metrics.width as f32,
        //     metrics.height as f32,
        // );
        // dt.stroke(
        //     &path.finish(),
        //     &Source::Solid(SolidSource::from_unpremultiplied_argb(255, 255, 255, 150)),
        //     &StrokeStyle::default(),
        //     &DrawOptions::default(),
        // );

        //
        let mut image_data = Vec::with_capacity(coverage.len());
        for cov in coverage.iter() {
            let pixel = rgb_to_u32(0, 0, 0, *cov as usize);
            image_data.push(pixel);
        }
        dt.draw_image_at(
            glyph.x,
            glyph.y,
            &raqote::Image {
                width: metrics.width as i32,
                height: metrics.height as i32,
                data: &image_data,
            },
            &raqote::DrawOptions {
                blend_mode: raqote::BlendMode::Darken,
                alpha: 1.0,
                antialias: raqote::AntialiasMode::Gray,
            },
        );
    }
}

pub fn rgb_to_u32(red: usize, green: usize, blue: usize, alpha: usize) -> u32 {
    let r = red.clamp(0, 255);
    let g = green.clamp(0, 255);
    let b = blue.clamp(0, 255);
    let a = alpha.clamp(0, 255);
    ((a << 24) | (r << 16) | (g << 8) | b) as u32
}
