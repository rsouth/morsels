use druid::commands::SHOW_OPEN_PANEL;
use druid::{DelegateCtx, FileDialogOptions, WindowId};
use std::iter::FromIterator;

pub(crate) fn process(ctx: &mut DelegateCtx, command: String, window_id: WindowId) {
    println!("Processing command [{}]", command);

    if !command.starts_with(":") {
        return
    }

    let mut cmd_iter = command.split_whitespace();
    match cmd_iter.next() {
        // new project
        Some(":n") => {
            // let args = cmd_iter.collect(); //cmd_iter.into_iter().flatten();  // .collect_vec();

            let mut args = "".to_string();
            // cmd_iter.for_each(|s| args.push_str(s));
            // match cmd_iter.next() {
            //     None => { ".devswap" }
            //     Some(txt) => { txt }
            // }


            // let s = String::from_iter(args);
            println!("New project [{}]", args);
        }
        // open project ...
        Some(":o") => {
            let opts = FileDialogOptions::default();
            ctx.submit_command(SHOW_OPEN_PANEL.with(opts).to(window_id));
        }
        None => {}
        _ => {}
    }

    // match command.as_str() {
    //     ":n" => {
    //
    //     }
    //     // open file
    //     ":o" => {
    //         let opts = FileDialogOptions::default();
    //         ctx.submit_command(SHOW_OPEN_PANEL.with(opts).to(window_id));
    //     }
    //     _ => {}
    // }
}
