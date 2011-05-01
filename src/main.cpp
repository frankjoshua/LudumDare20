#include "MGE.h"

#include <GL/glut.h>

#include <iostream>
using namespace std;

#define _USE_MATH_DEFINES
#include <math.h>

#include <sys/time.h>

#define SWING_TIME 100
#define HANG_TIME 1000
#define DRAG_SPEED 0.001
#define WALK_SPEED 0.005
#define TURN_SPEED 0.005
#define TURN_TIME 400
#define BADDIE_SPEED 0.01
#define BADDIE_RANGE 0.05
#define SWORD_RANGE 0.3

void state_splash_screen( int n );
void state_game( int n );
void state_game_over( int n );

class Guy :
    public MGE::Drawables::AnimatedSprite
{

    public:

        Guy() :
            AnimatedSprite( 0,
                    200,
                    0,
                    0,
                    0.2,
                    0.2,
                    0 )
        {
            add_frame( MGE::Helpers::texture_from_image( "../assets/guy1.png" ) );
            add_frame( MGE::Helpers::texture_from_image( "../assets/guy2.png" ) );
            add_frame( MGE::Helpers::texture_from_image( "../assets/guy3.png" ) );
            add_frame( MGE::Helpers::texture_from_image( "../assets/guy4.png" ) );
            add_frame( MGE::Helpers::texture_from_image( "../assets/guy5.png" ) );
            add_frame( MGE::Helpers::texture_from_image( "../assets/guy6.png" ) );
            add_frame( MGE::Helpers::texture_from_image( "../assets/guy7.png" ) );
            add_frame( MGE::Helpers::texture_from_image( "../assets/guy8.png" ) );

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

        bool figure_angle_and_vector() {
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

        	                        if(current_angle != new_angle){
        	                        	float angleDiff = current_angle - new_angle;
        	                        	float distance = TURN_SPEED * time_since_draw();
        	                        	if(distance > TURN_SPEED * 33){
        	                        		distance = TURN_SPEED;
        	                        	}

        	                        	if(angleDiff > -3 && angleDiff < 3){
        	                        		if(current_angle > new_angle){
												current_angle -= distance;
											} else {
												current_angle += distance;
											}
        	                        	} else {
        	                        		if(current_angle > new_angle){
												current_angle += distance;
											} else {
												current_angle -= distance;
											}
        	                        	}

        	                        	if(current_angle < 0){
        	                        		current_angle = 6;
        	                        	} else if (current_angle > 6){
        	                        		current_angle = 0;
        	                        	}

        	                        	angleDiff = current_angle - new_angle;

        	                        	if(angleDiff > -TURN_SPEED && angleDiff < TURN_SPEED){
        	                        		current_angle = new_angle;
        	                        	}

        	                        }
        	                    }
        	                    else {
        	                        current_speed = 0;
        	                    }
        	                    return true;
        	                }
        	  return false;
        }

        bool draw() {
            figure_angle_and_vector();


            float x_delta = current_speed * cos(current_angle);
            float y_delta = current_speed * sin(current_angle);
            
            x( x()+x_delta );
            y( y()+y_delta );

            rotation( current_angle - M_PI/2 );

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

class Game;

class Sword :
    public MGE::EventHandlers::Mouse::Button,
    public MGE::EventHandlers::Mouse::Motion,
    public MGE::Drawables::Sprite,
    public MGE::Timer
{
     public:
         
         Sword( Guy& guy ) :
            Sprite( -1,
                    MGE::Helpers::texture_from_image( "../assets/sword.png" ),
                    0,
                    0,
                    0.08,
                    0.5,
                    0,
                    false ),
            guy_( guy ) {}

         double x() const { return guy_.x(); }
         double y() const { return guy_.y(); }

         bool swinging() { return swinging_; }
         float swing_angle() { return target_angle_ + M_PI/4; }

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
                    MGE::Helpers::texture_from_image("../assets/baddie.png"),
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
            check_kills();
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

        void check_kills() {
            if( sword.visible() ) {
                std::list<Baddie*>::iterator i = baddies.begin();
                while( i != baddies.end() ) {
                    
                    float angle = MGE::Helpers::line_angle(
                            sword.x(), sword.y(),
                            (*i)->x(), (*i)->y() );

                    float distance = MGE::Helpers::line_length(
                            sword.x(), sword.y(),
                            (*i)->x(), (*i)->y() );

                    if( sword.swing_angle() - M_PI/3 < angle &&
                        sword.swing_angle() + M_PI/3 > angle &&
                        distance < SWORD_RANGE )
                    {
                        delete *i;
                        baddies.erase(i);
                        break;
                    }

                    i++;
                }
            }

            timeout(
                    33,
                    bind(
                        &Game::check_kills,
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

