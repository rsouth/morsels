use druid::Data;
use druid::Lens;
use std::sync::Arc;

#[derive(Clone, Data, Lens)]
pub struct AppData {
    display_window_x: f64,
    display_window_y: f64,
    window_visible: bool,
    current_text: Arc<String>,
}

impl Default for AppData {
    fn default() -> Self {
        AppData {
            display_window_x: 0.0,
            display_window_y: 0.0,
            window_visible: true,
            current_text: Arc::new("".to_string()),
        }
    }
}

impl AppData {
    pub fn toggle_window(&mut self) -> bool {
        self.window_visible = !self.window_visible;
        self.window_visible
    }
}
