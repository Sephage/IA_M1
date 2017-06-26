/*
Simple programme d'affichage de points et de segments en opengl
utilise GL et glut
*/



#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include <GL/glut.h>
#include <time.h>

#include <unistd.h>
#include <float.h>
#include <stdint.h>

#include "base_opengl.h"
#include "kohonen.h"

int cpt=0; 
int anglex, angley, xold, yold, x, y;
char presse,pau;
int nb_input = 10;

/* affiche la chaine fmt a partir des coordonn�es x,y*/
void draw_text(float x, float y, const char *fmt, ...)
{
    char		buf[1024];			//Holds Our String
    char		*text = buf;
    va_list		ap;				// Pointer To List Of Arguments

    if (fmt == NULL)				// If There's No Text
    return;					// Do Nothing

    va_start(ap, fmt);				// Parses The String For Variables
    vsprintf(text, fmt, ap);			// And Converts Symbols To Actual Numbers
    va_end(ap);					// Results Are Stored In Text

    glPushMatrix();
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    glRasterPos2f(x, y);
    while (*text)
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, *text++);

    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

/* *************************************************** */
/* main */

int main(int argc,char **argv) {
    int c; 
    /*int rand = 0;

    int errflg = 0;
    extern char* optarg; 
    extern int optind, opterr; 
    char* progname = argv[0];

    while ((c = getopt(argc , argv, "n:r")) != -1){
        switch (c) {
            case 'r':
                rand = 1;
                break;
            case 'n':
                nb_input = atoi(optarg);
                break;
            case '?':
                errflg = 1;
                break;
        }
    }
    if (errflg)
        //use(progname);

    /*if(!rand){
        nb_input = init_input(&input, "input.sav");
        for(c=0 ; c < nb_input ; c++){
            input[c].x = input[c].x/WIDTH;
            input[c].y = input[c].y/HEIGHT;
            printf("(%f;%f)\n", input[c].x, input[c].y);
        }
    }
    else{*/
    srand48(time(NULL));
    srand(time(NULL));
    input = malloc(nb_input * sizeof(Coordinate));
    for(c=0 ; c<nb_input ; c++){
        input[c].x = 2*drand48() - 1;
        input[c].y = 2*drand48() - 1;
    }
    // }
 


    // init kohonen
    srand(time(NULL));
    //init_dataset();
    init_neurons();

    /* initilisation de glut et creation de la fenetre */
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowPosition(200,200);
    glutInitWindowSize(530,530);
    glutCreateWindow("Kohonen");

    /* initialisation OpenGL */
    glClearColor(0,0,0,0);
    glColor3f(1,1,1);
    glPointSize(4);
    glEnable(GL_DEPTH_TEST);

    /* enregistrement des fonctions de rappel */
    glutDisplayFunc(affichage);
    glutKeyboardFunc(clavier);
    glutReshapeFunc(reshape);
    glutIdleFunc(idle);
    glutMouseFunc(mouse);
    glutMotionFunc(mousemotion);

    /* Entree dans la boucle principale */
    glutMainLoop();
    return 0;
}


/* *************************************************** */

void affichage() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();
    glRotatef(-angley,1,0,0);
    glRotatef(-anglex,0,1,0);

    glColor3f(1.0,1.0,1.0);
    draw_text(-0.9,0.9,"nb iter: %d",cpt);


    glBegin(GL_POINTS);
    glColor3f(0.0,1.0,0.0);
    for (size_t i = 0; i < nb_input; i++) {
        Coordinate coord = input[i];
        glVertex2f(coord.x, coord.y);
    }
    // neurons
    glColor3f(1.0,0.0,0.0);
    for (size_t i = 0; i < NB_NEURONS; i++) {
        Coordinate neur = neurons[i];
        glVertex2f(neur.x, neur.y);
    }
    glEnd();


    glBegin(GL_LINE_LOOP);
    for (size_t i = 0; i < NB_NEURONS; i++) {
        Coordinate neur = neurons[i];
        glVertex2f(neur.x, neur.y);
    }
    glEnd();

    glFlush();


    /* On echange les buffers */
    glutSwapBuffers();
}


void idle(){
    int i;
    if(pau){
        int index;
        cpt++;
        index = rand()%nb_input;

        // simulate
        update_weight(&input[index]);

        // compare
        int winner_index;
        winner_index = compute_activity(weight_map);

        // update
        update_neurons(input[index], cpt, winner_index);

        // display
        if (cpt%1000 == 0) {
            glutPostRedisplay();
        }
    }
}


void clavier(unsigned char touche,int x,int y) {
    int i;
    switch(touche) {
        case 'p':
        pau = !pau;
        break;

        case 'q': 
        exit(0);
    } /* switch */

} /* clavier */




void reshape(int x,int y) {
    if (x<y)
    glViewport(0,(y-x)/2,x,x);
    else
    glViewport((x-y)/2,0,y,y);
} /* reshape */



void mouse(int bouton,int etat,int x,int y) {
    if (bouton==GLUT_LEFT_BUTTON && etat ==GLUT_DOWN) {
        presse=1; 
        xold=x;   
        yold=y;
    }

    if (bouton==GLUT_LEFT_BUTTON && etat ==GLUT_UP) {
        presse=0; 
    }
} /* mouse */



void mousemotion(int x,int y) {
    if (presse) { /* si le bouton gauche est presse */
        /* on mofifie les angles de rotation de l'objet en fonction de la position actuelle de la souris et de la derniere position sauvegard?e */
        anglex=anglex+(x-xold);
        angley=angley+(y-yold);
        glutPostRedisplay();
    }

    xold=x; 
    yold=y;
} /* mousemotion */
