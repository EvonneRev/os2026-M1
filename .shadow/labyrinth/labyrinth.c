#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <testkit.h>
#include "labyrinth.h"

static bool printOriginalMap(const char *filename);

int main(int argc, char *argv[]) {
    // TODO: Implement this function
    const char *filename = NULL;
    const char *direction = NULL;

    char playerId = '\0';
    bool hasPlayer = false;

    // --version 只允许单独使用。
    if (argc == 2 && strcmp(argv[1], "--version") == 0) {
        if (puts(VERSION_INFO) == EOF) {
            return 1;
        }
        return fflush(stdout) == 0 ? 0 : 1;
    }

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--map") == 0 ||
            strcmp(argv[i], "-m") == 0) {
            if (filename != NULL || i + 1 >= argc) {
                goto invalid_arguments;
            }

            filename = argv[++i];
        } else if (strcmp(argv[i], "--player") == 0 ||
                   strcmp(argv[i], "-p") == 0) {
            if (hasPlayer || i + 1 >= argc) {
                goto invalid_arguments;
            }

            const char *value = argv[++i];

            if (strlen(value) != 1 || !isValidPlayer(value[0])) {
                goto invalid_arguments;
            }

            playerId = value[0];
            hasPlayer = true;
        } else if (strcmp(argv[i], "--move") == 0) {
            if (direction != NULL || i + 1 >= argc) {
                goto invalid_arguments;
            }

            direction = argv[++i];

            if (strcmp(direction, "up") != 0 &&
                strcmp(direction, "down") != 0 &&
                strcmp(direction, "left") != 0 &&
                strcmp(direction, "right") != 0) {
                goto invalid_arguments;
            }
        } else {
            goto invalid_arguments;
        }
    }

    if (filename == NULL || !hasPlayer) {
        goto invalid_arguments;
    }

    Labyrinth labyrinth = {0};

    if (!loadMap(&labyrinth, filename)) {
        fprintf(stderr, "Failed to load map\n");
        return 1;
    }

    if (!isConnected(&labyrinth)) {
        fprintf(stderr, "Map is not connected\n");
        return 1;
    }

    // 无移动请求：输出原地图。
    if (direction == NULL) {
        if (!printOriginalMap(filename)) {
            fprintf(stderr, "Failed to print map\n");
            return 1;
        }

        return 0;
    }

    if (!movePlayer(&labyrinth, playerId, direction)) {
        fprintf(stderr, "Failed to move player\n");
        return 1;
    }

    if (!saveMap(&labyrinth, filename)) {
        fprintf(stderr, "Failed to save map\n");
        return 1;
    }

    return 0;

invalid_arguments:
    fprintf(stderr, "Invalid arguments\n");
    return 1;
}

void printUsage() {
    printf("Usage:\n");
    printf("  labyrinth --map map.txt --player id\n");
    printf("  labyrinth -m map.txt -p id\n");
    printf("  labyrinth --map map.txt --player id --move direction\n");
    printf("  labyrinth --version\n");
}

bool isValidPlayer(char playerId) {
    // TODO: Implement this function
    if(playerId > '9' || playerId < '0')
    return false;
}

bool loadMap(Labyrinth *labyrinth, const char *filename) {
    // TODO: Implement this function
    if(labyrinth == NULL || filename == NULL){
        return false;
    }
    FILE *fp = fopen(filename,"rb");
    if(fp == NULL){
        return false;
    }
    Labyrinth temp ={0};
    int row = 0;
    int col = 0;
    int ch;
    while((ch = fgetc(fp)) != EOF){
        //处理windows换行符\r并统一为\n
        if(ch == '\r'){
            int next = fgetc(fp);
            if(next != '\n'){
                fclose(fp);
                return false;
            }
            ch = '\n';
        }
        if(ch == '\n'){
            //不接受空行
            if(col == 0){
                fclose(fp);
                return false;
            }
            if(row == 0){
                temp.cols = col;
            }else if(col != temp.cols){
                fclose(fp);
                return false;
            }

            row++;
            col = 0;
            continue;
        }
        // 只接受墙、空地和玩家字符。
        bool valid = ch == '#' || ch == '.' ||
                     (ch >= '0' && ch <= '9');

        if (!valid) {
            fclose(fp);
            return false;
        }

        if(row >= MAX_ROWS || col >= MAX_COLS){
            fclose(fp);
            return false;
        }
        temp.map[row][col] = (char)ch;
        col++;

    }
    if(ferror(fp)){
        fclose(fp);
        return false;
    }

    if(col > 0){
        if(row == 0){
            temp.cols = col;

        }else if(col != temp.cols){
            fclose(fp);
            return false;
        }

        row++;
    }
    if(row == 0){
        fclose(fp);
        return false;
    }
    temp.rows = row;
    *labyrinth = temp;
    return true;
}

Position findPlayer(Labyrinth *labyrinth, char playerId) {
    // TODO: Implement this function
    for(int i=0;i<labyrinth->rows;i++){
        for(int j=0;j<labyrinth->cols;j++){
            if(labyrinth->map[i][j] == playerId){
                Position pos = {i,j};
                return pos;
            }
        }
    }
    Position pos = {-1, -1};
    return pos;
}

Position findFirstEmptySpace(Labyrinth *labyrinth) {
    // TODO: Implement this function
    for(int row=0;row<labyrinth->rows;row++){
        for(int col=0;col<labyrinth->cols;col++){
            if(labyrinth->map[row][col] == '.'){
                Position pos = {row,col};
                return pos;
            }
        }
    }
    Position pos = {-1, -1};
    return pos;
}

bool isEmptySpace(Labyrinth *labyrinth, int row, int col) {
    // TODO: Implement this function
    return row >= 0 && row < labyrinth->rows && col >= 0 && col < labyrinth->cols && labyrinth->map[row][col] == '.';
}

bool movePlayer(Labyrinth *labyrinth, char playerId, const char *direction) {
    // TODO: Implement this function
    if(!isValidPlayer(playerId) || direction == NULL){
        return false;
    }
    int dr = 0;   //控制上下
    int dc = 0;   //控制左右
    if (strcmp(direction, "up") == 0) {
        dr = -1;
    } else if (strcmp(direction, "down") == 0) {
        dr = 1;
    } else if (strcmp(direction, "left") == 0) {
        dc = -1;
    } else if (strcmp(direction, "right") == 0) {
        dc = 1;
    } else {
        return false;
    }

    Position pos = findPlayer(labyrinth,playerId);
    // 玩家不存在：放置玩家，本次操作到此结束。
    if(pos.row == -1){
        Position empty = findFirstEmptySpace(labyrinth);

        if(empty.row == -1){
            return false;
        }

        labyrinth->map[empty.row][empty.col] = playerId;
        return true;
    }
    int nextRow = pos.row + dr;
    int nextCol = pos.col + dc;

    if (!isEmptySpace(labyrinth, nextRow, nextCol)) {
        return false;
    }
    labyrinth->map[pos.row][pos.col] = '.';
    labyrinth->map[nextRow][nextCol] = playerId;

    return true;
}

bool saveMap(Labyrinth *labyrinth, const char *filename) {
    // TODO: Implement this function
    FILE *fp = fopen(filename,"wb");
    if(fp == NULL){
        return false;
    }
    bool success = true;

    for (int row = 0; row < labyrinth->rows; row++) {
        size_t count = fwrite(
            labyrinth->map[row],
            sizeof(char),
            (size_t)labyrinth->cols,
            fp
        );

        if (count != (size_t)labyrinth->cols) {
            success = false;
            break;
        }

        if (fputc('\n', fp) == EOF) {
            success = false;
            break;
        }
    }

    if (fclose(fp) != 0) {
        success = false;
    }

    return success;
}

//添加一个辅助函数，判断某格是否可通行
static bool isWalkable(Labyrinth *labyrinth,int row,int col){
    if(row < 0 || row > labyrinth->rows || col < 0 || col > labyrinth->cols){
        return false;
    }
    char cell = labyrinth->map[row][col];
    return cell == '.' || isValidPlayer(cell);

}

// Check if all empty spaces are connected using DFS
void dfs(Labyrinth *labyrinth, int row, int col, bool visited[MAX_ROWS][MAX_COLS]) {
    // TODO: Implement this function
    if(!isWalkable(labyrinth,row,col) || visited[row][col]){
        return;
    }

    Position stack[MAX_COLS*MAX_ROWS];
    int top = 0;
    const int dr[4] = {-1,1,0,0};
    const int dc[4] = {0,0,-1,1};

    visited[row][col] = true;
    stack[top++] = (Position){row,col};

    while(top > 0){
        Position current = stack[--top];
        for(int i=0;i<4;i++){
            int nextRow = current.row + dr[i];
            int nextCol = current.col + dc[i];
            if(visited[nextRow][nextCol] || !isWalkable(labyrinth, nextRow, nextCol)){
                continue;
            }
            visited[nextRow][nextCol] = true;
            stack[top++] = (Position){nextRow, nextCol};
        }
    }

}

bool isConnected(Labyrinth *labyrinth) {
    // TODO: Implement this function
    bool visited[MAX_ROWS][MAX_COLS] = {false};
    Position start = {-1,-1};
    //找到一个Walkable格子作为起点
    for (int row = 0; row < labyrinth->rows; row++) {
        for (int col = 0; col < labyrinth->cols; col++) {
            if (isWalkable(labyrinth, row, col)) {
                start = (Position){row, col};
                break;
            }
        }

        if (start.row != -1) {
            break;
        }
    }
    if (start.row == -1) {
        return false;
    }

    dfs(labyrinth,start.row,start.col,visited);
    for (int row = 0; row < labyrinth->rows; row++) {
        for (int col = 0; col < labyrinth->cols; col++) {
            if (isWalkable(labyrinth, row, col) &&
                !visited[row][col]) {
                return false;
            }
        }
    }

    return true;
}

static bool printOriginalMap(const char *filename) {
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL) {
        return false;
    }

    unsigned char buffer[4096];
    size_t count;
    bool success = true;

    while ((count = fread(buffer, 1, sizeof(buffer), fp)) > 0) {
        if (fwrite(buffer, 1, count, stdout) != count) {
            success = false;
            break;
        }
    }

    if (ferror(fp)) {
        success = false;
    }

    if (fclose(fp) != 0) {
        success = false;
    }

    if (fflush(stdout) != 0) {
        success = false;
    }

    return success;
}
