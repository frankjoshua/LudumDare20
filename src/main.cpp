#include "MGE.h"

#include <GL/glut.h>

#include <iostream>
using namespace std;

#define _USE_MATH_DEFINES
#include <math.h>

#include <sys/time.h>

#define SWING_TIME   100
#define HANG_TIME    1000
#define DRAG_SPEED   0.005
#define WALK_SPEED   0.005
#define TURN_SPEED   0.005
#define TURN_TIME    100
#define BADDIE_SPEED 0.02
#define BADDIE_RANGE 0.05
#define TREASURE_RANGE 0.15
#define SWORD_RANGE  0.3
#define WALL_X 0.5
#define WALL_Y 0.32
#define SCREEN_X_THRESHOLD 1.05
#define SCREEN_Y_THRESHOLD 1.05
#define TREASURE_TIMEOUT 2000

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
                    0 ),
            pulling_texture( MGE::Helpers::texture_from_image( "../assets/guy_stand.png") ),
            pulling_(false)
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

        void pull() {
            w(0.55);
            h(0.5);
            pulling_ = true;
        }

        void stop_pulling() {
            w(0.2);
            h(0.2);
            pulling_ = false;
        }

        bool pulling() {
            return pulling_;
        }

    protected:
    void calcAngle()
    {
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

        virtual GLuint texture() const {
            if( pulling_ ) {
                return pulling_texture;
            }
            else {
                return AnimatedSprite::texture();
            }
        }

        bool draw() {
            figure_angle_and_vector();

            float x_delta = current_speed * cos(current_angle);
            float y_delta = current_speed * sin(current_angle);
            
            float new_x = x()+x_delta;
            float new_y = y()+y_delta;

            if( ( new_x < -WALL_X && new_y > WALL_Y ) ||
                ( new_x > WALL_X && new_y > WALL_Y ) ||
                ( new_x > WALL_X && new_y < -WALL_Y ) ||
                ( new_x < -WALL_X && new_y < -WALL_Y ) )
            {
                if( y() > WALL_Y && x() > -WALL_X && x() < WALL_X ) {
                    y( new_y );                
                }
                else if( y() < -WALL_Y && x() > -WALL_X && x() < WALL_X ) {
                    y( new_y );                
                }
                else if( x() > WALL_X && y() > -WALL_Y && y() < WALL_Y ) {
                    x( new_x );
                }
                else if( x() < -WALL_X && y() > -WALL_Y && y() < WALL_Y ) {
                    x( new_x );
                }
            }
            else {
                x( x()+x_delta );
                y( y()+y_delta );
            }

            if( x() > SCREEN_X_THRESHOLD || x() < -SCREEN_X_THRESHOLD ) {
                x( -1*x() );
            }
            else if( y() > SCREEN_Y_THRESHOLD || y() < -SCREEN_Y_THRESHOLD ) {
                y( -1*y() );
            }

            rotation( current_angle - M_PI/2 );

            return MGE::Drawables::Sprite::draw();
        }

    private:

        GLuint pulling_texture;

        bool pulling_;

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
                    MGE::Helpers::texture_from_image( "../assets/arm.png" ),
                    0,
                    0,
                    0.15,
                    0.5,
                    0,
                    1,
                    false ),
            guy_( guy ) {
        	 treasure_x = 1;
        	 treasure_y = 1;
         }

         double x() const { return guy_.x(); }
         double y() const { return guy_.y(); }

         bool swinging() { return swinging_; }
         float swing_angle() { return target_angle_ + M_PI/4; }
         float setTreasureX(float tX){ treasure_x = tX; }
         float setTreasureY(float tY){ treasure_y = tY; }

         void got_treasure(){ huntNewTreasure = true; }

     protected:

         virtual bool handle_mouse_motion(int x, int y) {
             mouse_x = MGE::Helpers::mouse_x_to_screen_x(x);
             mouse_y = MGE::Helpers::mouse_y_to_screen_y(y);

             cout<< mouse_x << " " << mouse_y <<endl;

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
             guy_.move_towards( treasure_x, treasure_y, DRAG_SPEED );
         }

         virtual bool handle_button_up(int button, int x, int y ) {
            if( button == 0 ) {
                visible(false);
                 guy_.move_towards(
                         guy_.x(), guy_.y(), 0 );

                 guy_.stop_pulling();

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
                    visible(false);
                    guy_.pull();
                 }

             }
             else if( visible() || guy_.pulling() ) {
                 guy_.move_towards( mouse_x, mouse_y, DRAG_SPEED );
             } else if (huntNewTreasure){
            	 guy_.move_towards( treasure_x, treasure_y, DRAG_SPEED );
             }
             
             return MGE::Drawables::Sprite::draw();
         }

     private:

         Guy& guy_;

         float target_angle_;
         bool swinging_;

         float mouse_x;
         float mouse_y;

         float treasure_x;
         float treasure_y;

         bool huntNewTreasure;
};

class Health {
    
    public:

       Health() {
            for( int i=0; i<3; i++ ) {
                health.push_back(
                        new MGE::Drawables::Sprite(
                           1,
                           MGE::Helpers::texture_from_image("../assets/heart.png"),
                           -0.93+i*0.12,
                           0.9,
                           0.1,
                           0.1 ) );
            }
            for( int i=0; i<3; i++ ) {
                health.push_back(
                        new MGE::Drawables::Sprite(
                           1,
                           MGE::Helpers::texture_from_image("../assets/heart.png"),
                           -0.93+i*0.12,
                           0.75,
                           0.1,
                           0.1 ) );
            }
       }

       ~Health() {
           delete health.back();
           health.pop_back();
       }

       void hit() {
           if( health.size() == 1 ) {
               glutTimerFunc(1,state_game_over,0);
           }
           else {
               delete health.back();
               health.pop_back();
           }
       }

    private:

       std::list<MGE::Drawables::Sprite*> health;


};



class Baddie :
    public MGE::Drawables::AnimatedSprite
{
    
    public:

        Baddie( float direction,
                Guy& guy,
                Health& health ) :
            AnimatedSprite( -1,
                    120,
                    1.5*sin(direction),
                    1.5*cos(direction),
                    0.142,
                    0.3,
                    0 ),
            guy_( guy ),
            health_( health )
        {
            add_frame( MGE::Helpers::texture_from_image( "../assets/bat3.png" ) );
            add_frame( MGE::Helpers::texture_from_image( "../assets/bat2.png" ) );
            add_frame( MGE::Helpers::texture_from_image( "../assets/bat3.png" ) );

            move();
        }

    protected:

        void move() {
            float angle = MGE::Helpers::line_angle(
                    x(), y(),
                    guy_.x(), guy_.y() );
            rotation(angle);
            float x_delta = BADDIE_SPEED * cos(angle);
            float y_delta = BADDIE_SPEED * sin(angle);
            
            x( x()+x_delta );
            y( y()+y_delta );

            float x_distance = x() - guy_.x();
            float y_distance = y() - guy_.y();

            float distance = sqrt( x_distance*x_distance + y_distance*y_distance );

            if( distance < BADDIE_RANGE ) {
                health_.hit();
                visible(false);
            }
            else {
                timeout(
                        33,
                        bind(
                            &Baddie::move,
                            this ) );
            }
        }

    private:

        Guy& guy_;
        Health& health_;

};

    class Treasure :
        public MGE::Drawables::Sprite,
        public MGE::Timer
    {

        public:

        	Treasure( float startX, float startY ) :
                Sprite( -1,
                        MGE::Helpers::texture_from_image(
    						"../assets/treasurechest.png" ),
                        startX,
                        startY,
                        0.2,
                        0.204,
                        0 )
            {
               //Constructor stuff here
        		//x(startX);
        		//y(startY);
            }

    };

class Score : public MGE::Drawables::Base {

    public:

        Score() :
            score(0) {}

        unsigned int score;

    protected:

        bool draw() {
            glLoadIdentity();
            glDisable(GL_TEXTURE_2D);
            glColor3f(1,1,1);
            glRasterPos3f(0.7, 0.85, 0);

            char label[] = "Score: ";
            char score_str[5];
            sprintf(score_str,"%4d",score);

            for( char* c=label; *c!='\0'; c++ ) {
                glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,*c);
            }
            for( char* c=score_str; *c!='\0'; c++ ) {
                glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,*c);
            }
            glEnable(GL_TEXTURE_2D);
        
            return true;
        }

};

class State {
    public:
        State() {};
        virtual ~State() {};
};

State* current_state;

void SplashScreen( int n ) {

    public:

        SplashScreen() :
            stars_a(
                    MGE::Helpers::texture_from_image(
                        "../assets/splashscreen/splashstarsb.png"),
                    0,0,
                    2,2,
                    0,
                    0 ),
            stars_b(
                    MGE::Helpers::texture_from_image(
                        "../assets/splashscreen/splashstarsr.png"),
                    0,0,
                    2,2,
                    0,
                    0 ),
            stars_c(
                    MGE::Helpers::texture_from_image(
                        "../assets/splashscreen/splashstarsy.png"),
                    0,0,
                    2,2,
                    0,
                    0 ),
            logo(
                    MGE::Helpers::texture_from_image(
                        "../assets/splashscreen.png"),
                    0,0,
                    2,2,
                    0,
                    0 ),

    MGE::Drawables::Sprite stars_a;
    MGE::Drawables::Sprite stars_b;
    MGE::Drawables::Sprite stars_c;

    MGE::Drawables::Sprite logo;

}

class Game : public State,
    public MGE::Timer 
{
    
    public:

        Game() :
            background(INT_MIN,0,0,0,1),
            background_image(
                    -2,
                    MGE::Helpers::texture_from_image("../assets/background.png"),
                    0,
                    0,
                    2,
                    2 ),
            sword(guy) 
        {
            baddie_timeout = 2000;
            treasureScore = 0;
            srand ( time(NULL) );
            new_baddie();
            new_treasure();
            check_kills();
            detect_treasure_contact();
        }

        ~Game() {
            cout<< "Leaving game" <<endl;
            //Remove Baddies
            std::list<Baddie*>::iterator i = baddies.begin();
            while( i != baddies.end() ) {
                delete *i;
                i++;
            }
            //Remove Treasures
            std::list<Treasure*>::iterator t = treasures.begin();
			while( t != treasures.end() ) {
				delete *t;
				t++;
			}
        }

        void new_baddie() {
            float angle = 1.1*M_PI*float(rand())/INT_MAX;

            baddies.push_back( new Baddie( angle, guy, health ) );

            baddie_timeout *= 0.95;
            timeout(baddie_timeout,
                    bind(
                        &Game::new_baddie,
                        this ) );
        }

        void new_treasure() {
            float position;
            float new_x = 1;
            float new_y = 1;

            while(
                ( new_x < -WALL_X && new_y > WALL_Y ) ||
                ( new_x > WALL_X && new_y > WALL_Y ) ||
                ( new_x > WALL_X && new_y < -WALL_Y ) ||
                ( new_x < -WALL_X && new_y < -WALL_Y ) )
            {
                new_x = rand()*2.0/INT_MAX -1;
                new_y = rand()*2.0/INT_MAX -1;
            }

            treasures.push_back( new Treasure( new_x, new_y ) );

            timeout(TREASURE_TIMEOUT,
                    bind(
                        &Game::new_treasure,
                        this ));
        }

        void check_kills() {
            if( sword.visible() || guy.pulling() ) {
                std::list<Baddie*>::iterator i = baddies.begin();
                while( i != baddies.end() ) {
                    
                    float angle;
                    
                    angle = MGE::Helpers::line_angle(
                            sword.x(), sword.y(),
                            (*i)->x(), (*i)->y() );

                    float distance = MGE::Helpers::line_length(
                            sword.x(), sword.y(),
                            (*i)->x(), (*i)->y() );

                    if( !guy.pulling() &&
                        sword.swing_angle() - M_PI/3 < angle &&
                        sword.swing_angle() + M_PI/3 > angle &&
                        distance < SWORD_RANGE )
                    {
                        delete *i;
                        baddies.erase(i);
                        score.score++;
                        break;
                    }
                    else if(
                        guy.pulling() &&
                        guy.rotation() + M_PI/2 - M_PI/3 < angle &&
                        guy.rotation() + M_PI/2 + M_PI/3 > angle &&
                        distance < SWORD_RANGE )
                    {
                        delete *i;
                        baddies.erase(i);
                        score.score++;
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

        void detect_treasure_contact(){
        			//Look for treasure
        			std::list<Treasure*>::iterator t = treasures.begin();
        			float shortestDistance = 2;
        			Treasure* closestTreasure;
        			bool huntTreasure = false;
        			while( t != treasures.end() ) {
        				float x_distance = (*t)->x() - guy.x();
        			    float y_distance = (*t)->y() - guy.y();

        				float distance = sqrt( x_distance*x_distance + y_distance*y_distance );
        				if(distance < shortestDistance){
        					shortestDistance = distance;
        					closestTreasure = *t;
        					huntTreasure = true;
        				}

        				if( distance < TREASURE_RANGE ) {
        					score.score++;
        					delete *t;
        					treasures.erase(t);
        					huntTreasure = false;
        					sword.got_treasure();
        					shortestDistance = 2;
        					break;
        				}

        				t++;
        			}

        			//Run from Baddies
        			bool runFromBaddie = false;
        			Baddie* closestBaddie;
        			std::list<Baddie*>::iterator b = baddies.begin();
        			shortestDistance += .05;
        			while(b != baddies.end())
        			{
        				float x_distance = (*b)->x() - guy.x();
						float y_distance = (*b)->y() - guy.y();

						float distance = sqrt( x_distance*x_distance + y_distance*y_distance );
						if(distance < shortestDistance && distance < .25){
							shortestDistance = distance;
							huntTreasure = false;
							closestBaddie = *b;
							runFromBaddie = true;
						}

						b++;
        			}

        			if(huntTreasure){
						sword.setTreasureX(closestTreasure->x());
						sword.setTreasureY(closestTreasure->y());
        			} else if(runFromBaddie) {
        				float bX = closestBaddie->x();
        				float bY = closestBaddie->y();
        				float sX = sword.x();
        				float sY = sword.y();

        				if(sX > bX){
        					sword.setTreasureX(1);
        				} else {
        					sword.setTreasureX(-1);
        				}

        				if(sY > bY){
							sword.setTreasureY(1);
						} else {
							sword.setTreasureY(-1);
						}

        			}

        			timeout(
								33,
								bind(
									&Game::detect_treasure_contact,
									this ) );
        	}

    private:

        MGE::Drawables::ClearScreen background;
        MGE::Drawables::Sprite background_image;

        unsigned int baddie_timeout;
        int treasureScore;

        Guy guy;
        Sword sword;
        Score score;
        Health health;

        std::list<Baddie*> baddies;

        std::list<Treasure*> treasures;

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

