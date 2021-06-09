use crate::AppData;
use druid::widget::{CrossAxisAlignment, Flex, FlexParams, Label, TextBox};
use druid::{Color, FontDescriptor, FontFamily, FontWeight, Insets, Widget, WidgetExt};
use env_logger::Env;

fn header_row() -> impl Widget<AppData> {
    Flex::row()
        .with_flex_child(
            Label::new(" ~ echo \"headspace\" > /dev/swap".to_string())
                .with_font(FontDescriptor::new(FontFamily::MONOSPACE).with_size(16.0)),
            FlexParams::new(1.0, CrossAxisAlignment::Start),
        )
        .border(Color::PURPLE, 1.0)
}

fn text_area() -> impl Widget<AppData> {
    TextBox::multiline().lens(AppData::current_text).expand()
}

fn bottom_row() -> impl Widget<AppData> {
    Label::new("stattus bar".to_string()).with_font(FontDescriptor::new(FontFamily::MONOSPACE))
}

pub fn ui_builder() -> impl Widget<AppData> {
    Flex::column()
        // Title row
        .with_child(header_row())
        .with_default_spacer()
        // text box
        .with_flex_child(text_area(), 1.0)
        .with_default_spacer()
        // bottom row
        .with_child(bottom_row())
        .padding(Insets::new(10_f64, 10_f64, 10_f64, 10_f64))
        .debug_paint_layout()
}

// fn label_widget<T: Data>(widget: impl Widget<T> + 'static, label: &str) -> impl Widget<T> {
//     Flex::column()
//         .must_fill_main_axis(true)
//         .with_flex_child(widget.center(), 1.0)
//         .with_child(
//             Painter::new(|ctx, _: &_, _: &_| {
//                 let size = ctx.size().to_rect();
//                 ctx.fill(size, &Color::WHITE)
//             })
//                 .fix_height(1.0),
//         )
//         .with_child(Label::new(label).center().fix_height(40.0))
//         .border(Color::WHITE, 1.0)
// }
