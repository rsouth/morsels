use crate::{ESC_HOT_KEY, EXEC_CMD};
use druid::widget::Controller;
use druid::{Env, Event, EventCtx, KbKey, Target, Widget};

pub(crate) struct CommandBoxController;
impl Default for CommandBoxController {
    fn default() -> Self {
        CommandBoxController {}
    }
}
impl<T, W: Widget<T>> Controller<T, W> for CommandBoxController {
    fn event(&mut self, child: &mut W, ctx: &mut EventCtx, event: &Event, data: &mut T, env: &Env) {
        // println!("Evt: {:?}", event);
        match &event {
            // Esc to switch to Command mode
            Event::Command(x) if x.is(ESC_HOT_KEY) => {
                println!("Entering Command mode");
                ctx.request_focus();
            }
            // Enter to execute Command
            Event::KeyDown(key_event) if key_event.key == KbKey::Enter => {
                println!("command_box event(Enter) -> {:?}", key_event);
                ctx.submit_command(EXEC_CMD.to(Target::Auto));
                ctx.set_handled();
            }

            _ => child.event(ctx, event, data, env),
        }
    }
}
