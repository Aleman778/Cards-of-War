
static int
eat_string(char** scanner, const char* pattern) {
    int count = 0;
    char* scan = *scanner;
    while (*pattern) {
        count++;
        if (*scan++ != *pattern++) {
            return false;
        }
    }
    
    *scanner = scan;
    return true;
}

static int
eat_integer(char** scanner) {
    int result = 0;
    b32 negative = false;
    char* scan = *scanner;
    if (*scan == '-') {
        negative = true;
        scan++;
    }
    
    for (; *scan; scan++) {
        if (*scan < '0' || *scan > '9') {
            break;
        }
        int number = (int) (*scan - '0');
        result = result*10 + number;
    }
    
    *scanner = scan;
    if (negative) {
        result = -result;
    }
    
    return result;
}

void*
read_entire_file(const char* filename) {
    FILE* file = fopen(filename, "rb");
    fseek(file, 0, SEEK_END);
    usize filesize = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    void* contents = malloc(filesize);
    fread(contents, 1, filesize, file);
    fclose(file);
    
    return contents;
}

void
read_tmx_map_data(const char* filename, int map_data[GRID_SIZE_X][GRID_SIZE_Y]) {
    char* contents = (char*) read_entire_file(filename);
    
    // NOTE(alexander): hardcoded and may be invalidated whenever tilesets is changed
    const s32 player_gid = 36;
    const s32 move_up_gid = 21;
    const s32 move_left_gid = 26;
    const s32 move_down_gid = 27;
    const s32 move_right_gid = 28;
    const s32 moving_platform_first_gid = 32;
    const s32 moving_platform_last_gid = 34;
    
    
    for (char* scan = (char*) contents; *scan; scan++) {
        if (eat_string(&scan, "<map")) {
            for (; *scan; scan++) {
                if (eat_string(&scan, ">")) {
                    break;
                }
                if (eat_string(&scan, " width=\"")) {
                    int width = eat_integer(&scan);
                    assert(width == 20); // NOTE(alexander): do we want larger maps?
                } else if (eat_string(&scan, " height=\"")) {
                    int height = eat_integer(&scan);
                    assert(height == 10); // NOTE(alexander): do we want larger maps?
                }
            }
        }
        
        if (eat_string(&scan, "<layer")) {
            for (; *scan; scan++) {
                if (eat_string(&scan, "</layer>")) {
                    break;
                }
                if (eat_string(&scan, "<data encoding=\"csv\">")) {
                    int tile_x = 0;
                    int tile_y = 0;
                    map_data[tile_x][tile_y] = 0;
                    for (; *scan; scan++) {
                        if (eat_string(&scan, "</data>")) {
                            break;
                        }
                        
                        if (*scan == ',') {
                            scan++;
                            
                            tile_y++;
                            if (tile_y >= GRID_SIZE_Y) {
                                tile_y = 0;
                                tile_x++;
                            }
                            //assert(tile_index < MAX_TILEMAP_SIZE && "number of tiles exceeds its limit");
                            map_data[tile_x][tile_y] = 0;
                        }
                        
                        if (*scan == ' ' || *scan == '\n' || *scan == '\t' || *scan == '\r') {
                            continue;
                        }
                        
                        int number = (int) (*scan - '0');
                        assert(number >= 0 && number <= 9);
                        map_data[tile_x][tile_y] = map_data[tile_x][tile_y]*10 + number;
                    }
                }
            }
        }
        
#if 0
        // TODO(alexander): handle other types of object groups, this is only for colliders 
        if (eat_string(&scan, "<objectgroup")) {
            for (; *scan; scan++) {
                if (eat_string(&scan, "</objectgroup>")) {
                    break;
                }
                
                if (eat_string(&scan, "<object")) {
                    //assert(state->num_entities < MAX_NUM_ENTITIES && "number of entities exceeds its limit");
                    //Entity* entity = &state->entities[state->num_entities++];
                    //entity->type = EntityType_AABB_Collider;
                    
                    for (; *scan; scan++) {
                        if (eat_string(&scan, "/>") || eat_string(&scan, "</object>")) {
                            break;
                        }
                        
                        if (eat_string(&scan, "gid=\"")) {
                            s32 gid = eat_integer(&scan);
                            entity->graphics_id = gid;
                            
                            // NOTE(alexander): this creates different entity types
                            if (gid == player_gid) {
                                entity->type = EntityType_Player;
                            } else if (gid >= moving_platform_first_gid &&
                                       gid <= moving_platform_last_gid) {
                                entity->type = EntityType_Moving_Platform;
                            } else if (gid == move_up_gid) {
                                entity->type = EntityType_Move_Up;
                            } else if (gid == move_left_gid) {
                                entity->type = EntityType_Move_Left;
                            } else if (gid == move_down_gid) {
                                entity->type = EntityType_Move_Down;
                            } else if (gid == move_right_gid) {
                                entity->type = EntityType_Move_Right;
                            } else {
                                entity->type = EntityType_None;
                            }
                        } else if (eat_string(&scan, " x=\"")) {
                            entity->position.x = (f32) eat_integer(&scan);
                        } else if (eat_string(&scan, " y=\"")) {
                            entity->position.y = (f32) eat_integer(&scan);
                        } else if (eat_string(&scan, " width=\"")) {
                            entity->size.width = (f32) eat_integer(&scan);
                        } else if (eat_string(&scan, " height=\"")) {
                            entity->size.height = (f32) eat_integer(&scan);
                        }
                        
                        if (eat_string(&scan, "<polygon points=\"")) {
                            v2 origin = entity->position;
                            v2 p;
                            p.x = origin.x + (f32) eat_integer(&scan);
                            assert(*scan++ == ',');
                            p.y = origin.y + (f32) eat_integer(&scan);
                            v2 first = p;
                            if (*scan++ == '"') {
                                break;
                            }
                            
                            for (; *scan && *scan != '"'; scan++) {
                                v2 p2;
                                p2.x = origin.x + (f32) eat_integer(&scan);
                                assert(*scan++ == ',');
                                p2.y = origin.y + (f32) eat_integer(&scan);
                                
                                entity->type = EntityType_Line_Collider;
                                entity->position = p * state->pixels_to_meters;
                                entity->size = (p2 - p) * state->pixels_to_meters;
                                p = p2;
                                
                                assert(state->num_entities < MAX_NUM_ENTITIES);
                                entity = &state->entities[state->num_entities++];
                                
                                if (*scan == '"') {
                                    scan++;
                                    break;
                                }
                            }
                            
                            entity->type = EntityType_Line_Collider;
                            entity->position = p * state->pixels_to_meters;
                            entity->size = (first - p) * state->pixels_to_meters;
                        }
                    }
                    
                    if (entity->type != EntityType_Line_Collider) {
                        // NOTE(alexander): convert pixels to meters
                        entity->position.x *= state->pixels_to_meters;
                        entity->position.y *= state->pixels_to_meters;
                        entity->size.width *= state->pixels_to_meters;
                        entity->size.height *= state->pixels_to_meters;
                    }
                    
                    if (entity->type == EntityType_Moving_Platform) {
                        entity->size.height = 0.0f;
                    }
                }
            }
        }
#endif
    }
    
    free(contents);
}
