use druid::commands::SHOW_OPEN_PANEL;
use druid::{DelegateCtx, FileDialogOptions, WindowId};

pub(crate) fn process(ctx: &mut DelegateCtx, command: String, window_id: WindowId) {
    println!("Processing command [{}]", command);

    match command.as_str() {
        ":o" => {
            let opts = FileDialogOptions::default();
            ctx.submit_command(SHOW_OPEN_PANEL.with(opts).to(window_id));
        }
        _ => {}
    }
}
