

typedef s32 Game_State_Type;
enum {
    GameState_Discard_Cards,
    GameState_Select_Cards,
    GameState_Turn,
    GameState_Animation,
    GameState_Last,
};

typedef struct {
    Game_State_Type type;
    entity_t* current_entity;
} Game_State;
