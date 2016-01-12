#pragma once

void creatMaineMenu(void);
void menu(int value);
void destroyMenu(void);
void createItemMenu(void);

void startApp(int argc, char **argv);

void idle();
void redraw();
void drawCotents(int mode);
void updateView(int width, int height);
void reshape(int width, int height);
void drawRectangle(int x, int y, int width, int length);

void key(unsigned char k, int x, int y);

void list_hits(int hits, unsigned int *names);
void mouseClick(int button, int state, int x, int y);
void motion(int x, int y);

void draw_block(float x, float y, float z);

void drawText(const char* str, int x, int y);
void cvInit();
void updateTransformArray(int mode, bool pos,int count, float* p);
void capture_thread();