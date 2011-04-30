#include "MGE.h"

#include <GL/glut.h>

#include <iostream>
using namespace std;

#define _USE_MATH_DEFINES
#include <math.h>

#include <sys/time.h>

#define SWING_TIME 100
#define DRAG_SPEED 0.0001
#define WALK_SPEED 0.0005

class Guy :
    public MGE::Drawables::Sprite
{

    public:

        Guy() :
            Sprite( 0,
                    SOIL_load_OGL_texture(
						"../assets/guy.png",
						SOIL_LOAD_RGBA,
						SOIL_CREATE_NEW_ID,
						SOIL_FLAG_MIPMAPS | 
							SOIL_FLAG_INVERT_Y |
							SOIL_FLAG_MULTIPLY_ALPHA |
							SOIL_FLAG_COMPRESS_TO_DXT ),
                    0,
                    0,
                    0.2,
                    0.2,
                    0 ),
            dragging_( false ) {}

        bool dragging_;

    protected:

        bool draw() {
            if( !dragging_ ) {
                float x_target = -1;
                float y_target = 1;

                float angle = MGE::Helpers::line_angle(
                        x(), y(),
                        x_target, y_target );

                float x_delta = WALK_SPEED * cos(angle);
                float y_delta = WALK_SPEED * sin(angle);

                x( x()+x_delta );
                y( y()+y_delta );
            }

            MGE::Drawables::Sprite::draw();
        }

};

class Sword :
    public MGE::EventHandlers::Mouse::Button,
    public MGE::EventHandlers::Mouse::Motion,
    public MGE::Drawables::Sprite
{
     public:
         
         Sword( Guy& guy ) :
            Sprite( -1,
                    SOIL_load_OGL_texture(
						"../assets/sword.png",
						SOIL_LOAD_RGBA,
						SOIL_CREATE_NEW_ID,
						SOIL_FLAG_MIPMAPS | 
							SOIL_FLAG_INVERT_Y |
							SOIL_FLAG_MULTIPLY_ALPHA |
							SOIL_FLAG_COMPRESS_TO_DXT ),
                    0,
                    0,
                    0.08,
                    0.5,
                    0,
                    false ),
            guy_( guy ) {}

         double x() const { return guy_.x(); }
         double y() const { return guy_.y(); }

     protected:

         virtual bool handle_mouse_motion(int x, int y) {
             float mouse_x = MGE::Helpers::mouse_x_to_screen_x(x);
             float mouse_y = MGE::Helpers::mouse_y_to_screen_y(y);

             double angle = MGE::Helpers::line_angle(
                     guy_.x(), guy_.y(),
                     mouse_x, mouse_y );

             // To account for an initial rotation of the image
             target_angle_ = angle-M_PI/2; 

             if( !swinging_ && visible() ) {
                 rotation( target_angle_ );
             }

             return true;
         }

         virtual bool handle_button_down(int button, int x, int y) {
             if( button == 0 ) {
                 swinging_ = true;
                 rotation( target_angle_ - M_PI/4 );
                 visible(true);
                 request_redraw();
             }
         }

         virtual bool handle_button_up(int button, int x, int y ) {
            if( button == 0 ) {
                guy_.dragging_ = false;
                visible(false);
            }
         }

         virtual bool draw() {
             static unsigned int stroke_time = 100;

             static struct timeval last;
             struct timeval now;

             if( swinging_ ) {
                 gettimeofday(&now, NULL);
                 
                 float slice = ((target_angle_ + M_PI/4) - (target_angle_ - M_PI/4))/stroke_time;
                 
                 rotation( rotation()+slice*time_since_draw() );

                 if(rotation() >= target_angle_ + M_PI/4 ) {
                    swinging_ = false;
                    rotation( target_angle_ );
                 }
             }
             else if( visible() ) {
                 guy_.dragging_ = true;

                 float x_delta = DRAG_SPEED * cos(target_angle_ + M_PI/2);
                 float y_delta = DRAG_SPEED * sin(target_angle_ + M_PI/2);

                 guy_.x( guy_.x()+x_delta );
                 guy_.y( guy_.y()+y_delta );
             }
             
             gettimeofday(&last,NULL);

             return MGE::Drawables::Sprite::draw();
         }

     private:

         Guy& guy_;

         float target_angle_;
         bool swinging_;

};

class MousePtr :
    public MGE::EventHandlers::Mouse::Motion,
    public MGE::Drawables::Base
{

    public:

        MousePtr(const Guy& guy) :
            Base(10),
            guy_(guy) {}

    protected:

        bool handle_mouse_motion(int x, int y) {
            x_ = MGE::Helpers::mouse_x_to_screen_x(x);
            y_ = MGE::Helpers::mouse_y_to_screen_y(y);

            return true;
        }

        bool draw() {
            glLoadIdentity();
            glColor4f(1,0,0,0.5);

            glBegin(GL_LINES);
                glVertex3f(guy_.x(), guy_.y(), 0);
                glVertex3f(x_, y_, 0);

                glVertex3f(guy_.x(), guy_.y(), 0 );
                glVertex3f(x_, guy_.y(), 0 );

                glVertex3f(x_, y_, 0 );
                glVertex3f(x_, guy_.y(), 0 );
            glEnd();

            return true;
        }

    private:

        float x_,y_;

        const Guy& guy_;

};


int main( int argc, char** argv ) {
    MGE::App app("You Are The Sword: Ludum Dare 20 Entry");
    app.initialize(argc,argv);

    MGE::Drawables::ClearScreen background(INT_MIN,0,0,1,1);
    Guy guy;
    Sword sword(guy);

    MousePtr mouse(guy);

    return app.run();
}

