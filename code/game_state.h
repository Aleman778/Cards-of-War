

typedef s32 Game_State_Type;
enum {
    GameState_Discard_Cards,
    GameState_Select_Cards,
    GameState_Turn,
    GameState_Animation,
    GameState_EnemyTurn,
    GameState_Last,
};

typedef struct {
    Game_State_Type type;
    Game_State_Type next_state;
    entity_t* current_entity;
} Game_State;
