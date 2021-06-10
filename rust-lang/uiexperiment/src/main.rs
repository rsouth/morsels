mod command_box;
mod command_processor;
mod data;
mod view;

use std::thread;

use crate::data::AppData;
use druid::commands::CONFIGURE_WINDOW;
use druid::{
    kurbo::Point, widget::prelude::*, AppDelegate, AppLauncher, Command, DelegateCtx, ExtEventSink,
    Handled, HotKey, KbKey, Selector, Target, WindowConfig, WindowDesc, WindowId,
};

const GLOBAL_HOT_KEY: Selector<WindowId> = Selector::new("dev.untitled1.toggle-window-hotkey");
const ESC_HOT_KEY: Selector = Selector::new("dev.untitled1.esc-hotkey");
const EXEC_CMD: Selector<String> = Selector::new("dev.untitled1.execute-command");

pub fn main() {
    let screen_rect = druid::Screen::get_display_rect();

    let win_height = screen_rect.height() * 0.777;
    let main_window = WindowDesc::new(view::ui_builder())
        .title("/dev/swap sandbox")
        // .transparent(true)
        // .show_titlebar(false)
        .set_position((100.0, screen_rect.height() - win_height - 100.0))
        .window_size((screen_rect.width() * 0.333, win_height));

    let data = AppData::default();
    let window_id = main_window.id;
    let app = AppLauncher::with_window(main_window)
        .log_to_console()
        .delegate(Delegate::default());

    let event_sink = app.get_external_handle();
    global_hotkey_listener(event_sink, window_id);

    app.launch(data).expect("launch failed");
}

struct Delegate {
    hot_key_esc: HotKey,
}
impl Default for Delegate {
    fn default() -> Self {
        Delegate {
            hot_key_esc: HotKey::new(None, KbKey::Escape),
        }
    }
}
impl AppDelegate<AppData> for Delegate {
    fn event(
        &mut self,
        _ctx: &mut DelegateCtx,
        _window_id: WindowId,
        event: Event,
        _data: &mut AppData,
        _env: &Env,
    ) -> Option<Event> {
        // println!("Event: {:?}", event);
        match &event {
            Event::KeyDown(key) if self.hot_key_esc.matches(key) => {
                _ctx.submit_command(Command::from(ESC_HOT_KEY));
            }
            _ => (),
        };

        Some(event)
    }

    fn command(
        &mut self,
        _ctx: &mut DelegateCtx,
        _target: Target,
        cmd: &Command,
        data: &mut AppData,
        _env: &Env,
    ) -> Handled {
        if let Some(number) = cmd.get(GLOBAL_HOT_KEY) {
            println!("Event sink got toggle visible event");
            if data.toggle_window() {
                println!("Showing window {:?}", number);
                let wc = WindowConfig::default().set_position(Point { x: -0.0, y: 0.0 });
                _ctx.submit_command(CONFIGURE_WINDOW.with(wc).to(*number));
            } else {
                println!("Hiding window {:?}", number);
                let wc = WindowConfig::default().set_position(Point {
                    x: -10000.0,
                    y: 100.0,
                });
                _ctx.submit_command(CONFIGURE_WINDOW.with(wc).to(*number));
            }

            Handled::Yes
        } else if let Some(_) = cmd.get(EXEC_CMD) {
            println!("Execute Command: {}", data.command_text);
            command_processor::process(data.command_text.to_string());
            Handled::Yes
        } else {
            Handled::No
        }
    }

    fn window_added(
        &mut self,
        id: WindowId,
        _data: &mut AppData,
        _env: &Env,
        _ctx: &mut DelegateCtx,
    ) {
        println!("Window added, {:?}", id);
    }
}

fn global_hotkey_listener(sink: ExtEventSink, winid: WindowId) {
    thread::spawn(move || {
        let mut hk = hotkey::Listener::new();
        hk.register_hotkey(
            hotkey::modifiers::CONTROL | hotkey::modifiers::SHIFT,
            'A' as u32,
            move || {
                println!("Ctrl-Shift-A pressed!");
                // println!("{:?}", main_window.id);

                sink.submit_command(GLOBAL_HOT_KEY, winid, Target::Auto)
                    .expect("command failed to submit");
            },
        )
        .unwrap();

        hk.listen();
    });
}
