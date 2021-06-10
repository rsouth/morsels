use crate::command_box::CommandBoxController;
use crate::AppData;
use druid::widget::{CrossAxisAlignment, Flex, FlexParams, Label, TextBox};
use druid::{FontDescriptor, FontFamily, Insets, Widget, WidgetExt};

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
        .debug_paint_layout()
}

fn header_row() -> impl Widget<AppData> {
    Flex::row().with_flex_child(
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
                .lens(AppData::command_text)
                .controller(CommandBoxController::default())
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
