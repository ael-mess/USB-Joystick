#ifndef _PACMAN_H_
#define _PACMAN_H_

    /***********
    * INCLUDES *
    ***********/
    #include <stdio.h>
    #include <curses.h>
    #include <string.h>
    #include <stdlib.h>
    #include <time.h>
    #include <unistd.h>
    #include <sys/timeb.h>
    #include <poll.h>


    #define EXIT_MSG "Good bye!"
    #define END_MSG "Game Over"
    #define QUIT_MSG "Bye"
    #define LEVEL_ERR "Cannot find level file: "
    #define LEVEL_WIDTH 28
    #define LEVEL_HEIGHT 29

    /*************
    * PROTOTYPES *
    *************/
    void IntroScreen();                                     //Show introduction screen and menu
    void CheckCollision();                                  //See if Pacman and Ghosts collided
    void CheckScreenSize();                                 //Make sure resolution is at least 32x29
    void CreateWindows(int y, int x, int y0, int x0);       //Make ncurses windows
    void Delay();                                           //Slow down game for better control
    void DrawWindow();                                      //Refresh display
    void ExitProgram(const char *message);                  //Exit and display something
    void GetInput();                                        //Get user input
    void InitCurses();                                      //Start up ncurses
    void LoadLevel(char *levelfile);                        //Load level into memory
    void MainLoop();                                        //Main program function
    void MoveGhosts();                                      //Update Ghosts' location
    void MovePacman();                                      //Update Pacman's location
    void PauseGame();                                       //Pause
    void main_pac_man(int argc, char *argv[100]);            //main function




    #ifndef DATAROOTDIR
    #define DATAROOTDIR ""
    #endif
    #define LEVELS_FILE DATAROOTDIR "pacman-1.3/Levels/level__.dat"

#endif
