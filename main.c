// Blender Renderer
// By and copyright (2011) Samuel Pauls

// The simplest possible C program that loads an Object file that was exported
// by Blender and displays it's geometry and textures.  A great learning
// resource!

#include <stdlib.h> // for EXIT_SUCCESS
#include <stdio.h> // for printf and scanf
#include <string.h>
#include <assert.h>

#include <GL/glut.h> // OpenGL Utility Toolkit - for using the Windowing system
#include <GL/gl.h> // OpenGL - for rendering 3D graphics
#include <GL/glu.h> // OpenGL Utility Library - extra features built atop of GL

#include "lodepng.h" // Lode Vandevenne's excellent PNG loader


// A 3D point.  Used for the scene's geometry.
typedef struct {
  float x, y, z;
} Vertex3D;

// A 2D point.  Used for a scene's texture coordinates on the geometry.
typedef struct {
  float x, y;
} Vertex2D;

// While an Object file may contain multiple objects, each with a set of
// vertices and texture vertices, all vertex indecies and all texture vertex
// indecies are unique.  Therefore, we load them in single lists.
#define MAX_VERTICES 1500
Vertex3D vertices[MAX_VERTICES];
int vertex_count = 0;
Vertex2D texture_vertices[MAX_VERTICES];
int texture_vertex_count = 0;

// A face is a polygon that makes up part of an object.
#define MAX_FACE_VERTICES 4
typedef struct {
  int vertices[MAX_FACE_VERTICES];
  int texture_vertices[MAX_FACE_VERTICES];
  int vertex_count;
} Face;

// An object is a major part of the scene, such as hair, clothing, and a body.
typedef struct {
  #define MAX_NAME_LENGTH 50
  char name[MAX_NAME_LENGTH];

  #define MAX_FACES 1000
  Face faces[MAX_FACES];
  int face_count;

  GLuint texture;
} Object;

// Objects are parts of the scene, such as hair, body, and clothing.
#define MAX_OBJECTS 5
Object objects[MAX_OBJECTS];
int object_count = 0;

// Draw the scene.
void display() {
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // The model view matrix is used for positioning primitives as we render them.
  // It can also be used to position the camera by doing the opposite.
  glMatrixMode (GL_MODELVIEW);
  glLoadIdentity();

  // position the camera.
  static const float CAMERA_X = 0.0F;
  static const float CAMERA_Y = 0.0F;
  static const float CAMERA_Z = 30.0F;
  glTranslatef (-CAMERA_X, -CAMERA_Y, -CAMERA_Z);

  // Rotate the scene.
  static float scene_rotation = 0.0F;
  static const float ROTATE_SPEED = 1.0F;
  scene_rotation += ROTATE_SPEED;
  glRotatef (scene_rotation, 0.0F, 1.0F, 0.0F);

  // For each object...
  int o;
  for (o = 0; o < object_count; ++o) {
    Object *object = &objects[o];
    //glBindTexture (GL_TEXTURE_2D, object->texture);

    // For each face of the current object...
    int f;
    for (f = 0; f < object->face_count; ++f) {
      Face *face = &object->faces[f];
 	//glBegin (GL_POINTS);//GL_LINES
	glBegin (GL_LINE_LOOP);
      //glBegin (GL_POLYGON);
        int v;
        for (v = 0; v < face->vertex_count; ++v) {
          // Place a texture coordinate.
          //int texture_vertex_index = face->texture_vertices[v];
          //Vertex2D *texture_vertex = &texture_vertices[texture_vertex_index];
          //glTexCoord2f (texture_vertex->x, texture_vertex->y);

          // Place a geometry vertex.
          int vertex_index = face->vertices[v];
          Vertex3D *vertex = &vertices[vertex_index];
          glVertex3f (vertex->x, vertex->y, vertex->z);
        }
      glEnd();
    }
  }

  // We just drew the scene to the back buffer.  Swap it with the front buffer
  // which is displayed on screen.  If we only used a single buffer, we'd see
  // artifacts from the drawing operations.
  glutSwapBuffers();

  // dessiner frame suivante.
  glutPostRedisplay();
}

int main (int argc, char *argv[]) {
  // Create a Window for OpenGL.
  glutInit (&argc, argv);
  glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB); // double bufing avoids tearing
  glutInitWindowSize (640, 480);
  glutCreateWindow ("Blender Reader");
  glutDisplayFunc (&display);


  // Set the projection matrix for a perspective view.  Orthographic was the
  // other option.
  glMatrixMode (GL_PROJECTION);
  //glLoadIdentity();
  gluPerspective (65.0, 640.0F / 480.0F, 1, 40);

  //glClearColor (0.0F, 0.0F, 0.0F, 1.0F); // black

  // Enable fragment depth testing so that only the fragments nearest to the
  // camera are rendered in the final image.
  //glEnable (GL_DEPTH_TEST);
  //glDepthFunc (GL_LEQUAL);

  // Enable texture mapping so that we can render textured polygons.
  //glEnable (GL_TEXTURE_2D);


  // Open the Object file that contains geometry and texture vertices.
  // This particular scene is by Peter McAlpine.
  static const char *SCENE_FILENAME = "eumi/eumi.obj";
  //static const char *SCENE_FILENAME = "Kiefer/Kiefer.obj";
  static const char *READ_MODE = "r";
  FILE *f = fopen (SCENE_FILENAME, READ_MODE);

  Object *current_object;

  // Read each line of the Object file.
  static const int MAX_LINE_LENGTH = 100;
  char line[MAX_LINE_LENGTH];
  while (fgets (line, MAX_LINE_LENGTH, f) != NULL) {
    if (strncmp (line, "o ", 2) == 0) { // If starting a new object...
      current_object = &objects[object_count];

      // Read the object's name.
      sscanf (line, "o %s", current_object->name);
      current_object->face_count = 0;

      // Load the PNG as the object's texture.
      // The texture must be loaded into OpenGL after OpenGL is initialized.
      static const int MAX_FILENAME_LENGTH = 100;
      char filename[MAX_FILENAME_LENGTH];
      sprintf (filename, "eumi/eumi_%s.png", current_object->name);
      unsigned char *buffer, *image;
      size_t buffer_size, image_size;
      //LodePNG_Decoder decoder;
      //LodePNG_Decoder_init (&decoder);
      //LodePNG_loadFile (&buffer, &buffer_size, filename);
      //LodePNG_Decoder_decode (&decoder, &image, &image_size, buffer, buffer_size);
      //assert (!decoder.error);
      //glGenTextures (1, &current_object->texture);
      //glBindTexture (GL_TEXTURE_2D, current_object->texture);
      //glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB, decoder.infoPng.width, decoder.infoPng.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
      //glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      //glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      //free (image);
      //free (buffer);
      //LodePNG_Decoder_cleanup (&decoder);

      object_count++;
    } else if (strncmp (line, "v ", 2) == 0) { // If describing a vertex...
      Vertex3D *v = &vertices[vertex_count++];
      sscanf (line, "v %f %f %f", &v->x, &v->y, &v->z);
    } else if (strncmp (line, "vt ", 3) == 0) { // If texture vertex...
      Vertex2D *vt = &texture_vertices[texture_vertex_count++];
      sscanf (line, "vt %f %f", &vt->x, &vt->y);
      vt->y = 1 - vt->y;
    } else if (strncmp (line, "f ", 2) == 0) { // If an object face...
      Face *face = &current_object->faces[current_object->face_count++];
      face->vertex_count = 0;
      char *subline = &line[2];
      while (subline != NULL) {
        int vertex_index, texture_vertex_index,normal_vertex;

        if (sscanf (subline, "%i/%i/%i", &vertex_index, &texture_vertex_index, &normal_vertex) == 3){}
        else if (sscanf (subline, "%i//%i", &vertex_index, &texture_vertex_index) == 2){}
        else if (sscanf (subline, "%i/%i", &vertex_index, &texture_vertex_index) == 2){}
        else if (sscanf (subline, "%i", &vertex_index) == 1){}

        int fv = face->vertex_count;
        face->vertices[fv] = vertex_index - 1;
        face->texture_vertices[fv] = texture_vertex_index - 1;
        face->vertex_count++;
        subline = strchr (subline, ' ');
        if (subline != NULL) {
          subline = &subline[1];
        }
      }
    } // if type of line
  } // for each line

  fclose (f);

  int o;

  // Display interesting stats about the loaded Object file.
  printf ("Vertices: %i\n", vertex_count);
  //printf ("Texture vertices: %i\n", texture_vertex_count);
  //printf ("Objects: %i\n", object_count);
  for (o = 0; o < object_count; ++o) {
    printf ("  %s faces: %i\n", objects[o].name, objects[o].face_count);
  }

  // Start the main program loop.
  glutMainLoop(); // never returns

  //glDisable (GL_TEXTURE_2D);

  // If the main loop ever ended, we'd want to free the textures.
  //for (o = 0; o < object_count; ++o) {
   // glDeleteTextures (1, &objects[o].texture);
  //}

  return EXIT_SUCCESS;
}
