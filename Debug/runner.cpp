#define _CRT_SECURE_NO_WARNINGS


#include <iostream>
#include <GL/glut.h>
#include <math.h>
#include "loadStuff.h"
#include <sstream>
#include <windows.h>
#include <mmsystem.h>


#include <fstream>
#include <conio.h>

using namespace std;



/*
egad this is ugly code. sorry for that. also, this 
game is partly derived from Gustav Taxén's particle system
explosion demo, Mila's awesome texture map stuff, 
Andrey Mirtchovski's starfield tutorial, and Jeff Minter's
work on t2k and t3k (concept and sfx). 

cheers

dh

*/




typedef struct _obj {
	float x, y, z;
	float vx, vy, vz;
	float r, g, b;				
	int rotx, roty, rotz;		
	int drotx, droty, drotz;	
	int type;	
} obj;



        

struct particleData
{
	float   position[3];
	float   speed[3];
	float   color[3];
};
typedef struct particleData    particleData;



struct debrisData
{
	float   position[3];
	float   speed[3];
	float   orientation[3];        
	float   orientationSpeed[3];
	float   color[3];
	float   scale[3];
};
typedef struct debrisData    debrisData;


float splodeX, splodeY, splodeZ;


int triangles = 0;
int showTitle = 1;
int dead = 0;
float scale = 0;
int currentScore = 0;
int highScore = 0;
int startTime = 0;
int getStartTime = 0;
int newHighScore = 0;
int autoshot = 0;



GLfloat  light0Amb[4] = { 1.0, 0.6, 0.2, 1.0 };
GLfloat  light0Dif[4] = { 1.0, 0.6, 0.2, 1.0 };
GLfloat  light0Spec[4] = { 0.0, 0.0, 0.0, 1.0 };
GLfloat  light0Pos[4] = { 0.0, 0.0, 0.0, 1.0 };

GLfloat  light1Amb[4] = { 0.0, 0.0, 0.0, 1.0 };
GLfloat  light1Dif[4] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat  light1Spec[4] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat  light1Pos[4] = { 0.0, 5.0, 5.0, 0.0 };

GLfloat  materialAmb[4] = { 0.25, 0.22, 0.26, 1.0 };
GLfloat  materialDif[4] = { 0.63, 0.57, 0.60, 1.0 };
GLfloat  materialSpec[4] = { 0.99, 0.91, 0.81, 1.0 };
GLfloat  materialShininess = 27.8;

#define NUM_PARTICLES    1000         
#define NUM_DEBRIS       1000  
particleData     particles[NUM_PARTICLES];
debrisData       debris[NUM_DEBRIS];
int              fuel = 0;                

int      wantNormalize = 0;

static loadStuff objToDraw;
static loadStuff teddy;
static loadStuff bunny;


string curMusic = "tres.wav";

string musicArr[5] = { "MindEye.wav", "tres.wav", "ease.wav", "conDem.wav", "ultrayak.wav" };

string currentf;
string fname1 = "texture.ppm";
string fname4 = "texture4.ppm";
string fname5 = "texture5.ppm";

obj shots[20];
int numMissiles = 1;
//int missileArrSize = 5;
//int activeMissile[1];
obj missiles[1];
obj *stars = NULL;
int numStars = 250;
int winWidth = 800;
int winHeight = 800;
int shotFlag = 0;
float mouseMovedX, mouseMovedY;
float lastX, lastY = 0;
int numShots = 0;
int lastTime = 0;
int numSplodes = 0;
double const pi = 3.1415926;
float rotX = 0;
float rotY = 0;
int buttonDown = 0;
int framerate = 7;


int roll(int min, int max)
{
	// x is in [0,1[
	double x = rand() / static_cast<double>(RAND_MAX + 1);

	// [0,1[ * (max - min) + min is in [min,max[
	int that = min + static_cast<int>(x * (max - min));

	return that;
}

void newSpeed(float dest[3])
{
	float    x;
	float    y;
	float    z;
	float    len;

	x = (2.0 * ((GLfloat)rand()) / ((GLfloat)RAND_MAX)) - 1.0;
	y = (2.0 * ((GLfloat)rand()) / ((GLfloat)RAND_MAX)) - 1.0;
	z = (2.0 * ((GLfloat)rand()) / ((GLfloat)RAND_MAX)) - 1.0;

	/*
	* Normalizing the speed vectors gives a "fireball" effect
	*
	*/

	//if (wantNormalize)
	//{
		len = sqrt(x * x + y * y + z * z);

		if (len)
		{
			x = x / len;
			y = y / len;
			z = z / len;
		}
	//}

	dest[0] = x;
	dest[1] = y;
	dest[2] = z;
}



void newExplosion(float x, float y, float z, int ring)
{
	int    i;

	for (i = 0; i < NUM_PARTICLES; i++)
	{
		particles[i].position[0] = x;// 0.0;
		particles[i].position[1] = y;// 0.0;
		particles[i].position[2] = z;// 0.0;

		//particles[i].color[0] = rand() % 10 + 0.1;
		//particles[i].color[1] = rand() % 10 + 0.1;
		//particles[i].color[2] = rand() % 10 + 0.1;

		int randy = roll(0, 2);

		//red
		if (randy == 0) {
			particles[i].color[0] = 1.0;
			particles[i].color[1] = 0;
			particles[i].color[2] = 0;
		}


		//orange
		if (randy == 1) {
			particles[i].color[0] = 1.0;
			particles[i].color[1] = 0.6;
			particles[i].color[2] = 0;
		}


		//yellow
		if (randy == 2) {
			particles[i].color[0] = 1.0;
			particles[i].color[1] = 1.0;
			particles[i].color[2] = 0;
		}


		newSpeed(particles[i].speed);
	}

	for (i = 0; i < NUM_DEBRIS; i++)
	{
		debris[i].position[0] = x;// 0.0;
		debris[i].position[1] = y;// 0.0;
		debris[i].position[2] = z;// 0.0;

		debris[i].orientation[0] = 0.0;
		debris[i].orientation[1] = 0.0;
		debris[i].orientation[2] = 0.0;

		int randy = roll(0, 2);

		//red
		if (randy == 0) {
			debris[i].color[0] = 1.0;
			debris[i].color[1] = 0;
			debris[i].color[2] = 0;
		}


		//orange
		if (randy == 1) {
			debris[i].color[0] = 1.0;
			debris[i].color[1] = 0.6;
			debris[i].color[2] = 0;
		}


		//yellow
		if (randy == 2) {
			debris[i].color[0] = 1.0;
			debris[i].color[1] = 1.0;
			debris[i].color[2] = 0;
		}

		debris[i].scale[0] = (2.0 *
			((GLfloat)rand()) / ((GLfloat)RAND_MAX)) - 1.0;
		debris[i].scale[1] = (2.0 *
			((GLfloat)rand()) / ((GLfloat)RAND_MAX)) - 1.0;
		debris[i].scale[2] = (2.0 *
			((GLfloat)rand()) / ((GLfloat)RAND_MAX)) - 1.0;

		newSpeed(debris[i].speed);
		newSpeed(debris[i].orientationSpeed);
	}

	fuel = 100;
	//t2kboom.play();
	//PlaySound(TEXT("t2kboom.wav"), NULL, SND_RESOURCE);

	int randy = roll(0, 2);

	if (ring == 0) {

		if (framerate > 1) {
			if (randy > 0) {
				mciSendString("play boomt2k.wav", NULL, 0, NULL);
			}
			else {
				mciSendString("play explosion.wav", NULL, 0, NULL);
			}
		}
		else {
			if (randy > 0) {
				mciSendString("play flossie.wav", NULL, 0, NULL);
			}
			else {
				mciSendString("play moo_cow.wav", NULL, 0, NULL);
			}
		}






	}

	if (ring == 1) {
		mciSendString("play hit.wav", NULL, 0, NULL);
	}

	if (ring == 2) {
		mciSendString("play thunder.wav", NULL, 0, NULL);
	}
	
	
}




unsigned char* loadPPM(const char* filename, int& width, int& height)
{
	const int BUFSIZE = 128;
	FILE* fp;
	unsigned int read;
	unsigned char* rawData;
	char buf[3][BUFSIZE];
	char* retval_fgets;
	size_t retval_sscanf;

	if ((fp = fopen(filename, "rb")) == NULL)
	{
		std::cerr << "error reading ppm file, could not locate " << filename << std::endl;
		width = 0;
		height = 0;
		return NULL;
	}

	// Read magic number:
	retval_fgets = fgets(buf[0], BUFSIZE, fp);

	// Read width and height:
	do
	{
		retval_fgets = fgets(buf[0], BUFSIZE, fp);
	} while (buf[0][0] == '#');
	retval_sscanf = sscanf(buf[0], "%s %s", buf[1], buf[2]);
	width = atoi(buf[1]);
	height = atoi(buf[2]);

	// Read maxval:
	do
	{
		retval_fgets = fgets(buf[0], BUFSIZE, fp);
	} while (buf[0][0] == '#');

	// Read image data:
	rawData = new unsigned char[width * height * 3];
	read = fread(rawData, width * height * 3, 1, fp);
	fclose(fp);
	if (read != 1)
	{
		std::cerr << "error parsing ppm file, incomplete data" << std::endl;
		delete[] rawData;
		width = 0;
		height = 0;
		return NULL;
	}

	return rawData;
}



void loadTexture(string filename)
{
	GLuint texture[1];     // storage for one texture
	int twidth, theight;   // texture width/height [pixels]
	unsigned char* tdata;  // texture pixel data

	// Load image file
	tdata = loadPPM(filename.c_str(), twidth, theight);
	
	if (tdata == NULL) {
		cout << "fuuccckkk" << endl;
		return;
	}

	// Create ID for texture
	glGenTextures(1, &texture[0]);

	// Set this texture to be the one we are working with
	glBindTexture(GL_TEXTURE_2D, texture[0]);

	// Generate the texture
	glTexImage2D(GL_TEXTURE_2D, 0, 3, twidth, theight, 0, GL_RGB, GL_UNSIGNED_BYTE, tdata);

	// Set bi-linear filtering for both minification and magnification
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void drawSphere(float radius, int slices, int stacks) {

	float x, y, z;
	float u, v;
	glColor3d(1, 1, 1);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);

	for (float latitude = 0; latitude < pi; latitude = latitude + pi / stacks) {
		glBegin(GL_QUAD_STRIP);

		for (float longitude = 0; longitude < 2 * pi; longitude = longitude + 2 * pi / slices) {
			u = latitude / pi;
			v = longitude / (2 * pi);

			x = radius*cosf(latitude) * sinf(longitude);
			y = radius*sinf(latitude) * sinf(longitude);
			z = -radius*cosf(longitude);

			glNormal3f(x, y, z);
			glTexCoord2d(u, v); glVertex3f(x, y, z);

			x = radius*cosf(latitude + pi / stacks) * sinf(longitude);
			y = radius*sinf(latitude + pi / stacks) * sinf(longitude);
			z = -radius*cosf(longitude);

			glNormal3f(x, y, z);
			glTexCoord2d(u, v); glVertex3f(x, y, z);
		}

		glEnd();
	}


	glDisable(GL_TEXTURE_2D);
}


void initStuff() {

	//loadTexture(currentFilename);
	stars = (obj*)malloc(sizeof(obj)* 500);
	lastTime = glutGet(GLUT_ELAPSED_TIME);

	glClearColor(0, 0, 0, 0);
	glViewport(0, 0, winWidth, winHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1, 1, -1, 1, -1, 1000);
	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_DEPTH_TEST);


	for (int i = 0; i < 20; i++) {
		shots[i].z = 0.001;

		shots[i].vx = 0; 
		shots[i].vy = 0; 
		shots[i].vz = -0.008;

		shots[i].r = 0;
		shots[i].g = 1; 
		shots[i].b = 0;

	}

	for (int i = 0; i < numStars; i++) {
		stars[i].x = (float)rand() / (float)(RAND_MAX) -0.5;
		stars[i].y = (float)rand() / (float)(RAND_MAX) -0.5;
		stars[i].z = (float)rand() / (float)(RAND_MAX) -0.5;

		stars[i].vx = 0; 
		stars[i].vy = 0;
		stars[i].vz = (float)rand() / (float)(RAND_MAX * 500) - 1 / 250;

		stars[i].r = ((double)rand() / (RAND_MAX)) - 0.35;
		stars[i].g = 0; // ((double)rand() / (RAND_MAX));
		stars[i].b = ((double)rand() / (RAND_MAX)) - 0.2;

		//stars[i].r = ((double)rand() / (RAND_MAX));
		//stars[i].g = ((double)rand() / (RAND_MAX));
		//stars[i].b = ((double)rand() / (RAND_MAX));
	}


	for (int i = 0; i < 20; i++) {
		shots[i].z = 0.001;

		shots[i].vx = 0; 
		shots[i].vy = 0; 
		shots[i].vz = -0.008;

		shots[i].r = 0;
		shots[i].g = 1; 
		shots[i].b = 0;
	}



	for (int i = 0; i < numMissiles; i++) {
		/*

		missiles[i].x = (float)rand() / (float)(RAND_MAX)-0.5;
		missiles[i].y = (float)rand() / (float)(RAND_MAX)-0.5;
		missiles[i].z = (float)rand() / (float)(RAND_MAX)-0.5;

		missiles[i].vx = 0; // (float)rand()/(float)(RAND_MAX*500) - 1/250;
		missiles[i].vy = 0; // (float)rand()/(float)(RAND_MAX*500) - 1/250;
		missiles[i].vz = (float)rand() / (((float)RAND_MAX)*100.0);

		missiles[i].r = 0;
		missiles[i].g = 0; //(float)rand()/(float)(RAND_MAX) + sphere[i].vz;
		missiles[i].b = 1 - missiles[i].vz;//(float)rand()/(float)(RAND_MAX) + sphere[i].vz;

		missiles[i].rotx = rand() % 10 - 5;
		missiles[i].roty = rand() % 10 - 5;
		missiles[i].rotz = rand() % 10 - 5;

		missiles[i].drotx = rand() % 10 - 5;
		missiles[i].droty = rand() % 10 - 5;
		missiles[i].drotz = rand() % 10 - 5;

		missiles[i].type = rand() % 4;

		*/






		
		missiles[i].x = (float)rand() / (float)(RAND_MAX)-0.5;
		missiles[i].y = (float)rand() / (float)(RAND_MAX)-0.5;
		missiles[i].z = -3 * (2 + i);//(float)rand() / (float)(RAND_MAX)-0.5;
		missiles[i].vx = 0;
		missiles[i].vy = 0;
		missiles[i].vz = .005;// (float)rand() / (float)(RAND_MAX * 500) - 1 / 250;
		// (float)rand() / (((float)RAND_MAX)*100.0);

		missiles[i].r = 0;
		missiles[i].g = 0;
		missiles[i].b = 1 - missiles[i].vz;

		missiles[i].rotx = .001; //rand() % 10 - 5;
		missiles[i].roty = .001; //rand() % 10 - 5;
		missiles[i].rotz = .001; //rand() % 10 - 5;

		missiles[i].drotx = rand() % 10 - 5;
		missiles[i].droty = rand() % 10 - 5;
		missiles[i].drotz = rand() % 10 - 5;
		
		missiles[i].type = roll(0, 4);
		
	}


	glPointSize(2);

	//teddy.load("teddy.obj");
	//bunny.load("bunny.obj");
}


void initMissile() {

	missiles[0].x = (float)rand() / (float)(RAND_MAX)-0.5;
	missiles[0].y = (float)rand() / (float)(RAND_MAX)-0.5;
	missiles[0].z = -2;//-5.5;//(float)rand() / (float)(RAND_MAX)-0.5;
	missiles[0].vx = 0;
	missiles[0].vy = 0;
	missiles[0].vz = .005;// (float)rand() / (float)(RAND_MAX * 500) - 1 / 250;
	// (float)rand() / (((float)RAND_MAX)*100.0);



	missiles[0].rotx = .001; //rand() % 10 - 5;
	missiles[0].roty = .001; //rand() % 10 - 5;
	missiles[0].rotz = .001; //rand() % 10 - 5;

	missiles[0].drotx = rand() % 10 - 5;
	missiles[0].droty = rand() % 10 - 5;
	missiles[0].drotz = rand() % 10 - 5;

	missiles[0].type = roll(0, 4);

}





void drawRing() {
	GLfloat yang = 0.0;
	GLfloat xang = 0.0;
	int ang;
	int delang = 2;
	float y0 = 0.0;
	float y1 = 0.01;
	float r0 = 0.1;
	float x, z, xold, zold;


	glRotatef(90, 1.0, 0, 0.0);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glLoadIdentity();
	//glRotatef(yang, 1.0, 1.0, 0.0);
	//glTranslatef(-0.4, 0.25, 0.0);
	//glRotatef(xang, 0.0, 0.0, 1.0);
	glBegin(GL_QUAD_STRIP);
	//glColor3f(0.7, 0.7, 0.7);
	glVertex3f(r0, y0, 0.0);
	glVertex3f(r0, y1, 0.0);
	xold = r0;
	zold = 0.0;

	for (ang = delang; ang <= 360; ang += delang) {

		glColor3f(rand() % 10 + 0.1, rand() % 10 * 0.1, rand() % 10 * 0.1);
		x = r0*cos((double)ang*2.0*pi / 360.0);
		z = r0*sin((double)ang*2.0*pi / 360.0);
		glVertex3f(x, y0, z);
		glVertex3f(x, y1, z);
		glVertex3f(xold, y0, zold);
		glVertex3f(x, y0, z);
		glVertex3f(xold, y1, zold);
		glVertex3f(x, y1, z);
		xold = x;
		zold = z;
	}
	glEnd();

	//glutSwapBuffers();
	//glFlush();

}




void drawMissile() {

	int ang, i;
	int delang = 10;
	float r[9] = {0, .08, .04, .04, .06, .06, .04, .04, .02 };


	float y[9] = {0, 0, .04, .2, .22, .26, .28, .38, .4 };

	float x1, x2, z1, z2, x3, z3;


//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//	glLoadIdentity();
	glRotatef(rotY, 1.0, 0, 0);
	glRotatef(rotX, 0, 1.0, 1.0);

	for (i = 0; i < 8; i++)
	{


		glBegin(GL_QUAD_STRIP);
		for (ang = 0; ang <= 360; ang += delang)
		{
			x1 = r[i] * cos((double)ang*2.0*pi / 360.0);
			x2 = r[i + 1] * cos((double)ang*2.0*pi / 360.0);
			x3 = r[i] * cos((double)(ang + delang)*2.0*pi / 360.0);

			z1 = r[i] * sin((double)ang*2.0*pi / 360.0);
			z2 = r[i + 1] * sin((double)ang*2.0*pi / 360.0);
			z3 = r[i] * sin((double)(ang + delang)*2.0*pi / 360.0);

			glColor3f(rand() % 10 * 0.1, rand() % 10 * 0.1, rand() % 10 * 0.1);

			//glColor3f((1.0 + cos((double)ang*pi*2.0 / 360)) / 2.1,
			//	(1.0 + cos((double)ang*pi*2.0 / 360)) / 2.1,
			//	(1.0 + cos((double)ang*pi*2.0 / 360)) / 2.1);

			glVertex3f(x1, y[i], z1);
			//cout << "v1:: " << x1 << " " << y[i] << " " << z1 << endl;

			glVertex3f(x2, y[i + 1], z2);
			//cout << "v2:: " << x2 << " " << y[i + 1] << " " << z2 << endl;

			//glVertex3f(x1, y[i], z1);   
			//cout << "v3:: " << x1 << " " << y[i] << " " << z1 << endl;

			//glVertex3f(x3, y[i], z3);
			//cout << "v4:: " << x3 << " " << y[i] << " " << z3 << endl << endl << endl;
		};
		glEnd();




	}

	//glutSwapBuffers();
	//glFlush();


}


void reshapeStuff (int width, int height) {
	winWidth = width;
	winHeight = height;
	glClearColor(0, 0, 0, 0);
	glViewport(0, 0, winWidth, winHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1, 1, -1, 1, -1, 1000);
	glFrustum(-1, 1, -1, 1, 0.1, 2);
	glMatrixMode(GL_MODELVIEW);
}





void drawOBJ(loadStuff obj) {
	glScalef(0.1f, 0.1f, 0.1f);
	glBegin(GL_TRIANGLES);
	float *arrayVerticies = obj.get_verticies_pointer();
	float *arrayNormals = obj.get_normals_pointer();

	for (int i = 0; i <= obj.get_nVerts(); i++) {
		glNormal3f(arrayNormals[3 * i], arrayNormals[3 * i + 1], arrayNormals[3 * i + 2] );
		glVertex3f(arrayVerticies[3 * i], arrayVerticies[3 * i + 1], arrayVerticies[3 * i + 2] );

	}

	glEnd();
	
	
}




void displayStuff() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



	float scoreXPos = -4;
	glColor3f(1.0, 1.0, 0);

	char *scoreCard[] = { "SCORE: ", "HIGH SCORE: "};

	for (int i = 0; i < 2; i++) {
		glRasterPos3f(scoreXPos, 4, -.5);

		for (int k = 0; k < strlen(scoreCard[i]); k++) {
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, scoreCard[i][k]);
		}

		scoreXPos += 6;
		glRasterPos3f(scoreXPos, 4, -.5);

	}


	char curScoreArr[10];
	sprintf(curScoreArr, "%1i", currentScore);

	glRasterPos3f(-2.85, 4, -.5);
	for (int i = 0; i < 10; i++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, curScoreArr[i]);
	}
	

	glColor3f(((double)rand() / (RAND_MAX)), ((double)rand() / (RAND_MAX)), ((double)rand() / (RAND_MAX)));
	char slash[] = { "/x" };
	glRasterPos3f(0.05, 4, -.5);
	for (int i = 0; i < 10; i++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, slash[i]);
	}

	char curMultArr[10];
	sprintf(curMultArr, "%1i", abs(framerate - 8));

	glRasterPos3f(-.2, 4, -.5);
	for (int i = 0; i < 10; i++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, curMultArr[i]);
	}
	glColor3f(1, 1, 0);


	char curHighScoreArr[10];
	sprintf(curHighScoreArr, "%1i", highScore);

	glRasterPos3f(3.65, 4, -.5);
	for (int i = 0; i < 10; i++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, curHighScoreArr[i]);
	}


	
	char *crossHair[] = { "[  ]"};

	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	

	glColor3f(1, 1, 0);
	float crossX = -.20;

	if (dead == 0) {

		for (int k = 0; k < strlen(crossHair[0]); k++) {
			glRasterPos3f(crossX, 0, -.5);
			glutBitmapCharacter(GLUT_BITMAP_8_BY_13, crossHair[0][k]);
			crossX += .1;
		}

	}
	
	glTranslatef(-mouseMovedX, mouseMovedY, 0);


	if (showTitle == 1) {
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//glClearColor(0, 0, 0, 0);
		float titleXPos = -2.5 + (glutGet(GLUT_WINDOW_WIDTH) / 700) ;
		float titleYPos = 1;
		//glColor3f(((double)rand() / (RAND_MAX)), ((double)rand() / (RAND_MAX)), ((double)rand() / (RAND_MAX)));
		//glColor3f(1.0, 0.0, 0);

		char *titleCard[] = { "                      'S  P  L  O  D  E", 
							  " DAVID HOLIDAY CS596 FALL2014 FINAL ",
							  " ",
							  "  Use the mouse to shoot or dodge rocks. ",
							  "          Collect rings to multiply score. ",
							  " ",
							  "                   ~ left click to begin ~",
							};


		for (int i = 0; i < 7; i++) {
			//if (i == 6) {
				glColor3f(((double)rand() / (RAND_MAX)), ((double)rand() / (RAND_MAX)), ((double)rand() / (RAND_MAX)));
			//}

			glRasterPos3f(titleXPos, titleYPos, -.5);
			
			

			for (int k = 0; k < strlen(titleCard[i]); k++) {
				glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, titleCard[i][k]);			
			}

			titleYPos -= .3;


			glRasterPos3f(titleXPos, titleYPos, -.5);


		}


		//
		numMissiles = 0;

		//glutSwapBuffers();
		//return;
	}

	for (int i = 0; i < numStars; i++) {

		if (framerate == 1) {
			stars[i].r = ((double)rand() / (RAND_MAX));
			stars[i].g = ((double)rand() / (RAND_MAX));
			stars[i].b = ((double)rand() / (RAND_MAX));
		}

		glBegin(GL_LINE_STRIP);
		glColor3f(stars[i].r, stars[i].g, stars[i].b);
		glVertex3f(stars[i].x, stars[i].y, stars[i].z);
		glColor3f(0, 0, 0);		
		glVertex3f(stars[i].x, stars[i].y, stars[i].z - 20 * stars[i].vz);
		glEnd();
	}


	for (int r = 0; r < 20; r++) {

		if (shots[r].z > 0.0) {
			continue;
		}
		
		glLineWidth(6);
		glBegin(GL_LINE_STRIP);
		glColor3f(shots[r].r, shots[r].g, shots[r].b);
		glVertex3f(shots[r].x - 0.001, shots[r].y, shots[r].z);
		glColor3f(0, 0, 0.3);
		glVertex3f(shots[r].x - 0.001, shots[r].y, shots[r].z + shots[r].vz);
		glEnd();
		glBegin(GL_LINE_STRIP);
		glColor3f(shots[r].r, shots[r].g, shots[r].b);
		glVertex3f(shots[r].x + 0.001, shots[r].y, shots[r].z);
		glColor3f(0, 0, 0.3);
		glVertex3f(shots[r].x + 0.001, shots[r].y, shots[r].z + shots[r].vz);
		glEnd();
		glLineWidth(1);
	}


	
	if (showTitle == 1) {

		// collide with shot? 
		for (int r = 0; r < 20; r++) {


			if (shots[r].z > 0.0) {
				continue;
			}

			

			if (shots[r].z < -0.2 && shotFlag) {
				cout << "splode!" << " " << 0 << " " << 0 << " " << 0 << endl;
				numMissiles = 1;
				showTitle = 0;
				initMissile();
				triangles = 1;
				framerate = 7;
				getStartTime = 1;
				//newExplosion();
				newExplosion(0, 0 , 0, 2);

				int randy = roll(0, 5);
				string what = "play " + musicArr[randy];
				cout << what << " " << randy << endl;
				mciSendString("stop t3kintro.wav", NULL, 0, NULL);
				mciSendString(what.c_str(), NULL, 0, NULL);
				curMusic = musicArr[randy];
				
			}


		}


	}

	

	for (int i = 0; i < numMissiles; i++) {
		glPushMatrix();
		//int i = roll(0, missileArrSize - 1);

		glColor3f(missiles[i].r, missiles[i].g, missiles[i].b);
		glTranslatef(missiles[i].x, missiles[i].y, missiles[i].z);
		
		// collide with cam? 
		if (missiles[i].z > -0.1) {
			
			float xDiff = abs(abs(missiles[i].x) - abs(mouseMovedX));
			float yDiff = abs(abs(missiles[i].y) - abs(mouseMovedY));

			
			if (xDiff < 0.1 && yDiff < 0.1 && missiles[i].z < 0.1 && missiles[i].type < 3) {
				cout << "you dead fred!" << " " << missiles[i].z << endl;
				triangles = 1;
				dead = 1;
				newExplosion(0, 0, 0, 0);
				numStars = 0;
				numMissiles = 0;
				numSplodes = 0;
			}

			if (xDiff < 0.1 && yDiff < 0.1 && missiles[i].z < 0.1 && missiles[i].type == 3) {

				int doit = 0;

				if (framerate == 2) {
					doit = 1;
				}

				framerate = framerate > 1 ? (framerate-1) : 1;

				if (doit == 1 && framerate == 1) {
					doit = 0;
					mciSendString("play t2kyess.wav", NULL, 0, NULL);
				}


				if (framerate == 6) {
					autoshot = 1;
					mciSendString("play shotauto.wav", NULL, 0, NULL);
				}
				else  {
					int randy = roll(0, 3);

					if (randy == 0) {
						mciSendString("play t2kex.wav", NULL, 0, NULL);
					}

					if (randy == 1) {
						mciSendString("play yest2k.wav", NULL, 0, NULL);
					}
				
					if (randy == 2) {
						mciSendString("play powt2k.wav", NULL, 0, NULL);
					}
				}


				initMissile();
				cout << "multiplier is now: " << " " << framerate << endl;
				
			}

			//return;
			
		}

		// collide with shot? 
		for (int r = 0; r < 20; r++) {


			if (shots[r].z > 0.0) {
				continue;
			}
			
			float xDiff = abs(abs(missiles[i].x) - abs(shots[r].x));
			float yDiff = abs(abs(missiles[i].y) - abs(shots[r].y));
			float zDiff = abs(abs(missiles[i].z) - abs(shots[r].z));

			if (xDiff < 0.1 && yDiff < 0.1 && zDiff < 0.1 && shotFlag) {
				cout << "splode!" << " " << missiles[i].x << " " << missiles[i].y << " " << missiles[i].z << endl;
				
				numSplodes++;

				splodeX = missiles[i].x;
				splodeY = missiles[i].y;
				splodeZ = missiles[i].z;

				
				//newExplosion();

				if (missiles[i].type > 2) {
					newExplosion(missiles[i].x, missiles[i].y, missiles[i].z, 1);
					framerate = (framerate < 7) ? framerate + 1 : framerate;



				}
				else {
					newExplosion(missiles[i].x, missiles[i].y, missiles[i].z, 0);
				}

				triangles = 0;
				initMissile();
				
			}





		}
		
		//glColor3f((double)rand() / (RAND_MAX), (double)rand() / (RAND_MAX), (double)rand() / (RAND_MAX));
		//glutSolidTorus(1, 2, 2, 20);
		//glRotatef(missiles[i].rotx, 1, 0, 0);
		//glRotatef(missiles[i].roty, 0, 1, 0);
		//glRotatef(missiles[i].rotz, 0, 0, 1);
		//glColor3f(0.4, 1, 1);
		//glutWireTeapot(.07);
		//drawRing();

		//if (currentf != fname1) {
		//	loadTexture(fname1);
		//	currentf = fname1;
		//}

		//drawSphere(.1, 25, 25);
		glRotatef(missiles[i].roty, 0, 1, 0);

		if (missiles[i].type == 0) {

			if (currentf != fname1) {
				loadTexture("texture.ppm");
				currentf = fname1;
			}

			drawSphere(.05, 25, 25);

		}
		
		
		if (missiles[i].type == 1) {

			if (currentf != fname4) {
				
				loadTexture("texture4.ppm");
				currentf = fname4;
			}

			drawSphere(.05, 25, 25);

		}

		
		if (missiles[i].type == 2) {

			if (currentf != fname5) {
				loadTexture(fname5);
				currentf = fname5;
			}

			drawSphere(.05, 25, 25);

		}
		

		if (missiles[i].type == 3) {
			
			drawRing();

		}

		glPopMatrix();

		
	}
	
	glPopMatrix();
	if (fuel > 0) {

		glPushMatrix();

		//glDisable(GL_LIGHTING);
		//glDisable(GL_DEPTH_TEST);

		glBegin(GL_POINTS);

		for (int i = 0; i < NUM_PARTICLES; i++) {
			glColor3fv(particles[i].color);
			glVertex3fv(particles[i].position);
		}

		glEnd();

		glPopMatrix();

		//glEnable(GL_LIGHTING);
		//glEnable(GL_LIGHT0);
		//glEnable(GL_DEPTH_TEST);

		glNormal3f(0.0, 0.0, 1.0);

		for (int i = 0; i < NUM_DEBRIS; i++) {
			glColor3fv(debris[i].color);

			glPushMatrix();

			glTranslatef(debris[i].position[0],
				debris[i].position[1],
				debris[i].position[2]);

			glRotatef(debris[i].orientation[0], 1.0, 0.0, 0.0);
			glRotatef(debris[i].orientation[1], 0.0, 1.0, 0.0);
			glRotatef(debris[i].orientation[2], 0.0, 0.0, 1.0);

			glScalef(debris[i].scale[0],
				debris[i].scale[1],
				debris[i].scale[2]);

			if (triangles == 1) {
				glBegin(GL_TRIANGLES);
					glVertex3f(0.0, 0.5, 0.0);
					glVertex3f(-0.25, 0.0, 0.0);
					glVertex3f(0.25, 0.0, 0.0);
				glEnd();
			}
			else {
				glVertex3f(0.0, 0.5, 0.0);
				glVertex3f(-0.25, 0.0, 0.0);
				glVertex3f(0.25, 0.0, 0.0);
			}



			glPopMatrix();
		}


	}
	else if (dead == 1) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		showTitle = 1;

		shotFlag = 0;
		autoshot = 0;


		//if (newHighScore == 0) {
			newHighScore = 1;
		//}

		framerate = 7;

		int randy = roll(0, 2);

		if (randy == 0) {
			mciSendString("play loser.wav", NULL, 0, NULL);
		}

		if (randy == 1) {
			mciSendString("play game_over.wav", NULL, 0, NULL);
		}
		
		string what = "stop " + curMusic;
		mciSendString(what.c_str(), NULL, 0, NULL);
		mciSendString("play t3kintro.wav", NULL, 0, NULL);
		dead = 0;
	}


//drawSphere(0.1, 20, 20);
//drawMissile();

	glutSwapBuffers();
}


void mouseEnterExit(int state) {
		//glutWarpPointer(
		//	glutGet(GLUT_WINDOW_WIDTH) / 2, 
		//	glutGet(GLUT_WINDOW_HEIGHT) / 2);

}



void passiveMouseStuff(int x, int y) {

	float newX = x;
	float newY = y;

	if (x < (winWidth * .95) && x > (winWidth * .05) ) {
		mouseMovedX = ((float)x / (float)winWidth) - .5;
		//mouseMovedX = x - lastX * .5;
	}
	else {
		if (x > winWidth *.95) {
			newX = winWidth * .95;
		}

		if (x < winWidth * .05) {
			newY = winWidth * .05;
		}
	}


		
	if (y < (winHeight * .95) && y >(winHeight * .05)) {
		mouseMovedY = (float)y / (float)winHeight - .5;
		//mouseMovedY = y - lastY * .5;
	}		
	else {
		if (y > winHeight *.95) {
			newY = winHeight * .95;
		}

		if (y < winHeight * .05) {
			newY = winHeight * .05;
		}
	}

	//glutWarpPointer((int)newX, (int)newY);



	//lastX = x;
	//lastY = y;

	glutPostRedisplay();

}





void mouseStuffa(int x, int y) {

	if (x < (winWidth * .95) && x >(winWidth * .05)) {
		mouseMovedX = (float)x / (float)winWidth - .5;
	}

	if (y < (winHeight * .95) && y >(winHeight * .05)) {
		mouseMovedY = (float)y / (float)winHeight - .5;
	}


	if (dead == 0 && framerate < 7) {
		//switch (button) {
		//case 0:

			//PlaySound(TEXT("bonk.wav"), NULL, SND_FILENAME);
		

	
	mciSendString("play bonk.wav", NULL, 0, NULL);
			if (++numShots < 5) {

				for (int i = 0; i < 5; i++) {

					if (shots[i].z > 0.0) {
						shots[i].x = mouseMovedX;
						shots[i].y = -mouseMovedY + 0.1;
						shots[i].z = 0;
						break;
					}

				}
			}
			else {
				numShots = 4;
			}

			shotFlag = 1;

			rotX += x - mouseMovedX;
			rotY -= y - mouseMovedY;
			



		//	break;
		}
	//}


	glutPostRedisplay();

}




void mouseStuff(int button, int state, int x, int y) {


	if (state == GLUT_DOWN) {

		switch (button) {
		case 0:
			buttonDown = 1;
			//PlaySound(TEXT("bonk.wav"), NULL, SND_FILENAME);
			mciSendString("play bonk.wav", NULL, 0, NULL);
			if (++numShots < 20) {

				for (int i = 0; i < 20; i++) {

					if (shots[i].z > 0.0) {
						shots[i].x = mouseMovedX;
						shots[i].y = -mouseMovedY + 0.1;
						shots[i].z = 0;
						break;
					}

				}
			}
			else {
				numShots = 19;
			}

			shotFlag = 1;

			rotX += x - mouseMovedX;
			rotY -= y - mouseMovedY;
			break;
		}
	}
	else {
		buttonDown = 0;
	}

	glutPostRedisplay();

}



void idleStuff() {

	// Measure speed
	int currentTime = glutGet(GLUT_ELAPSED_TIME);
	int elapsedTime = currentTime - lastTime;

	if (getStartTime == 1) {
		getStartTime = 0;
		startTime = currentTime;
	}




	while (elapsedTime < framerate) {
		currentTime = glutGet(GLUT_ELAPSED_TIME);
		elapsedTime = currentTime - lastTime;
	}

	int multiplier = abs(framerate - 8);

	if (showTitle == 0 && dead == 0) {
		currentScore = (currentTime - startTime) * numSplodes * multiplier;

		if (framerate == 1) {
			currentScore *= 10;
		}

		//highScore = (currentScore > highScore) ? currentScore : highScore;

		
		if (currentScore > highScore && newHighScore == 1) {		
			mciSendString("play dude.wav", NULL, 0, NULL);
			newHighScore = currentScore;
			highScore = currentScore;
		}
		else if (currentScore > highScore) {
			highScore = currentScore;
		}
		
			
		
	}
	


	if (dead == 0 && framerate < 7 && buttonDown == 1) {

		
		if (++numShots < 5) {
			mciSendString("play bonk.wav", NULL, 0, NULL);

			for (int i = 0; i < 5; i++) {

				if (shots[i].z > 0.0) {
					shots[i].x = mouseMovedX;
					shots[i].y = -mouseMovedY + 0.1;
					shots[i].z = 0;
					break;
				}

			}
		}
		else {
			numShots = 4;
		}

		shotFlag = 1;

	}




	for (int i = 0; i < numStars; i++) {
		stars[i].z += stars[i].vz;
		stars[i].z = stars[i].z > 0 ? -0.5 : stars[i].z;
	}


	for (int i = 0; i < numMissiles; i++) {
		missiles[i].z += missiles[i].vz;
		missiles[i].rotx += missiles[i].drotx;
		missiles[i].roty += missiles[i].droty;
		missiles[i].rotz += missiles[i].drotz;
		//missiles[i].z = missiles[i].z > 0.5 ? -5.5 : missiles[i].z;

		if (missiles[i].z > 0.5) {
			initMissile();
		}

	}


	if (shotFlag) {

		for (int i = 0; i < 20; i++) {

			if (shots[i].z <= 0.0) {
				shots[i].z += shots[i].vz;
			}

			if (shots[i].z < -0.5) {
				shots[i].z = 0.001;
				numShots--;
			}

			if (!numShots) {
				shotFlag = 0;
				break;
			}

		}
	}


	
	if (fuel > 0) {
		for (int i = 0; i < NUM_PARTICLES; i++) {
			particles[i].position[0] += particles[i].speed[0] * 0.2;
			particles[i].position[1] += particles[i].speed[1] * 0.2;
			particles[i].position[2] += particles[i].speed[2] * 0.2;

			particles[i].color[0] -= 1.0 / 500.0;

			if (particles[i].color[0] < 0.0) {
				particles[i].color[0] = 0.0;
			}

			particles[i].color[1] -= 1.0 / 100.0;

			if (particles[i].color[1] < 0.0) {
				particles[i].color[1] = 0.0;
			}

			particles[i].color[2] -= 1.0 / 50.0;

			if (particles[i].color[2] < 0.0) {
				particles[i].color[2] = 0.0;
			}
		}

		for (int i = 0; i < NUM_DEBRIS; i++)
		{
			debris[i].position[0] += debris[i].speed[0] * 0.1;
			debris[i].position[1] += debris[i].speed[1] * 0.1;
			debris[i].position[2] += debris[i].speed[2] * 0.1;

			debris[i].orientation[0] += debris[i].orientationSpeed[0] * 10;
			debris[i].orientation[1] += debris[i].orientationSpeed[1] * 10;
			debris[i].orientation[2] += debris[i].orientationSpeed[2] * 10;
		}

		--fuel;

		if (fuel == 0) {
			numStars = 250;
		}


	}




	glutPostRedisplay();
	lastTime = currentTime;

}



void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
		case 'q':
			exit(0);

	default:
		break;
	}
}



int main(int argc, char** argv) {
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowPosition(150, 150);
	glutInitWindowSize(winWidth, winHeight);
	glEnable(GL_TEXTURE_2D);
	glutCreateWindow("Holiday CS596 FINAL");
	glutDisplayFunc(displayStuff);
	glutIdleFunc(idleStuff);
	glutReshapeFunc(reshapeStuff);
	glutMouseFunc(mouseStuff);
	glutMotionFunc(mouseStuffa);
	glutEntryFunc(mouseEnterExit);
	glutPassiveMotionFunc(passiveMouseStuff);
	glutKeyboardFunc(keyboard);





	
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light0Amb);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light0Dif);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light0Spec);
	glLightfv(GL_LIGHT0, GL_POSITION, light0Pos);
	glLightfv(GL_LIGHT1, GL_AMBIENT, light1Amb);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light1Dif);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light1Spec);
	glLightfv(GL_LIGHT1, GL_POSITION, light1Pos);
	glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, materialAmb);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, materialDif);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, materialSpec);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, materialShininess);
	glEnable(GL_NORMALIZE);
	glutSetCursor(GLUT_CURSOR_NONE);



	initStuff();
	mciSendString("play t3kintro.wav", NULL, 0, NULL);
	glutFullScreen();
	glutMainLoop();
	return 0;
}