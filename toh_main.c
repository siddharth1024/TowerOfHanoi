#include <stdio.h>
#include <conio.h>
#include <malloc.h>
#include <stdlib.h>
#include <graphics.h>
#include <dos.h>
#include <string.h>
#define disks 5
#define peg struct stack

typedef peg {
    char name;
    signed int top;
    char a[disks];
};

/*GLOBAL VARIABLES */
int r, property, pointer, debug, moves;
int origin_x, origin_y, spacing_factor, disk_color;
int *state, current_state;

/*VIEWPORT SETTINGS */
int peg_height, peg_width, spacing, peg_startx, peg_starty;

/*GRAPHICS FUNCTIONS */
void setup_graphics();
void create_rectangle(int x, int y, int w, int h);
void create_peg(int x, int y, int w, int h, char *name);
void create_disk(int size, int _peg, int floor);
void setup_disks();
void view_state(int n);
void wipe_state();

//Stack functions
void push(peg *s, char x);
char pop(peg *s);
char top(peg *s);
void display(peg *s);
int *push_state(int *state, int r, int c);

//Array functions
int array_compare(char *a, char *b);
char *array_copy(peg *p);
//tower of hanoi functions
void move_disc(peg *src, peg *dest);
void solve_tower(peg *peg1, peg *peg2, peg *peg3, int *state);
void save_state(peg *peg1, peg *peg2, peg *peg3, int *state);

//Math functions
int pow(int base, int exponent);

//Intro functions
void animate_text(char *text, int delay, int tabs);

int main(void) {
    char welcome_text[] = {
        "*****************************"
    };
    char intro[] = {
        "Tower of Hanoi Using Graphics"
    };
    char name1[] = {
        "Siddharth Goswami"
    };
    char inst1[] = {
        "'<' for previous state"
    };
    char inst2[] = {
        "'>' for next state"
    };
    char inst3[] = {
        "'x' to exit"
    };
    char msg[80];
    int i, j, count = 0, pointer = 0;
    peg *peg1, *peg2, *peg3;

    /*GRAPHICS SETTINGS */
    current_state = 0;
    peg_height = 100;
    peg_width = 120;
    spacing = 60;
    peg_startx = spacing;
    peg_starty = peg_height *2;

    debug = 0;
    moves = pow(2, disks) - 1;

    clrscr();

    /*INTRO SETTINGS */
    animate_text(welcome_text, 10, 0);
    animate_text(intro, 10, 0);
    animate_text(name1, 10, 3);

    getche();

    r = moves *disks;
    property = 4;

    pointer = pointer; //just to ignore 'never used' warning

    peg1 = (peg *) calloc(2, sizeof(peg));
    peg1->name = '1';
    peg2 = (peg *) calloc(2, sizeof(peg));
    peg2->name = '2';
    peg3 = (peg *) calloc(2, sizeof(peg));
    peg3->name = '3';
    state = (int *) calloc(10, r *property *sizeof(int));

    peg1->top = peg2->top = peg3->top = -1;

    for (i = 0; i < r; i++) {
        for (j = 0; j < property; j++) { *(state + i *property + j) = ++count;
        }
    }

    //push disks to first peg
    for (i = 0; i < disks; i++) push(peg1, disks - i - 1 + 'A');
    setup_graphics();
    setbkcolor(0);
    origin_x = peg_startx - 120;
    origin_y = peg_starty;
    spacing_factor = peg_width + spacing;
    create_peg(peg_startx, peg_starty, peg_width, peg_height, "PEG A");
    create_peg(peg_startx + peg_width *1 + spacing *1, peg_starty, peg_width, peg_height, "PEG B");
    create_peg(peg_startx + peg_width *2 + spacing *2, peg_starty, peg_width, peg_height, "PEG C");
    disk_color = 5;

    save_state(peg1, peg2, peg3, state);
    solve_tower(peg1, peg2, peg3, state);
    save_state(peg1, peg2, peg3, state);

    sprintf(msg, inst1);
    outtextxy(16, getmaxy() - 8 *3, msg);
    sprintf(msg, inst2);
    outtextxy(16, getmaxy() - 8 *2, msg);
    sprintf(msg, inst3);
    outtextxy(16, getmaxy() - 8 *1, msg);

    if (debug) {
        for (i = 0; i < r + 3; i++) {
            for (j = 0; j < property; j++) {
                printf("%d ", *(state + i *property + j));
            }
            printf("n");
        }
    }

    view_state(current_state);

    while (1) {
        switch (getche()) {
        case ',':
        case '<':
            current_state--;
            current_state = current_state % moves;
            if (current_state < 0) current_state = moves + current_state + 1;
            wipe_state();
            break;
        case '.':
        case '>':
            current_state++;
            wipe_state();
            break;
        case 'x':
        case 'X':
            exit(1);
            break;
        }
    }
}

void push(struct stack *s, char x) {
    if (s->top == disks - 1) printf("\nSTACK OVERFLOW");
    else if (x == 0 || x == '0') printf("\nNothing to push");
    else s->a[++s->top] = x;
}

char pop(struct stack *s) {
    char x;
    if (s->top == -1) return 'Z'; //UNDERFLOW
    return s->a[s->top--];
}

void display(peg *s) {
    int i;
    printf("n[%c]", s->name);
    for (i = 0; i <= s->top; i++) printf("%c", s->a[i]);
    printf("---[%c]", s->name);
}

char top(peg *s) {
    return (s->top == -1) ? 127 : s->a[s->top];
}

void move_disc(peg *x, peg *y) {
    //compare size of disk and perform legal move
    if (top(x) < top(y)) {
        //        printf("nmove disk %c from %c to %c",top(x),y->name,x->name);
        push(y, pop(x));
    } else {
        //        printf("nmove disk %c from %c to %c",top(y),y->name,x->name);
        push(x, pop(y));
    }
}

int array_compare(char *a, char *b) {
    int same = 1, i;
    for (i = 0; i < disks; i++)
        if (a[i] != b[i]) same = 0;
    return same;
}

char *array_copy(peg *p) {
    int i;
    char *ra = (char *) malloc(disks);
    for (i = 0; i < disks; i++) ra[i] = p->a[i];
    return ra;
}

void solve_tower(peg *_1, peg *_2, peg *_3, int *state) {
    //save state of 1st peg into solution
    char *solution = array_copy(_1);

    /*Solver Logic : 
    1.) Make legal move between 1 and 3
    2.) Make legal move between 1 and 2
    3.) Make legal move between 2 and 3
    4.) Repeat until complete
    if number of disks is even then replace step 1 with step 2.
    */
    while (1) {
        move_disc(_1, disks % 2 ? _3 : _2);
        save_state(_1, _2, _3, state);
        if (array_compare(_3->a, solution)) return;

        move_disc(_1, disks % 2 ? _2 : _3);
        save_state(_1, _2, _3, state);
        if (array_compare(_3->a, solution)) return;

        move_disc(_2, _3);
        save_state(_1, _2, _3, state);
        if (array_compare(_3->a, solution)) return;
    }
}
void save_state(peg *peg1, peg *peg2, peg *peg3, int *state) {
    int i;
    static int d = 0;
    if (peg1->top >= 0) {
        for (i = 0; i <= peg1->top; i++) { 
        	*push_state(state, pointer, 0) = d; //state
        	*push_state(state, pointer, 1) = 1; //peg
        	*push_state(state, pointer, 2) = peg1->a[i]; //disk
        	*push_state(state, pointer++, 3) = i; //floor
        }
    }
    if (peg2->top >= 0) {
        for (i = 0; i <= peg2->top; i++) { 
        	*push_state(state, pointer, 0) = d; 
        	*push_state(state, pointer, 1) = 2; 
        	*push_state(state, pointer, 2) = peg2->a[i]; 
        	*push_state(state, pointer++, 3) = i;
        }
    }
    if (peg3->top >= 0) {
        for (i = 0; i <= peg3->top; i++) { 
        	*push_state(state, pointer, 0) = d; 
        	*push_state(state, pointer, 1) = 3; 
        	*push_state(state, pointer, 2) = peg3->a[i]; 
        	*push_state(state, pointer++, 3) = i;
        }
    }
    d++;
}

int pow(int base, int exponent) {
    int ans = 1;
    while (exponent--) ans = ans *base;
    return ans;
}

int *push_state(int *state, int r, int c) {
    return (state + r *property + c);
}

/*GRAPHICS FUNCTIONS */
void setup_graphics() {
    int gdriver = DETECT, gmode, errorcode;
    initgraph( & gdriver, & gmode, "C:\\TC\\BGI\\");
    cleardevice();
    errorcode = graphresult();
    if (errorcode != grOk) /*an error occurred */ {
	printf("Graphics error: %sn", grapherrormsg(errorcode));
	printf("Press any key to halt:");
	getch();
	exit(1);
    }
}

void create_rectangle(int x, int y, int w, int h) {
    bar(x, y, x + w, y + h);
}

void create_peg(int x, int y, int w, int h, char *name) {
    char msg[80];
    int stroke = 10;
    setfillstyle(SOLID_FILL, BLUE);
    create_rectangle(x, y, w, stroke); //horizontal
    setfillstyle(SOLID_FILL, 8);
    create_rectangle(x + w / 2 - stroke / 4, y - h - 1, stroke / 2, h); //vertical
    sprintf(msg, name);
    outtextxy(x + w / 2 - stroke / 4 - sizeof(name) *8, y + 2, msg);
}

void create_disk(int size, int _peg, int floor) {
    char msg[20];
    char name[4] = {
	'@'
    };
    int height = 20;
    int width = (size + 1) *20;
    setfillstyle(SOLID_FILL, size + disk_color);
    name[0] += size;
    create_rectangle
	(
	    origin_x + _peg *spacing_factor - width / 2,
	    origin_y - (floor *height) - height - 1,
	    width,
	    height
	);
    setcolor(0);
    sprintf(msg, name);
    outtextxy(0 + spacing_factor *_peg - 64,
	(origin_y - floor *height) - height - 1 + 8, msg);
}

void wipe_state() {
    setfillstyle(SOLID_FILL, getbkcolor());
    setcolor(WHITE);
    create_rectangle(0, 0, getmaxx(), 400);
    create_peg(peg_startx, peg_starty, peg_width, peg_height, "PEG A");
    create_peg(peg_startx + peg_width *1 + spacing *1, peg_starty, peg_width, peg_height, "PEG B");
    create_peg(peg_startx + peg_width *2 + spacing *2, peg_starty, peg_width, peg_height, "PEG C");
    view_state(current_state);
}

void setup_disks() {
    int i;
    for (i = 1; i <= disks; i++) {
	create_disk(i, 3, disks - i);
    }
}

void view_state(int n) {
    int i, d, p, f;
    char msg[30];
    char name[] = {
	"CURRENT STATE :   / "
    };
    name[16] = (char)(48 + n / 10);
    name[17] = (char)(48 + n % 10);
    name[19] = (char)(48 + moves / 10);
    name[20] = (char)(48 + moves % 10);
    n = n % (moves + 1);
    sprintf(msg, name);
    outtextxy(8, 8, msg);
    for (i = 0; i < disks; i++) {
	d = *push_state(state, n *disks + i, 2) % 64;
	p = *push_state(state, n *disks + i, 1);
	f = *push_state(state, n *disks + i, 3);
	create_disk(d, p, f);
    }
}

void animate_text(char *text, int d, int tabs) {
    int i;
    for (i = 0; i < tabs; i++) printf("%c", 9);
    for (i = 0; i < strlen(text); i++) {
	printf("%c", text[i]);
	delay(d);
    }
    printf("\n\n\t\t");
}
