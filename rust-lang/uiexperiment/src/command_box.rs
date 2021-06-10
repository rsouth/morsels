use crate::ESC_HOT_KEY;
use druid::widget::Controller;
use druid::{Env, Event, EventCtx, Widget};

pub(crate) struct CommandBoxController;
impl Default for CommandBoxController {
    fn default() -> Self {
        CommandBoxController {}
    }
}
impl<T, W: Widget<T>> Controller<T, W> for CommandBoxController {
    fn event(&mut self, child: &mut W, ctx: &mut EventCtx, event: &Event, data: &mut T, env: &Env) {
        match &event {
            // Esc to switch to Command mode
            Event::Command(x) if x.is(ESC_HOT_KEY) => {
                println!("Entering Command mode");
                ctx.request_focus();
            }
            _ => child.event(ctx, event, data, env),
        }
    }
}
