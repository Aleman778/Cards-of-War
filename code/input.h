
typedef struct {
    u32 num_half_transitions;
    b32 ended_down;
} Input_Button;

typedef struct {
    v2 mouse;
    Input_Button mouse_buttons[6];
} Input;

b32
button_was_pressed(Input_Button* button) {
    return (button->num_half_transitions > 2 || 
            (button->num_half_transitions == 1 && !button->ended_down));
}

b32
button_is_down(Input_Button* button) {
    return button->ended_down;
}
