#include <Windows.h>
#include <GL/glut.h>
#include <math.h>
#include <time.h>

// Rotation angle
float rotationAngle = 0.0;

// Timer interval in milliseconds
const int timerInterval = 20; // 20 frames per second

// Position offsets for the smaller satellite
float satelliteXOffset = -150.0;
float satelliteYOffset = 10.0;

// Movement speed for the smaller satellite
const float moveSpeed = 10.0;

// Shear factors for keyboard
float shearX = 0.0;
float shearY = 0.0;

// Global variables
float timeOfDay = 0; // Variable to simulate the time of day
float timeIncrement = 0.001; // Speed of time progression

// Bezier curve control points (adjusted to stay in the upper part of the screen)
float bezierControlPoints[4][2] = {
    {100.0, 700.0},
    {400.0, 800.0},
    {800.0, 600.0},
    {1100.0, 750.0}
};

// Bezier curve parameter
float t = 0.0;


// Parameters for spotlight animation
float spotlightX = 600.0;
float spotlightY = 100.0;
float spotlightAngle = 0.0;
const float spotlightSpeed = 1.0; // Speed of spotlight rotation

// Function to initialize OpenGL settings
void init()
{
    glClearColor(0.0, 0.0, 0.2, 0.0); // Dark blue background for the night sky
    glMatrixMode(GL_PROJECTION); // Set the projection matrix
    gluOrtho2D(0.0, 1200, 0.0, 900); // Set the coordinate system

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
}

// Function to draw the spotlight
void drawSpotlight(float x, float y, float angle)
{
    glPushMatrix();
    glTranslatef(1000, y, 0.0); // Move to the spotlight position
    glRotatef(angle, 0.0, 0.0, 1.0); // Rotate the spotlight

    // Draw the spotlight beam
    glColor3f(1.0, 1.0, 0.0); // Yellow color for spotlight beam
    glBegin(GL_TRIANGLES);
    glVertex2f(0.0, 0.0); // Tip of the spotlight
    glVertex2f(-50.0, 400.0); // Left edge of the beam
    glVertex2f(50.0, 400.0); // Right edge of the beam
    glEnd();

    // Draw the spotlight base
    glColor3f(0.5, 0.5, 0.5); // Gray color for spotlight base
    glBegin(GL_QUADS);
    glVertex2f(-20.0, 0.0);
    glVertex2f(20.0, 0.0);
    glVertex2f(20.0, -30.0);
    glVertex2f(-20.0, -30.0);
    glEnd();

    glPopMatrix();
}

// Moon
void drawMoon() {
    glPushMatrix();

    // Calculate position of the moon based on timeOfDay
    float y = 150.0 + 700.0 * sin(timeOfDay * 3.14159); // Move vertically between 150 and 850
    float x = 1100.0; // Fixed x position

    // Draw the moon
    glColor3f(0.9, 0.9, 0.9); // Light gray color for the moon
    glTranslatef(x, y, 0.0);

    glBegin(GL_POLYGON);
    for (int i = 0; i < 360; i++) {
        float degInRad = i * 3.14159 / 180;
        glVertex2f(cos(degInRad) * 50, sin(degInRad) * 50);
    }
    glEnd();

    glPopMatrix();
}

// Function to draw a silhouette building
void drawBuilding(float baseX, float height) {
    glPushMatrix();
    glTranslatef(baseX, 0.0, 0.0); // Position the building

    // Building base
    glColor3f(0.1, 0.1, 0.1); // Dark gray color
    glBegin(GL_POLYGON);
    glVertex2i(0, 100); // Bottom left corner
    glVertex2i(0, 100 + height); // Top left corner
    glVertex2i(100, 100 + height); // Top right corner
    glVertex2i(100, 100); // Bottom right corner
    glEnd();

    // Windows
    glColor3f(0.3, 0.3, 0.3); // Lighter gray color for windows
    for (int y = 120; y < 100 + height - 20; y += 40) {
        for (int x = 10; x < 90; x += 20) {
            glBegin(GL_POLYGON);
            glVertex2i(x, y); // Bottom left corner
            glVertex2i(x, y + 20); // Top left corner
            glVertex2i(x + 10, y + 20); // Top right corner
            glVertex2i(x + 10, y); // Bottom right corner
            glEnd();
        }
    }

    glPopMatrix();
}


// Main Satellite
void drawSatellite(float xOffset, float yOffset, float scale)
{
    glPushMatrix();
    glTranslatef(xOffset, yOffset, 0.0);
    glScalef(scale, scale, 1.0);

    // Satellite - Main Body (Circle)
    glColor3f(0.5, 0.5, 0.5); // Gray color for satellite body
    glBegin(GL_POLYGON);
    for (int i = 0; i < 360; i++) {
        float degInRad = i * 3.14159 / 180;
        glVertex2f(600 + cos(degInRad) * 150, 300 + sin(degInRad) * 80);
    }
    glEnd();

    // Satellite - Antenna 1 (Cylinder)
    glColor3f(0.8, 0.8, 0.8); // Light gray color for antenna
    glBegin(GL_POLYGON);
    for (int i = 0; i < 360; i++) {
        float degInRad = i * 3.14159 / 180;
        glVertex2f(600 + cos(degInRad) * 10, 300 + sin(degInRad) * 120); // Adjust radius or length as needed
    }
    glEnd();

    // Satellite - Antenna 2 (Cylinder)
    glBegin(GL_POLYGON);
    for (int i = 0; i < 360; i++) {
        float degInRad = i * 3.14159 / 180;
        glVertex2f(600 + cos(degInRad) * 150, 300 + sin(degInRad) * 70); // Adjust radius or length as needed
    }
    glEnd();

    // Satellite - Tower (Cylinder)
    glColor3f(0.8, 0.8, 0.8); // Light gray color for tower
    glBegin(GL_POLYGON);
    for (int i = 0; i < 360; i++) {
        float degInRad = i * 3.14159 / 180;
        glVertex2f(600 + cos(degInRad) * 50, 50 + sin(degInRad) * 400); // Adjust position and size as needed
    }
    glEnd();

    // Satellite - Pole (Triangle)
    glColor3f(0.6, 0.6, 0.6); // Light gray color for pole
    glBegin(GL_TRIANGLES);
    glVertex2i(600, 450); // Top point
    glVertex2i(620, 300); // Bottom left point
    glVertex2i(590, 300); // Bottom right point
    glEnd();

    // Satellite - Transmitter (Point)
    glColor3f(1.0, 0.0, 0.0); // Red color for transmitter
    glPointSize(10); // Larger point size for transmitter
    glBegin(GL_POINTS);
    glVertex2i(600, 450); // Transmitter position
    glEnd();

    glPopMatrix();
}

// Floating Satelite in the sky
void drawRotatingSatellite()
{
    glPushMatrix();
    glTranslatef(800.0, 800.0, 0.0); // Move to the center of the window
    glRotatef(rotationAngle, 0.0, 0.0, 1.0); // Rotate around the z-axis
    glScalef(1.0, 1.0, 1.0); // Set scaling (no scaling here)

    // Body
    glColor3f(0.8, 0.8, 0.8); // Light gray color
    glBegin(GL_QUADS);
    glVertex2f(-30.0, -15.0);
    glVertex2f(30.0, -15.0);
    glVertex2f(30.0, 15.0);
    glVertex2f(-30.0, 15.0);
    glEnd();

    // Solar panels
    glColor3f(0.2, 0.2, 0.8); // Dark blue color for solar panels
    glBegin(GL_QUADS);
    glVertex2f(-60.0, -5.0);
    glVertex2f(-30.0, -5.0);
    glVertex2f(-30.0, 5.0);
    glVertex2f(-60.0, 5.0);

    glVertex2f(30.0, -5.0);
    glVertex2f(60.0, -5.0);
    glVertex2f(60.0, 5.0);
    glVertex2f(30.0, 5.0);
    glEnd();

    // Antennas
    glColor3f(0.6, 0.6, 0.6); // Gray color for antennas
    glBegin(GL_LINES);
    glVertex2f(0.0, 15.0);
    glVertex2f(0.0, 30.0);
    glVertex2f(0.0, -15.0);
    glVertex2f(0.0, -30.0);
    glEnd();

    glPopMatrix();
}

// Refleksi
void drawReflectedRotatingSatellite()
{
    glPushMatrix();
    glTranslatef(200.0, 800.0, 0.0); // Move to the bottom half of the window
    glRotatef(rotationAngle, 0.0, 0.0, -1.0); // Rotate around the z-axis
    glScalef(1.0, -1.0, 1.0); // Flip vertically to create reflection

    // Body
    glColor3f(0.8, 0.8, 0.8); // Light gray color
    glBegin(GL_QUADS);
    glVertex2f(-30.0, -15.0);
    glVertex2f(30.0, -15.0);
    glVertex2f(30.0, 15.0);
    glVertex2f(-30.0, 15.0);
    glEnd();

    // Solar panels
    glColor3f(0.2, 0.2, 0.8); // Dark blue color for solar panels
    glBegin(GL_QUADS);
    glVertex2f(-60.0, -5.0);
    glVertex2f(-30.0, -5.0);
    glVertex2f(-30.0, 5.0);
    glVertex2f(-60.0, 5.0);

    glVertex2f(30.0, -5.0);
    glVertex2f(60.0, -5.0);
    glVertex2f(60.0, 5.0);
    glVertex2f(30.0, 5.0);
    glEnd();

    // Antennas
    glColor3f(0.6, 0.6, 0.6); // Gray color for antennas
    glBegin(GL_LINES);
    glVertex2f(0.0, 15.0);
    glVertex2f(0.0, 30.0);
    glVertex2f(0.0, -15.0);
    glVertex2f(0.0, -30.0);
    glEnd();

    glPopMatrix();
}

//Pesawat dengan gerakan kurva bezier
void drawAirplane(float xOffset, float yOffset, float scale)
{
    glPushMatrix();
    glTranslatef(xOffset, yOffset, 0.0);
    glScalef(scale, scale, 1.0);

    // Draw the airplane body
    glColor3f(1.0, 1.0, 1.0); // White color
    glBegin(GL_QUADS);
    glVertex2f(-60.0, -10.0);
    glVertex2f(60.0, -10.0);
    glVertex2f(60.0, 10.0);
    glVertex2f(-60.0, 10.0);
    glEnd();

    // Draw the airplane wings
    glColor3f(0.0, 1.0, .0); // Green color
    glBegin(GL_TRIANGLES);
    glVertex2f(-20.0, 10.0);
    glVertex2f(0.0, 40.0);
    glVertex2f(20.0, 10.0);

    glVertex2f(-20.0, -10.0);
    glVertex2f(0.0, -40.0);
    glVertex2f(20.0, -10.0);
    glEnd();

    // Draw the airplane tail
    glColor3f(0.0, 0.0, 1.0); // Green color
    glBegin(GL_TRIANGLES);
    glVertex2f(-60.0, 10.0);
    glVertex2f(-80.0, 30.0);
    glVertex2f(-60.0, -10.0);
    glEnd();

    glPopMatrix();
}

// Kurva Bezier untuk pesawat
void drawBezierCurve()
{
    glColor3f(1.0, 1.0, 1.0); // White color for the curve
    glBegin(GL_LINE_STRIP);
    for (float t = 0.0; t <= 1.0; t += 0.01) {
        float x = pow(1 - t, 3) * bezierControlPoints[0][0] +
            3 * t * pow(1 - t, 2) * bezierControlPoints[1][0] +
            3 * pow(t, 2) * (1 - t) * bezierControlPoints[2][0] +
            pow(t, 3) * bezierControlPoints[3][0];
        float y = pow(1 - t, 3) * bezierControlPoints[0][1] +
            3 * t * pow(1 - t, 2) * bezierControlPoints[1][1] +
            3 * pow(t, 2) * (1 - t) * bezierControlPoints[2][1] +
            pow(t, 3) * bezierControlPoints[3][1];
        glVertex2f(x, y);
    }
    glEnd();
}

//Shear untuk bintang
void drawStar(float x, float y, float size, float shearX, float shearY)
{
    glPushMatrix();
    glTranslatef(x, y, 0.0); // Translate to the star position
    GLfloat shearMatrix[16] = {
        1.0, shearY, 0.0, 0.0,
        shearX, 1.0, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        0.0, 0.0, 0.0, 1.0
    };
    glMultMatrixf(shearMatrix); // Apply shear transformation

    // Draw the star
    glColor3f(1.0, 1.0, 0.0); // Yellow color
    glBegin(GL_TRIANGLE_FAN);
    // Titik pusat bintang
    glVertex2f(0.0, 0.0);

    for (int i = 0; i <= 10; ++i) {
        float angle = i * M_PI / 5.0;
        float radius = (i % 2 == 0) ? size : size / 2;
        glVertex2f(radius * cos(angle), radius * sin(angle));
    }

    glEnd();

    glPopMatrix();
}

// Function to handle keyboard input
void keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
        case 'w':
            satelliteYOffset += moveSpeed; // Move up
            break;
        case 's':
            satelliteYOffset -= moveSpeed; // Move down
            break;
        case 'a':
            satelliteXOffset -= moveSpeed; // Move left
            break;
        case 'd':
            satelliteXOffset += moveSpeed; // Move right
            break;
        case '+':
            shearX += 0.1; //shear +
            shearY += 0.1;
            break;
        case '-':
            shearX -= 0.1; // shear -
            shearY -= 0.1;
            break;
        default:
            break;
    }
    glutPostRedisplay(); // Request display update
}

// Timer function to update rotation angle and request redisplay
void update(int value)
{
	 // Increment the time of night
    timeOfDay += timeIncrement;
    if (timeOfDay > 2.0) {
        timeOfDay = 0.0; // Reset to simulate continuous night cycle
    }
    
     // Update the spotlight angle
    spotlightAngle += spotlightSpeed;
    if (spotlightAngle > 360.0)
    {
        spotlightAngle -= 360.0;
    }
    
    //Rotation Angle for floating satelite
    rotationAngle += 0.5; // Slower increment of the rotation angle
    if (rotationAngle > 360.0)
    {
        rotationAngle -= 360.0;
    }
    t += 0.00005; // Slower increment of the parameter t
    if (t > 1.0)
    {
        t = 0.0; // Reset t if it exceeds 1
    }
    glutPostRedisplay(); // Request display update
    glutTimerFunc(timerInterval, update, 0); // Set the timer to call this function again
}

// Function to draw the scene
void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the display
    
        
     // Draw the spotlight
    drawSpotlight(spotlightX, spotlightY, spotlightAngle);
    
	// Draw stars in the sky using shear transformations
    drawStar(100.0, 800.0, 10.0, shearX, shearY);
    drawStar(300.0, 850.0, 15.0, shearX, shearY);
    drawStar(500.0, 780.0, 12.0, shearX, shearY);
    drawStar(700.0, 820.0, 10.0, shearX, shearY);
    drawStar(900.0, 870.0, 14.0, shearX, shearY);
    drawStar(1100.0, 800.0, 10.0, shearX, shearY);
    drawStar(950.0, 750.0, 12.0, shearX, shearY);
    
	// Draw the moon
    drawMoon();
	
	// Draw the bezier curve
    //drawBezierCurve();

    // Compute the position on the Bezier curve
    float x = pow(1 - t, 3) * bezierControlPoints[0][0] +
        3 * t * pow(1 - t, 2) * bezierControlPoints[1][0] +
        3 * pow(t, 2) * (1 - t) * bezierControlPoints[2][0] +
        pow(t, 3) * bezierControlPoints[3][0];
    float y = pow(1 - t, 3) * bezierControlPoints[0][1] +
        3 * t * pow(1 - t, 2) * bezierControlPoints[1][1] +
        3 * pow(t, 2) * (1 - t) * bezierControlPoints[2][1] +
        pow(t, 3) * bezierControlPoints[3][1];

    // Draw the airplane at the computed position
   drawAirplane(x, y, 0.5);

    // Increment the Bezier curve parameter
    t += 0.001;
    if (t > 1.0)
        t = 0.0;
    // Ground
    glColor3f(0.0, 0.6, 0.0); // green color for ground
    glBegin(GL_POLYGON);
    glVertex2i(0, 100); // Bottom left corner
    glVertex2i(1200, 100); // Bottom right corner
    glVertex2i(1200, 0); // Top right corner
    glVertex2i(0, 0); // Top left corner
    glEnd();
    
    // Draw the silhouette buildings with varying heights
    drawBuilding(900.0, 200.0);
    drawBuilding(1000.0, 300.0);
    drawBuilding(1100.0, 250.0);

    // Draw the first satellite
    drawSatellite(0.0, 0.0, 1.0);

    // Draw the second satellite using translation and scaling
    drawSatellite(satelliteXOffset, satelliteYOffset, 0.8);
    drawSatellite(satelliteXOffset, satelliteYOffset, 0.5);
    
    // Draw big satelite
    //drawSatellite(satelliteXOffset, satelliteYOffset, 1.6);

	// Draw the main rotate statelite
	drawRotatingSatellite();
	
	// Draw the reflected rotating satellite
    drawReflectedRotatingSatellite();

	glutSwapBuffers();
	  
    glFlush();
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1200, 900);
    glutCreateWindow("Rayhan Al Farassy: Satelite");
    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(timerInterval, update, 0); // Start the timer for auto-rotation
    glutMainLoop();
    return 0;
}
