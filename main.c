#include "raylib.h"
#include <math.h>
#include <stdio.h>

#define SCREEN_WIDTH 1440
#define SCREEN_HEIGHT 880

#define GRID_SIZE 80
#define GRID_COLS (SCREEN_WIDTH / GRID_SIZE)
#define GRID_ROWS (SCREEN_HEIGHT / GRID_SIZE)

#define MAX_PARTICLES 400
#define PARTICLE_SIZE 10
#define MAX_CELL_PARTICLES 200

struct Cell{
    int count;
    int indices[MAX_CELL_PARTICLES];
};
typedef struct Cell Cell;

Vector2 positions[MAX_PARTICLES];
Vector2 velocities[MAX_PARTICLES];
bool colliding[MAX_PARTICLES];

Cell grid[GRID_ROWS][GRID_COLS];

int selectedParticle = -1;
bool toggle_mode = true;
bool toggle_text = true;

int comparisons = 0;


void initialise_particles(){
    for (int i = 0; i < MAX_PARTICLES; i++){
        positions[i] = (Vector2){
            GetRandomValue(0, SCREEN_WIDTH - PARTICLE_SIZE),
            GetRandomValue(0, SCREEN_HEIGHT - PARTICLE_SIZE)
        };

        velocities[i] = (Vector2){
            GetRandomValue(-100, 100) / 60.0f,
            GetRandomValue(-100, 100) / 60.0f
        };
    }
}

void clear_grid(){
    for (int r = 0; r < GRID_ROWS; r++)
        for (int c = 0; c < GRID_COLS; c++)
            grid[r][c].count = 0;
}

void build_grid(){
    for (int i = 0; i < MAX_PARTICLES; i++){

        int col = positions[i].x / GRID_SIZE;
        int row = positions[i].y / GRID_SIZE;

        if (col >= 0 && col < GRID_COLS && row >= 0 && row < GRID_ROWS){

            int count = grid[row][col].count;

            if (count < MAX_CELL_PARTICLES){
                grid[row][col].indices[count] = i;
                grid[row][col].count++;
            }
        }
    }
}

void update_particles(){

    for (int i = 0; i < MAX_PARTICLES; i++){

        positions[i].x += velocities[i].x;
        positions[i].y += velocities[i].y;

        if (positions[i].x <= 0 || positions[i].x >= SCREEN_WIDTH - PARTICLE_SIZE)
            velocities[i].x *= -1;

        if (positions[i].y <= 0 || positions[i].y >= SCREEN_HEIGHT - PARTICLE_SIZE)
            velocities[i].y *= -1;
    }
}

void is_clicked(){

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){

        Vector2 mouse = GetMousePosition();
        selectedParticle = -1;

        for (int i = 0; i < MAX_PARTICLES; i++){

            Rectangle rect = { positions[i].x, positions[i].y, PARTICLE_SIZE,PARTICLE_SIZE};

            if (CheckCollisionPointRec(mouse, rect)){
                selectedParticle = i;
                break;
            }
        }
    }
}

bool are_colliding(Vector2 center1, Vector2 center2){
    float dx = center1.x - center2.x;
    float dy = center1.y - center2.y;
    float distance = sqrtf(dx*dx + dy*dy);

    return (distance < PARTICLE_SIZE);
}

void brute_force() {

    Vector2 center1 = {
        positions[selectedParticle].x + PARTICLE_SIZE / 2,
        positions[selectedParticle].y + PARTICLE_SIZE / 2
    };

    for (int i = 0; i < MAX_PARTICLES; i++){

        if (i == selectedParticle){
            continue;
        }
        comparisons++;

        Vector2 center2 = {
            positions[i].x + PARTICLE_SIZE / 2,
            positions[i].y + PARTICLE_SIZE / 2
        };

        bool is_colliding = are_colliding(center1, center2);

        DrawLineEx(center1, center2, 1, is_colliding ? RED : PURPLE);

        if (is_colliding){
            colliding[i] = true;
            colliding[selectedParticle] = true;
        }
    }
}

void spatial_partitioning(){

    int col = positions[selectedParticle].x / GRID_SIZE;
    int row = positions[selectedParticle].y / GRID_SIZE;

    Vector2 center1 = {
        positions[selectedParticle].x + PARTICLE_SIZE / 2,
        positions[selectedParticle].y + PARTICLE_SIZE / 2
    };

    for (int r = row - 1; r <= row + 1; r++){
        for (int c = col - 1; c <= col + 1; c++){

            if (r >= 0 && r < GRID_ROWS && c >= 0 && c < GRID_COLS){

                DrawRectangle( c * GRID_SIZE, r * GRID_SIZE, GRID_SIZE, GRID_SIZE, Fade(YELLOW, 0.2f));

                for (int i = 0; i < grid[r][c].count; i++){

                    int other = grid[r][c].indices[i];
                    if (other == selectedParticle){ 
                        continue;
                    }

                    comparisons++;

                    Vector2 center2 = {
                        positions[other].x + PARTICLE_SIZE / 2,
                        positions[other].y + PARTICLE_SIZE / 2
                    };

                    bool is_colliding= are_colliding(center1, center2);

                    DrawLineEx(center1, center2, 2, is_colliding ? RED : WHITE);

                    if (is_colliding){
                        colliding[other] = true;
                        colliding[selectedParticle] = true;
                    }
                }
            }
        }
    }
}

void render_particles(){

    for (int i = 0; i < MAX_PARTICLES; i++){

        Color color = GREEN;

        if (i == selectedParticle){
            color = ORANGE;
        }

        if (colliding[i]){
            color = RED;
        }

        DrawRectangle(positions[i].x, positions[i].y, PARTICLE_SIZE, PARTICLE_SIZE, color);
    }
}

void draw_grid(){
    for (int x = 0; x <= SCREEN_WIDTH; x += GRID_SIZE){
        DrawLineEx((Vector2){x, 0}, (Vector2){x, SCREEN_HEIGHT}, 2, GRAY);
    }

    for (int y = 0; y <= SCREEN_HEIGHT; y += GRID_SIZE){
        DrawLineEx((Vector2){0, y}, (Vector2){SCREEN_WIDTH, y}, 2, GRAY);
    }
}

int main(){
    SetConfigFlags(FLAG_WINDOW_UNDECORATED);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT,"Spatial Partitioning");

    initialise_particles();
    SetTargetFPS(60);

    while (!WindowShouldClose()){

        if (IsKeyPressed(KEY_ESCAPE)){
            CloseWindow();
            return 0;
        }

        if (IsKeyPressed(KEY_TAB)){
            toggle_mode = !toggle_mode;
        }

        if (IsKeyPressed(KEY_H)){
            toggle_text = !toggle_text;
        }

        update_particles();

        draw_grid();
        clear_grid();
        build_grid();

        is_clicked();

        comparisons = 0;
        for (int i = 0; i < MAX_PARTICLES; i++){
            colliding[i] = false;
        }

        BeginDrawing();
        ClearBackground(BLACK);

        if (selectedParticle != -1){
            if (toggle_mode){
                spatial_partitioning();
            }
            else{
                brute_force();
            }
        }

        render_particles();

        if (toggle_text){
            DrawText(toggle_mode ? "MODE: SPATIAL PARTITION" : "MODE: BRUTE FORCE", 10, 40, 20, toggle_mode ? YELLOW : PURPLE);

            DrawText(TextFormat("Comparisons: %d", comparisons), 10, 70, 20, WHITE);

            DrawText(TextFormat("Max Particles : %d", MAX_PARTICLES), 10, 100, 20, WHITE);

            DrawText(TextFormat("Max Cell Particles : %d", MAX_CELL_PARTICLES), 10, 130, 20, WHITE);

            DrawText("TAB = Toggle Mode", 10, 160, 20, GRAY);

            DrawText("Click particle to visualise", 10, 190, 20, WHITE);

            DrawText("Press 'H' to hide text", 10, 220, 20, ORANGE);
            DrawText("Press 'ESC' to exit", 10, 250, 20, RED);

            DrawFPS(10, 10);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
