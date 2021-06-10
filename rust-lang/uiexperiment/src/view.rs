use crate::AppData;
use druid::widget::{CrossAxisAlignment, Flex, FlexParams, Label, TextBox};
use druid::{Color, FontDescriptor, FontFamily, Insets, Widget, WidgetExt};

fn header_row() -> impl Widget<AppData> {
    Flex::row()
        .with_flex_child(
            Label::new(" ~ echo \"headspace\" > /dev/swap".to_string())
                .with_font(FontDescriptor::new(FontFamily::MONOSPACE).with_size(16.0)),
            FlexParams::new(1.0, CrossAxisAlignment::Start),
        )
}

fn text_area() -> impl Widget<AppData> {
    TextBox::multiline().lens(AppData::current_text).expand()
}

fn command_row() -> impl Widget<AppData> {
    Flex::row()
        .with_flex_child(
            TextBox::new()
                .with_placeholder("[Esc] for command mode; :e to edit previous")
                .lens(AppData::current_text)
                .expand_width(),
            FlexParams::new(1., CrossAxisAlignment::Start),
        )
        .with_default_spacer()
        .with_child(
            // TODO Lens in to AppData::status of some kind (window status? command status?)
            Label::new("Status Text".to_string())
                .with_font(FontDescriptor::new(FontFamily::MONOSPACE)),
        )
}

pub fn ui_builder() -> impl Widget<AppData> {
    Flex::column()
        // Title row
        .with_child(header_row())
        .with_default_spacer()
        // text box
        .with_flex_child(text_area(), 1.0)
        .with_default_spacer()
        // command row
        .with_child(command_row())
        .with_default_spacer()
        // // status row
        // .with_child(bottom_row())
        .padding(Insets::new(10_f64, 10_f64, 10_f64, 10_f64))
        // .debug_paint_layout()
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
