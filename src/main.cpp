#include "MGE.h"

#include <GL/glut.h>

#include <iostream>
using namespace std;

#define _USE_MATH_DEFINES
#include <math.h>

#include <sys/time.h>

#define SWING_TIME 100
#define HANG_TIME 500
#define DRAG_SPEED 0.0001
#define WALK_SPEED 0.0005
#define TURN_TIME 400
#define BADDIE_SPEED 0.01
#define BADDIE_RANGE 0.01

void state_splash_screen( int n );
void state_game( int n );
void state_game_over( int n );

class Guy :
    public MGE::Drawables::Sprite,
    public MGE::Timer
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
                    0 )
        {
            old_vector[0] = 0;
            old_vector[1] = 0;
            old_speed = 0;
            old_angle = 0;
            
            current_vector[0] = 0;
            current_vector[1] = 0;
            current_speed = 0;
            current_angle = 0;

            new_vector[0] = 0;
            new_vector[1] = 0;
            new_speed = 0;
            new_angle = 0;
        }

        void move_towards( float x, float y, float speed ) {
            old_vector[0] = current_vector[0];
            old_vector[1] = current_vector[1];

            new_vector[0] = x;
            new_vector[1] = y;

            old_speed = current_speed;
            new_speed = speed;
        }

        void kill() {
            glutTimerFunc(1,state_game_over,0);
        }

    protected:

        void figure_angle_and_vector() {
            if( current_vector[0] != new_vector[0] ||
                current_vector[1] != new_vector[1] ||
                current_speed != new_speed ||
                current_angle != new_angle )
            {
                current_vector[0] = new_vector[0];
                current_vector[1] = new_vector[1];

                float x_distance = x() - current_vector[0];
                x_distance = x_distance < 0 ? x_distance*-1 : x_distance;

                float y_distance = y() - current_vector[1];
                y_distance = y_distance < 0 ? y_distance*-1 : y_distance;

                if( x_distance > 0.001 &&
                    y_distance > 0.001 )
                {
                    current_speed = new_speed;

                    old_angle = current_angle;
                    new_angle = MGE::Helpers::line_angle(
                        x(), y(),
                        current_vector[0], current_vector[1] );
                    current_angle = new_angle;
                }
                else {
                    current_speed = 0;
                }
            }
        }

        bool draw() {
            figure_angle_and_vector();

            float x_delta = current_speed * cos(current_angle);
            float y_delta = current_speed * sin(current_angle);
            
            x( x()+x_delta );
            y( y()+y_delta );

            rotation( current_angle );

            return MGE::Drawables::Sprite::draw();
        }

    private:

        float old_vector[2];
        float current_vector[2];
        float new_vector[2];

        float new_angle;
        float current_angle;
        float old_angle;

        float old_speed;
        float current_speed;
        float new_speed;
};

class Sword :
    public MGE::EventHandlers::Mouse::Button,
    public MGE::EventHandlers::Mouse::Motion,
    public MGE::Drawables::Sprite,
    public MGE::Timer
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
             mouse_x = MGE::Helpers::mouse_x_to_screen_x(x);
             mouse_y = MGE::Helpers::mouse_y_to_screen_y(y);

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
                 guy_.move_towards(
                         guy_.x(), guy_.y(), 0 );
                 rotation( target_angle_ - M_PI/4 );
                 visible(true);
                 request_redraw();
             }
         }

         void stop_dragging() {
             guy_.move_towards( -1, 1, DRAG_SPEED );
         }

         virtual bool handle_button_up(int button, int x, int y ) {
            if( button == 0 ) {
                visible(false);
                 guy_.move_towards(
                         guy_.x(), guy_.y(), 0 );

                timeout(
                        HANG_TIME,
                        bind(
                            &Sword::stop_dragging,
                            this ) );
            }
         }

         virtual bool draw() {
             if( swinging_ ) {
                 float slice = ((target_angle_ + M_PI/4) - (target_angle_ - M_PI/4))/SWING_TIME;
                 
                 rotation( rotation()+slice*time_since_draw() );

                 if(rotation() >= target_angle_ + M_PI/4 ) {
                    swinging_ = false;
                    rotation( target_angle_ );
                 }
             }
             else if( visible() ) {
                 guy_.move_towards( mouse_x, mouse_y, DRAG_SPEED );
             }
             
             return MGE::Drawables::Sprite::draw();
         }

     private:

         Guy& guy_;

         float target_angle_;
         bool swinging_;

         float mouse_x;
         float mouse_y;

};

class Baddie :
    public MGE::Drawables::Sprite,
    public MGE::Timer
{
    
    public:

        Baddie( float direction,
                Guy& guy ) :
            Sprite( -1,
                    SOIL_load_OGL_texture(
						"../assets/baddie.png",
						SOIL_LOAD_RGBA,
						SOIL_CREATE_NEW_ID,
						SOIL_FLAG_MIPMAPS | 
							SOIL_FLAG_INVERT_Y |
							SOIL_FLAG_MULTIPLY_ALPHA |
							SOIL_FLAG_COMPRESS_TO_DXT ),
                    1.5*sin(direction),
                    1.5*cos(direction),
                    0.2,
                    0.2,
                    0 ),
            guy_( guy )
        {
            move();
        }

    protected:

        void move() {
            float angle = MGE::Helpers::line_angle(
                    x(), y(),
                    guy_.x(), guy_.y() );

            float x_delta = BADDIE_SPEED * cos(angle);
            float y_delta = BADDIE_SPEED * sin(angle);
            
            x( x()+x_delta );
            y( y()+y_delta );

            float x_distance = x() - guy_.x();
            float y_distance = y() - guy_.y();

            float distance = sqrt( x_distance*x_distance + y_distance*y_distance );

            if( distance < BADDIE_RANGE ) {
                guy_.kill();
            }
            
            timeout(
                    33,
                    bind(
                        &Baddie::move,
                        this ) );
        }

    private:

        Guy& guy_;

};

class State {
    public:
        State() {};
        virtual ~State() {};
};

State* current_state;

void state_splash_screen( int n ) {

}

class Game : public State,
    public MGE::Timer 
{
    
    public:

        Game() :
            background(INT_MIN,0,0,1,1),
            sword(guy) 
        {
            baddie_timeout = 2000;
            new_baddie();
        }

        ~Game() {
            cout<< "Leaving game" <<endl;
            std::list<Baddie*>::iterator i = baddies.begin();
            while( i != baddies.end() ) {
                delete *i;
                i++;
            }
        }

        void new_baddie() {
            float angle = 1.1*M_PI*float(rand())/INT_MAX;

            baddies.push_back( new Baddie( angle, guy ) );

            baddie_timeout *= 0.99;
            timeout(baddie_timeout,
                    bind(
                        &Game::new_baddie,
                        this ) );
        }

    private:

        MGE::Drawables::ClearScreen background;

        unsigned int baddie_timeout;

        Guy guy;
        Sword sword;

        std::list<Baddie*> baddies;

};

void state_game( int n ) {
    delete current_state;
    current_state = new Game;
}

class GameOver : public State {
    public:

        GameOver() :
            background(INT_MIN,1,0,0,1) {}

        ~GameOver() {}

    private:

        MGE::Drawables::ClearScreen background;
};

void state_game_over( int n ) {
    delete current_state;
    current_state = new GameOver;
}


int main( int argc, char** argv ) {
    MGE::App app("You Are The Sword: Ludum Dare 20 Entry");
    app.initialize(argc,argv);

    state_game(0);

    return app.run();
}

