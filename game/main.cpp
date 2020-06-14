#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include "/Users/yang-yejin/Library/Mobile Documents/com~apple~CloudDocs/Downloads/GLUT.framework/Headers/glut.h"
#include "LoadShaders.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>
#include <time.h>
#include <string.h>
#include "loadobj.h"
#define TINYOBJLOADER_USE_DOUBLE
#include "tiny_obj_loader.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <unistd.h>

using namespace glm ;
using namespace std ;
using namespace tinyobj ;
typedef std::vector<GLfloat> GLvec ;


#define FPUSH_VTX3(p,vx,vy,vz)\
do{\
p.push_back(vx);\
p.push_back(vy);\
p.push_back(vz);\
}while(0)

#define FSET_VTX3(vx,vy,vz,valx,valy,valz)\
do{\
vx = (float)(valx);\
vy = (float)(valy);\
vz = (float)(valz);\
}while(0)

#define FPUSH_VTX3_AT(p, i, vx, vy, vz)\
do{ \
GLuint i3 = 3*(i); \
p[i3+0] = (float)(vx) ; \
p[i3+1] = (float)(vy) ; \
p[i3+2] = (float)(vz) ; \
}while(0)


#define UVARS(name) glGetUniformLocation(program, name)

#define MAP_FIND(map_obj, item)\
((map_obj).find(item) != (map_obj).end())

 #define IS_MIPMAP(flag) \
((flag) == GL_LINEAR_MIPMAP_LINEAR || \
(flag) == GL_LINEAR_MIPMAP_NEAREST || \
(flag) == GL_NEAREST_MIPMAP_LINEAR|| \
(flag) == GL_NEAREST_MIPMAP_NEAREST)


GLint program ;
GLint shading_mode = 2 ;
void build_program() ;
bool fin = false ;

void keyboard(unsigned char key, int x, int y) ;
void display() ;
void bind_buffer(GLint buffer, GLvec& vec, int program, const GLchar* attri_name, GLint attri_size) ;
void bind_buffer(GLint buffer, int program, const GLchar* attri_name, GLint attri_size);
mat4 parallel(double r, double aspect, double n, double f) ;

void init() ;
void init_menu() ;


int button_pressed[3] = {GLUT_UP, GLUT_UP, GLUT_UP};
int mouse_pos[2] = { 0, 0 };
void mouse(int button, int state, int x, int y) ;
void motion(int x, int y) ;
void cb_special(int key, int x, int y) ;
void cb_main_menu(int value) ;

static bool has_file(const char* filepath) ;
bool load_tex(
const char* basedir,
vector<real_t>& texcoords_out,
map<string, unsigned int>& texmap_out,
const vector<real_t>& texcoords,
const vector<shape_t>& shapes,
const vector<material_t>& materials,
GLint min_filter, GLint mag_filter) ;
void render(int color_mode) ;
GLuint generate_tex(const char* tex_file_path, GLint min_filter,GLint mag_filter) ;
void draw_obj_model(int model_idx, int color_mode, int object_code) ;
GLuint generate_background();

 struct State
{
    GLfloat theta;
    GLfloat orbit_theta;
    GLfloat x_pos, y_pos, z_pos ;
    
    State():
        theta(0),
        orbit_theta(0),
        y_pos(0),
        z_pos(0),
        x_pos(0){}
    
    glm::mat4 get_transf()
    {
        mat4 M(1.0f);
        M = translate(M, vec3(x_pos, y_pos, z_pos));
        M = rotate(M, theta, vec3(0.0f, 1.0f, 0.0f)) ;
        return M;
    }
    
};
struct Camera{
    enum { ORTHOGRAPHIC, PERSPECTIVE };
    glm::vec3 eye;
    glm::vec3 center;
    glm::vec3 up;
    float zoom_factor;
    int projection_mode;
    float z_near;
    float z_far;
    float fovy;
    float x_right;
    
    Camera() :
    eye(0,4,3),
    center(0, 0, 0),
    up(0, 1, 0),
    zoom_factor(1.0f),
    projection_mode(ORTHOGRAPHIC),
    z_near(0.01f),
    z_far(100.0f),
    fovy((float)(M_PI/180.0*(30.0))),
    x_right(1.2f)
    {}
    mat4 get_viewing() { return lookAt(eye, center, up); }
    mat4 get_projection(float aspect)
    {
        glm::mat4 P(1.0f);
        switch (projection_mode)
        {
            case ORTHOGRAPHIC:
                P = parallel(zoom_factor*x_right, aspect, z_near, z_far);
                break;
            case PERSPECTIVE:
                P = perspective(zoom_factor*fovy, aspect, z_near, z_far);
                break;
        }
        return P;
    }
};
Camera camera ;
int user = -1 ;
enum {MODEL_USER1, MODEL_USER2, HELI, NUM_OF_MODELS};
enum {PICKING=1, PHONG, GOURAUD} ;

//path 지우지 말고 주석처리해놓기!
const char* back2d_path = "/Users/yang-yejin/Desktop/graphics/term_tex/game/game/o4.jpg" ;
const char* vert_dir = "/Users/yang-yejin/Desktop/graphics/term_tex/game/game/viewing.vert" ;
const char* frag_dir = "/Users/yang-yejin/Desktop/graphics/term_tex/game/game/viewing.frag" ;
const char* base_dir = "/Users/yang-yejin/Desktop/graphics/term_tex/game/game/" ;
const char* model_files[NUM_OF_MODELS] = {
"/Users/yang-yejin/Desktop/graphics/term_tex/game/game/ARC170.obj",
"/Users/yang-yejin/Desktop/graphics/term_tex/game/game/bixler.obj",
"/Users/yang-yejin/Desktop/graphics/term_tex/game/game/10621_CoastGuardHelicopter.obj"
   };
//const char* model_files[NUM_OF_MODELS] = {
//   "/Users/yang-yejin/Desktop/graphics/term_tex/game/game/ARC170.obj",
//"/Users/yang-yejin/Desktop/graphics/term_tex/game/game/bixler.obj"
//   };
//const char* vert_dir = "/Users/im-aron/Documents/4-1/ComputerGraphics/graphics_game/game/viewing.vert" ;
//const char* frag_dir = "/Users/im-aron/Documents/4-1/ComputerGraphics/graphics_game/game/viewing.frag" ;
//const char* base_dir = "/Users/im-aron/Documents/4-1/ComputerGraphics/graphics_game/game/" ;
//const char* model_files[NUM_OF_MODELS] = {
//   "/Users/im-aron/Documents/4-1/ComputerGraphics/graphics_game/game/ARC170.obj",
//"/Users/im-aron/Documents/4-1/ComputerGraphics/graphics_game/game/bixler.obj"
//   };
GLuint vao[NUM_OF_MODELS], vbo[NUM_OF_MODELS][3];

GLuint texture1;
GLuint rec_vao, rec_vbo[3];

GLvec rec_vertices;
GLvec rec_colors;
GLvec rec_texcoord;

void get_rect_3d(GLvec &p, GLfloat width, GLfloat height, GLfloat z);
void get_vertex_color(GLvec &color, GLuint n, GLfloat r, GLfloat g, GLfloat b);
void get_rect_texcoord(GLvec &q);

float model_scales[NUM_OF_MODELS] = {0.2f, 0.2f, 0.2f};
State state[NUM_OF_MODELS];
const int n_heli = 10 ;
State heli_state[n_heli] ;
vector<real_t> vertices[NUM_OF_MODELS];
vector<real_t> normals[NUM_OF_MODELS];
vector<real_t> colors[NUM_OF_MODELS];
vector<vector<unsigned int>> vertex_map[NUM_OF_MODELS];
vector<vector<unsigned int>> material_map[NUM_OF_MODELS];
vector<shape_t> shapes[NUM_OF_MODELS];
vector<material_t> materials[NUM_OF_MODELS];
vector<real_t> texcoords[NUM_OF_MODELS];
map<string, unsigned int> texmap[NUM_OF_MODELS];
bool is_obj_valid = false;
bool is_tex_valid = false;
int w_width;
int w_height;

float z_bottom = 1.4f , z_top = -z_bottom, right_most = 1.1f, left_most = -right_most ;

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE|GLUT_DEPTH|GLUT_3_2_CORE_PROFILE);
    glutInitWindowSize(2048, 2048);
    glutCreateWindow("texture");
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
        exit(EXIT_FAILURE);
    }
    
    init();
    init_menu() ;
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard) ;
    glutMotionFunc(motion) ;
    glutMouseFunc(mouse) ;
    glutSpecialFunc(cb_special) ;
    glutMainLoop();
}

void init(){
    w_width = glutGet(GLUT_WINDOW_WIDTH) ;
    w_height = glutGet(GLUT_WINDOW_HEIGHT) ;
    
    build_program();

    state[MODEL_USER1].x_pos = -0.2f ;
    state[MODEL_USER2].x_pos = 0.2f ;

    srand(time(NULL)) ;
    for(int i = 0 ; i < n_heli ; i++){
        heli_state[i].x_pos = left_most + rand() % 200 * 0.01 ;
        heli_state[i].z_pos = z_top - rand() % 500 * 0.01 ;
    }

    for (unsigned int k = 0; k < NUM_OF_MODELS; ++k)
        {
            attrib_t attrib;
            is_obj_valid = load_obj(model_files[k], base_dir, vertices[k], normals[k], vertex_map[k], material_map[k], attrib, shapes[k], materials[k], model_scales[k]);
            
            glActiveTexture(GL_TEXTURE0);
            is_tex_valid = load_tex(base_dir, texcoords[k],
            texmap[k], attrib.texcoords, shapes[k], materials[k], GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR);
            printf("%d ](%s), obj valid : %d, tex valid : %d\n", k, model_files[k], is_obj_valid, is_tex_valid) ;
    
    }
    glGenVertexArrays(NUM_OF_MODELS, vao) ;
    
    for(int i = 0 ; i < NUM_OF_MODELS ; i++){
        glBindVertexArray(vao[i]) ;
        glGenBuffers(3, vbo[i]) ;
        bind_buffer(vbo[i][0], vertices[i], program, "vPosition", 3);
        bind_buffer(vbo[i][1], normals[i], program, "vNormal", 3);
        bind_buffer(vbo[i][2], texcoords[i], program, "vTexcoord", 2);
    }


    get_rect_3d(rec_vertices, w_width/1000, w_height/1000, 0.5f);
    get_vertex_color(rec_colors, rec_vertices.size() / 2, 0.8f, 0.2f, 0.5f);
    get_rect_texcoord(rec_texcoord);
    generate_background();

    glGenVertexArrays(1, &rec_vao);
    glBindVertexArray(rec_vao);
    glGenBuffers(3, rec_vbo);
    bind_buffer(rec_vbo[0], rec_vertices, program, "vPosition", 3);
    bind_buffer(rec_vbo[1], rec_colors, program, "vColor", 3);
    bind_buffer(rec_vbo[2], rec_texcoord, program, "aTexCoord", 2);


    
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_CULL_FACE);
    glDepthFunc(GL_BACK);
    
    
}


void display()
{
    if(user < 0){
        render(shading_mode) ;
        glFlush();
        for (int i = 0; i < 2; ++i) {
            state[i].theta = clock() * 0.000005f;
        }
    }
    else{
        render(shading_mode) ;
        glFlush() ;
    }
    
    
    glutPostRedisplay();
     
}


void render(int color_mode){
    
    glClearColor(1.f, 1.f, 1.f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    
    GLint location, M_location ;
    double aspect = 1.0 * w_width / w_height ;
    

    if (is_obj_valid) {
        mat4 M(1.0f);
        mat4 V = camera.get_viewing() ;
        mat4 P = camera.get_projection(aspect)  ;
        M_location = glGetUniformLocation(program, "M");
        glUniformMatrix4fv(M_location, 1, GL_FALSE, value_ptr(M)) ;
        location = glGetUniformLocation(program, "P");
        glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(P)) ;
        location = glGetUniformLocation(program, "V");
        glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(V)) ;
        
        glUniform1i(UVARS("ColorMode"), 0);
        glBindVertexArray(rec_vao);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);

        glUniform1i(UVARS("ourTexture"), 0);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        if(user == -1){
            for (int i = 0; i < 2; ++i)
            {
                //(set uniform variables of shaders for model i)
                M = state[i].get_transf() ;
                glUniformMatrix4fv(M_location, 1, GL_FALSE, value_ptr(M)) ;
                draw_obj_model(i, color_mode, i+1);
            }
        }
        else{
            if(!fin){
                M = state[user].get_transf() ;
                glUniformMatrix4fv(M_location, 1, GL_FALSE, value_ptr(M)) ;
                draw_obj_model(user, color_mode, user+1);
                
                for(int i = 0 ; i < n_heli ; i++){
                    M = heli_state[i].get_transf() ;
                    glUniformMatrix4fv(M_location, 1, GL_FALSE, value_ptr(M)) ;
                    draw_obj_model(HELI, color_mode, HELI+1);
                    float around = 0.05f ;
                    if(heli_state[i].z_pos + around >= state[user].z_pos && heli_state[i].z_pos - around <= state[user].z_pos
                       && heli_state[i].x_pos + around >= state[user].x_pos && heli_state[i].x_pos - around <= state[user].x_pos){
                        fin = true ;
                        printf("!!! user z : %f, plane z : %f\n",  state[user].z_pos, heli_state[i].z_pos) ;
                        break ;
                    }
                    heli_state[i].z_pos < z_bottom+0.2f ? heli_state[i].z_pos += 0.01f : heli_state[i].z_pos = z_top - rand() % 500 * 0.01 ;

                }

            }
            else{
               // game finished
            }
        }
    }
    if (color_mode != PICKING) {
        glutSwapBuffers();
    }
}

void mouse(int button, int s, int x, int y)
{
    button_pressed[button] = s;
    mouse_pos[0] = x;
    mouse_pos[1] = y;
    
    render(PICKING);
    glFlush();


    if (s == GLUT_UP) {
        unsigned char res[4];
        int height = glutGet(GLUT_WINDOW_HEIGHT);
        glReadPixels(x, height - y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &res);

        if(user < 0 && ( res[0] == 1 || res[0] == 2)){
            user = res[0]-1 ;
            state[user].theta = 0.0f ;
            state[user].x_pos =  0.0f ;
            state[user].z_pos =  z_bottom ;
            printf("selet user model %d\n", res[0]) ;
        }
        
    }
}


void cb_special(int key, int x, int y){
    GLfloat d_move = 0.08f ;


    if(user == -1) return ;
    
    if(key == GLUT_KEY_UP){
    z_top > state[user].z_pos - d_move ? state[user].z_pos = z_top : state[user].z_pos -= d_move ;
             //   printf("z_pos : %f\n", state[user].z_pos) ;
    }
    else if(key == GLUT_KEY_DOWN){
    z_bottom < state[user].z_pos + d_move ? state[user].z_pos =  z_bottom : state[user].z_pos += d_move ;
           //   printf("z_pos : %f\n", state[user].z_pos) ;
    }
    else if(key == GLUT_KEY_RIGHT){
    right_most < state[user].x_pos + d_move ? state[user].x_pos =  right_most : state[user].x_pos += d_move  ;
     //   printf("x_pos : %f\n", state[user].x_pos) ;
    }
    else if(key == GLUT_KEY_LEFT){
    left_most > state[user].x_pos - d_move ? state[user].x_pos =  left_most : state[user].x_pos -= d_move  ;
    //    printf("x_pos : %f\n", state[user].x_pos) ;
    }

    mouse_pos[0] = x;
    mouse_pos[1] = y;
    glutPostRedisplay();
}

void build_program(){
    ShaderInfo shaders[] = {
        {GL_VERTEX_SHADER, vert_dir},
        {GL_FRAGMENT_SHADER, frag_dir},
        {GL_NONE, NULL}
    } ;

    
    program = LoadShaders(shaders) ;
    glUseProgram(program) ;
}

static bool has_file(const char* filepath) {
     FILE *fp;
    if ((fp = fopen(filepath, "rb")) == 0) {
            fclose(fp);
         return true;
     }
     return false;
}

bool load_tex(const char* basedir, vector<real_t>& texcoords_out, map<string, unsigned int>& texmap_out, const vector<real_t>& texcoords, const vector<shape_t>& shapes, const vector<material_t>& materials, GLint min_filter=GL_LINEAR_MIPMAP_LINEAR, GLint mag_filter=GL_LINEAR_MIPMAP_LINEAR){
    
    unsigned int total_num_of_vertices = 0;
    unsigned int num_of_shapes = shapes.size();
    for (unsigned int s = 0; s < num_of_shapes; ++s)
    {
        total_num_of_vertices += shapes[s].mesh.indices.size();
    }
    texcoords_out.resize(total_num_of_vertices * 2);
    
    real_t* texcoords_dst_ptr = texcoords_out.data();
    const real_t* texcoords_src_ptr = texcoords.size() > 0 ? texcoords.data() : NULL;
    
    for (unsigned int s = 0; s < num_of_shapes; ++s)
    {
        const mesh_t& mesh = shapes[s].mesh;
        unsigned int num_of_faces = mesh.indices.size() / 3;
        for (unsigned int f = 0; f < num_of_faces; ++f)
        {
        // Get indices to the three vertices of a triangle.
            int idx[3] = {
                mesh.indices[3 * f + 0].texcoord_index,
                mesh.indices[3 * f + 1].texcoord_index,
                mesh.indices[3 * f + 2].texcoord_index
            };
        // Compute and copy valid texture coordinates.
            real_t tc[3][2];
            if (texcoords_src_ptr != NULL) {
                if (idx[0] < 0 || idx[1] < 0 || idx[2] < 0) {
                        fprintf(stderr, "Invalid texture coordinate index\n");
                        return false;
                }
                for (unsigned int i = 0; i < 3; ++i) {
                    memcpy(tc[i], texcoords_src_ptr + idx[i] * 2, sizeof(real_t) * 2);
                    tc[i][1] = 1.0f - tc[i][1];// flip the t coordinate.
                    
                }
            }
            else {
                tc[0][0] = tc[0][1] = 0;
                tc[1][0] = tc[1][1] = 0;
                tc[2][0] = tc[2][1] = 0;
            }
            memcpy(texcoords_dst_ptr, tc, sizeof(real_t) * 6);
            texcoords_dst_ptr += 6;
        }
    }
    
    GLuint texture_id;
    unsigned int num_of_materials = materials.size();
    for (unsigned int m = 0; m < num_of_materials; ++m)
    {
        const material_t& mat = materials[m];
        const string& texname = mat.diffuse_texname;
        if (texname.empty()) continue;
        if (MAP_FIND(texmap_out, texname)) continue;
        // Open the texture image file.
        string full_texpath =  texname;
          if (!has_file(full_texpath.c_str()))
          {
              full_texpath = basedir + texname;
              printf("full_texpath(1) : %s\n", full_texpath.c_str()) ;
              if (!has_file(full_texpath.c_str())) {
                  fprintf(stderr, "Failed to find %s\n", texname.c_str());
                  return false;
              }
            }
        // Generate a texture object.
        printf("full_texpath(2) : %s\n", full_texpath.c_str()) ;
        texture_id = generate_tex(full_texpath.c_str(), min_filter, mag_filter);
        if (texture_id < 0)
            return false;
        // Register the texture id.
        texmap_out[texname] = texture_id;
        
    }
    return true ;
}

GLuint generate_background(){
    GLuint texture;
    
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    
    int width, height, nrChannels;
    unsigned char *data = stbi_load(back2d_path, &width, &height, &nrChannels, 0);
    if (data)
    {
        std::cout<< "Suceed to load texture" << std::endl;
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
//    glBindTexture(GL_TEXTURE_2D, 0);
    return texture;
}

GLuint generate_tex(const char* tex_file_path, GLint min_filter,GLint mag_filter){
    int width, height, num_of_components;
    unsigned char* image = stbi_load(
    tex_file_path, &width, &height, &num_of_components, STBI_default);
    if (!image) {
        string path = string(base_dir) + string(tex_file_path) ;
        image = stbi_load(
        path.c_str(), &width, &height, &num_of_components, STBI_default);
        if(!image){
            printf("retry open tex file path (%s),-> failed \n", path.c_str()) ;
            return false;
        }
        printf("retry open tex file path (%s),-> success \n", path.c_str()) ;
    }
    // Generate a texture object and set its parameters.
    GLuint texture_id;
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter);
    
    bool is_supported = true;
    
    switch (num_of_components) {
        case 3:
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
        width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image); break;
        case 4:
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
        width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image); break;
        default:
        is_supported = false; break;
    }
    if (IS_MIPMAP(min_filter) || IS_MIPMAP(mag_filter))
        glGenerateMipmap(GL_TEXTURE_2D);
    // Release the loaded image data. ......
    
    stbi_image_free(image);
    glBindTexture(GL_TEXTURE_2D, 0);
    if (!is_supported)
    {
        fprintf(stderr, "Unsupported image format: %d components\n", num_of_components);
        glDeleteTextures(1, &texture_id); texture_id = -1;
    }
    return texture_id;
}



void draw_obj_model(int model_idx, int color_mode, int object_code){
    glUniform1i(UVARS("ColorMode"), color_mode);
    glUniform1i(UVARS("ObjectCode"), object_code);
    glBindVertexArray(vao[model_idx]);
    auto& _shapes = shapes[model_idx];
    auto& _materials = materials[model_idx];
    auto& _vertex_map = vertex_map[model_idx];
    auto& _material_map = material_map[model_idx];
    auto& _texmap = texmap[model_idx];

    for (unsigned int i = 0; i < _shapes.size(); ++i)
    {
        for (unsigned int j = 0; j < _material_map[i].size(); ++j) {
        int m_id = _material_map[i][j];
        if (m_id < 0) {
            glUniform1f(UVARS("n"), 10.0f);
            glUniform3f(UVARS("Ka"), 0.3f, 0.3f, 0.3f);
            glUniform3f(UVARS("Kd"), 1.0f, 1.0f, 1.0f);
            glUniform3f(UVARS("Ks"), 0.8f, 0.8f, 0.8f);
        }
        else {
            glUniform1f(UVARS("n"), _materials[m_id].shininess);
            glUniform3fv(UVARS("Ka"), 1, _materials[m_id].ambient);
            glUniform3fv(UVARS("Kd"), 1, _materials[m_id].diffuse);
            glUniform3fv(UVARS("Ks"), 1, _materials[m_id].specular);
            auto texitem = _texmap.find(_materials[m_id].diffuse_texname);
            if (texitem != _texmap.end()) {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, texitem->second);
                glUniform1i(UVARS("sampler"), 0);
            }
         }

        glDrawArrays(GL_TRIANGLES, _vertex_map[i][j], _vertex_map[i][j+1] - _vertex_map[i][j]);
        }
    }
}

mat4 parallel(double r, double aspect, double n, double f){
    double l = -r ;
    double width = 2*r ;
    double height = width / aspect ;
    double t = height / 2 ;
    double b = -t ;
    return ortho(l, r, b, t, n, f) ;
}

void bind_buffer(GLint buffer, GLvec& vec, int program, const GLchar* attri_name, GLint attri_size)
{
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vec.size(), vec.data(), GL_STATIC_DRAW);
    GLuint location = glGetAttribLocation(program, attri_name);
    glVertexAttribPointer(location, attri_size, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(location);
}
void bind_buffer(GLint buffer, int program, const GLchar* attri_name, GLint attri_size)
{
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    GLuint location = glGetAttribLocation(program, attri_name);
    glVertexAttribPointer(location, attri_size, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(location);
}
void init_menu(){
    glutCreateMenu(cb_main_menu);
    glutAddMenuEntry("Orthographic projection", 0);
    glutAddMenuEntry("Perspective projection", 1);
    glutAddMenuEntry("Phong Shading", 2);
    glutAddMenuEntry("Gouraud Shading", 3);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void motion(int x, int y)
{
    //Track tool (shift+ left mouse + drag)
    //Tumble tool (alt + left mouse + drag)
    //Zoom tool ( command + left mouse + drag)
    //Dolly tool ( alt + command + left mouse + drag)
    
    int modifiers = glutGetModifiers();
    int is_alt_active = modifiers & GLUT_ACTIVE_ALT;
    int is_command_active = modifiers & GLUT_ACTIVE_COMMAND;
    int is_shift_active = modifiers & GLUT_ACTIVE_SHIFT;
    int w = glutGet(GLUT_WINDOW_WIDTH);
    int h = glutGet(GLUT_WINDOW_HEIGHT);
    GLfloat dx = 1.f*(x - mouse_pos[0]) / w;
    GLfloat dy = -1.f*(y - mouse_pos[1]) / h;
    
    
    if (button_pressed[GLUT_LEFT_BUTTON] == GLUT_DOWN)
    {
        if(is_alt_active && is_command_active){
            //Dolly tool ( alt + command + left mouse + drag)
            printf("Dolly Tool\n") ;
            vec3 disp = camera.eye - camera.center;
            if (dy > 0)
                camera.eye = camera.center + 0.95f*disp ;
            else
                camera.eye = camera.center + 1.05f*disp ;
               
        }
        else if (is_alt_active)
        {
            //Tumble tool (alt + left mouse + drag)
            printf("Tumble Tool\n") ;
            vec4 disp(camera.eye - camera.center, 1);

            GLfloat alpha = 2.0f;
            mat4 V = camera.get_viewing();
            mat4 Rx = rotate(mat4(1.0f), alpha*dy, vec3(transpose(V)[0]));
            mat4 Ry = rotate(mat4(1.0f), -alpha*dx, vec3(0,1,0));
            mat4 R = Ry*Rx;
            camera.eye = camera.center + vec3(R*disp);
            camera.up = mat3(R)*camera.up;
        }
        else if(is_shift_active){
            //Track tool (shift+ left mouse + drag)
            printf("Track Tool\n") ;
            mat4 VT = transpose(camera.get_viewing());
            camera.eye += vec3(-dx* VT[0] + -dy * VT[1]);
            camera.center += vec3(-dx* VT[0] + -dy * VT[1]);
        }
        else if(is_command_active){
            //Zoom tool ( command + left mouse + drag)
            printf("Zoom Tool\n") ;
            
            if (dy > 0)
                camera.zoom_factor *= 0.98f;
            else
                camera.zoom_factor *= 1.02f;
                
        }
    }
    
    
    mouse_pos[0] = x;
    mouse_pos[1] = y;
    glutPostRedisplay();

}

void cb_main_menu(int value) {
    printf("cb_main_menu >> value: %d\n", value);
    if(value == 0 || value == 1) camera.projection_mode = value ;
    else if(value == 2 || value == 3) shading_mode = value ;
    glutPostRedisplay() ;
}
#define VSET2(v, a, b, c) do {(v)[0] = (a); (v)[1] = (b); (v)[2] = (c);} while(0)
#define VSET2PP(v, a, b, c) do {VSET2(v, a, b, c); v += 3;} while(0)

#define VSET2_2D(v, a, b) do {(v)[0] = (a); (v)[1] = (b);} while(0)
#define VSET2PP_2D(v, a, b) do {VSET2_2D(v, a, b); v += 2;} while(0)

void get_rect_3d(GLvec &p, GLfloat width, GLfloat height, GLfloat z)
{
    GLfloat w2 = width / 2;
    GLfloat h2 = height / 2;

    p.resize(18);
    GLfloat *data = p.data();

    VSET2PP(data, -w2, -h2, z);
    VSET2PP(data, +w2, -h2, z);
    VSET2PP(data, -w2, +h2, z);

    VSET2PP(data, +w2, -h2, z);
    VSET2PP(data, +w2, +h2, z);
    VSET2PP(data, -w2, +h2, z);
}

void get_vertex_color(GLvec &color, GLuint n, GLfloat r, GLfloat g, GLfloat b)
{
    color.resize(n * 3);
    for(GLuint i = 0; i < n; ++i){
        color[i * 3 + 0] = r;
        color[i * 3 + 1] = g;
        color[i * 3 + 2] = b;
    }
}

void get_rect_texcoord(GLvec &q){
    q.resize(12);
    GLfloat *data = q.data();
    
    VSET2PP_2D(data, 1.0f, 1.0f);
    VSET2PP_2D(data, 1.0f, 0.0f);
    VSET2PP_2D(data, 0.0f, 1.0f);

    VSET2PP_2D(data, 1.0f, 0.0f);
    VSET2PP_2D(data, 0.0f, 0.0f);
    VSET2PP_2D(data, 0.0f, 1.0f);
}

void keyboard(unsigned char key, int x, int y)
{
       switch (key) {
//           case '1' :
//           case '2' :
//               shading_mode = key - '1' ;
//               printf("shading mode : %d\n", shading_mode) ;
//               glutPostRedisplay() ;
//               break ;
//
//           case 'w' : show_wireframe = (show_wireframe == 0? 1: 0); printf("wireframe : %d\n", show_wireframe) ;glutPostRedisplay() ; break;
//           case 'g' : printf("grid\n") ; show_grid = (show_grid == 0? 1: 0); glutPostRedisplay() ; break;
//           case '+' : light_intense += 0.05f ; if(light_intense > 1.0f) light_intense = 1.0f ; printf("light intensity : %f\n", light_intense) ;glutPostRedisplay() ; break;
//          case '-' : light_intense -= 0.05f ; if(light_intense < 0.0f) light_intense = 0.0f ; printf("light intensity : %f\n", light_intense) ;glutPostRedisplay() ; break;
       }
}
